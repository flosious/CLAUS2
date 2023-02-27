/*
	Copyright (C) 2021, 2022 Florian Bärwolf
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
/*****  files_::dsims_t::contents_t  ****/
/***************************************/

//files_::dsims_t::contents_t::contents_t(std::string& filename_with_path, const std::string& contents_string)
//    : files_::sims_t::contents_t(filename_with_path,"\t",{"*** DATA FILES ***"},contents_string), log_c
//{
//}


files_::dsims_t::contents_t::contents_t(name_t& sims_filename, const std::string& contents_string)
    : files_::sims_t::contents_t(sims_filename.filename_with_path,"\t",{"*** DATA FILES ***"},contents_string), log_c
{
    if (sims_filename.filename_type_ending()=="dp_rpc_asc")
        use_Ipr_shift_corretion = false;
}

void files_::dsims_t::contents_t::to_screen(std::string prefix)
{

	std::cout << prefix << "contents:" << std::endl;
	std::cout << prefix << "\tdelimiter:\t'" << delimiter <<"'" << std::endl;
	if (identifiers.size()==1) std::cout << prefix << "\tidentifiers:\t" << *identifiers.begin() << std::endl;
	else std::cout << prefix << "\tidentifiers:\t<" << identifiers.size() << ">" << std::endl;
	std::cout << prefix << "\t"<<"creation_date_time:\t" << tools::time::tm_struct_to_string_time(creation_date_time()) << std::endl;
	std::cout << prefix << "\t"<<"start_date_time:\t" << tools::time::tm_struct_to_string_time(start_date_time()) << std::endl;
	std::cout << prefix << "\t"<<"clusters:" << std::endl;
	for (auto c:clusters())
		std::cout << prefix << "\t\t" << c.to_string() << std::endl;
	std::cout << prefix << "\t"<<"Ipr:\t" << Ipr().to_string() << std::endl;
	
	std::cout << prefix << "\t"<<"chamber_pressure:\t" << chamber_pressure().to_string() << std::endl;
	std::cout << prefix << "\t"<<"field_aperture:\t" << field_aperture().to_string() << std::endl;
	std::cout << prefix << "\t"<<"contrast_aperture:\t" << contrast_aperture().to_string() << std::endl;
	std::cout << prefix << "\t"<<"entrance_slit:\t" << entrance_slit().to_string() << std::endl;
	std::cout << prefix << "\t"<<"exit_slit:\t" << exit_slit().to_string() << std::endl;
	std::cout << prefix << "\t"<<"mass_resolution:\t" << mass_resolution().to_string() << std::endl;
	std::cout << prefix << "\t"<<"energy_window:\t" << energy_window().to_string() << std::endl;
	std::cout << prefix << "\t"<<"egate:\t" << egate().to_string() << std::endl;
	
	std::cout << prefix << "\t"<<"secondary_polarity:\t" << secondary_polarity() << std::endl;
	std::cout << prefix << "\t"<<"secondary_voltage:\t" << secondary_voltage().to_string() << std::endl;
	
	std::cout << prefix << "\t"<<"em_voltage:\t" << em_voltage().to_string() << std::endl;
	std::cout << prefix << "\t"<<"em_yield:\t" << em_yield().to_string() << std::endl;
	
	std::cout << prefix << "\t"<<"sputter_element:\t" << sputter_element().to_string() << std::endl;
	std::cout << prefix << "\t"<<"sputter_energy:\t" << sputter_energy().to_string() << std::endl;
	std::cout << prefix << "\t"<<"sputter_rastersize:\t" << sputter_rastersize().to_string() << std::endl;
	std::cout << prefix << "\t"<<"analysis_rastersize:\t" << analyzed_area().to_string() << std::endl;
	std::cout << prefix << "\t"<< "matrix\t" << matrix() << std::endl;
}

