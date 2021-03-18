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

#include "files.hpp"

/***********************/
/** D SIMS**/



/***************************************/
/*****  files_::dsims_t::contents_t::Ipr_t       ****/
/***************************************/

// const sputter_depth_t files_::dsims_t::contents_t::Ipr_t::sputter_depth() const
// {
// 	return sputter_depth_p;
// }
// 
// const sputter_time_t files_::dsims_t::contents_t::Ipr_t::sputter_time() const
// {
// 	return sputter_time_p;
// }
// 
// const sputter_current_t files_::dsims_t::contents_t::Ipr_t::sputter_current() const
// {
// 	return sputter_current_p;
// }

files_::dsims_t::contents_t::Ipr_t::Ipr_t(sputter_current_t sputter_current_s, 
							 sputter_time_t sputter_time_s, 
							 sputter_depth_t sputter_depth_s) 
																	: 	sputter_current(sputter_current_s), 
																		sputter_time(sputter_time_s), 
																		sputter_depth(sputter_depth_s)
{
}

const std::__cxx11::string files_::dsims_t::contents_t::Ipr_t::to_string(const string del) const
{
	stringstream out;
	out << sputter_time.to_string() << del << sputter_depth.to_string() << del << sputter_current.to_string();
	return out.str();
}


/***************************************/
/*****  files_::dsims_t::contents_t  ****/
/***************************************/

files_::dsims_t::contents_t::contents_t(string& filename_with_path) : files_::sims_t::contents_t(filename_with_path,"\t",{"*** DATA FILES ***"})
{
}

void files_::dsims_t::contents_t::to_screen(string prefix)
{
// 	cout << prefix << "name:" << endl;
// 	cout << prefix << "\t"<<"filename_with_path:\t" << name.filename_with_path << endl;
// 	cout << prefix << "\t"<<"filename:\t" << name.filename() << endl;
// 	cout << prefix << "\t"<<"filename_type_ending:\t" << name.filename_type_ending() << endl;
// 	cout << prefix << "\t"<<"filename_without_crater_depths:\t" << name.filename_without_crater_depths() << endl;
// 	cout << prefix << "\t"<<"simple_name:\t" << name.simple_name() << endl;
// 	cout << prefix << "\t"<<"directory:\t" << name.directory() << endl;
// 	
// 	cout << prefix << "\tdelimiter:\t'" << name.delimiter <<"'" << endl;
// // 	if (name.identifiers.size()==1) cout << prefix << "\tidentifiers:\t" << *name.identifiers.begin() << endl;
// // 	else cout << prefix << "\tidentifiers:\t<" << name.identifiers.size() << ">" << endl;
// 	
// 	cout << prefix << "\t" <<"olcdb:\t" << name.olcdb() << endl;
// 	cout << prefix << "\t" <<"lot:\t" << name.lot() << endl;
// 	cout << prefix << "\t" << "lot_split:\t" << name.lot_split() << endl;
// 	cout << prefix << "\t"<<"wafer:\t" << name.wafer() << endl;
// 	cout << prefix << "\t" << "chip x;y:\t" << name.chip_x() << ";" << name.chip_y() << endl;
// 	cout << prefix << "\t" << "monitor:\t" << name.monitor() << endl;
// 	cout << prefix << "\t"<<"group:\t" << name.group() << endl;
// 	cout << prefix << "\t"<<"repetition:\t" << name.repetition() << endl;
// 	
// 	if (name.not_parseable_filename_parts().size()==1) cout << prefix << "\t"<<"not_parseable_filename_parts:\t" << *name.not_parseable_filename_parts().begin() << endl;
// 	else cout << prefix << "\t"<<"not_parseable_filename_parts:\t<" << name.not_parseable_filename_parts().size() << ">" << endl;
// 	
// 	cout << prefix << "\t"<<"sputter_element:\t" << name.sputter_element().to_string() << endl;
// 	cout << prefix << "\t"<<"sputter_energy:\t" << name.sputter_energy().to_string() << endl;
// 	cout << prefix << "\t"<<"secondary_polarity:\t" << name.secondary_polarity() << endl;
// 	
// 	cout << prefix << "\t"<<"total_sputter_depths:\t" << name.total_sputter_depths().to_string() << endl;
	
	
	cout << prefix << "contents:" << endl;
	cout << prefix << "\tdelimiter:\t'" << delimiter <<"'" << endl;
	if (identifiers.size()==1) cout << prefix << "\tidentifiers:\t" << *identifiers.begin() << endl;
	else cout << prefix << "\tidentifiers:\t<" << identifiers.size() << ">" << endl;
	cout << prefix << "\t"<<"creation_date_time:\t" << tools::time::tm_struct_to_string_time(creation_date_time()) << endl;
	cout << prefix << "\t"<<"start_date_time:\t" << tools::time::tm_struct_to_string_time(start_date_time()) << endl;
	cout << prefix << "\t"<<"clusters:" << endl;
	for (auto c:clusters())
		cout << prefix << "\t\t" << c.to_string() << endl;
	cout << prefix << "\t"<<"Ipr:\t" << Ipr().to_string() << endl;
	
	cout << prefix << "\t"<<"chamber_pressure:\t" << chamber_pressure().to_string() << endl;
	cout << prefix << "\t"<<"field_aperture:\t" << field_aperture().to_string() << endl;
	cout << prefix << "\t"<<"contrast_aperture:\t" << contrast_aperture().to_string() << endl;
	cout << prefix << "\t"<<"entrance_slit:\t" << entrance_slit().to_string() << endl;
	cout << prefix << "\t"<<"exit_slit:\t" << exit_slit().to_string() << endl;
	cout << prefix << "\t"<<"mass_resolution:\t" << mass_resolution().to_string() << endl;
	cout << prefix << "\t"<<"energy_window:\t" << energy_window().to_string() << endl;
	cout << prefix << "\t"<<"egate:\t" << egate().to_string() << endl;
	
	cout << prefix << "\t"<<"secondary_polarity:\t" << secondary_polarity() << endl;
	cout << prefix << "\t"<<"secondary_voltage:\t" << secondary_voltage().to_string() << endl;
	
	cout << prefix << "\t"<<"em_voltage:\t" << em_voltage().to_string() << endl;
	cout << prefix << "\t"<<"em_yield:\t" << em_yield().to_string() << endl;
	
	cout << prefix << "\t"<<"sputter_element:\t" << sputter_element().to_string() << endl;
	cout << prefix << "\t"<<"sputter_energy:\t" << sputter_energy().to_string() << endl;
	cout << prefix << "\t"<<"sputter_rastersize:\t" << sputter_rastersize().to_string() << endl;
	cout << prefix << "\t"<<"analysis_rastersize:\t" << analyzed_area().to_string() << endl;
	cout << prefix << "\t"<< "matrix\t" << matrix().to_string() << endl;
}




