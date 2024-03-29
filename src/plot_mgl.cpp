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

plot_t::plot_t(bool Y1_log10, bool Y2_log10, bool Y3_log10) : logger(__FILE__,"plot_t")
{
	Y1.log10_scale= Y1_log10;
	Y2.log10_scale= Y2_log10;
	Y3.log10_scale= Y3_log10;
	Y1.color = "G";
	Y2.color = "B";
	Y3.color = "R";
}


int plot_t::Draw(mglGraph* gr)
{
	if (windows.size()!=0)
	{
		int x_n = ceil((double)sqrt(windows.size()));
		int y_n = ceil((double)windows.size() / x_n );
// 		std::cout << std::endl << "windows.size: " << windows.size() << std::endl;
// 		std::cout << "x_n: " << x_n << std::endl;
// 		std::cout << "y_n: " << y_n << std::endl;
		// create sub-plots
		for (int i=0; i < windows.size(); i++)
		{
			gr->SubPlot(x_n,y_n,i);
			windows.at(i).draw(gr);
		}
// 		gr->Legend(0.5,1.04,"A-","value 0.004");
// 		gr->Legend();
		return 0;
	}
	/*this is the old version*/
	tools::str::filter_t filter;
    //logger::debug(15,"plot_t::Draw()","entering");
	/*x axis*/
	std::vector<const quantity::quantity_t*> Xs;
	for (auto& c:Y1.curves)
		Xs.push_back(&c.XY.X());
	for (auto& c:Y2.curves)
		Xs.push_back(&c.XY.X());
	for (auto& c:Y3.curves)
		Xs.push_back(&c.XY.X());
	
	for (auto& c:Y1.points)
		Xs.push_back(&c.XY.X());
	for (auto& c:Y2.points)
		Xs.push_back(&c.XY.X());
	for (auto& c:Y3.points)
		Xs.push_back(&c.XY.X());
	
	if (Xs.size()==0)
	{
        //logger::error("plot_t::Draw()","no X axis","","returning 0");
		return 0;
	}

	axis_t::range_t X_range(Xs);
	
	gr->SetFontSize(3);
	gr->SetRange('x',X_range.start,X_range.stop);
	gr->SetOrigin(0,-1);
	gr->Axis("x");

	std::stringstream x_l;
	if (Xs.front()->is_relative())
		x_l << "rel. ";
	x_l << Xs.front()->name() << " [" << Xs.front()->unit().to_string() << "]";
	//will escape special characters
	gr->Label('x',filter.escape_special_characters(x_l.str()).c_str(),0);
	/********/
	
	if (Y1.check())
		Y1.draw(gr,X_range.start);
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

	gr->Legend(0.5,1.04,"A-","value 0.04");
// 	gr->Legend();
    //logger::debug(15,"plot_t::Draw()","exiting");
	return 0;
}

void plot_t::axis_t::add_line(double x_start, double y_start, double x_stop, double y_stop, std::string color, std::string text)
{
	lines.push_back({x_start,y_start,x_stop,y_stop,color,text});
}

void plot_t::axis_t::add_arrow(double x_start, double y_start, double x_stop, double y_stop, std::string color, std::string text)
{
	lines.push_back({x_start,y_start,x_stop,y_stop,color,text});
}


void plot_t::to_screen(const std::string window_title, double sleep_sec)
{
// 	std::cout << "window_title\t" << window_title << std::endl;
	mglFLTK gr(this,window_title.c_str());
// 	gr.Run();
	if (sleep_sec==0)
		gr.Run();
	else
	{
		gr.RunThr();
#ifdef __unix__
		usleep(sleep_sec*1000*1000);
#else
		Sleep(sleep_sec*1000);
#endif
	}
}

void plot_t::to_file(const std::string filename)
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

plot_t::axis_t::line_t::line_t(double x_start, double y_star, double x_stop, double y_stop, std::string color, std::string text) :
                        x_start(x_start), y_start(y_star),x_stop(x_stop),y_stop(y_stop), color(color), text(text),
                        logger(__FILE__,"plot_t::axis_t::line_t")
{
}


