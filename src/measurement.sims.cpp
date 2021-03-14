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

void measurements_::sims_t::add_clusters(vector<cluster_t>& clusters_s)
{
	cluster_t* C_p;
	for (auto& C: clusters_s)
	{
		C_p = tools::find_in_V(C,clusters_s);
		if (C_p==nullptr)
			clusters.push_back(C);
	}
}


measurements_::sims_t::sims_t(files::sims_t::name_t& filename, 
							  files::sims_t::contents_t& filecontents, 
							  list<sample_t>& samples_list, 
							  vector<files::jpg_t>* jpg_files,
							  vector<files::profiler_t>* profiler_files) : 
									measurement_t(filename,filecontents,samples_list)
{
	clusters = filecontents.clusters();
	
// 	sort(jpg_files->begin(),jpg_files->end());
// 	sort(profiler_files->begin(),profiler_files->end());
	bool found_any=false;
	
	/*JPEG files*/
	if (jpg_files!=nullptr)
	{
		found_any=false;
		for (int i=jpg_files->size()-1;i>=0;i--)
		{
			measurement_t JM(jpg_files->at(i).name,samples_list);
			if (*this != JM)
			{
				if (!found_any) continue;
				else break;
			}
			found_any = true;

			crater.total_sputter_depths << jpg_files->at(i).name.total_sputter_depths();
			jpg_files->erase(jpg_files->begin()+i);
		}
	}
	
	/*PROFILER files*/
// 	if (profiler_files!=nullptr)
// 	{
// 		for (vector<files::profiler_t>::iterator it=profiler_files->begin();it!=profiler_files->end();it++)
// 		{
// 			measurement_t PM(it->name,samples_list);
// 			if (*this != PM) continue;
// 			crater.linescans.push_back(it->contents.linescan());
// 			crater.total_sputter_depths << it->name.total_sputter_depths();
// 			profiler_files->erase(it);
// 			it--;
// 		}
// 	}
	if (profiler_files!=nullptr)
	{
		found_any=false;
		for (int i=profiler_files->size()-1;i>=0;i--)
		{
			measurement_t JM(profiler_files->at(i).name,samples_list);
			if (*this != JM)
			{
				if (!found_any) continue;
				else break;
			}
			found_any = true;
			crater.linescans.push_back(profiler_files->at(i).contents.linescan());
			crater.total_sputter_depths << profiler_files->at(i).name.total_sputter_depths();
			profiler_files->erase(profiler_files->begin()+i);
		}
	}
}

measurements_::sims_t::sims_t(files::sims_t::name_t& filename, list<sample_t>& samples_list) : measurement_t(filename,samples_list)
{
	crater.total_sputter_depths = filename.total_sputter_depths();
}



std::__cxx11::string measurements_::sims_t::to_string(const std::__cxx11::string del) const
{
	stringstream ss;
	ss << measurement_t::to_string() << del;
	ss << "crater: " << crater.total_sputter_depths.to_string() << del;
	ss << "clusters: <" << clusters.size() << ">" ;
	return ss.str();
}

void measurements_::sims_t::plot_now(double sleep_sec)
{
	mglFLTK gr(this,to_string().c_str());
	
	if (sleep_sec==0)
		gr.Run();
	else
	{
		gr.RunThr();
		usleep(sleep_sec*1000*1000);
	}
// 	getchar();
}


