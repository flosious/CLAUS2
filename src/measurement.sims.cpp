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

/********************************************/
/* *measurements_::sims_t::matrix_clusters_c */
/********************************************/

string measurements_::sims_t::matrix_clusters_c::to_string(const string del) const
{
	stringstream out;
	unsigned int i=0;
	unsigned int s = clusters.size();
	for (auto& C : clusters)
	{
		out << C->to_string();
		if (i < s-1)
			out << "+";
		i++;
	}
	return out.str();
}

/*"30Si" can be a reference cluster for elemental Si or isotopical purified Si*/
/*"30Si 28Si" can be a reference cluster for elemental Si or isotopical purified Si*/
/*"74Ge 28Si" can be a reference cluster for elemental Si+Ge or isotopical purified Si+Ge*/
measurements_::sims_t::matrix_clusters_c::matrix_clusters_c(vector<cluster_t>& clusters_s, const vector<isotope_t> matrix_isotopes)
{

	if (matrix_isotopes.size()==0)
		return;
	if (clusters_s.size()==0)
		return;
	
	bool insert=false;
	for (auto& C : clusters_s) 
	{
		insert = true;
		for (auto& I : C.isotopes)
		{
			if (find(matrix_isotopes.begin(),matrix_isotopes.end(),I)==matrix_isotopes.end())
			{
				insert=false;
				break;
			}
		}
		if (insert) clusters.push_back(&C);
	}
}

bool measurements_::sims_t::matrix_clusters_c::is_cluster_in_matrix(const cluster_t& cluster)
{
	for (auto& C : clusters)
		if (*C == cluster)
			return true;
	return false;
}

intensity_t measurements_::sims_t::matrix_clusters_c::intensity_sum() const
{
	intensity_t intensity;
	for (auto& C : clusters)
	{
		if (!intensity.is_set())
			intensity = C->intensity;
		else
			intensity += C->intensity;
	}
	return intensity;
}

concentration_t measurements_::sims_t::matrix_clusters_c::concentration_sum() const
{
	intensity_t concentration;
	for (auto& C : clusters)
	{
		if (!concentration.is_set())
			concentration = C->concentration;
		else
			concentration += C->concentration;
	}
	return concentration;
}