std::vector<cluster_t> files_::dsims_t::contents_t::clusters()
{
    log_f;
	if (columns().size()==0)
		return {};
	std::vector<cluster_t> collected_clusters;
	for (auto& clustername : cluster_names())
	{
		
		if (clustername=="Ipr") continue;
		quantity::sputter_time_t sputter_time;
		quantity::depth_t sputter_depth_s;
		quantity::concentration_t concentration_s;
		quantity::intensity_t intensity_s;
		
		for (auto& col:columns())
		{
			if (col.cluster_name != clustername) continue;
            if (col.dimension=="Time")
            {
                sputter_time = quantity::sputter_time_t(col.data,col.unit);
                logger.debug(clustername).quantity(sputter_time.to_string_short());
            }
            else if (col.dimension=="Depth")
            {
                sputter_depth_s = quantity::depth_t(col.data,col.unit);
                logger.debug(clustername).quantity(sputter_depth_s.to_string_short());
            }
            else if (col.dimension=="C")
            {
                concentration_s = quantity::concentration_t(col.data,col.unit);
                logger.debug(clustername).quantity(concentration_s.to_string_short());
            }
            else if (col.dimension=="I")
            {
                intensity_s = quantity::intensity_t(col.data,col.unit);
                logger.debug(clustername).quantity(intensity_s.to_string_short());
            }
		}
		
		cluster_t cluster(clustername,sputter_time,intensity_s,sputter_depth_s,concentration_s);
		if (cluster.is_set()) 
		{
			collected_clusters.push_back(cluster);
            //logger::debug(11,"files_::dsims_t::contents_t::clusters()","cluster.to_string()",cluster.to_string());
		}
		else
        {
            //logger::error("files_::dsims_t::contents_t::clusters()", "!cluster.is_set()",clustername);
        }
	}
	return collected_clusters;
}

const std::vector<files_::dsims_t::contents_t::column_t> files_::dsims_t::contents_t::columns()
{
	if (columns_s.size()>0)
		return columns_s;
    if (check_Ipr() && use_Ipr_shift_corretion) ipr_shift_correction();
	if (!parse_units_dimensions_clusternames())
	{
        //logger::error("files_::dsims_t::contents_t::columns()","could not parse_units_dimensions_clusternames","","return empty");
		return {};
	}
	remove_corrupted_lines();
	if (cluster_names().size()<1)
	{
        //logger::error("files_::dsims_t::contents_t::columns()", "cluster_names().size()<1 && !parse_units_dimensions_clusternames()","","returning empty");
		return {};
	}
	
// 	std::vector<files_::sims_t::contents_t::column_t> cols;
	std::vector<std::vector<std::string>> data_cols_lines = tools::mat::transpose_matrix(raw_data_tensor()[0]);
	const int cols_per_element = dimensions().size()/cluster_names().size();
	
	for (int i=0;i<dimensions().size() && i<data_cols_lines.size() ;i++)
	{
		files_::sims_t::contents_t::column_t col;
		col.cluster_name=cluster_names()[i/cols_per_element];
		col.dimension=dimensions()[i];
		col.unit=units()[i];
		
		std::vector<std::string> col_data_string = data_cols_lines[i];
		col.data=tools::mat::str_vec_to_double_vec(col_data_string);
		if (col.data.size()==0) continue;
		columns_s.push_back(col);
        //logger::debug(13,"files_::dsims_t::contents_t::columns()",col.to_string());
	}
	
	if (dimensions().size() != data_cols_lines.size())
	{
        //logger::warning(3,"files_::dsims_t::contents_t::columns()","dimensions().size() != data_cols_lines.size()","bug in ckb_asc Cs+","fixed using last unit in line");
		columns_s.back().unit = units().back();
        //logger::debug(12,"files_::dsims_t::contents_t::columns()","cols_per_element=",tools::to_string(cols_per_element));
        //logger::debug(12,"files_::dsims_t::contents_t::columns()","dimensions().size()=",tools::to_string(dimensions().size()));
        //logger::debug(12,"files_::dsims_t::contents_t::columns()","cluster_names().size()=",tools::to_string(cluster_names().size()));
        //logger::debug(12,"files_::dsims_t::contents_t::columns()","units().size()=",tools::to_string(units().size()));
        //logger::debug(12,"files_::dsims_t::contents_t::columns()","data_cols_lines.size()=",tools::to_string(data_cols_lines.size()));
	}
	return columns_s;
}

