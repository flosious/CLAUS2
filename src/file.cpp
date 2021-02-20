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

#include "file.hpp"



/***STATICS***/

// void files::load(vector<string>& filenames_with_path)
// {
// 	files::feed_files_list(filenames_with_path, files::dsims_dp_rpc_asc_t::files_list);
// 	files::feed_files_list(filenames_with_path, files::tofsims_TXT_t::files_list);
// // 	files::feed_files_list(filenames_with_path, files::dsims_jpg_t::files_list);
// // 	files::feed_files_list(filenames_with_path, files::profiler_t::files_list);
// 	
// 	for (auto& f:files::dsims_dp_rpc_asc_t::files_list)
// 		logger::info("recognized dsims_dp_rpc_asc_t",f.name.filename_with_path());
// 	for (auto& f:files::tofsims_TXT_t::files_list)
// 		logger::info("recognized tofsims_TXT_t",f.name.filename_with_path());
// // 	for (auto& f:files::dsims_jpg_t::files_list)
// // 		logger::info("recognized dsims_jpg_t",f.name.filename_with_path());
// // 	for (auto& f:files::profiler_t::files_list)
// // 		logger::info("recognized profiler_t",f.name.filename_with_path());
// }

// const list<file_t*> files::files_list()
// {
// 	list<file_t*> files_;
// 	cout << "in" << endl;
// 	
// 	for (auto& d: files::dsims_dp_rpc_asc_t::files_list)
// 		files_.push_back(&d);
// 	
// 	for (auto& d: files::tofsims_TXT_t::files_list)
// 		files_.push_back(&d);
// 	
// 	for (auto& f:files_)
// 		cout << f->name->filename_with_path() << endl;
// // 	for (auto& d: files::dsims_jpg_t::files_list)
// // 		files.push_back(&d);
// 	
// // 	for (auto& d: files::profiler_t::files_list)
// // 		files.push_back(&d);
// 	
// 	
// 	cout << "OUT" << endl;
// 	return files_;
// }
/*
list<files::tofsims_TXT_t> files::tofsims_TXT_t::files_list;
list<files::dsims_dp_rpc_asc_t> files::dsims_dp_rpc_asc_t::files_list;
list<files::dsims_jpg_t> files::dsims_jpg_t::files_list;
list<files::profiler_t> files::profiler_t::files_list;*/

/***************************/
/********  file_t  *********/
/***************************/
// file_t files::file_t::file() const
// {
// 	return *this;
// }


files::file_t::file_t(const string& filename_with_path_s) : filename_with_path(filename_with_path_s)
{
}

// files::file_t::file_t(name_t name_s, contents_t contents_s) :name(name_s), contents(contents_s)
// {
// // 	logger::info("files::file_t::file_t",filename_with_path_s);
// }

const string files::file_t::creation_date_time() const
{
	time_t cdt = tools::file::creation_date(filename_with_path);
	return tools::time::time_t_to_string(cdt);
}

bool files::file_t::operator<(const file_t& obj) const
{
	if (filename_with_path >= obj.filename_with_path) 
		return false;
	return true;
}

bool files::file_t::operator==(const file_t& obj) const
{
	if (filename_with_path == obj.filename_with_path)
	{
		return true;
	}
	return false;
}


/***************************/
/*** files::file_t::contents_t  ***/
/***************************/

string files::file_t::contents_t::value_by_key(string key)
{
	if (raw_header_tensor().size()==0) return "";
	if (raw_header_tensor()[0].size()==0) return "";
	for (auto& line: raw_header_tensor()[0])
	{
		if (line.size()==0) continue;
		if (line.at(0).find(key)==string::npos) continue;
		string value_string = tools::vec::combine_vec_to_string(line,delimiter);
		tools::str::remove_substring_from_mainstring(&value_string,"=");
		tools::str::remove_substring_from_mainstring(&value_string,key);
		tools::str::remove_spaces_from_string_start(&value_string);
		return value_string;
	}
	return "";
}