vector<cluster_t> files_::dsims_t::contents_t::clusters()
{
	vector<cluster_t> collected_clusters;
	for (auto& clustername : cluster_names())
	{
		
		if (clustername=="Ipr") continue;
		sputter_time_t sputter_time_s;
		sputter_depth_t sputter_depth_s;
		concentration_t concentration_s;
		intensity_t intensity_s;
		for (auto& col:columns())
		{
			if (col.cluster_name != clustername) continue;
			if (col.dimension=="Time") sputter_time_s = sputter_time_t(col.data,col.unit);
			if (col.dimension=="Depth") sputter_depth_s = sputter_depth_t(col.data,col.unit);
			if (col.dimension=="C") concentration_s = concentration_t(col.data,col.unit);
			if (col.dimension=="I") intensity_s = intensity_t(col.data,col.unit);
		}
		cluster_t cluster(clustername,sputter_time_s,intensity_s,sputter_depth_s,concentration_s);
		if (cluster.is_set()) collected_clusters.push_back(cluster);
		else
			logger::error("files_::dsims_t::contents_t::clusters() cluster.is_set()", false);
	}
	return collected_clusters;
}

const vector<files_::dsims_t::contents_t::column_t> files_::dsims_t::contents_t::columns()
{
	if (check_Ipr()) ipr_shift_correction();
	if (!parse_units_dimensions_clusternames())
		return {};
	remove_corrupted_lines();
	if (cluster_names().size()<1 && !parse_units_dimensions_clusternames())
	{
		logger::debug("files_::dsims_t::contents_t::columns(): !parse_units_dimensions_clusternames");
		return {};
	}
	
	vector<files_::sims_t::contents_t::column_t> cols;
	vector<vector<string>> data_cols_lines = tools::mat::transpose_matrix(raw_data_tensor()[0]);
	const int cols_per_element = dimensions().size()/cluster_names().size();
// 	cout << "dimensions().size()=" << dimensions().size() << endl;
// 	cout << "cluster_names().size()=" << cluster_names().size() << endl;
// 	cout << "units().size()=" << units().size() << endl;
// 	cout << "data_cols_lines().size()=" << data_cols_lines.size() << endl;
	
	for (int i=0;i<dimensions().size() && i<data_cols_lines.size() ;i++)
	{
		files_::sims_t::contents_t::column_t col;
		col.cluster_name=cluster_names()[i/cols_per_element];
		col.dimension=dimensions()[i];
		col.unit=units()[i];
		
		vector<string> col_data_string = data_cols_lines[i];
		col.data=tools::mat::str_vec_to_double_vec(col_data_string);
		if (col.data.size()==0) continue;
		cols.push_back(col);
	}
// 	cout << "START"<<endl;
// 	print(cluster_names());
// 	print(dimensions());
// 	print(units());
// 	for (auto& col : cols)
// 	{
// 		cout << "col.cluster_name=" << col.cluster_name << endl;
// 		cout << "col.dimension=" << col.dimension << endl;
// 		cout << "col.unit=" << col.unit << endl;
// 	}
// 	cout << "STOP"<<endl;
	return cols;
}