int measurements_::sims_t::Draw(mglGraph* gr)
{
	bool set=false;
	for (auto& C: clusters)
	{
		int data_points=0;
		if (C.sputter_depth().is_set())
			data_points = C.sputter_depth().data.size();
		else
			data_points = C.sputter_time().data.size();
		
		mglData y(data_points);
		mglData x(data_points);
		
		if (C.sputter_time().is_set() && !set)
		{
			
			gr->SetRange('x',0,statistics::get_max_from_Y(C.sputter_time().data));
	// 		gr->Axis("X");
			x.Set(C.sputter_time().data);
			gr->Label('x',"sputter time",0);
// 			gr->Plot(x);
		}
			
		if (C.intensity().is_set())
		{
			y.Set(C.intensity().data);
			
			gr->SetRange('y',1,1E6);
			gr->SetFunc("","lg(y)");
			
	// 		gr->SetOrigin(0,0,0);
			gr->Axis("x");
			gr->Axis("!Ey");
	// 		gr->Label(y,"intensity T","E"); -> labes each data point
			gr->Label('y',"intensity",0);
	// 		gr->Plot(x,y);
			gr->Plot(y,"k","legend 'intensity'");
	// 		concentration_p = concentration_t((intensity()*1E17).data);
		}
		
		if (C.sputter_time().is_set() && !set)
		{
			set=true;
// 			gr->SetRange('x',0,statistics::get_max_from_Y(C.sputter_time().data));
// 			gr->SetOrigin(0,1E6);
// 			gr->Axis("x_!");
		}
		
	// 	x.Set(sputter_time().data);
	// 	gr->Label('x',"");
			
		if (C.concentration().is_set())
		{
			y.Set(C.concentration().data);
			
			gr->SetRange('y',1E15,1E21);
			
			gr->SetOrigin(statistics::get_max_from_Y(C.sputter_time().data),0);
			
			gr->Axis("!yUE","B");
	// 		gr->SetFunc("","lg(y)");
			gr->Label('y',"#B{concentration}",0);
	// 		gr->SetTickShift(mglPoint(100,10,0,0)); // does not work?
			gr->Plot(y,"B","legend 'concentration'");
		}
	}
	string title = "\n"+to_string();
// 	gr->Title(title.c_str(),"",3);
	gr->Grid();
	gr->Box();
	
	return 0;
}

void measurements_::sims_t::export_origin_ascii(std::__cxx11::string path, const std::__cxx11::string delimiter)
{
	if (clusters.size()==0)
	{
		logger::info("measurements_::sims_t::export_origin_ascii: no clusters to export","abort");
		return;
	}
	vector<origin_t::column_t> cols;
	for (auto& C : clusters)
	{
		stringstream comment,longname;
		comment << "^" << C.to_string(" ^") << " ";
		longname << "^" << C.to_string(" ^") << " ";
		
		if (sample->simple_name!="")
			comment << sample->simple_name;
		else
			comment << sample->lot << sample->lot_split << "_w" << sample->wafer;
		
		if (C.sputter_time().is_set()) 
			cols.push_back({C.sputter_time().data,longname.str() + " sputter_time",C.sputter_time().unit().to_string(),comment.str()});
		if (C.intensity().is_set()) 
			cols.push_back({C.intensity().data,longname.str() + " intensity",C.intensity().unit().to_string(),comment.str()});
		if (C.sputter_depth().is_set())
			cols.push_back({C.sputter_depth().data,longname.str() + " sputter_depth",C.sputter_depth().unit().to_string(),comment.str()});
		if (C.concentration().is_set()) 
			cols.push_back({C.concentration().data,longname.str() + " concentration",C.concentration().unit().to_string(),comment.str()});
	}
	
	vector<vector<string>> matrix;
	for (auto& col : cols)
		matrix.push_back(col.vec());
	matrix = tools::mat::transpose_matrix(matrix);
	string out = tools::mat::format_matrix_to_string(&matrix);
	if (path!="")
	{
		path = tools::file::check_directory_string(path);
		tools::file::mkpath(path,0750);
	}
	stringstream filename;
	filename << olcdb;
	if (sample->simple_name!="")
		filename << "_" << sample->simple_name;
	else
		filename << "_" << sample->lot << sample->lot_split << "_w" << sample->wafer;
	
	if (sample->monitor!="")
		filename << "_m" << sample->monitor;
	if (sample->chip.is_set())
		filename << "_c" << sample->chip.to_string();
	if (group!="")
		filename << "_g" << group;
	if (repetition!="")
		filename << "_r" << repetition;
	filename << ".txt";
	
	tools::file::write_to_file(path+filename.str(),&out,false);
}