matrix_t files::file_t::contents_t::matrix()
{
	return value_by_key("matrix");
// 	if (raw_header_tensor().size()==0) return {};
// 	if (raw_header_tensor()[0].size()==0) return {};
// 	set<element_t> matrix_elements_s;
// 	for (auto& line: raw_header_tensor()[0])
// 	{
// 		if (line.size()==0) continue;
// 		if (line.at(0).find("matrix")==string::npos) continue;
// 		string matrix_string = tools::vec::combine_vec_to_string(line,delimiter);
// 		tools::str::remove_substring_from_mainstring(&matrix_string,"=");
// 		tools::str::remove_substring_from_mainstring(&matrix_string,"matrix");
// 		tools::str::remove_spaces_from_string_start(&matrix_string);
// 		return matrix_t(matrix_string);
// 	}
// 	return {};
}



const string& files::file_t::contents_t::contents_string()
{
	if (contents_p.size()==0)
	{
		
		if (!tools::file::file_exists(filename_with_path_p)) 
		{
			logger::warning("file does not exist - skipping", filename_with_path_p);
			return contents_p;
		}
		contents_p = tools::file::load_file_to_string(filename_with_path_p);
// 		logger::debug("tools::file::load_file_to_string(filename_with_path_p)",filename_with_path_p);;
	}
	return contents_p;
}

files::file_t::contents_t::contents_t(const string& filename_with_path_s,
									  const string delimiter_s,
									  const set< string > identifiers_s ) : 	
															filename_with_path_p(filename_with_path_s),
															delimiter(delimiter_s), 
															identifiers(identifiers_s)
{
}


const bool files::file_t::contents_t::is_correct_type()
{
	if (contents_string()=="") return false;
	for (auto& fci : identifiers)
	{
		if (contents_string().find(fci)==string::npos)
		{
// 			logger::info("files::file_t::contents_t::is_correct_file_type()","FALSE");
			return false;
		}
	}
// 	logger::info("files::file_t::contents_t::is_correct_file_type() ",filename_with_path_p);
	return true;
}

bool files::file_t::contents_t::parse_data_and_header_tensors(vector<vector<vector<std::__cxx11::string> > >* raw_header_tensor, vector<vector<vector<std::__cxx11::string> > >* raw_data_tensor) 
{
	raw_data_tensor->clear();
	raw_header_tensor->clear();
	vector<vector<string>> raw_mat = tools::mat::format_string_to_matrix(contents_string(),LINE_DELIMITER,delimiter);
	
	tools::mat::remove_empty_lines_from_matrix(&raw_mat);
	vector<vector<string> > header_temp, data_temp;
	bool data_scanned=false;
	bool header_scanned=false;
	if (raw_mat.size()<1) return false;
	int check_number;
	for (int i=0;i<raw_mat.size();i++) {
		check_number=1;
		for (int j=0;j<(raw_mat.at(i)).size();j++) {
		  if ((raw_mat.at(i)[j].size()>0) && tools::str::is_number((raw_mat).at(i)[j])!=1) {
			check_number=0;
		  }
		}
		if (check_number==0 || ((raw_mat.at(i).size())==2 && ((raw_mat.at(i)[1].size())==0) )) { // no number, so its a header-part
			header_temp.push_back((raw_mat).at(i));
			header_scanned=true;
			if (data_scanned ) { 
				raw_data_tensor->push_back(data_temp);
				data_temp.clear();
				data_scanned=false;
			}
		} else { // it's a number, so its a data-part
			data_temp.push_back((raw_mat).at(i));
			data_scanned=true;
			if (header_scanned) {
				raw_header_tensor->push_back(header_temp);
				header_temp.clear();
				header_scanned=false;
			}
		}
	}
	if (data_scanned ) { 
		raw_data_tensor->push_back(data_temp);
		data_temp.clear();
		data_scanned=false;
	}
	if (header_scanned) {
		raw_header_tensor->push_back(header_temp);
		header_temp.clear();
		header_scanned=false;
	}
	contents_p.clear();
	return true;
}
vector<vector<vector<std::__cxx11::string> > >& files::file_t::contents_t::raw_header_tensor()
{
	if (raw_header_tensor_p.size()>0) return raw_header_tensor_p;
	
	if (!parse_data_and_header_tensors(&raw_header_tensor_p, &raw_data_tensor_p))
	{
		logger::debug("files::file_t::raw_header_tensor() !parse_data_and_header_tensors(&raw_header_tensor_p, &raw_data_tensor_p)");
	}
	return raw_header_tensor_p;
}
vector<vector<vector<std::__cxx11::string> > >& files::file_t::contents_t::raw_data_tensor()
{

	if (raw_data_tensor_p.size()>0) return raw_data_tensor_p;
	if (!parse_data_and_header_tensors(&raw_header_tensor_p, &raw_data_tensor_p))
	{
		logger::debug("files::file_t::raw_data_tensor() !parse_data_and_header_tensors(&raw_header_tensor_p, &raw_data_tensor_p)");
// 		return {};
	}
	return raw_data_tensor_p;
}