/************************************************************************************/
/*******************       plot_t::axis_t              ******************************/
/************************************************************************************/

void plot_t::axis_t::add_curve(const quantity::map_t& XY, const std::string legend)
{
	add_curve(XY.X(),XY.Y(),legend);
}

void plot_t::axis_t::add_polynom(const quantity::map_t& XY, const fit_functions::polynom_t& polynom_s, const std::string legend, const std::string color, const unsigned int  points)
{
	const quantity::quantity_t new_X=XY.X().change_resolution(points) ;
	add_points(XY.polyfit(polynom_s,new_X),legend, color);
}

void plot_t::axis_t::add_points(const quantity::map_t& XY, const std::string legend, const std::string color)
{
	if (XY.Y().is_set() && XY.X().is_set())
		points.push_back({XY,legend,color});
// 	add_points(XY.X(),XY.Y(),legend,color);
}
void plot_t::axis_t::add_points(const quantity::quantity_t& X, const quantity::quantity_t& Y, const std::string legend, const std::string color)
{
    //logger::debug(15,"plot_t::axis_t::add_points()","entering");
	if (Y.is_set() && X.is_set())
		points.push_back({quantity::map_t(X,Y),legend,color});
}

void plot_t::axis_t::add_curve(const std::vector<double>& X, const std::vector<double>& Y, const std::string legend)
{
	quantity::quantity_t Xq ("X",X,0);
	quantity::quantity_t Yq ("Y",Y,0);
	add_curve(Xq,Yq,legend);
}

void plot_t::axis_t::add_curve(const quantity::quantity_t& X, const quantity::quantity_t& Y, const std::string legend)
{
    //logger::debug(15,"plot_t::axis_t::add_curve()","entering");
	if (Y.is_set() && X.is_set())
		curves.push_back({quantity::map_t(X,Y),legend});
}



bool plot_t::axis_t::check()
{
    //logger::debug(15,"plot_t::axis_t::check()","entering");
	if (curves.size()==0)
	{
// 		logger::error("plot_t::axis_t::check()","no curves","","false");
		return true;
	}
	for (auto& c : curves)
	{
		if (c.XY.X().name()!=curves.front().XY.X().name())
		{
            //logger::error("plot_t::axis_t::check()","quantity name of X axis do not match for all curves",c.XY.X().name() + " vs " + curves.front().XY.X().name(),"false");
			return false;
		}
		if (c.XY.X().unit()!=curves.front().XY.X().unit())
		{
            //logger::error("plot_t::axis_t::check()","quantity unit of X axis do not match for all curves",c.XY.X().unit().to_string(),"false");
			return false;
		}
		if (c.XY.Y().unit()!=curves.front().XY.Y().unit())
		{
            //logger::error("plot_t::axis_t::check()","quantity unit of Y axis do not match for all curves",c.XY.Y().unit().to_string(),"false");
			return false;
		}
		if (c.XY.Y().name()!=curves.front().XY.Y().name())
		{
            //logger::warning(3,"plot_t::axis_t::check()","quantity name of Y axis do not match for all curves",c.XY.Y().name());
// 			return false;
		}
	}
    //logger::debug(15,"plot_t::axis_t::check()","exiting");
	return true;
}



// std::string plot_t::axis_t::unit_string(const unit_t& unit)
// {
// 	const std::vector<std::string> units = {"at%","at/ccm","nm","s"};
// 	for (auto& u : units)
// 		if (unit == unit_t(u))
// 			return u;
// 	return unit.to_string(); //any fitting unit std::string will do
// }
// 
// std::string plot_t::axis_t::unit_string(quantity::quantity_t& quantity)
// {
// 	const std::vector<std::string> units = {"at%","at/ccm","nm","s"};
// 	for (auto& u : units)
// 		if (unit == unit_t(u))
// 			return u;
// 	return quantity.unit().to_string(); //any fitting unit std::string will do
// }