vector<isotope_t> measurements_::sims_t::matrix_clusters_c::isotopes() const
{
	set<isotope_t> isotopes;
	if (clusters.size()==0)
		return {};
	for (auto& C : clusters)
	{
		isotopes.insert(C->isotopes.begin(),C->isotopes.end());
	}
	return {isotopes.begin(),isotopes.end()};
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

measurements_::sims_t::matrix_clusters_c measurements_::sims_t::matrix_clusters(const vector<isotope_t>& matrix_isotopes)
{
	///if no new isotopes, use the known ones
	if (matrix_isotopes.size()!=0)
		isotopes_in_matrix = matrix_isotopes;
	///if no isotopes are known, look in database
	if (isotopes_in_matrix.size()==0)
		isotopes_in_matrix = sample->matrix().isotopes;
	
	return matrix_clusters_c(clusters,isotopes_in_matrix);
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

measurements_::sims_t::calc_t measurements_::sims_t::calc()
{
	return calc_t{(*this)};
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
	if (crater.linescans.size()>0)
		ss << "linescans: <" << crater.linescans.size() <<">" << del;
	ss << "clusters: <" << clusters.size() << ">" << del;
	logger::debug(11,"measurements_::sims_t::to_string","","","exiting");
	return ss.str();
}

void measurements_::sims_t::plot_now(double sleep_sec)
{
	plot_t plot;
	quantity_t X;
	if (crater.sputter_depth.is_set())
	{
		X = crater.sputter_depth.change_unit({"nm"});
	}
	else if (crater.sputter_time.is_set())
		X = crater.sputter_time.change_unit({"min"});
	else
	{
		logger::error("measurements_::sims_t::to_string", "neither sputter_time nor sputter_depth is set","","no plot");
		return;
	}
	if (crater.sputter_current().is_set())
		plot.Y3.add_curve(X,crater.sputter_current().change_unit({"nA"}));
	plot.Y1.range(1,1E6,true);
	
	matrix_clusters_c MC = matrix_clusters();
	intensity_t ref_intensity = MC.intensity_sum();
	if (MC.clusters.size()>1 && ref_intensity.is_set())
		plot.Y1.add_curve(X,ref_intensity,MC.to_string());
	
	for (auto& C: clusters)
	{
		if (C.intensity.is_set())
		{
			plot.Y1.add_curve(X,C.intensity,C.to_string());
			if (sample->implant(C.corresponding_isotope(sample->matrix().isotopes)).dose.is_set())
			{
				// unit is not same as unit(x)
				quantity_t min = calc().implant(C).minimum_starting_position().change_unit(X.unit());
				if (min.is_set())
					plot.Y1.add_arrow(min.data.at(0),0.1,min.data.at(0),1E6,"bA",C.to_string() );
// 					plot.Y1.add_arrow(min.data.at(0),0.1,min.data.at(0),1E6,"bA",C.to_string() +" implant\\_minimum");
			}
		}
// 		cout << C.RSF.to_string() << endl;
// 		cout << C.SF.to_string() << endl;
		if (C.concentration.is_set())
			plot.Y2.add_curve(X,C.concentration,C.to_string());
		else if (C.RSF.is_set() && C.RSF.data.size()>1) // vector
			plot.Y2.add_curve(X,C.RSF,C.to_string());
	}
	
	plot.to_screen(to_string(),sleep_sec);
}

void measurements_::sims_t::export_origin_ascii(std::__cxx11::string path, const std::__cxx11::string delimiter)
{
	if (clusters.size()==0)
	{
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

// cluster_t& measurements_::sims_t::matrix_cluster()
// {
// 	if (matrix_cluster_s.is_set())
// 		return matrix_cluster_s;
// 	if (reference_clusters().size()==0)
// 	{
// 		logger::error("measurements_::sims_t::matrix_cluster()","reference_clusters().size()==0",to_string(),"skipping");
// 		return matrix_cluster_s;
// 	}
// 	intensity_t intensity;
// 	concentration_t concentration;
// 	set<isotope_t> isos;
// 	for (auto& C : reference_clusters())
// 	{
// 		isos.insert(C->isotopes.begin(),C->isotopes.end());
// 		if (!intensity.is_set())
// 			intensity = C->intensity;
// 		else
// 			intensity += C->intensity;
// 		if (!concentration.is_set())
// 			concentration = C->concentration;
// 		else
// 			concentration += C->concentration;
// 	}
// // 	matrix_cluster_s = cluster_t(isos,(*reference_clusters().begin())->sputter_time,intensity,(*reference_clusters().begin())->sputter_depth,concentration);
// 	matrix_cluster_s = cluster_t(isos,{},intensity,{},concentration);
// 	return matrix_cluster_s;
// }

isotope_t measurements_::sims_t::isotope_corresponding_to_cluster(const cluster_t& cluster)
{
// 	set<isotope_t> isos;
// 	for (auto& i:cluster.isotopes)
// 	{
// 		if (find(reference_isotopes().begin(),reference_isotopes().end(),i)==reference_isotopes().end())
// 			isos.insert(i);
// 	}
// 	if (isos.size()>1)
// 	{
// 		logger::error("measurements_::sims_t::isotope_corresponding_to_cluster()","detected more than 1 possible isotope in cluster; unknown matrix isotopes? check database",cluster.to_string(),"returning empty");
// 		return {};
// 	}
// 	return *isos.begin();
	return cluster.corresponding_isotope(sample->matrix().isotopes);
}

// const vector<isotope_t> measurements_::sims_t::reference_isotopes() const
// {
// 	set<isotope_t> isos;
// 	for (auto c : reference_clusters())
// 		isos.insert(c->isotopes.begin(),c->isotopes.end());
// 	return {isos.begin(),isos.end()};
// }

set<cluster_t *> measurements_::sims_t::clusters_corresponding_to_isotope(const isotope_t& isotope)
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


// const set<const cluster_t*> measurements_::sims_t::matrix_clusters() const
// {
// 	return reference_clusters();
// }

/*"30Si" can be a reference cluster for elemental Si or isotopical purified Si*/
/*"30Si 28Si" can be a reference cluster for elemental Si or isotopical purified Si*/
/*"74Ge 28Si" can be a reference cluster for elemental Si+Ge or isotopical purified Si+Ge*/
/// reference_cluster: does NOT contain isotopes, which are NOT found in matrix->isotopes
// const set<const cluster_t*> measurements_::sims_t::reference_clusters() const
// {
// 	set<const cluster_t*> ref_clusters;
// // 	if (reference_cluster!=nullptr)
// // 		ref_clusters.insert(reference_cluster);
// 	if (!sample->matrix().is_set())
// 		return ref_clusters;
// 	
// // 	set<string> element_symbols_in_matrix;
// // 	for (auto& I : sample->matrix().isotopes)
// // 		element_symbols_in_matrix.insert(I.symbol);
// 	
// 	bool insert=false;
// 	for (auto& C : clusters)
// 	{
// 		insert = true;
// 		for (auto& I : C.isotopes)
// 		{
// 			if (find(sample->matrix().isotopes.begin(),sample->matrix().isotopes.end(),I)==sample->matrix().isotopes.end())
// 			{
// 				insert=false;
// 				break;
// 			}
// 		}
// 		if (insert) ref_clusters.insert(&C);
// 	}
// 	
// 	return ref_clusters;
// }

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

cluster_t* measurements_::sims_t::cluster(const cluster_t& cluster_s)
{
	for (auto& C : clusters)
		if (cluster_s==C)
			return &C;
	return nullptr;
}

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