const files_::dsims_t::contents_t::Ipr_t files_::dsims_t::contents_t::Ipr()
{
	sputter_current_t sputter_current_s;
	sputter_time_t sputter_time_s;
	sputter_depth_t sputter_depth_s;
	for (auto& col:columns())
	{
		if (col.cluster_name != "Ipr") continue;
		if (col.dimension=="Time") sputter_time_s = sputter_time_t(col.data,col.unit);
		if (col.dimension=="Depth") sputter_depth_s = sputter_depth_t(col.data,col.unit);
		if (col.dimension=="I") sputter_current_s = sputter_current_t(col.data,col.unit);
	}
	return {sputter_current_s,sputter_time_s,sputter_depth_s};
}

const vector<std::__cxx11::string>& files_::dsims_t::contents_t::dimensions()
{
	if (dimensions_p.size()>0) return dimensions_p;
	parse_units_dimensions_clusternames();
	return dimensions_p;
}
const vector<std::__cxx11::string>& files_::dsims_t::contents_t::units()
{
	if (units_p.size()>0) return units_p;
	parse_units_dimensions_clusternames();
	return units_p;
}
const vector<std::__cxx11::string>& files_::dsims_t::contents_t::cluster_names()
{
	if (cluster_names_p.size()>0) return cluster_names_p;
	parse_units_dimensions_clusternames();
	return cluster_names_p;
}

