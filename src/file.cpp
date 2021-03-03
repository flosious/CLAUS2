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


// files::file_t::file_t(const string& filename_with_path_s) : filename_with_path(filename_with_path_s)
// {
// }

files::file_t::file_t(filenames::filename_t& name,const string& delimiter,const set<string>& identifiers) : name(name), delimiter(delimiter),identifiers(identifiers)
{
}

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
/*** files::file_t  ***/
/***************************/

string files::file_t::value_by_key(string key)
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

matrix_t files::file_t::matrix()
{
	return value_by_key("matrix");
}

const string& files::file_t::contents_string()
{
	if (contents_p.size()==0)
	{
		
		if (!tools::file::file_exists(name.filename_with_path())) 
		{
			logger::warning("file does not exist - skipping", name.filename_with_path());
			return contents_p;
		}
		contents_p = tools::file::load_file_to_string(name.filename_with_path());
// 		logger::debug("tools::file::load_file_to_string(filename_with_path_p)",filename_with_path_p);;
	}
	return contents_p;
}

const bool files::file_t::is_correct_type()
{
	if (contents_string()=="") return false;
	for (auto& fci : identifiers)
	{
		if (contents_string().find(fci)==string::npos)
		{
// 			logger::info("files::file_t::is_correct_file_type()","FALSE");
			return false;
		}
	}
// 	logger::info("files::file_t::is_correct_file_type() ",filename_with_path_p);
	return true;
}

bool files::file_t::parse_data_and_header_tensors(vector<vector<vector<std::__cxx11::string> > >* raw_header_tensor, vector<vector<vector<std::__cxx11::string> > >* raw_data_tensor) 
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
vector<vector<vector<std::__cxx11::string> > >& files::file_t::raw_header_tensor()
{
	if (raw_header_tensor_p.size()>0) return raw_header_tensor_p;
	
	if (!parse_data_and_header_tensors(&raw_header_tensor_p, &raw_data_tensor_p))
	{
		logger::debug("files::file_t::raw_header_tensor() !parse_data_and_header_tensors(&raw_header_tensor_p, &raw_data_tensor_p)");
	}
	return raw_header_tensor_p;
}
vector<vector<vector<std::__cxx11::string> > >& files::file_t::raw_data_tensor()
{

	if (raw_data_tensor_p.size()>0) return raw_data_tensor_p;
	if (!parse_data_and_header_tensors(&raw_header_tensor_p, &raw_data_tensor_p))
	{
		logger::debug("files::file_t::raw_data_tensor() !parse_data_and_header_tensors(&raw_header_tensor_p, &raw_data_tensor_p)");
// 		return {};
	}
	return raw_data_tensor_p;
}

void files::file_t::to_screen(string prefix)
{
	cout << prefix << "delimiter:\t'" << delimiter <<"'" << endl;
	if (identifiers.size()==1) cout << prefix << "identifiers:\t" << *identifiers.begin() << endl;
	else cout << prefix << "identifiers:\t<" << identifiers.size() << ">" << endl;
}

std::__cxx11::string files::file_t::to_string()
{
	return "";
}




/*****************************************/
/*****       files::profiler_t    ********/
/*****************************************/