void plot_t::axis_t::draw(mglGraph* gr, double x_origin) 
{
	tools::str::filter_t filter;
    //logger::debug(15,"plot_t::axis_t::draw()","curves: " + tools::to_string(curves.size()),"points: " + tools::to_string(points.size()),"entering");
	if (!check()) 
	{
        //logger::error("plot_t::axis_t::draw()","check returned false","","aboarting plot for this axis");
		return;
	}
	if (curves.size() == 0 && points.size() == 0)
		return;
	gr->SetRange('y',range().start,range().stop);
// 	std::cout << range().to_string() << std::endl;
	gr->SetOrigin(x_origin,-1);

	if (log10_scale) gr->SetFunc("","lg(y)");
	else gr->SetFunc("","");
	if (log10_scale) gr->Axis("!Ey");
	else gr->Axis("y",color.c_str());
	
	std::stringstream y_l;
	y_l.str("");;
	std::string y_name="", y_unit="", y_rel="";
	if (curves.size()>0)
	{
		y_name =curves.front().XY.Y().name();
		y_unit = curves.front().XY.Y().unit().to_string();
		if (curves.front().XY.Y().is_relative())
			y_rel = "rel. ";
	}
	else if (points.size()>0)
	{
		y_name =points.front().XY.Y().name();
		y_unit = points.front().XY.Y().unit().to_string();
		if (points.front().XY.Y().is_relative())
			y_rel = "rel. ";
	}
	y_l << y_rel << y_name << " [" << y_unit << "]";
// 	gr->Legend();
	gr->Label('y',("#"+color+"{"+ filter.escape_special_characters(y_l.str())+"}").c_str(),0);
	
	for (auto& c: points)
	{
		mglData x(c.XY.X().data());
		mglData y(c.XY.Y().data());
		gr->Plot(x,y,c.color.c_str(),filter.escape_special_characters("legend '"+ c.legende +"'").c_str()); 
// 		std::cout << std::endl << filter.escape_special_characters("legend '"+ c.legende +"'").c_str() << std::endl;
// 		gr->Plot(x,y,c.color.c_str()); 
	}
	int legends_above_curves = 0;
	for (auto& c: curves)
	{
		if (!c.XY.is_set())
			continue;
		mglData x(c.XY.X().data());
		mglData y(c.XY.Y().data());
		gr->Plot(x,y,color.c_str(),("legend '"+ filter.escape_special_characters(c.legende) +"'").c_str());
// 		gr->Plot(x,y,color.c_str());
// 		if (c.legende.length()>5)
// 			continue;
		if (c.XY.Y().data().size()<11) 
			continue;
		int y_offset=0;
		if (log10_scale)
			y_offset = 1;
		gr->SetFontSize(2);
		/// write legend above the curve
// 		gr->Puts(mglPoint(c.XY.X().data().at(c.XY.Y().data().size()*0.1)*0.9/legends_above_curves,c.XY.Y().data().at(c.XY.Y().data().size()*0.05)+y_offset),filter.escape_special_characters(c.legende).c_str(),"m");
		int x_idx = c.XY.X().data().size()/ (curves.size()+2 )*(legends_above_curves+1);
		
		gr->Puts(mglPoint(c.XY.X().at(x_idx).data().front(),c.XY.Y().data().at(x_idx)+y_offset),filter.escape_special_characters(c.legende).c_str(),"m");
		legends_above_curves++;
// 		gr->Puts(mglPoint(c.XY.X().data().at(c.XY.Y().data().size()*0.1),c.XY.Y().data().at(c.XY.Y().data().size()*0.05)+y_offset),filter.escape_special_characters(c.legende).c_str(),"m");
		gr->SetFontSize(3); // default
	}
	for (auto& line : lines)
	{
		gr->Line( mglPoint(line.x_start,line.y_start), mglPoint(line.x_stop,line.y_stop),line.color.c_str());
		if (line.text!="") // add text box below start
		{
			gr->SetFontSize(2);
// 			gr->Puts(mglPoint(range().start,line.y_start), mglPoint(range().stop,line.y_start), line.text.c_str());
			gr->Puts(mglPoint(line.x_start,line.y_start), mglPoint(line.x_stop,line.y_stop), line.text.c_str());
			gr->SetFontSize(3);
		}
	}
// 	for (auto& polynom : polynoms)
// 	{
// 		if (!polynom.successfully_fitted())
// 			continue;
// 		unsigned int number_of_points = 100;
// 		std::vector<double> x_data(number_of_points);
// 		double res = (range().stop - range().start) / (number_of_points-1); // X achse nicht Y!!
// 		for (int i=0;i<x_data.size();i++)
// 			x_data.at(i) = i * res + range().start;
// 	}
// 	gr->SetSize(1000,1000);
    //logger::debug(15,"plot_t::axis_t::draw()","exiting");
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
	
	std::vector<const quantity::quantity_t*> Ys;
	for (auto& c: curves)
		Ys.push_back(&c.XY.Y());
	for (auto& c: points)
		Ys.push_back(&c.XY.Y());
	range_t range(Ys);
	if (log10_scale) 
		return range.log10();
	
	return range;
}