bool files_::dsims_t::contents_t::parse_units_dimensions_clusternames()
{
	cluster_names_p.clear();
	units_p.clear();
	dimensions_p.clear();
	/*1st header */
	static const int c=0;
	tools::str::replace_chars(&raw_header_tensor().at(c)," [","[");
	const int sample_names_line_number=(raw_header_tensor().at(c)).size()-3;
	const int elements_line_number=(raw_header_tensor().at(c)).size()-2;
	const int dimensions_units_line_number=(raw_header_tensor().at(c)).size()-1;
	
	
	vector<string> samples;
	for (int i=0;i<((raw_header_tensor().at(c))[sample_names_line_number]).size();i++) {
		if (( (raw_header_tensor().at(c))[sample_names_line_number][i]).size()>0 ) {
			string result = ((raw_header_tensor().at(c))[sample_names_line_number][i]);
			samples.push_back(result);
		}
	}
	if (samples.size()!=1) 
	{
		logger::error("files_::dsims_t::contents_t::parse_raw_header_tensor(): samples.size()!=1");
		return false;
	}
	
	for (int i=0;i<((raw_header_tensor().at(c))[elements_line_number]).size();i++) {
        string result = ((raw_header_tensor().at(c))[elements_line_number][i]);

		if (result.size()>0 ) {
			cluster_names_p.push_back(result);
		} else {
        }
	}
	
	for (int i=0;i<((raw_header_tensor().at(c))[dimensions_units_line_number]).size();i++) {
		if ((((raw_header_tensor().at(c))[dimensions_units_line_number])[i]).size()>0) {
			units_p.push_back(tools::str::get_string_between_string_A_and_next_B(&((raw_header_tensor().at(c))[dimensions_units_line_number][i]),"[","]"));
			dimensions_p.push_back((tools::str::get_strings_between_delimiter(((raw_header_tensor().at(c))[dimensions_units_line_number][i]),"[")).at(0));
		}
	}
        
	if (units_p.size()!=dimensions_p.size()) {
		logger::fatal("files_::dsims_t::contents_t::parse_raw_header_tensor(): units.size()!=dimensions.size()");
		return false;
	}
		
	int cols_per_element = dimensions_p.size()/cluster_names_p.size();
	if (cols_per_element!=3)
	{
		/* 
		 * dp_asc has a bug in the exported data: dimension and unit are one X- and one Y-axis to long
		 * to fix it, we remove it from the end
		 */
// 		if (check_Ipr())
// 		{
// 			dimensions_p.erase(dimensions_p.end()-4,dimensions_p.end()-2);
// 			units_p.erase(units_p.end()-4,units_p.end()-2);
// 		} 
	}
// 	print(dimensions_p);
// 	print(units_p);
// 	if (units_p.size()!=dimensions_p.size()) {
// 		logger::fatal("files_::dsims_t::contents_t::parse_raw_header_tensor(): units.size()!=dimensions.size()");
// 		return false;
// 	}


    return true;
}

bool files_::dsims_t::contents_t::check_Ipr() 
{
	is_Ipr=false;
	is_Ipr = tools::mat::find_str_in_mat(cluster_names(),"Ipr");
	return is_Ipr;
}

set<int> files_::dsims_t::contents_t::corrupted_lines()
{
	set<int> corrupted_lines_p;
	vector<vector<string>> cols_lines = tools::mat::transpose_matrix(raw_data_tensor().at(0));
	vector<double> col;
	
	int colssize=cols_lines.size();
	int max_size=-1;
	if (check_Ipr()) colssize = colssize-3;
	for (int c=0;c<colssize;c++)
	{
		col = tools::mat::str_vec_to_double_vec(cols_lines[c]);
		if (col.size()<2) continue;
		if (max_size==-1) 
		{
			max_size = col.size();
			continue;
		}
		
		if (col.size()<max_size) 
		{
			corrupted_lines_p.insert(max_size-1);
			max_size = col.size();
		}
		if (col.size()>max_size) 
		{
			corrupted_lines_p.insert(col.size()-1);
		}
	}
	
	return corrupted_lines_p;
}

void files_::dsims_t::contents_t::remove_corrupted_lines()
{
	set<int> corrupted_lines_p=corrupted_lines();
	if (raw_data_tensor().size()==0) return;
	for (set<int>::reverse_iterator it=corrupted_lines_p.rbegin();it!=corrupted_lines_p.rend();++it)
	{
		logger::info("files_::dsims_t::contents_t::remove_corrupted_lines() " + std::to_string(*it), filename_with_path);
		raw_data_tensor_p.at(0).erase(raw_data_tensor_p.at(0).begin()+*it);
	}
}

/*
 * exported data is bugged, there are to many "\t" in the lines, where there is no cluster data, but Ipr data
 */
