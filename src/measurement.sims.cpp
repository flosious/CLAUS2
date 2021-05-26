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


/****************************************/
/* *measurements_::sims_t::equlibrium_t */
/****************************************/

measurements_::sims_t::equlibrium_t::equlibrium_t(const measurements_::sims_t& M, 
												  unsigned int& equilibrium_start_index_s) : 
												  M(M), equilibrium_start_index_s(equilibrium_start_index_s)
{
}

quantity_t & measurements_::sims_t::equlibrium_t::erase_inequilibrium_data(quantity_t& Q)
{
	if (equilibrium_start_index()>0)
		Q = Q.remove_data_by_index(0,equilibrium_start_index());
	return Q;
}

cluster_t & measurements_::sims_t::equlibrium_t::erase_inequilibrium_data(cluster_t& C)
{
	C.intensity = erase_inequilibrium_data(C.intensity);
	C.concentration = erase_inequilibrium_data(C.concentration);
	C.SF = erase_inequilibrium_data(C.SF);
	C.sputter_depth = erase_inequilibrium_data(C.sputter_depth);
	C.sputter_time = erase_inequilibrium_data(C.sputter_time);
	return C;
}

crater_t & measurements_::sims_t::equlibrium_t::erase_inequilibrium_data(crater_t& C)
{
	C.sputter_beam.sputter_current = erase_inequilibrium_data(C.sputter_beam.sputter_current);
	C.sputter_beam.sputter_depth = erase_inequilibrium_data(C.sputter_beam.sputter_depth);
	C.sputter_beam.sputter_time = erase_inequilibrium_data(C.sputter_beam.sputter_time);
	
	C.sputter_depth = erase_inequilibrium_data(C.sputter_depth);
	C.sputter_time = erase_inequilibrium_data(C.sputter_time);
	return C;
}

///TODO unvollständig!!
measurements_::sims_t measurements_::sims_t::equlibrium_t::measurement()
{
	if (equilibrium_start_index()==0)
		return M;
	sims_t M_out = M;
	for (auto& C:M_out.clusters)
	{
		if (!erase_inequilibrium_data(C).is_set())
		{
			logger::error("measurements_::sims_t::equlibrium_t::measurement()","could not delete cluster erase_inequilibrium_data",C.to_string(),"empty cluster");
			C = {};
		}
	}
	erase_inequilibrium_data(M_out.crater);
// 	M_out.crater.
	return M_out;
}

const unsigned int measurements_::sims_t::equlibrium_t::equilibrium_start_index()
{
	///use saved value
	if (equilibrium_start_index_s>0)
		return equilibrium_start_index_s;
	unsigned int temp=0;
	for (auto C:M.matrix_clusters())
	{
		temp = equilibrium_start_index(*C);
		if (temp > equilibrium_start_index_s)
			equilibrium_start_index_s=temp;
	}
	return equilibrium_start_index_s;
}

const unsigned int measurements_::sims_t::equlibrium_t::equilibrium_start_index(const cluster_t& C)
{
	unsigned int index1=0, index2=0;
	if (C.intensity.is_set())
		index1 = equilibrium_start_index(C.intensity);
	
	if (C.concentration.is_set())
		index2 = equilibrium_start_index(C.concentration);
	
	if (index1>index2)
		return index1;
	return index2;
}

