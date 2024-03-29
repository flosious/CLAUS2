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

files_::file_t::contents_t::contents_t(std::string& filename_with_path,
                                       const std::string& delimiter,
                                       const std::set<std::string>& identifiers,
                                       std::vector<unsigned int> delete_cols_before_parsing,
                                       const std::string& contents_s) :
    filename_with_path(filename_with_path), delimiter(delimiter),
    identifiers(identifiers),
    logger(__FILE__,"files_::file_t::contents_t"),
    delete_cols_before_parsing(delete_cols_before_parsing),
    contents_p(contents_s)
{
}

void files_::file_t::contents_t::load_file_to_contents_p()
{
    if (!tools::file::file_exists(filename_with_path))
    {
        logger.error(__func__,filename_with_path).signal("does not exist");
        //logger::error("files_::file_t::contents_t::contents_string()", "!tools::file::file_exists(filename_with_path)", filename_with_path,"skipping file");
        return;
    }
    contents_p = tools::file::load_file_to_string(filename_with_path);
}

const std::string files_::file_t::contents_t::creation_date_time() const
{
	time_t cdt = tools::file::creation_date(filename_with_path);
	return tools::time::time_t_to_string(cdt);
}

bool files_::file_t::contents_t::operator<(const files_::file_t::contents_t& obj) const
{
	if (filename_with_path >= obj.filename_with_path) 
		return false;
	return true;
}

bool files_::file_t::contents_t::operator==(const files_::file_t::contents_t& obj) const
{
	if (filename_with_path == obj.filename_with_path)
	{
		return true;
	}
	return false;
}

std::string files_::file_t::contents_t::value_by_key(std::string key)
{
	if (raw_header_tensor().size()==0) return "";
	if (raw_header_tensor()[0].size()==0) return "";
	for (auto& line: raw_header_tensor()[0])
	{
		if (line.size()==0) continue;
        if (line.at(0).find(key)==std::string::npos) continue;
		std::string value_string = tools::vec::combine_vec_to_string(line,delimiter);
		tools::str::remove_substring_from_mainstring(&value_string,"=");
		tools::str::remove_substring_from_mainstring(&value_string,key);
		tools::str::remove_spaces_from_string_start(&value_string);
		return value_string;
	}
	return "";
}

std::string files_::file_t::contents_t::matrix()
{
	return value_by_key("matrix");
}

const std::string& files_::file_t::contents_t::contents_string()
{

	return contents_p;
}

bool files_::file_t::contents_t::is_correct_type()
{
    if (contents_string()=="")
    {
        load_file_to_contents_p();
    }
	for (auto& fci : identifiers)
	{
        if (contents_string().find(fci)==std::string::npos)
		{
			return false;
		}
	}
	return true;
}

bool files_::file_t::contents_t::parse_data_and_header_tensors(std::vector<std::vector<std::vector<std::string> > >* raw_header_tensor, std::vector<std::vector<std::vector<std::string> > >* raw_data_tensor) 
{
	raw_data_tensor->clear();
	raw_header_tensor->clear();
	std::vector<std::vector<std::string>> raw_mat = tools::mat::format_string_to_matrix(contents_string(),LINE_DELIMITER,delimiter);
    if (delete_cols_before_parsing.size()>0)
    {
        auto raw_mat_transposed = tools::mat::transpose_matrix(raw_mat);
        tools::vec::erase(raw_mat_transposed,delete_cols_before_parsing);
        logger.info(__func__,"delete_cols_before_parsing").signal("deleted");
        raw_mat = tools::mat::transpose_matrix(raw_mat_transposed);
    }

	tools::mat::remove_empty_lines_from_matrix(&raw_mat);
	std::vector<std::vector<std::string> > header_temp, data_temp;
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
std::vector<std::vector<std::vector<std::string> > >& files_::file_t::contents_t::raw_header_tensor()
{
	if (raw_header_tensor_p.size()>0) return raw_header_tensor_p;
	
	if (!parse_data_and_header_tensors(&raw_header_tensor_p, &raw_data_tensor_p))
	{
        //logger::debug(33,"files_::file_t::contents_t::raw_header_tensor()", "!parse_data_and_header_tensors(&raw_header_tensor_p, &raw_data_tensor_p)","no header");
	}
	return raw_header_tensor_p;
}
std::vector<std::vector<std::vector<std::string> > >& files_::file_t::contents_t::raw_data_tensor()
{
	if (raw_data_tensor_p.size()>0) return raw_data_tensor_p;
	if (!parse_data_and_header_tensors(&raw_header_tensor_p, &raw_data_tensor_p))
	{
// 		logger::debug("files_::file_t::contents_t::raw_data_tensor() !parse_data_and_header_tensors(&raw_header_tensor_p, &raw_data_tensor_p)");
        //logger::debug(33,"files_::file_t::contents_t::raw_data_tensor()", "!parse_data_and_header_tensors(&raw_header_tensor_p, &raw_data_tensor_p)","no data");
// 		return {};
	}
	return raw_data_tensor_p;
}

void files_::file_t::contents_t::to_screen(std::string prefix)
{
	std::cout << prefix << "delimiter:\t'" << delimiter <<"'" << std::endl;
	if (identifiers.size()==1) std::cout << prefix << "identifiers:\t" << *identifiers.begin() << std::endl;
	else std::cout << prefix << "identifiers:\t<" << identifiers.size() << ">" << std::endl;
}

std::string files_::file_t::contents_t::to_string(const std::string del)
{
	std::stringstream out;
	out << "contents_string().size()="<< contents_string().size() << del;
	out << "creation_date_time(): " << creation_date_time();
	return out.str();
}