bool files_::dsims_t::contents_t::ipr_shift_correction() 
{
	raw_data_tensor();
	vector<vector<string>>* mat = &raw_data_tensor_p.at(0);
    if (mat->size()==0) 
	{
		logger::error("files_::dsims_t::contents_t::ipr_shift_correction(): raw_data_tensor_p.at(0).size()==0", (int)mat->size());
		return false;
	}
	int cols_per_element = dimensions().size()/cluster_names().size();
	if (cols_per_element!=3) cols_per_element=2;
	int Ipr_col;
	if (cols_per_element==3) Ipr_col = ((tools::mat::find_in_vec(cluster_names(),"Ipr"))+1)*3;
	else Ipr_col = dimensions().size()-1-1;
	int Ipr_offset_line;
	int Ipr_col_at_EOL;
    int total_lines=mat->size();
	for (Ipr_col_at_EOL=0;Ipr_col_at_EOL<mat->back().size();Ipr_col_at_EOL++) {
		if (!tools::str::is_empty(mat->back().at(Ipr_col_at_EOL))) break;
	}
	Ipr_col_at_EOL++;
	for (Ipr_offset_line=0;Ipr_offset_line<total_lines;Ipr_offset_line++) 
	{
		if (mat->at(Ipr_offset_line)[cols_per_element-1]=="") break; // die 3. spalte enthaelt immer daten (intensität des 1. isotops)
	}
	vector<vector<string>> Ipr_mat;
	// cut off Ipr from data mat
	Ipr_mat.insert(Ipr_mat.begin(),mat->begin()+Ipr_offset_line,mat->end());	
	// delete first columns within the matrix
	Ipr_mat=tools::mat::transpose_matrix(Ipr_mat);
	Ipr_mat.erase(Ipr_mat.begin(),Ipr_mat.begin()+(Ipr_mat.size()-cluster_names().size()*cols_per_element));
	Ipr_mat=tools::mat::transpose_matrix(Ipr_mat);
	mat->erase(mat->begin()+Ipr_offset_line,mat->end()); 
	// add them together
	mat->insert(mat->end(),Ipr_mat.begin(),Ipr_mat.end());
    return true;
}

const quantity_t files_::dsims_t::contents_t::get_quantity_from_dimension_and_clustername(const string col_dimension, const string ignore_clustername)
{
	string unit="";
	vector<vector<double>> data_vecs;
	for (auto& col:columns())
	{
		if (col.dimension!=col_dimension) continue;
		if (col.cluster_name==ignore_clustername) continue;
		data_vecs.push_back(col.data);
		unit = col.unit;
	}
	if (unit=="") 
	{
		logger::error("files_::dsims_t::contents_t::sputter_time() unit",unit);
		return {};
	}
	return quantity_t{statistics::common_vector(data_vecs),unit};
}

const map<string,string>& files_::dsims_t::contents_t::infos_and_settings()
{
	if (infos_and_settings_p.size()>0) return infos_and_settings_p;
	string key, value;
	for (auto header_mat:raw_header_tensor())
	{
// 		tools::mat::remove_empty_cols_from_matrix(&header_mat);
		for (auto& header_line:header_mat)
		{
			if (header_line.size()<2) continue;
			key.clear();
			value.clear();
			for (auto& word:header_line)
			{
				if (word=="") continue;
				if (key=="") key=word;
				else if (value=="") value=word;
				else break;
			}
			infos_and_settings_p.insert(pair<string,string>(key,value));
		}
	}
	return infos_and_settings_p;
}

const tm files_::dsims_t::contents_t::start_date_time()
{
	// Analysis date   03/06/2020    
	// Analysis time   10:43
	// "03/06/2020 10:43"
	stringstream time;
	time << infos_and_settings().at("Analysis date") << " " << infos_and_settings().at("Analysis time") << ":00"; 
	return tools::time::string_time_to_tm_structure(time.str(),"%m/%d/%Y %H:%M:%S");
}

const tm files_::dsims_t::contents_t::creation_date_time()
{
	// Creation date   03/06/2020    
	// Creation time   10:43
	// "03/06/2020 10:43"
	stringstream time;
	time << infos_and_settings().at("Creation date") << " " << infos_and_settings().at("Creation time") <<":00"; 
	return tools::time::string_time_to_tm_structure(time.str(),"%m/%d/%Y %H:%M:%S");
}

const string files_::dsims_t::contents_t::secondary_polarity(const string find_this)
{
	if (infos_and_settings().find(find_this)==infos_and_settings().end()) return {};
	return infos_and_settings().at(find_this);
}

const energy_t files_::dsims_t::contents_t::sputter_energy(const string find_this)
{
	if (infos_and_settings().find(find_this)==infos_and_settings().end()) return {};
	double data = tools::str::str_to_double(infos_and_settings().at(find_this));
	return energy_t({data},{"eV"});
}

const element_t files_::dsims_t::contents_t::sputter_element(const string find_this)
{
	if (infos_and_settings().find(find_this)==infos_and_settings().end()) return {};
	string ele = infos_and_settings().at(find_this);
	ele.erase(remove(ele.begin(), ele.end(), '+'), ele.end());
	ele.erase(remove(ele.begin(), ele.end(), '-'), ele.end());
	ele.erase(remove(ele.begin(), ele.end(), '2'), ele.end());
	return element_t({ele});
}