const crater_t::sputter_beam_t files_::dsims_t::contents_t::Ipr()
{
	quantity::current_t sputter_current_s;
	quantity::sputter_time_t sputter_time;
	quantity::depth_t sputter_depth_s;
// 	if (columns().size()==0)
// 		logger::error("files_::dsims_t::contents_t::Ipr()","columns().size()==0","","return empty");
	for (auto& col:columns())
	{
		if (col.cluster_name != "Ipr") continue;
		if (col.dimension=="Time") sputter_time = quantity::sputter_time_t(col.data,col.unit);
		if (col.dimension=="Depth") sputter_depth_s = quantity::depth_t(col.data,col.unit);
		if (col.dimension=="I") 
		sputter_current_s = quantity::current_t(col.data,col.unit);
	}
    //logger::debug(10,"files_::dsims_t::contents_t::Ipr()","sputter_current_s.to_string()",sputter_current_s.to_string());
	return {sputter_current_s,sputter_time,sputter_depth_s};
}

const std::vector<std::string>& files_::dsims_t::contents_t::dimensions()
{
	if (dimensions_p.size()>0) return dimensions_p;
	parse_units_dimensions_clusternames();
	return dimensions_p;
}
const std::vector<std::string>& files_::dsims_t::contents_t::units()
{
	if (units_p.size()>0) return units_p;
	parse_units_dimensions_clusternames();
	return units_p;
}
const std::vector<std::string>& files_::dsims_t::contents_t::cluster_names()
{
	if (cluster_names_p.size()>0) return cluster_names_p;
	parse_units_dimensions_clusternames();
	return cluster_names_p;
}