void files::file_t::contents_t::to_screen(string prefix)
{
	cout << prefix << "delimiter:\t'" << delimiter <<"'" << endl;
	if (identifiers.size()==1) cout << prefix << "identifiers:\t" << *identifiers.begin() << endl;
	else cout << prefix << "identifiers:\t<" << identifiers.size() << ">" << endl;
}

/***************************/
/** files::file_t::name_t **/

files::file_t::name_t::name_t(const string& name_with_path_s,const string delimiter_s,const set<string> identifiers_s) : 	filename_with_path_p(name_with_path_s),
																													delimiter(delimiter_s),
																													identifiers(identifiers_s)
{
}

const string files::file_t::name_t::simple_name()
{
	string simple_name_p;
	if ((lot()=="") && not_parseable_filename_parts().size()>0) simple_name_p=tools::vec::combine_vec_to_string(not_parseable_filename_parts(), delimiter);
	else if ((wafer()<0)) simple_name_p=lot() +lot_split() + tools::vec::combine_vec_to_string(not_parseable_filename_parts(), delimiter);
	else simple_name_p="";
	return simple_name_p;
}

const bool files::file_t::name_t::is_correct_type()
{
	if (filename_type_ending()=="") return false;
	for (auto& fti : identifiers)
	{
// 		cout << "name_t fti=" << fti << endl;
		if (fti==filename_type_ending()) 
		{
// 			logger::info("files::file_t::name_t::is_correct_file_type()","TRUE");
			return true;
		}
	}
// 	logger::info("files::file_t::name_t::is_correct_file_type()","FALSE");
	return false;
}

int files::file_t::name_t::chip_x()
{
	parse_filename_parts();
// 	if (chip_x_p<0)
// 		logger::info("files::file_t::chip_x() " ,chip_x_p);
	return chip_x_p;
}
int files::file_t::name_t::chip_y()
{
	parse_filename_parts();
// 	if (chip_y_p<0)
// 		logger::info("files::file_t::chip_y() " +filename(), chip_y_p);
	return chip_y_p;
}
std::__cxx11::string files::file_t::name_t::group()
{
	parse_filename_parts();
	if (group_p=="")
		logger::info("files::file_t::group() " +filename(),group_p);
	return group_p;
}
std::__cxx11::string files::file_t::name_t::lot()
{
	parse_filename_parts();
	if (lot_p=="")
		logger::warning("files::file_t::lot() " +filename(), lot_p);
	return lot_p;
}
std::__cxx11::string files::file_t::name_t::lot_split()
{
	parse_filename_parts();
// 	if (lot_split_p=="")
// 		logger::info("files::file_t::lot_split() " +filename(), lot_split_p);
	return lot_split_p;
}
std::__cxx11::string files::file_t::name_t::monitor()
{
	parse_filename_parts();
// 	if (monitor_p=="")
// 		logger::info("files::file_t::monitor() " +filename(),monitor_p);
	return monitor_p;
}
const int files::file_t::name_t::olcdb()
{
	parse_filename_parts();
	if (olcdb_p<0)
		logger::warning("files::file_t::olcdb() ",olcdb_p);
	return olcdb_p;
}
const vector<std::__cxx11::string>& files::file_t::name_t::not_parseable_filename_parts()
{
	parse_filename_parts();
	return not_parseable_filename_parts_p;
}
int files::file_t::name_t::wafer()
{
	parse_filename_parts();
	if (wafer_p<0)
		logger::warning("files::file_t::wafer() ",wafer_p);
	return wafer_p;
}
std::__cxx11::string files::file_t::name_t::repetition()
{
	parse_filename_parts();
	return repetition_p;
}