void plot_t::axis_t::range(double start_s, double stop_s, bool log10_scale_s)
{
	start = start_s;
	stop = stop_s;
	log10_scale = log10_scale_s;
}

void plot_t::axis_t::add_curve(const quantity::quantity_t& Y, const std::string legend)
{
	quantity::quantity_t X("idx", Y.data_X_1D(),units::SI::one);
	add_curve(X,Y,legend);
}

/***********************************************/
/*******    plot_t::axis_t::range_t    *********/
/***********************************************/

std::string plot_t::axis_t::range_t::to_string() const
{
	std::stringstream out;
	out << "start= " << start << "\tstop= " << stop << std::endl;
	return out.str();
}


plot_t::axis_t::range_t plot_t::axis_t::range_t::log10() const
{
    //logger::debug(15,"plot_t::axis_t::range_t plot_t::axis_t::range_t::log10()","");
	double log_start, log_stop;
	if (start<0) 
		log_start = 1;
	else 
		log_start =  pow(10,floor(std::log10(start)));
	if (stop<0)
	{
        //logger::error("plot_t::axis_t::range_t::log10()","stop range below 0",tools::to_string(stop),"using 1");
		log_stop = 1;
	}
	else
		log_stop =  pow(10,ceil(std::log10(stop)));
	return range_t(log_start,log_stop);
}

plot_t::axis_t::range_t::range_t(double start, double stop) : start(start), stop(stop), backgound_minimum_p(-1), logger(__FILE__,"plot_t::axis_t::range_t")
{
}


plot_t::axis_t::range_t::range_t(const quantity::quantity_t* Ys) : logger(__FILE__,"plot_t::axis_t::range_t")
{
	std::vector<const quantity::quantity_t*> X {Ys};
	*this = range_t(X);
}

plot_t::axis_t::range_t::range_t(const std::vector<const quantity::quantity_t *> Ys) : backgound_minimum_p(backgound_minimum_c(Ys)), logger(__FILE__,"plot_t::axis_t::range_t")
{
    //logger::debug(15,"plot_t::axis_t::range_t::range_t()","entering");

	if (Ys.size()==0)
		return;
	quantity::quantity_t mins_maxs;
	for (auto& y : Ys)
	{
		auto yr = y->remove_inf().remove_nan();
		if (mins_maxs.is_set() && (yr.name() != mins_maxs.name()))
        {
            //logger::warning(3,"plot_t::axis_range_t::axis_range_t()","Y has elements with different quantity names",yr.to_string(),mins_maxs.to_string());
        }
		
		if (mins_maxs.is_set() && (yr.unit() != mins_maxs.unit()))
        {
            //logger::error("plot_t::axis_range_t::axis_range_t()","Y has elements with different quantity units",yr.to_string(),mins_maxs.to_string());
        }
		mins_maxs << yr.max();
		mins_maxs << yr.min();
		
	}
	auto start = mins_maxs.min().data().at(0);
	auto stop = mins_maxs.max().data().at(0);
	
	///nur 1 punkt plotten
	if (start==stop)
	{
		if (start<0)
		{
// 			*this = range_t(2*start,0);
			start = 2*start;
			stop = 0;
		}
		else
		{
// 			*this = range_t(0,2*start);
			stop = 2*stop;
			start = 0;
		}
	}
	else // bereich etwas vergrößern
	{
		
		auto delta = stop-start;
// 		stop = stop+0.05*delta;
		start = start*0.95;
		if (start<2)
			start = start-0.05*delta;
		stop = stop * 1.05;
	}
    //logger::debug(12,"plot_t::axis_t::range_t::range_t()",Ys.front()->to_string(),"start=" + std::to_string(start) + " stop=" + std::to_string(stop));
	*this = range_t(start,stop);
    //logger::debug(15,"plot_t::axis_t::range_t::range_t()","exiting");
}