bool files_::dsims_t::contents_t::parse_units_dimensions_clusternames()
{
    //logger::debug(21,"files_::dsims_t::contents_t::parse_units_dimensions_clusternames()","entering","");
	cluster_names_p.clear();
	units_p.clear();
	dimensions_p.clear();
	/*1st header */
	static const int c=0;
	tools::str::replace_chars(&raw_header_tensor().at(c)," [","[");
	const int sample_names_line_number=(raw_header_tensor().at(c)).size()-3;
	const int elements_line_number=(raw_header_tensor().at(c)).size()-2;
	const int dimensions_units_line_number=(raw_header_tensor().at(c)).size()-1;
	
	
	std::vector<std::string> samples;
	for (int i=0;i<((raw_header_tensor().at(c))[sample_names_line_number]).size();i++) 
	{
		
		if (( (raw_header_tensor().at(c))[sample_names_line_number][i]).size()>0 ) 
		{
			
			std::string result = ((raw_header_tensor().at(c))[sample_names_line_number][i]);
			samples.push_back(result);
// 			std::cout << "samples.size()="<< samples.size() << std::endl;
		}
	}
	if (samples.size()!=1) 
	{
        //logger::error("files_::dsims_t::contents_t::parse_units_dimensions_clusternames()","samples.size()!=1",filename_with_path,"returning false");
		return false;
	}
	for (int i=0;i<((raw_header_tensor().at(c))[elements_line_number]).size();i++) {
        std::string result = ((raw_header_tensor().at(c))[elements_line_number][i]);

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
        
    if (units_p.size()!=dimensions_p.size())
    {
        //logger::error("files_::dsims_t::contents_t::parse_units_dimensions_clusternames()","units_p.size()!=dimensions_p.size()", tools::to_string(units_p.size())+"!="+tools::to_string(dimensions_p.size()));
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

    //logger::debug(21,"files_::dsims_t::contents_t::parse_units_dimensions_clusternames()","exiting","");
    return true;
}

bool files_::dsims_t::contents_t::check_Ipr() 
{
    is_Ipr=false;
    is_Ipr = tools::mat::find_str_in_mat(cluster_names(),"Ipr");
	return is_Ipr;
}

std::set<int> files_::dsims_t::contents_t::corrupted_lines()
{
	std::set<int> corrupted_lines_p;
	std::vector<std::vector<std::string>> cols_lines = tools::mat::transpose_matrix(raw_data_tensor().at(0));
	std::vector<double> col;
	
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
	std::set<int> corrupted_lines_p=corrupted_lines();
	if (raw_data_tensor().size()==0) return;
	for (std::set<int>::reverse_iterator it=corrupted_lines_p.rbegin();it!=corrupted_lines_p.rend();++it)
	{
        //logger::info(1,"files_::dsims_t::contents_t::remove_corrupted_lines()","line="+ std::to_string(*it), filename_with_path,"erasing line");
		raw_data_tensor_p.at(0).erase(raw_data_tensor_p.at(0).begin()+*it);
	}
}

/*
 * exported data is bugged, there are to many "\t" in the lines, where there is no cluster data, but Ipr data
 */
bool files_::dsims_t::contents_t::ipr_shift_correction() 
{
    log_f;
	raw_data_tensor();
	std::vector<std::vector<std::string>>* mat = &raw_data_tensor_p.at(0);
    if (mat->size()==0) 
	{
        //logger::error("files_::dsims_t::contents_t::ipr_shift_correction()","raw_data_tensor_p.at(0).size()==0",to_string(),"returning false");
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
	std::vector<std::vector<std::string>> Ipr_mat;
	// cut off Ipr from data mat
	Ipr_mat.insert(Ipr_mat.begin(),mat->begin()+Ipr_offset_line,mat->end());	
    // delete first columns within the matrix
    logger.debug("cols_per_element").value(cols_per_element);
    logger.debug("Ipr_offset_line").value(Ipr_offset_line);
    logger.debug("Ipr_col_at_EOL").value(Ipr_col_at_EOL);
    logger.debug("Ipr_col").value(Ipr_col);
    logger.debug("total_lines").value(total_lines);
    logger.debug("dimensions().size()").value(dimensions().size());
    logger.debug("cluster_names().size()").value(cluster_names().size());
	Ipr_mat=tools::mat::transpose_matrix(Ipr_mat);
	Ipr_mat.erase(Ipr_mat.begin(),Ipr_mat.begin()+(Ipr_mat.size()-cluster_names().size()*cols_per_element));
	Ipr_mat=tools::mat::transpose_matrix(Ipr_mat);
	mat->erase(mat->begin()+Ipr_offset_line,mat->end()); 
	// add them together
	mat->insert(mat->end(),Ipr_mat.begin(),Ipr_mat.end());
    return true;
}

const std::map<std::string,std::string>& files_::dsims_t::contents_t::infos_and_settings()
{
	if (infos_and_settings_p.size()>0) return infos_and_settings_p;
	std::string key, value;
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
			infos_and_settings_p.insert(std::pair<std::string,std::string>(key,value));
		}
	}
	return infos_and_settings_p;
}

const std::vector<double> files_::dsims_t::contents_t::infos_and_settings_data(const std::string find_this)
{
	if (infos_and_settings().find(find_this)==infos_and_settings().end()) return {};
	return {tools::str::str_to_double(infos_and_settings().at(find_this))};
}

const tm files_::dsims_t::contents_t::start_date_time()
{
	// Analysis date   03/06/2020    
	// Analysis time   10:43
	// "03/06/2020 10:43"
	std::stringstream time;
	time << infos_and_settings().at("Analysis date") << " " << infos_and_settings().at("Analysis time") << ":00"; 
	return tools::time::string_time_to_tm_structure(time.str(),"%m/%d/%Y %H:%M:%S");
}

const tm files_::dsims_t::contents_t::creation_date_time()
{
	// Creation date   03/06/2020    
	// Creation time   10:43
	// "03/06/2020 10:43"
	std::stringstream time;
	time << infos_and_settings().at("Creation date") << " " << infos_and_settings().at("Creation time") <<":00"; 
	return tools::time::string_time_to_tm_structure(time.str(),"%m/%d/%Y %H:%M:%S");
}

const std::string files_::dsims_t::contents_t::secondary_polarity(const std::string find_this)
{
	if (infos_and_settings().find(find_this)==infos_and_settings().end()) return {};
	return infos_and_settings().at(find_this);
}

const quantity::energy_t files_::dsims_t::contents_t::sputter_energy(const std::string find_this)
{
	if (infos_and_settings().find(find_this)==infos_and_settings().end()) return {};
	double data = tools::str::str_to_double(infos_and_settings().at(find_this));
	return quantity::energy_t(infos_and_settings_data(find_this),{"eV"});
}

const element_t files_::dsims_t::contents_t::sputter_element(const std::string find_this)
{
	if (infos_and_settings().find(find_this)==infos_and_settings().end()) return {};
	std::string ele = infos_and_settings().at(find_this);
	ele.erase(remove(ele.begin(), ele.end(), '+'), ele.end());
	ele.erase(remove(ele.begin(), ele.end(), '-'), ele.end());
	ele.erase(remove(ele.begin(), ele.end(), '2'), ele.end());
	return element_t({ele});
}

const quantity::secondary_voltage_t files_::dsims_t::contents_t::secondary_voltage(const std::string find_this)
{
	return quantity::secondary_voltage_t(infos_and_settings_data(find_this),{"V"});
}

const quantity::rastersize_t files_::dsims_t::contents_t::sputter_rastersize(const std::string find_this)
{
	return quantity::rastersize_t(infos_and_settings_data(find_this),{"um"});
}

const quantity::rastersize_t files_::dsims_t::contents_t::analyzed_area(const std::string find_this)
{
	return quantity::rastersize_t(infos_and_settings_data(find_this),{"um"});
}

const quantity::quantity_t files_::dsims_t::contents_t::chamber_pressure(const std::string find_this)
{	
	return quantity::quantity_t("chamber_pressure",infos_and_settings_data(find_this),{"mbar"});
}

const quantity::quantity_t files_::dsims_t::contents_t::contrast_aperture(const std::string find_this)
{
	return quantity::quantity_t("contrast_aperture",infos_and_settings_data(find_this),{"um"});
}

const quantity::quantity_t files_::dsims_t::contents_t::egate(const std::string find_this)
{
	return quantity::quantity_t("egate",infos_and_settings_data(find_this),{"%"});
}

const quantity::quantity_t files_::dsims_t::contents_t::em_voltage(const std::string find_this)
{
	return quantity::quantity_t("em_voltage",infos_and_settings_data(find_this),{"V"});
}

const quantity::quantity_t files_::dsims_t::contents_t::em_yield(const std::string find_this)
{
	return quantity::quantity_t("em_yield",infos_and_settings_data(find_this),{"%"});
}

const quantity::quantity_t files_::dsims_t::contents_t::entrance_slit(const std::string find_this)
{
	return quantity::quantity_t("entrance_slit",infos_and_settings_data(find_this),{"um"});
}

const quantity::quantity_t files_::dsims_t::contents_t::energy_window(const std::string find_this)
{
	return quantity::quantity_t("energy_window",infos_and_settings_data(find_this),{"eV"});
}

const quantity::quantity_t files_::dsims_t::contents_t::exit_slit(const std::string find_this)
{
	return quantity::quantity_t("exit_slit",infos_and_settings_data(find_this),{"um"});
}

const quantity::quantity_t files_::dsims_t::contents_t::field_aperture(const std::string find_this)
{
	return quantity::quantity_t("field_aperture",infos_and_settings_data(find_this),{"um"});
}

const quantity::quantity_t files_::dsims_t::contents_t::mass_resolution(const std::string find_this)
{
	return quantity::quantity_t("mass_resolution",infos_and_settings_data(find_this),{""});
}

const quantity::quantity_t files_::dsims_t::contents_t::total_acquisition_time(const std::string find_this)
{
	return quantity::quantity_t("total_acquisition_time",infos_and_settings_data(find_this),{"s"});
}

const quantity::quantity_t files_::dsims_t::contents_t::total_sputtering_time(const std::string find_this)
{
	return quantity::quantity_t("total_sputtering_time",infos_and_settings_data(find_this),{"s"});
}