void files::file_t::name_t::to_screen(string prefix)
{
	cout << prefix <<"delimiter\t'" << delimiter << "'" << endl;
	if (identifiers.size()==1) cout << prefix <<"identifier\t" << *identifiers.begin()  << endl;
	else cout << prefix <<"identifiers\t<" << identifiers.size() << ">" << endl;
	cout<< prefix << "filename=\t"<< filename()<<endl;
	cout<< prefix << "filename_with_path=\t"<< filename_with_path()<<endl;
	cout << prefix<< "directory=\t"<< directory()<<endl;
	
	cout<< prefix << "wafer=\t"<< wafer()<<endl;
	cout << prefix<< "lot=\t"<< lot()<<endl;
	cout << prefix<< "lot_split=\t"<< lot_split()<<endl;
	cout<< prefix << "chip_x=\t"<< chip_x()<<endl;
	cout << prefix<< "chip_y=\t"<< chip_y()<<endl;
	cout << prefix<< "monitor=\t"<< monitor()<<endl;

	cout << prefix<< "olcdb=\t"<< olcdb()<<endl;
	cout << prefix<< "group=\t"<< group()<<endl;
	cout << prefix<< "repetition=\t"<< repetition()<<endl;
	
	cout << prefix<< "not_parseable_filename_parts:\t<" << not_parseable_filename_parts().size()  << ">" << endl;
}

void files::file_t::name_t::parse_all_parts_at_once()
{
	smatch match;
	regex reg ("^([0-9]{4,})_([A-Z]{1,4}[0-9]{1,5})([#[0-9A-Za-z]*?]*?)_[wW]?([0-9]{1,2})(_.*?)([0-9]*?)([a-z]*?)$"); 
	string f = filename();
	if (regex_search(f,match,reg)) 
	{
		olcdb_p = 	tools::str::str_to_int(match[1]);
		lot_p = 		match[2];
		lot_split_p=	match[3];
		wafer_p = 	tools::str::str_to_int(match[4]);
		group_p = 	match[6];
		repetition_p=	match[7];
		not_parseable_filename_parts_p = tools::str::get_strings_between_delimiter(filename(),delimiter);
		for (auto& m:match)
			not_parseable_filename_parts_p.erase(remove(not_parseable_filename_parts_p.begin(),not_parseable_filename_parts_p.end(),m), not_parseable_filename_parts_p.end());
		logger::info("files::file_t::name_t::parse_all_parts_at_once()");
	}
}

void files::file_t::name_t::parse_filename_parts()
{
	if (parsed_filename_parts) return;
	vector<string> filename_parts = tools::str::get_strings_between_delimiter(filename(),delimiter);
	for (auto filename_part:filename_parts)
	{
		// parse everything just once (except crater depths)
		// THE ORDER IS VERY IMPORTANT
		if (olcdb_p==-1) if(parse_olcdb(filename_part)) continue;
		if (wafer_p==-1) if(parse_wafer(filename_part)) continue;
		if (lot_p=="") if(parse_lot(filename_part)) continue;
		if (chip_x_p==-1)	if(parse_chip(filename_part)) continue;
		if (monitor_p=="") if(parse_monitor(filename_part)) continue;
		if (group_p=="") if(parse_group(filename_part)) continue;
// 		if (parse_crater_depth(filename_part)) continue;
		// no parser worked
		not_parseable_filename_parts_p.push_back(filename_part);
	}
	if (lot_p=="" || olcdb_p==-1 || wafer_p==-1 || group_p=="") parse_all_parts_at_once();
	parsed_filename_parts = true;
// 	if (lot=="" && not_parseable_filename_parts.size()>0) lot=not_parseable_filename_parts[0];
}

