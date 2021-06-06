/*
	Copyright (C) 2021 Florian Bärwolf
	floribaer@gmx.de

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "plot.hpp"
#include <mgl2/mgl.h>
#include <mgl2/fltk.h>

// plot_t::plot_t(const quantity_t& X, const quantity_t& Y, std::__cxx11::string legend) : X(&X),Y1({std::make_pair(&Y,legend)})
// {
// }
// 
// plot_t::plot_t(const quantity_t& X, 
// 			   const quantity_t& Y1, std::__cxx11::string Y1_legend, 
// 			   const quantity_t& Y2, std::__cxx11::string Y2_legend)  : 
// 			   X(&X),
// 			   Y1({std::make_pair(&Y1,Y1_legend)}),
// 			   Y2({std::make_pair(&Y2,Y2_legend)}) 
// {
// }
// 
// plot_t::plot_t(const quantity_t& X, 
// 			   vector<pair<const quantity_t *, std::__cxx11::string> > Y1_quantities_to_legends, 
// 			   vector<pair<const quantity_t *, std::__cxx11::string> > Y2_quantities_to_legends) :
// 			   X(&X), Y1(Y1_quantities_to_legends), Y2(Y2_quantities_to_legends)
// {
// }

plot_t::plot_t(bool Y1_log10, bool Y2_log10, bool Y3_log10) 
{
	Y1.log10_scale= Y1_log10;
	Y2.log10_scale= Y2_log10;
	Y3.log10_scale= Y3_log10;
	Y1.color = "k";
	Y2.color = "B";
	Y3.color = "R";
}


int plot_t::Draw(mglGraph* gr)
{	
	logger::debug(15,"plot_t::Draw()","entering");
	/*x axis*/
	vector<const quantity_t*> Xs;
	for (auto& c:Y1.curves)
		Xs.push_back(&c.X);
	for (auto& c:Y2.curves)
		Xs.push_back(&c.X);
	for (auto& c:Y3.curves)
		Xs.push_back(&c.X);
	
	if (Xs.size()==0)
	{
		logger::error("plot_t::Draw()","no X axis","","returning 0");
		return 0;
	}
	
	axis_t::range_t X_range(Xs);
	
	
	
	gr->SetFontSize(3);
// 	X_range.log10();
	gr->SetRange('x',X_range.start,X_range.stop);
	gr->SetOrigin(0,-0.98);
	gr->Axis("x");
// 	mglData x(X->data.size());
// 	x.Set(X->data);
	
	stringstream x_l;
	x_l << Xs.front()->name() << " [" << Xs.front()->unit().to_string() << "]";
	tools::str::filter_t str_f(x_l.str());
	gr->Label('x',str_f.escape_special_characters().c_str(),0);
	/********/
	
	if (Y1.check())
		Y1.draw(gr);
	if (Y2.check())
	{
		if (Y1.check())
		{
			gr->SetRange('x',X_range.start,X_range.stop);
			gr->SetOrigin(X_range.stop,Y1.range().stop);
			gr->Axis("x");
		}
		Y2.draw(gr, X_range.stop);
	}
	if (Y3.check())
	{
		if (Y1.check() && !Y2.check())
		{
			gr->SetRange('x',X_range.start,X_range.stop);
			gr->SetOrigin(X_range.stop,Y1.range().stop);
			gr->Axis("x");
		}
		if (Y2.check()) Y3.draw(gr, X_range.stop*1.15);
		else Y3.draw(gr, X_range.stop);
	}
	
	for (auto& line : lines)
		gr->Line( mglPoint(line.x_start,line.y_start), mglPoint(line.x_stop,line.y_stop),line.color.c_str());
	
// 	gr->Grid();
// 	gr->Box();
// 	gr->Legend(0.49999,1.05,"^-");
	gr->Legend(0.5,1.04,"A-","value 0.04");
	logger::debug(15,"plot_t::Draw()","exiting");
	return 0;
}

// bool plot_t::Draw_axis(mglGraph* gr, int axis_number, bool logarithmic_scale)
// {
// 	vector<pair<const quantity_t*,string>>* Y;
// 	if (axis_number==1)
// 		Y = &Y1;
// 	else if (axis_number==2)
// 		Y = &Y2;
// 	else if (axis_number==3)
// 		Y = &Y3;
// 	else
// 	{
// 		logger::error("plot_t::Draw_axis","axis_number unknown ",tools::to_string(axis_number),"returning false");
// 		return false;
// 	}
// 	
// 	axis_range_t Y_range(*Y);
// 	if (logarithmic_scale) 
// 		Y_range.log10();
// 	for (auto& y : *Y)
// 	{
// 		mglData y_data(X->data.size());
// 		y_data.Set(y.first->data);
// 		gr->SetRange('y',Y_range.start,Y_range.stop);
// 		if (logarithmic_scale) gr->SetFunc("","lg(y)");
// 		gr->SetOrigin(Y_range.stop,0);
// 		gr->Axis("!Ey");
// 		stringstream y_l;
// 		y_l << "#B{" << y2.first->name() << " [" << y2.first->unit().to_string() << "]}";
// 		gr->Label('y',y_l.str().c_str(),0);
// 		string l = "legend '"+ y2.second +" "+y2.first->name()+"'";
// 		gr->Plot(x,y,"B",l.c_str());
// 	}
// 	
// }

