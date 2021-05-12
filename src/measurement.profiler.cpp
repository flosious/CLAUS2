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

#include "measurement.hpp"


measurements_::profiler_t::profiler_t(files_::profiler_t&  file, std::__cxx11::list< sample_t >& samples_list) : 
			measurement_t(file.name,file.contents,samples_list,"profiler"), primary_method_p(file.name.method())
{
	linescan = file.contents.linescan();
	total_sputter_depths = file.name.total_sputter_depths();
}

bool files_::profiler_t::operator<(profiler_t& obj)
{
	if (name.filename() < obj.name.filename()) return true;
	if (name.filename() > obj.name.filename()) return false;
	
	return false;
}

const string& measurements_::profiler_t::primary_method()
{
	return primary_method_p;
}


int measurements_::profiler_t::Draw(mglGraph* gr)
{
	if (linescan.z.data.size()!=linescan.xy.data.size())
	{
		logger::error("measurements_::profiler_t::Draw()", "linescan.z.data.size()!=linescan.xy.data.size()",tools::to_string(linescan.z.data.size()) +"!=" + tools::to_string(linescan.z.data.size()),"skipping");
		return 0;
	}
	int data_points=linescan.z.data.size();
	stringstream ss;
	string l;
	mglData y(data_points);
	mglData x(data_points);
		
	
	gr->SetRange('x',0,statistics::get_max_from_Y(linescan.xy.data));
	x.Set(linescan.xy.data);
	l = "XY"+linescan.xy.unit().to_string();
	gr->Label('x',l.c_str(),0);
			
	y.Set(linescan.z.data);
	gr->SetRange('y',statistics::get_min_from_Y(linescan.z.data),statistics::get_max_from_Y(linescan.z.data));
// 	gr->SetFunc("","lg(y)");
// 	gr->Axis("x");
// 	gr->Axis("y");
	gr->Axis();
	l = "z"+linescan.z.unit().to_string() ;
	gr->Label('y',l.c_str(),0);
// 	string l = "legend '"+C.to_string()+" intensity'";
// 	gr->Plot(y,"k",l.c_str());
	gr->Plot(y,"B");
	
	/*filter_impulse(0,2)*/
	y.Set(linescan.z.filter_impulse(0.1*linescan.z.data.size(),2).data);
	gr->Plot(y,"G");
	
	if (linescan.fit().is_set())
	{
		y.Set(linescan.fit().data);
		gr->Plot(y,"R");
		ss.str("");
		if (linescan.depth().is_set())
			ss << "depth = " << round(linescan.depth().data.at(0)) << linescan.depth().unit().to_string();
		gr->AddLegend(ss.str().c_str(),"");
		ss.str("");
		ss << "gof = " << round(linescan.fit_params().gof()*10)/10;
// 		ss << "gof = " << pow(statistics::get_correlation_Y1_Y2(linescan.z.filter_impulse(0,2).data,linescan.fit().data),100);
		gr->AddLegend(ss.str().c_str(),"");
// 		ss.str("");
// 		ss << "chisq = " << linescan.fit_params().chisq();
// 		gr->AddLegend(ss.str().c_str(),"");
	}
	
// 	string title = "\n"+to_string();
// 	gr->Title(title.c_str(),"",3);
	gr->Grid();
	gr->Box();
	
	gr->Legend(1,0.5); /*gr->Puts(mglPoint(0,statistics::get_max_from_Y(linescan.z.data)/2),"TEST","a");*/
// 	gr->Legend(0.5,1,"^#");
	return 0;
}

void measurements_::profiler_t::plot_now(double sleep_sec)
{
	mglFLTK gr(this,to_string().c_str());
	
	if (sleep_sec==0)
		gr.Run();
	else
	{
// 		gr.RunThr();
		usleep(sleep_sec*1000*1000);
	}
}