bool files::file_t::name_t::parse_monitor(string filename_part) 
{
	smatch match;
	regex reg1 ("^m-*([a-zA-Z0-9]+)$"); 
	regex reg2 ("^monitor-([a-zA-Z0-9]+)$"); 
	regex reg3 ("^[Q|q]-*([a-zA-Z0-9]+)$"); 
	if (regex_search(filename_part,match,reg1) || regex_search(filename_part,match,reg2) || regex_search(filename_part,match,reg3)) 
	{
		monitor_p = match[1];
		return true;
	}
	return false;
}

bool files::file_t::name_t::parse_chip(string filename_part)
{
	smatch match;
	regex reg1 ("^x([0-9]{1,2})y([0-9]{1,2})$", std::regex_constants::icase); 
	regex reg2 ("^chip-([0-9]{1,2})-([0-9]{1,2})$"); 
	regex reg3 ("^c([0-9]{1,2})-([0-9]{1,2})$"); 
	if (regex_search(filename_part,match,reg1) || regex_search(filename_part,match,reg2) || regex_search(filename_part,match,reg3)) 
	{
		chip_x_p = tools::str::str_to_int(match[1]);
		chip_y_p = tools::str::str_to_int(match[2]);
		return true;
	}
	return false;
}

bool files::file_t::name_t::parse_olcdb(string filename_part)
{
	smatch match;
	regex reg ("^([0-9]{4,})$"); 
	if (regex_search(filename_part,match,reg)) 
	{
		olcdb_p = tools::str::str_to_int(filename_part);
		return true;
	}
	return false;
}

bool files::file_t::name_t::parse_lot(string filename_part)
{
	smatch match;
	regex reg ("^([A-Z]{1,4}[0-9]{1,5})([#[0-9A-Za-z]*?]*?)$"); 
	if (regex_search(filename_part,match,reg)) 
	{
		lot_p = match[1];
		lot_split_p = match[2];
		return true;
	}
	return false;
}

bool files::file_t::name_t::parse_wafer(string filename_part)
{
	smatch match;
	regex reg ("^[wW]([0-9]{1,2})$"); 
	if (regex_search(filename_part,match,reg)) 
	{
		wafer_p = tools::str::str_to_int(match[1]);
		return true;
	}
	return false;
}

bool files::file_t::name_t::parse_group(string filename_part)
{
	smatch match;
	regex reg1 ("^g-?*([0-9]+?)([a-z]*?)$"); 
	regex reg2 ("^group-*([0-9]+?)([a-z]*?)$"); 
	if (regex_search(filename_part,match,reg1) || regex_search(filename_part,match,reg2) /*|| regex_search(filename_part,match,reg3)*/) 
	{
		group_p = tools::str::str_to_int(match[1]);
		repetition_p = match[2];
		return true;
	}
	return false;
}

bool files::file_t::name_t::parse_repetitor(string filename_part)
{
	smatch match;
	regex reg1 ("^r-?*([0-9]+?)$"); 
	regex reg2 ("^[rep|repetitor|repetition|repeat]-*([0-9]+?)$"); 
	if (regex_search(filename_part,match,reg1) || regex_search(filename_part,match,reg2) /*|| regex_search(filename_part,match,reg3)*/) 
	{
		repetition_p = match[1];
		return true;
	}
	return false;
}

std::__cxx11::string files::file_t::name_t::filename_type_ending() const
{
	string filename_type_ending_p =tools::file::extract_filetype_ending(filename_with_path_p,".");
// 	if (filename_type_ending_p=="")
// 		logger::error("files::file_t::filename_type_ending()",filename_type_ending_p);
	return filename_type_ending_p;
}

std::__cxx11::string files::file_t::name_t::filename_with_path() const
{
	return filename_with_path_p;
}