void plot_t::add_line(double x_start, double y_start, double x_stop, double y_stop, std::__cxx11::string color)
{
	lines.push_back({x_start,y_start,x_stop,y_stop,color});
}

void plot_t::add_arrow(double x_start, double y_start, double x_stop, double y_stop, std::__cxx11::string color)
{
	lines.push_back({x_start,y_start,x_stop,y_stop,color});
}


void plot_t::to_screen(const std::__cxx11::string window_title, double sleep_sec)
{
// 	cout << "window_title\t" << window_title << endl;
	mglFLTK gr(this,window_title.c_str());
// 	gr.Run();
	if (sleep_sec==0)
		gr.Run();
	else
	{
		gr.RunThr();
		usleep(sleep_sec*1000*1000);
	}
}

void plot_t::to_file(const std::__cxx11::string filename)
{
	mglFLTK gr(this);
// 	gr.SetQuality(6);
// 	gr.WritePNG(filename.c_str());
// 	gr.WriteJPEG(filename.c_str());
	gr.WriteFrame(filename.c_str());
}




/***************************************/
/* plot_t::axis_range_t::axis_range_t **/
/***************************************/



// plot_t::axis_range_t::axis_range_t(vector<pair<const quantity_t *, std::__cxx11::string> > Ys)
// {
// 	double temp=-1;
// 	for (auto& y : Ys)
// 	{
// 		temp = statistics::get_max_from_Y(y.first->data);
// 		if (stop==-1 || temp > stop)
// 			stop = temp;
// 		temp = statistics::get_min_from_Y(y.first->data);
// 		if (start==-1 || temp < start)
// 			start = temp;
// 		
// 		if (y.first->name() != Ys.front().first->name())
// 			logger::error("plot_t::axis_range_t::axis_range_t()","Y has elements with different quantity names",y.first->to_string() + " " + y.second,"ignoring");
// 		
// 		if (y.first->unit() != Ys.front().first->unit())
// 			logger::error("plot_t::axis_range_t::axis_range_t()","Y has elements with different quantity units",y.first->to_string()+" "+y.second,"ignoring");
// 	}
// }


/*
void plot_t::axis_range_t::log10()
{
	if (start<0) start=1;
	if (stop<0) stop=1;
	start =  pow(10,floor(std::log10(start)));
	stop =  pow(10,ceil(std::log10(stop)));
}*/



/************************************************************************************/
/*******************       plot_t::line_t              *****************************/
/************************************************************************************/

plot_t::line_t::line_t(double x_start, double y_star, double x_stop, double y_stop, std::__cxx11::string color) :
						x_start(x_start), y_start(y_star),x_stop(x_stop),y_stop(y_stop), color(color)
{
}


/************************************************************************************/
/*******************       plot_t::axis_t              ******************************/
/************************************************************************************/


void plot_t::axis_t::add_curve(const quantity_t& X, const quantity_t& Y, const std::__cxx11::string legend)
{
	logger::debug(9,"plot_t::axis_t::add_curve()","");
	if (Y.is_set() && X.is_set())
		curves.push_back({&X,&Y,legend});
}

bool plot_t::axis_t::check()
{
	logger::debug(15,"plot_t::axis_t::check()","entering");
	if (curves.size()==0)
	{
// 		logger::error("plot_t::axis_t::check()","no curves","","false");
		return false;
	}
	for (auto& c : curves)
	{
		if (c.X.name()!=curves.front().X.name())
		{
			logger::error("plot_t::axis_t::check()","quantity name of X axis do not match for all curves",c.X.name(),"false");
			return false;
		}
		if (c.Y.name()!=curves.front().Y.name())
		{
			logger::warning(3,"plot_t::axis_t::check()","quantity name of Y axis do not match for all curves",c.Y.name());
// 			return false;
		}
		if (c.X.unit()!=curves.front().X.unit())
		{
			logger::error("plot_t::axis_t::check()","quantity unit of X axis do not match for all curves",c.X.unit().to_string(),"false");
			return false;
		}
		if (c.Y.unit()!=curves.front().Y.unit())
		{
			logger::error("plot_t::axis_t::check()","quantity unit of Y axis do not match for all curves",c.Y.unit().to_string(),"false");
			return false;
		}
	}
	logger::debug(15,"plot_t::axis_t::check()","exiting");
	return true;
}