double plot_t::axis_t::range_t::range_t::backgound_minimum_c(const std::vector<const quantity::quantity_t*> Ys) const
{
	quantity::quantity_t QB;
	for (auto& Q : Ys)
	{
		if (Q->data().size()>100)
			QB << Q->get_data_by_index(Q->data().size()-21,Q->data().size()-1).mean();
	}
	if (QB.is_set())
		return QB.min().data().front();
	return -1;
}

double plot_t::axis_t::range_t::range_t::background_minimum() const
{
	return backgound_minimum_p;
}
/***********************************************/
/*******    plot_t::axis_t::points_t    ********/
/***********************************************/
// plot_t::axis_t::points_t::points_t(const quantity::quantity_t* X, const quantity::quantity_t* Y, const std::string legende, const std::string color) : X(*X),Y(*Y),legende(legende), color(color)
// {
// }

plot_t::axis_t::points_t::points_t(const quantity::map_t& XY, const std::string legende, const std::string color)
    : XY(XY.remove_inf()), legende(legende), color(color),
      logger(__FILE__,"plot_t::axis_t::points_t")
{
}


/****************************************/
/********  plot_t::window_t  ************/
/****************************************/


void plot_t::window_t::draw(mglGraph* gr)
{
	/*this is the old version*/
	tools::str::filter_t filter;
    //logger::debug(15,"plot_t::Draw()","entering");
	/*x axis*/
	std::vector<const quantity::quantity_t*> Xs;
	for (auto& c:Y1.curves)
		Xs.push_back(&c.XY.X());
	for (auto& c:Y2.curves)
		Xs.push_back(&c.XY.X());
	for (auto& c:Y3.curves)
		Xs.push_back(&c.XY.X());
	
	for (auto& c:Y1.points)
		Xs.push_back(&c.XY.X());
	for (auto& c:Y2.points)
		Xs.push_back(&c.XY.X());
	for (auto& c:Y3.points)
		Xs.push_back(&c.XY.X());
	
	if (Xs.size()==0)
	{
        //logger::error("plot_t::Draw()","no X axis","","returning 0");
		return ;
	}
	
// 	gr->Title(title.c_str());
	axis_t::range_t X_range(Xs);
	
	gr->SetFontSize(3);
	gr->SetRange('x',X_range.start,X_range.stop);
	gr->SetOrigin(0,-1);
	gr->Axis("x");

	std::stringstream x_l;
	x_l.str("");
	if (Xs.front()->is_relative())
		x_l << "rel. ";
	x_l << Xs.front()->name() << " [" << Xs.front()->unit().to_string() << "]";
	//will escape special characters
	gr->Label('x',filter.escape_special_characters(x_l.str()).c_str(),0);
	/********/
	gr->ClearLegend();
	if (Y1.check())
		Y1.draw(gr,X_range.start);
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

	gr->Legend(0.5,1.25,"-","value 0.04");
// 	gr->Box();
// 	gr->Legend(2);
    //logger::debug(15,"plot_t::Draw()","exiting");
}