const string files::file_t::name_t::filename() const
{
	return tools::file::extract_filename(filename_with_path_p);
}

const string files::file_t::name_t::directory() const
{
	return tools::file::extract_directory_from_filename(filename_with_path_p);
}


/********************/
/** sims_t::name_t **/
/********************/

files::sims_t::name_t::name_t(const string& filename_with_path_s,const string delimiter_s,const set<string> identifiers_s) : 
																	files::file_t::name_t(filename_with_path_s,delimiter_s,identifiers_s)
{
}

/************************/
/** sims_t::contents_t **/
/************************/

files::sims_t::contents_t::contents_t(const string& filename_with_path_s,const string delimiter_s,const set<string> identifiers_s) :
																	files::file_t::contents_t(filename_with_path_s,delimiter_s,identifiers_s)
{
}

/**************/
/*** sims_t ***/
/**************/

files::sims_t::sims_t(const string& filename_with_path_s) : file_t(filename_with_path_s)
{
}

// files::sims_t::sims_t(name_t name_s,contents_t contents_s) : file_t(name_s,contents_s)
// {
// }

const total_sputter_depth_t files::sims_t::name_t::total_sputter_depths()
{
	if (total_sputter_depths_p.size()==0)
	{
		for (auto& filename_part : not_parseable_filename_parts())
		{
			smatch match;
			regex reg ("^([0-9]{2,})(nm|A)$"); 
			if (regex_search(filename_part,match,reg)) 
			{
				string value = match[1];
				string unit = match[2];
				total_sputter_depths_p.push_back({{tools::str::str_to_double(value)},unit});
			}
		}
	}
	total_sputter_depth_t tspd;
	for (total_sputter_depth_t& t : total_sputter_depths_p)
		tspd << t;
	return tspd;
}

const string files::sims_t::name_t::filename_without_crater_depths()
{
	string filename_wo_crater_depths  = tools::file::extract_filename(filename_with_path());
	stringstream remove;
	for (double total_sputter_depth:total_sputter_depths().data())
	{
		remove.str("");
		remove << total_sputter_depth << total_sputter_depths().unit().name();
		tools::str::remove_substring_from_mainstring(&filename_wo_crater_depths ,remove.str() + delimiter);
		tools::str::remove_substring_from_mainstring(&filename_wo_crater_depths ,delimiter+ remove.str());
	}
	return filename_wo_crater_depths;
}

bool files::sims_t::name_t::parse_sputter_energy_element_polarity()
{
	smatch match;
	regex reg;
	for (auto& filename_part : not_parseable_filename_parts())
	{
		reg = ("^([0-9]{1,2})(kV|keV)(O|Cs)(\\+|-)$"); 
		if (regex_search(filename_part,match,reg)) 
		{	
			sputter_energy_p = energy_t{{tools::str::str_to_double(match[1])*1000},{"eV"}};
			sputter_element_p = (match[3]);
			secondary_polarity_p = match[4];
			return true;
		}
	
		reg = ("^([0-9]{2,5})(V|eV)(O|Cs)(\\+|-)$"); 
		if (regex_search(filename_part,match,reg)) 
		{
			sputter_energy_p = energy_t{{tools::str::str_to_double(match[1])},{"eV"}};
			sputter_element_p = (match[3]);
			secondary_polarity_p = match[4];
			return true;
		}
	}
	return false;
}

const energy_t files::sims_t::name_t::sputter_energy()
{
	if (!sputter_energy_p.is_set()) 
		parse_sputter_energy_element_polarity();
	return sputter_energy_p;
}

element_t files::sims_t::name_t::sputter_element()
{
	if (sputter_element_p=="") 
	{
		parse_sputter_energy_element_polarity();
	}
	return sputter_element_p;
}

const string files::sims_t::name_t::secondary_polarity()
{
	if (secondary_polarity_p=="") 
		parse_sputter_energy_element_polarity();
	return secondary_polarity_p;
}

/*****************************************/
/*****       files::profiler_t    ********/
/*****************************************/