void plot_t::axis_t::draw(mglGraph* gr, double x_origin) 
{
	logger::debug(15,"plot_t::axis_t::draw()","entering");
	if (!check()) 
	{
		logger::error("plot_t::axis_t::draw()","check returned false","","aboarting plot for this axis");
		return;
	}
	
// 	vector<const quantity_t*> Ys;
// 	for (auto& c: curves)
// 		Ys.push_back(c.Y);
// 	range_t y_range(Ys);
// 	if (log10_scale) y_range.log10();
	gr->SetRange('y',range().start,range().stop);
	
// 	if (log10_scale) gr->SetOrigin(x_origin,1);
// 	else gr->SetOrigin(x_origin);
	gr->SetOrigin(x_origin,-1);
	if (log10_scale) gr->SetFunc("","lg(y)");
	if (log10_scale) gr->Axis("!Ey");
	else gr->Axis("y",color.c_str());
	
// 	gr->Axis("y");
	stringstream y_l;
	y_l << curves.front().Y.name() << " [" << curves.front().Y.unit().to_string() << "]";
	tools::str::filter_t y_f(y_l.str());
	gr->Label('y',("#"+color+"{"+ y_f.escape_special_characters()+"}").c_str(),0);
	
	for (auto& c: curves)
	{
// 		gr->SetRange('x',x_start(),x_stop());
		mglData x(c.X.data);
// 		range_t y_range(c.Y);
// 		if (log10_scale) y_range.log10();
		mglData y(c.Y.data);
// 		gr->SetRange('y',y_range.start,y_range.stop);
// 		if (log10_scale) gr->SetFunc("","lg(y)");
// 		gr->Axis("!Ey");
		
// 		string l = "legend '"+ c.legende +" "+ c.Y->name()+"'";
		tools::str::filter_t l_f("legend '"+ c.legende +" "+ c.Y.name()+"'");
// 		gr->SetFontSize(2);
		
		gr->Plot(x,y,color.c_str(),l_f.escape_special_characters().c_str());
		
// 		if (log10_scale) gr->Puts(mglPoint(*c.X->max().data.begin()/6,*c.Y->median().data.begin()+1),c.legende.c_str(),"m");
// 		else gr->Puts(mglPoint(*c.X->max().data.begin()/6,*c.Y->median().data.begin()),c.legende.c_str(),"c");
		
		if (c.Y.data.size()<11) continue;
		int y_offset=0;
		if (log10_scale)
			y_offset = 1;
		gr->SetFontSize(2);
		gr->Puts(mglPoint(c.X.data.at(c.Y.data.size()*0.05),c.Y.data.at(c.Y.data.size()*0.05)+y_offset),c.legende.c_str(),"m");
		gr->SetFontSize(3); // default
		
	}
	logger::debug(15,"plot_t::axis_t::draw()","exiting");
}

plot_t::axis_t::range_t plot_t::axis_t::range()
{
	vector<const quantity_t*> Ys;
	for (auto& c: curves)
		Ys.push_back(&c.Y);
	range_t range(Ys);
	if (log10_scale) range.log10();
	return range;
}


/***********************************************/
/*******    plot_t::axis_t::range_t    *********/
/***********************************************/
void plot_t::axis_t::range_t::log10()
{
	if (start<0) start=1;
	if (stop<0) stop=1;
	start =  pow(10,floor(std::log10(start)));
	stop =  pow(10,ceil(std::log10(stop)));
}

plot_t::axis_t::range_t::range_t(const quantity_t* Ys)
{
	vector<const quantity_t*> X {Ys};
	*this = range_t(X);
}

plot_t::axis_t::range_t::range_t(const vector<const quantity_t *> Ys)
{
	logger::debug(15,"plot_t::axis_t::range_t::range_t()","entering");
	double temp=-1;
	for (auto& y : Ys)
	{
		if (!y->is_set())
		{
			logger::error("plot_t::axis_t::range_t::range_t()","!y->is_set()",y->to_string(),"skipping");
			continue;
		}
		temp = statistics::get_max_from_Y(y->data);
		if (stop==-1 || temp > stop)
			stop = temp;
		temp = statistics::get_min_from_Y(y->data);
		if (start==-1 || temp < start)
			start = temp;
		if (y->name() != Ys.front()->name())
			logger::warning(3,"plot_t::axis_range_t::axis_range_t()","Y has elements with different quantity names",y->to_string());
		
		if (y->unit() != Ys.front()->unit())
			logger::error("plot_t::axis_range_t::axis_range_t()","Y has elements with different quantity units",y->to_string(),"ignoring");
	}
	logger::debug(15,"plot_t::axis_t::range_t::range_t()","exiting");
}



/***********************************************/
/*******    plot_t::axis_t::curve_t    *********/
/***********************************************/
plot_t::axis_t::curve_t::curve_t(const quantity_t* X, const quantity_t* Y, const std::__cxx11::string legende) : X(*X),Y(*Y),legende(legende)
{
}

// double plot_t::axis_t::curve_t::x_stop()
// {
// 	return statistics::get_max_from_Y(X->data);
// }
// double plot_t::axis_t::curve_t::x_start()
// {
// 	return statistics::get_min_from_Y(X->data);
// }
// double plot_t::axis_t::curve_t::y_start()
// {
// 	return statistics::get_min_from_Y(Y->data);
// }
// double plot_t::axis_t::curve_t::y_stop()
// {
// 	return statistics::get_max_from_Y(Y->data);
// }
// 



