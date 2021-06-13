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
	//will escape special characters
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
	
	
	
// 	gr->Grid();
// 	gr->Box();
// 	gr->Legend(0.49999,1.05,"^-");
	gr->Legend(0.5,1.04,"A-","value 0.04");
	logger::debug(15,"plot_t::Draw()","exiting");
	return 0;
}

void plot_t::axis_t::add_line(double x_start, double y_start, double x_stop, double y_stop, std::__cxx11::string color, string text)
{
	lines.push_back({x_start,y_start,x_stop,y_stop,color,text});
}

void plot_t::axis_t::add_arrow(double x_start, double y_start, double x_stop, double y_stop, std::__cxx11::string color, string text)
{
	lines.push_back({x_start,y_start,x_stop,y_stop,color,text});
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



/************************************************************************************/
/*******************       plot_t::line_t              *****************************/
/************************************************************************************/

plot_t::axis_t::line_t::line_t(double x_start, double y_star, double x_stop, double y_stop, std::__cxx11::string color, string text) :
						x_start(x_start), y_start(y_star),x_stop(x_stop),y_stop(y_stop), color(color), text(text)
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
	gr->SetRange('y',range().start,range().stop);
	gr->SetOrigin(x_origin,-1);
	if (log10_scale) gr->SetFunc("","lg(y)");
	if (log10_scale) gr->Axis("!Ey");
	else gr->Axis("y",color.c_str());
	
	stringstream y_l;
	y_l << curves.front().Y.name() << " [" << curves.front().Y.unit().to_string() << "]";
	tools::str::filter_t y_f(y_l.str());
	gr->Label('y',("#"+color+"{"+ y_f.escape_special_characters()+"}").c_str(),0);
	
	for (auto& c: curves)
	{
		mglData x(c.X.data);
		mglData y(c.Y.data);
		tools::str::filter_t l_f("legend '"+ c.legende +" "+ c.Y.name()+"'");
		gr->Plot(x,y,color.c_str(),l_f.escape_special_characters().c_str());
		if (c.Y.data.size()<11) continue;
		int y_offset=0;
		if (log10_scale)
			y_offset = 1;
		gr->SetFontSize(2);
		gr->Puts(mglPoint(c.X.data.at(c.Y.data.size()*0.05),c.Y.data.at(c.Y.data.size()*0.05)+y_offset),c.legende.c_str(),"m");
		gr->SetFontSize(3); // default
	}
	for (auto& line : lines)
	{
		gr->Line( mglPoint(line.x_start,line.y_start), mglPoint(line.x_stop,line.y_stop),line.color.c_str());
		if (line.text!="") // add text box below start
		{
			gr->SetFontSize(2);
			gr->Puts(mglPoint(range().start-10,line.y_start), mglPoint(range().stop,line.y_start), line.text.c_str());
// 			gr->Puts(mglPoint(line.x_start,line.y_start), mglPoint(line.x_stop,line.y_stop), line.text.c_str());
			gr->SetFontSize(3);
		}
	}
// 	gr->SetSize(1000,1000);
	logger::debug(15,"plot_t::axis_t::draw()","exiting");
}

plot_t::axis_t::range_t plot_t::axis_t::range()
{
	if (start!=stop)
	{
		range_t R(start,stop);
		if (log10_scale)
			return R.log10();
		return R;
	}
	
	vector<const quantity_t*> Ys;
	for (auto& c: curves)
		Ys.push_back(&c.Y);
	range_t range(Ys);
	if (log10_scale) 
	{
		return range.log10();
	}
	return range;
}

void plot_t::axis_t::range(double start_s, double stop_s, bool log10_scale_s)
{
	start = start_s;
	stop = stop_s;
	log10_scale = log10_scale_s;
}


/***********************************************/
/*******    plot_t::axis_t::range_t    *********/
/***********************************************/

std::__cxx11::string plot_t::axis_t::range_t::to_string() const
{
	stringstream out;
	out << "start= " << start << "\tstop= " << stop << endl;
	return out.str();
}


plot_t::axis_t::range_t plot_t::axis_t::range_t::log10() const
{
	double log_start, log_stop;
	if (start<0) 
		log_start = 1;
	else 
		log_start =  pow(10,floor(std::log10(start)));
	if (stop<0)
	{
		logger::error("plot_t::axis_t::range_t::log10()","stop range below 0",tools::to_string(stop),"using 1");
		log_stop = 1;
	}
	else
		log_stop =  pow(10,ceil(std::log10(stop)));
	return range_t(log_start,log_stop);
}

plot_t::axis_t::range_t::range_t(double start, double stop) : start(start), stop(stop)
{
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