/*in here is the real magic*/
const unsigned int measurements_::sims_t::equlibrium_t::equilibrium_start_index(const quantity_t& Q)
{
	cout << "measurements_::sims_t::equlibrium_t::equilibrium_start_index" << endl;
	if (!Q.is_set())
		return 0;
	return 0;
// 	double treshold = statistics::get_mad_from_Y(Y)/2;
// 	double median = statistics::get_median_from_Y(Y);
// 	
// 		set<int> extrema_idx;
// 		vector<int> maxIdx, minIdx;
// 			
// 		if (!statistics::get_extrema_indices(maxIdx,minIdx,Y,treshold))
// 		{
// 			if (is_reference()) equilibrium_starting_pos = statistics::get_index_for_next_value_within_treshold(Y,median-treshold/2,median+treshold/2,1);
// // 			else equilibrium_starting_pos = measurement->equilibrium_starting_pos();
// 			else equilibrium_starting_pos = 0;
// 		}
// 		//type C
// 		else if (minIdx.size()==0 && maxIdx.size()==1) // just the global maximum
// 		{
// // 			cout << name() << " type C2" << endl;
// 			if (is_reference()) equilibrium_starting_pos = statistics::get_index_for_next_value_within_treshold(Y,median-treshold/2,median+treshold/2,1);
// // 			else equilibrium_starting_pos = measurement->equilibrium_starting_pos();
// 			else equilibrium_starting_pos = 0;
// 		}
// 		else
// 		{
// 			/*remove right sided*/
// 			for (auto& m:maxIdx)
// 				if (m>0.5*Y.size()) m=0;
// 			for (auto& m:minIdx)
// 				if (m>0.5*Y.size()) m=0;
// 				
// 			set<int> maxIdx_set (maxIdx.begin(),maxIdx.end());
// 			set<int> minIdx_set (minIdx.begin(),minIdx.end());
// 			
// 			/*remove trivials*/
// 			maxIdx_set.erase(0);
// 			maxIdx_set.erase(1);
// 			minIdx_set.erase(0);
// 			minIdx_set.erase(1);
// 			
// // 			print("maxIdx_set: " + name());
// // 			print(maxIdx_set);
// // 			print("minIdx_set: " + name());
// // 			print(minIdx_set);
// 			
// 			//type E D
// 			if (minIdx_set.size()==0 && maxIdx_set.size()==0)
// 			{
// 
// 				if (is_reference()) equilibrium_starting_pos = statistics::get_index_for_next_value_within_treshold(Y,median-treshold/2,median+treshold/2,1);
// 
// 				else equilibrium_starting_pos = 0;
// 			}
// 			// type G H
// 			else if (minIdx_set.size()==0 && maxIdx_set.size()!=0 )
// 			{
// 				if (is_reference()) equilibrium_starting_pos = statistics::get_index_for_next_value_within_treshold(Y,median-treshold/2,median+treshold/2,*maxIdx_set.begin());
// 				else equilibrium_starting_pos = 0;
// 
// 			}
// 			else 
// 			{
// 				equilibrium_starting_pos=*minIdx_set.begin(); 
// 				/* check total signal sum for miss interpretation */
// 				double sum=0;
// 				double sum_equilibrium=0;
// 				for (int i=0;i<Y.size();i++)
// 					sum+=Y[i];
// 				for (int i=equilibrium_starting_pos;i<Y.size();i++)
// 					sum_equilibrium+=Y[i];
// 				if ((sum_equilibrium-(Y.size()-equilibrium_starting_pos)*Y.back())/(sum-Y.size()*Y.back())<0.7)
// 					equilibrium_starting_pos=0;
// 			}
// 		}
// 		
}


/************************************/
/* *measurements_::sims_t::filter_t */
/************************************/
measurements_::sims_t::filter_t::filter_t(const measurements_::sims_t& M) : M(M)
{
}

measurements_::sims_t measurements_::sims_t::filter_t::impulses()
{
	sims_t M_filtered = M;
	for (auto& C:M_filtered.clusters)
	{
		C = C.filter_impulse();
	}
	return M_filtered;
}

/***********************************/
/****** measurements_::sims_t ******/
/***********************************/

measurements_::sims_t measurements_::sims_t::sputter_equilibrium()
{
	equlibrium_t STE(*this,equilibrium_start_index_s);
	return STE.measurement();
}

measurements_::sims_t::filter_t measurements_::sims_t::filter() const
{
	return filter_t(*this); 
}

void measurements_::sims_t::add_clusters(vector<cluster_t>& clusters)
{
	cluster_t* C_p;
	for (auto& C: clusters)
	{
		C_p = tools::find_in_V(C,clusters);
		if (C_p==nullptr)
			clusters.push_back(C);
	}
}

