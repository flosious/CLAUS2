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




measurements_::sims_t::sims_t(files_::sims_t::name_t& filename, 
							  files_::sims_t::contents_t& filecontents, 
							  list<sample_t>& samples_list, 
							  string method,
							  vector<files_::jpg_t>* jpg_files,
							  vector<files_::profiler_t>* profiler_files) : 
									measurement_t(filename,filecontents,samples_list,method)
{
	clusters = filecontents.clusters();
	
	bool found_any=false;
	/*JPEG files*/
	if (jpg_files!=nullptr)
	{
		sort(jpg_files->begin(),jpg_files->end());
		found_any=false;
		for (int i=jpg_files->size()-1;i>=0;i--)
		{
			if ((jpg_files->at(i).name.method()==method || jpg_files->at(i).name.method()=="") && 
					filename != jpg_files->at(i).name)
			{
				if (!found_any) continue;
				else break;
			}
			found_any = true;
			/*save essentials*/
			crater.total_sputter_depths << jpg_files->at(i).name.total_sputter_depths();
			/*delete*/
			jpg_files->erase(jpg_files->begin()+i);
		}
	}
	
	/*PROFILER files*/
	if (profiler_files!=nullptr)
	{
		sort(profiler_files->begin(),profiler_files->end());
		found_any=false;
		for (int i=profiler_files->size()-1;i>=0;i--)
		{
			if ((profiler_files->at(i).name.method()==method || profiler_files->at(i).name.method()=="") && 
					filename != profiler_files->at(i).name)
			{
				if (!found_any) continue;
				else break;
			}
			found_any = true;
			/*save essentials*/
			crater.total_sputter_depths << profiler_files->at(i).name.total_sputter_depths();
			crater.linescans.push_back(profiler_files->at(i).contents.linescan());
			/*delete*/
			profiler_files->erase(profiler_files->begin()+i);
		}
	}
}

measurements_::sims_t::sims_t(files_::sims_t::name_t& filename, list<sample_t>& samples_list, string method) : measurement_t(filename,samples_list,method)
{
	crater.total_sputter_depths = filename.total_sputter_depths();
}



std::__cxx11::string measurements_::sims_t::to_string(const std::__cxx11::string del) const
{
	stringstream ss;
	ss << measurement_t::to_string() << del;
	ss << "crater: ";
	ss << crater.total_sputter_depths.to_string() << del;
	ss << "linescans: <" << crater.linescans.size() <<">" << del;
	ss << "clusters: <" << clusters.size() << ">" << del;
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
// 			gr->AddLegend(,"k");
	// 		gr->SetOrigin(0,0,0);
			gr->Axis("x");
			gr->Axis("!Ey");
	// 		gr->Label(y,"intensity T","E"); -> labes each data point
			gr->Label('y',"intensity",0);
	// 		gr->Plot(x,y);
			string l = "legend '"+C.to_string()+" intensity'";
			gr->Plot(y,"k",l.c_str());
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
// 			gr->AddLegend(C.to_string().c_str(),"k");
			gr->SetOrigin(statistics::get_max_from_Y(C.sputter_time().data),0);
			
			gr->Axis("!yUE","B");
	// 		gr->SetFunc("","lg(y)");
			gr->Label('y',"#B{concentration}",0);
	// 		gr->SetTickShift(mglPoint(100,10,0,0)); // does not work?
			string l = "legend '"+C.to_string()+" concentration'";
			gr->Plot(y,"B",l.c_str());
		}
	}
	string title = "\n"+to_string();
// 	gr->Title(title.c_str(),"",3);
	gr->Grid();
	gr->Box();
	gr->Legend(3,"^#");
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

/*"30Si" can be a reference cluster for elemental Si or isotopical purified Si*/
/*"30Si 28Si" can be a reference cluster for elemental Si or isotopical purified Si*/
/*"74Ge 28Si" can be a reference cluster for elemental Si+Ge or isotopical purified Si+Ge*/
/// reference_cluster: does NOT contain isotopes, which are NOT found in matrix->isotopes
set<cluster_t*> measurements_::sims_t::reference_clusters()
{
	set<cluster_t*> ref_clusters;
	if (reference_cluster!=nullptr)
		ref_clusters.insert(reference_cluster);
	if (!sample->matrix().is_set())
		return ref_clusters;
	
// 	set<string> element_symbols_in_matrix;
// 	for (auto& I : sample->matrix().isotopes)
// 		element_symbols_in_matrix.insert(I.symbol);
	
	bool insert=false;
	//
	for (auto& C : clusters)
	{
		insert = true;
		for (auto& I : C.isotopes)
			if (find(sample->matrix().isotopes.begin(),sample->matrix().isotopes.end(),I)==sample->matrix().isotopes.end())
			{
				insert=false;
				break;
			}
		if (insert) ref_clusters.insert(&C);
	}
	
	return ref_clusters;
}

// bool measurements_::sims_t::operator==(measurements_::sims_t& obj)
// {
// 	if (measurement_t::operator!=(obj)) return false;
// 	if (settings!=nullptr && obj.settings!=nullptr)
// 		if (*settings!=*obj.settings) return false;
// 	return true;
// }
// 
// bool measurements_::sims_t::operator!=(measurements_::sims_t& obj)
// {
// 	return !operator==(obj);
// }
// 
// bool measurements_::sims_t::operator<(measurements_::sims_t& obj)
// {
// 	if (measurement_t::operator<(obj)) return true;
// 	if (*this == obj && settings!=nullptr && obj.settings!=nullptr)
// 		if (settings < obj.settings) return true;
// 	return false;
// }
// 
// 
// 