const secondary_voltage_t files_::dsims_t::contents_t::secondary_voltage(const string find_this)
{
	if (infos_and_settings().find(find_this)==infos_and_settings().end()) return {};
	double data = tools::str::str_to_double(infos_and_settings().at(find_this));
	return secondary_voltage_t({data},{"V"});
}

const rastersize_t files_::dsims_t::contents_t::sputter_rastersize(const string find_this)
{
	if (infos_and_settings().find(find_this)==infos_and_settings().end()) return {};
	double data = tools::str::str_to_double(infos_and_settings().at(find_this));
	return rastersize_t({data},{"um"});
}

const rastersize_t files_::dsims_t::contents_t::analyzed_area(const string find_this)
{
	if (infos_and_settings().find(find_this)==infos_and_settings().end()) return {};
	double data = tools::str::str_to_double(infos_and_settings().at(find_this));
	return rastersize_t({data},{"um"});
}

const quantity_t files_::dsims_t::contents_t::chamber_pressure(const string find_this)
{
	if (infos_and_settings().find(find_this)==infos_and_settings().end()) return {};
	double data = tools::str::str_to_double(infos_and_settings().at(find_this));
	return quantity_t("chamber_pressure",{data},{"mbar"});
}

const quantity_t files_::dsims_t::contents_t::contrast_aperture(const string find_this)
{
	if (infos_and_settings().find(find_this)==infos_and_settings().end()) return {};
	double data = tools::str::str_to_double(infos_and_settings().at(find_this));
	return quantity_t("contrast_aperture",{data},{"um"});
}

const quantity_t files_::dsims_t::contents_t::egate(const string find_this)
{
	if (infos_and_settings().find(find_this)==infos_and_settings().end()) return {};
	double data = tools::str::str_to_double(infos_and_settings().at(find_this));
	return quantity_t("egate",{data},{"%"});
}

const quantity_t files_::dsims_t::contents_t::em_voltage(const string find_this)
{
	if (infos_and_settings().find(find_this)==infos_and_settings().end()) return {};
	double data = tools::str::str_to_double(infos_and_settings().at(find_this));
	return quantity_t("em_voltage",{data},{"V"});
}

const quantity_t files_::dsims_t::contents_t::em_yield(const string find_this)
{
	if (infos_and_settings().find(find_this)==infos_and_settings().end()) return {};
	double data = tools::str::str_to_double(infos_and_settings().at(find_this));
	return quantity_t("em_yield",{data},{"%"});
}

const quantity_t files_::dsims_t::contents_t::entrance_slit(const string find_this)
{
	if (infos_and_settings().find(find_this)==infos_and_settings().end()) return {};
	double data = tools::str::str_to_double(infos_and_settings().at(find_this));
	return quantity_t("entrance_slit",{data},{"um"});
}

const quantity_t files_::dsims_t::contents_t::energy_window(const string find_this)
{
	if (infos_and_settings().find(find_this)==infos_and_settings().end()) return {};
	double data = tools::str::str_to_double(infos_and_settings().at(find_this));
	return quantity_t("energy_window",{data},{"eV"});
}

const quantity_t files_::dsims_t::contents_t::exit_slit(const string find_this)
{
	if (infos_and_settings().find(find_this)==infos_and_settings().end()) return {};
	double data = tools::str::str_to_double(infos_and_settings().at(find_this));
	return quantity_t("exit_slit",{data},{"um"});
}

const quantity_t files_::dsims_t::contents_t::field_aperture(const string find_this)
{
	if (infos_and_settings().find(find_this)==infos_and_settings().end()) return {};
	double data = tools::str::str_to_double(infos_and_settings().at(find_this));
	return quantity_t("field_aperture",{data},{"um"});
}

const quantity_t files_::dsims_t::contents_t::mass_resolution(const string find_this)
{
	if (infos_and_settings().find(find_this)==infos_and_settings().end()) return {};
	double data = tools::str::str_to_double(infos_and_settings().at(find_this));
	return quantity_t("mass_resolution",{data},{""});
}