measurements_::sims_t::sims_t(files_::sims_t::name_t& filename, 
							  files_::sims_t::contents_t& filecontents, 
							  list<sample_t>& samples_list, 
							  string method,
							  database_t& sql_wrapper,
							  vector<files_::jpg_t>* jpg_files,
							  vector<files_::profiler_t>* profiler_files) : 
									measurement_t(filename,filecontents,samples_list,method,sql_wrapper), clusters(filecontents.clusters())
{
// 	clusters = filecontents.clusters;
	
	crater.total_sputter_depths = filename.total_sputter_depths();
	
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

measurements_::sims_t::sims_t(files_::sims_t::name_t& filename, list<sample_t>& samples_list, string method,database_t& sql_wrapper) : measurement_t(filename,samples_list,method,sql_wrapper)
{
	crater.total_sputter_depths = filename.total_sputter_depths();
}



std::__cxx11::string measurements_::sims_t::to_string(const std::__cxx11::string del)
{
	logger::debug(11,"measurements_::sims_t::to_string","","","entering");
	stringstream ss;
	ss << measurement_t::to_string() << del;
	ss << "crater: ";
	ss << crater.total_sputter_depths.to_string() << del;
	ss << "linescans: <" << crater.linescans.size() <<">" << del;
	ss << "clusters: <" << clusters.size() << ">" << del;
	logger::debug(11,"measurements_::sims_t::to_string","","","exiting");
	return ss.str();
}

void measurements_::sims_t::plot_now(double sleep_sec)
{
// 	mglFLTK gr(this,to_string().c_str());
	
// 	if (sleep_sec==0)
// 		gr.Run();
// 	else
// 	{
// 		gr.RunThr();
// 		usleep(sleep_sec*1000*1000);
// 	}
	
	plot_t plot;
	quantity_t X;
	
	if (crater.sputter_time.is_set())
		X = crater.sputter_time.change_unit({"min"});
	else if (crater.sputter_depth.is_set())
		X = crater.sputter_depth.change_unit({"nm"});
	else
	{
		logger::error("measurements_::sims_t::to_string", "neither sputter_time nor sputter_depth is set","","no plot");
		return;
	}
	if (crater.sputter_current().is_set())
		plot.Y3.add_curve(X,crater.sputter_current().change_unit({"nA"}));
	for (auto& C: clusters)
	{
		if (C.intensity.is_set())
			plot.Y1.add_curve(X,C.intensity,C.to_string());
		if (C.concentration.is_set())
			plot.Y2.add_curve(X,C.concentration,C.to_string());
	}
	plot.to_screen(to_string(),sleep_sec);
}


// int measurements_::sims_t::Draw(mglGraph* gr)
// {
// 	bool set=false;
// 	double origin_y2=0;
// 	for (auto& C: clusters)
// 	{
// 		int data_points=0;
// 		
// 		if (C.sputter_depth.is_set())
// 			data_points = C.sputter_depth.data.size();
// 		else
// 			data_points = C.sputter_time.data.size();
// 		
// 		mglData y(data_points);
// 		mglData y2(data_points);
// 		mglData x(data_points);
// 		
// 		if (C.sputter_time.is_set() && !set)
// 		{
// 			origin_y2 = statistics::get_max_from_Y(C.sputter_time.data);
// // 			C.sputter_time = C.sputter_time.change_unit({"min"});
// 			gr->SetRange('x',0,origin_y2);
// 	// 		gr->Axis("X");
// 			x.Set(C.sputter_time.data);
// 			string x_label = "sputter time ["+C.sputter_time.unit().to_string()+"]";
// 			gr->Label('x',x_label.c_str(),0);
// // 			gr->Plot(x);
// 			
// 		}
// 		else if (crater.sputter_time.is_set() && !set)
// 		{
// // 			crater.sputter_time = crater.sputter_time.change_unit({"min"});
// 			origin_y2 = statistics::get_max_from_Y(crater.sputter_time.data);
// 			gr->SetRange('x',0,origin_y2);
// 			x.Set(crater.sputter_time.data);
// 			string x_label = "sputter time ["+crater.sputter_time.unit().to_string()+"]";
// 			gr->Label('x',x_label.c_str(),0);
// 			set=true;
// 		}
// 		else if (crater.sputter_depth.is_set() && !set)
// 		{
// 			origin_y2 = statistics::get_max_from_Y(crater.sputter_depth.data);
// 			gr->SetRange('x',0,origin_y2);
// 			x.Set(crater.sputter_depth.data);
// 			string x_label = "sputter depth ["+crater.sputter_depth.unit().to_string()+"]";
// 			gr->Label('x',x_label.c_str(),0);
// 			set=true;
// 		}
// 		else if (!set)
// 			logger::error("measurements_::sims_t::Draw()","sputter_time and sputter_depth not found",to_string());
// 		
// 		if (C.intensity.is_set())
// 		{
// 			y.Set(C.intensity.data);
// 			gr->SetRange('y',1,1E6);
// 			gr->SetFunc("","lg(y)");
// // 			gr->AddLegend(,"k");
// 	// 		gr->SetOrigin(0,0,0);
// 			gr->Axis("x");
// 			gr->Axis("!Ey");
// 	// 		gr->Label(y,"intensity T","E"); -> labes each data point
// 			string y_label = "intensity ["+C.intensity.unit().to_string()+"]";
// 			gr->Label('y',y_label.c_str(),0);
// 	// 		gr->Plot(x,y);
// 			string l = "legend '"+C.to_string()+" intensity'";
// 			gr->Plot(y,"k",l.c_str());
// 	// 		concentration_p = concentration_t((intensity()*1E17).data);
// 		}
// 		
// 		if (C.sputter_time.is_set() && !set)
// 		{
// 			set=true;
// // 			gr->SetRange('x',0,statistics::get_max_from_Y(C.sputter_time.data));
// // 			gr->SetOrigin(0,1E6);
// // 			gr->Axis("x_!");
// 		}
// 		
// 	// 	x.Set(sputter_time.data);
// 	// 	gr->Label('x',"");
// 			
// 		if (C.concentration.is_set())
// 		{
// 			y2.Set(C.concentration.data);
// // 			
// 			gr->SetRange('y',1E15,1E21);
// 			gr->SetFunc("","lg(y)");
// 			gr->SetOrigin(origin_y2,0);
// // 			gr->SetOrigin(statistics::get_max_from_Y(C.sputter_time.data),0);
// // 			
// 			gr->Axis("!yUE","B");
// // 			gr->Axis("y");
// 			string y_label = "#B{concentration ["+C.concentration.unit().to_string()+"]}";
// 			gr->Label('y',y_label.c_str());
// 			string l = "legend '"+C.to_string()+" concentration'";
// 			gr->Plot(y2,"B",l.c_str());
// 		}
// 	}
// 	string title = "\n"+to_string();
// // 	gr->Title(title.c_str(),"",3);
// 	gr->Grid();
// 	gr->Box();
// // 	gr->Legend(1,0,"^#");
// 	gr->Legend(0.5,1,"^#");
// 	return 0;
// }

void measurements_::sims_t::export_origin_ascii(std::__cxx11::string path, const std::__cxx11::string delimiter)
{
	if (clusters.size()==0)
	{
// 		logger::info("measurements_::sims_t::export_origin_ascii: no clusters to export","abort");
		logger::warning(1,"measurements_::sims_t::export_origin_ascii","clusters.size()==0",to_string());
		return;
	}
	cout << measurements_::sims_t::to_string() << endl;
	logger::debug(6,"measurements_::sims_t::export_origin_ascii","path",path,"starting export");
	vector<origin_t::column_t> cols;
	/*time-intensity-profile*/
	stringstream comment,longname;
	comment << "crater ";
	longname << "crater sputter_depth";
	if (sample->simple_name!="")
		comment << sample->simple_name;
	else
		comment << sample->lot << sample->lot_split << "_w" << sample->wafer_string();
	if (crater.sputter_depth.is_set())
		cols.push_back({crater.sputter_depth.data,longname.str(),crater.sputter_depth.unit().to_string(),comment.str()});
	for (auto& C : clusters)
	{
		comment.str("");
		longname.str("");
		comment << "^" << C.to_string(" ^") << " ";
		longname << "^" << C.to_string(" ^") << " ";
		
		if (sample->simple_name!="")
			comment << sample->simple_name;
		else
			comment << sample->lot << sample->lot_split << "_w" << sample->wafer_string();
		if (C.concentration.is_set()) 
			cols.push_back({C.concentration.data,longname.str() + " concentration",C.concentration.unit().to_string(),comment.str()});
	}
	comment.str("");
	longname.str("");
	comment << "crater ";
	longname << "crater sputter_time";
	if (sample->simple_name!="")
		comment << sample->simple_name;
	else
		comment << sample->lot << sample->lot_split << "_w" << sample->wafer_string();
	if (crater.sputter_time.is_set())
	{
		
		cols.push_back({crater.sputter_time.data,longname.str(),crater.sputter_time.unit().to_string(),comment.str()});
	}
	for (auto& C : clusters)
	{
		ostringstream comment,longname;
		comment << "^" << C.to_string(" ^") << " ";
		longname << "^" << C.to_string(" ^") << " ";
		
		if (sample->simple_name!="")
			comment << sample->simple_name;
		else
			comment << sample->lot << sample->lot_split << "_w" << sample->wafer_string();

		if (C.intensity.is_set()) 
			cols.push_back({C.intensity.data,longname.str() + " intensity",C.intensity.unit().to_string(),comment.str()});
	}
	if (crater.sputter_current().is_set())
	{
		comment.str("");
		comment << "crater ";
		if (sample->simple_name!="")
			comment << sample->simple_name;
		else
			comment << sample->lot << sample->lot_split << "_w" << sample->wafer_string();
		cols.push_back({crater.sputter_current().data,"crater "+ crater.sputter_current().name(),crater.sputter_current().unit().to_string(),comment.str()});
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
		filename << "_" << sample->lot << sample->lot_split << "_w" << sample->wafer_string();
	
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

cluster_t measurements_::sims_t::virtual_matrix_cluster()
{
	if (reference_clusters().size()==0)
	{
// 		logger::error("measurements_::sims_t::matrix_cluster: reference_clusters().size()==0");
		logger::error("measurements_::sims_t::matrix_cluster()","reference_clusters().size()==0",to_string(),"skipping");
		return {};
	}
	intensity_t intensity;
	set<isotope_t> isos;
	for (auto& C : reference_clusters())
	{
		isos.insert(C->isotopes.begin(),C->isotopes.end());
		if (!intensity.is_set())
			intensity = C->intensity;
		else
			intensity += C->intensity;
	}
	return {isos,(*reference_clusters().begin())->sputter_time,intensity,(*reference_clusters().begin())->sputter_depth};
}

isotope_t measurements_::sims_t::isotope_corresponding_to_cluster(cluster_t& cluster)
{
	set<isotope_t> isos;
	for (auto& i:cluster.isotopes)
	{
		if (find(reference_isotopes().begin(),reference_isotopes().end(),i)==reference_isotopes().end())
			isos.insert(i);
	}
	if (isos.size()>1)
	{
		logger::error("measurements_::sims_t::isotope_corresponding_to_cluster()","detected more than 1 possible isotope in cluster; unknown matrix isotopes? check database",cluster.to_string(),"returning empty");
		return {};
	}
	return *isos.begin();
}

const set<isotope_t> measurements_::sims_t::reference_isotopes()
{
	set<isotope_t> isos;
	for (auto c : reference_clusters())
		isos.insert(c->isotopes.begin(),c->isotopes.end());
	return isos;
}

set<cluster_t *> measurements_::sims_t::clusters_corresponding_to_isotope(isotope_t& isotope)
{
	set<cluster_t*> Cs;
	for (auto& C : clusters)
	{
		if (find(clusters.begin(),clusters.end(),C)!=clusters.end())
		{
			Cs.insert(&C);
		}
	}
	if (Cs.size()==0)
		logger::error("measurements_::sims_t::clusters_corresponding_to_isotope()","can not fond corresponding cluster","","return empty");
	return Cs;
}


const set<const cluster_t*> measurements_::sims_t::matrix_clusters() const
{
	return reference_clusters();
}

/*"30Si" can be a reference cluster for elemental Si or isotopical purified Si*/
/*"30Si 28Si" can be a reference cluster for elemental Si or isotopical purified Si*/
/*"74Ge 28Si" can be a reference cluster for elemental Si+Ge or isotopical purified Si+Ge*/
/// reference_cluster: does NOT contain isotopes, which are NOT found in matrix->isotopes
const set<const cluster_t*> measurements_::sims_t::reference_clusters() const
{
	set<const cluster_t*> ref_clusters;
// 	if (reference_cluster!=nullptr)
// 		ref_clusters.insert(reference_cluster);
	if (!sample->matrix().is_set())
		return ref_clusters;
	
// 	set<string> element_symbols_in_matrix;
// 	for (auto& I : sample->matrix().isotopes)
// 		element_symbols_in_matrix.insert(I.symbol);
	
	bool insert=false;
	for (auto& C : clusters)
	{
		insert = true;
		for (auto& I : C.isotopes)
		{
			if (find(sample->matrix().isotopes.begin(),sample->matrix().isotopes.end(),I)==sample->matrix().isotopes.end())
			{
				insert=false;
				break;
			}
		}
		if (insert) ref_clusters.insert(&C);
	}
	
	return ref_clusters;
}

bool measurements_::sims_t::add(measurements_::sims_t& adder)
{
	if (*this != adder)
		return false;
	crater.total_sputter_depths << adder.crater.total_sputter_depths;
	crater.linescans.insert(crater.linescans.end(),adder.crater.linescans.begin(),adder.crater.linescans.end());
	if (!crater.sputter_beam.sputter_current.is_set())
		crater.sputter_beam = adder.crater.sputter_beam;
	return true;
}

// vector<cluster_t>& measurements_::sims_t::clusters()
// {
// 	return clusters;
// }
// crater_t& measurements_::sims_t::crater
// {
// 	return crater;
// }

measurements_::sims_t measurements_::sims_t::change_resolution(sputter_depth_t sputter_depth_res)
{
	if (!sputter_depth_res.is_set())
	{
		logger::error("measurements_::sims_t::change_resolution","!sputter_depth_res.is_set()","","returning this");
		return *this;
	}
	if (sputter_depth_res.data.size()==1 && sputter_depth_res.data.at(0)<=0)
	{
		logger::error("measurements_::sims_t::change_resolution","!sputter_depth_res is <= 0","","returning this");
		return *this;
	}
	sims_t copy_M = *this;
	copy_M.crater = copy_M.crater.change_resolution(sputter_depth_res);
	if (!copy_M.crater.sputter_depth.is_set())
		return copy_M;
	for (auto& C: copy_M.clusters)
	{
		C = C.interpolate(copy_M.crater.sputter_depth,crater.sputter_depth);
	}
	
	return copy_M;
}
measurements_::sims_t measurements_::sims_t::change_resolution(sputter_time_t sputter_time_res)
{
	if (!sputter_time_res.is_set())
	{
		logger::error("measurements_::sims_t::change_resolution","!sputter_time_res.is_set()","","returning this");
		return *this;
	}
	if (sputter_time_res.data.size()==1 && sputter_time_res.data.at(0)<=0)
	{
		logger::error("measurements_::sims_t::change_resolution","!sputter_time_res is <= 0","","returning this");
		return *this;
	}
	sims_t copy_M = *this;
	copy_M.crater = copy_M.crater.change_resolution(sputter_time_res);
	if (!copy_M.crater.sputter_time.is_set()) 
		return copy_M;
	for (auto& C: copy_M.clusters)
	{
		C = C.interpolate(copy_M.crater.sputter_time,crater.sputter_time);
	}
	
	return copy_M;
}

/*************************************/
/*********     calc_t     ************/
/*************************************/


// measurements_::sims_t::calc_t measurements_::sims_t::calc()
// {
// 	return calc_t(*this);
// }
// 
// /*******************************/
// /***        calc_t         *****/
// /*******************************/
// 
// measurements_::sims_t::calc_t::calc_t(measurements_::sims_t& measurement) : measurement(measurement)
// {
// }
// 
// measurements_::sims_t::calc_t::SR_c measurements_::sims_t::calc_t::SR()
// {
// 	return SR_c(measurement);
// }
// 
// 
// /*******************************/
// /***     calc_t::SR_c      *****/
// /*******************************/
// 
// measurements_::sims_t::calc_t::SR_c::SR_c(measurements_::sims_t& measurement) : measurement(measurement)
// {
// }
// 
// bool measurements_::sims_t::calc_t::SR_c::from_crater_depth()
// {
// 	if (!measurement.crater.total_sputter_depth().is_set())
// 	{
// // 		logger::warning("measurements_::sims_t::calc_t::SR_c::from_crater_depth: !measurement.crater.total_sputter_depth().is_set()");
// 		logger::warning(1,"measurements_::sims_t::calc_t::SR_c::from_crater_depth()","!measurement.crater.total_sputter_depth().is_set()",measurement.to_string(),"returning FALSE");
// 		return false;
// 	}
// 	if (!measurement.crater.total_sputter_time(&measurement.clusters).is_set())
// 	{
// // 		logger::warning("measurements_::sims_t::calc_t::SR_c::from_crater_depth: !measurement.crater.total_sputter_time.is_set()");
// 		logger::warning(1,"measurements_::sims_t::calc_t::SR_c::from_crater_depth()","!measurement.crater.total_sputter_time(&measurement.clusters).is_set()",measurement.to_string(),"returning FALSE");
// 		return false;
// 	}
// 	measurement.crater.SR = (measurement.crater.total_sputter_depth() / measurement.crater.total_sputter_time); // skalar value
// // 	for (auto& C : measurement.clusters)
// // 		C.SR = measurement.crater.SR; // skalars
// 	return true;
// }

