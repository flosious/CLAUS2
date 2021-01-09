/*
	Copyright (C) 2020 Florian Bärwolf
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

unordered_set<string> files_t::ignore_filename_substrings;
bool files_t::use_wildcards_in_filenames = false;
bool files_t::use_directory_files_list = false;
bool files_t::search_sub_directories = false;


unordered_set<std::__cxx11::string> files_t::filenames()
{
	vector<string> files_vec = vec();
	unordered_set<string> files(files_vec.begin(),files_vec.end());
	return files;
}

vector<string> files_t::vec()
{
	vector<string> files;
	if (use_wildcards_in_filenames) add_filenames_from_wildcards(); 
	if (use_directory_files_list) files = all_files_in_directories;
	else files = vector<string>(files_list.begin(),files_list.end());
	filter_ignored_files(&files);
	return files;
}

unordered_set<string> files_t::wildcards_regex_match(string filename_w_wildcard)
{
	unordered_set<string> files;
	smatch match;
	regex filename_with_wildcard (filename_w_wildcard); 
	for (auto file:all_files_in_directories)
	{
		if (regex_match(file,match,filename_with_wildcard))
		{
			files.insert(file);
		}
	}
	return files;
}

unordered_set<string> files_t::wildcards_substring_match(string filename_w_wildcard)
{
	string filename_substring = tools::str::remove_chars_from_string(filename_w_wildcard,"*");
	unordered_set<string> files;
	for (auto file:all_files_in_directories)
	{
		if (file.find(filename_substring)!=string::npos)
		{
			files.insert(file);
		}
	}
	return files;
}

unordered_set<string> files_t::filenames_with_wildcard()
{
	unordered_set<string> filenames_with_wildcard;
	for (string file:files_list)
	{
		if (file.find("*")!=string::npos)
		{
			filenames_with_wildcard.insert(file);
			files_list.erase(file);
		}
	}

	return filenames_with_wildcard;
}

bool files_t::add_filenames_from_wildcards()
{
	for (auto& filename_w_wildcard:filenames_with_wildcard())
	{
		unordered_set<string> add_these_filenames = wildcards_substring_match(filename_w_wildcard);
		files_list.insert(add_these_filenames.begin(),add_these_filenames.end());
	}
	return true;

}

void files_t::filter_ignored_files(vector<string>* files)
{
	for (auto const& ignore_sub : ignore_filename_substrings)
	{
		if (ignore_sub.length()>1)	tools::vec::add(&ignored_files,tools::str::find_all_substrings(files,ignore_sub));
	}
	for (auto const& ignore_file : ignored_files)
	{
		int del = tools::mat::find_in_vec(files,ignore_file);
		if (del > -1) files->erase(files->begin()+del);
	}
}

bool files_t::filter(vector<string>& files_list_p) {
    files_list = unordered_set<string>(files_list_p.begin(),files_list_p.end());
	return filter(files_list);
}



bool files_t::filter(unordered_set<std::__cxx11::string>& files_list_p)
{
	/*file input name is directory, add all files in directory to file_list*/
	unordered_set<string> files_list_add = files_list_p;
	cout << "A\t" << files_list_add.size() << endl;
	bool search = true;
	do
	{
		search=false;
		unordered_set<string> files_list_temp;
		for (auto& dir:files_list_add)
		{
			if (tools::file::directory_exists(dir))
			{
				vector<string> file_list_from_dir_name = tools::file::get_file_list_from_dir_name(dir,true);
				files_list_temp.insert(file_list_from_dir_name.begin(),file_list_from_dir_name.end());
				search=true;
			}
			else cout << "dir does not exist!" << endl;
		}
		files_list_add = files_list_temp;
		cout <<"B\t" << files_list_add.size() << endl;
		files_list.insert(files_list_add.begin(),files_list_add.end());
		cout << "C\t" <<  files_list.size() << endl;
	}
	while (search && search_sub_directories);
	
	if (use_directory_files_list || use_wildcards_in_filenames)
		get_all_files_in_directories();
	
	return true;
}


vector<string> files_t::get_locations_from_filename(string filename)
{
	vector<string> locations;

	if (use_directory_files_list) locations = tools::str::find_all_substrings(&all_files_in_directories,filename);
	else
	{
		vector<string> files_list_vec (files_list.begin(),files_list.end());
		locations = tools::str::find_all_substrings(&files_list_vec,filename);
	}
	return locations;
}

vector<string> files_t::get_filenames_with_maximum_common_chars()
{
	vector<string> files_list_wo_type;

	if (use_directory_files_list) files_list_wo_type = all_files_in_directories;
	else files_list_wo_type = vector<string> (files_list.begin(),files_list.end()); // just use the input files
	
	tools::file::remove_file_type_from_name(&files_list_wo_type);
	files_list_wo_type = tools::str::remove_similarities(&files_list_wo_type);
	
	return files_list_wo_type;
}

bool files_t::get_all_files_in_directories() 
{
	if (directory_list().size()==0) return false;
	for (auto& directory:directory_list()) 
	{
		vector<string> file_list_from_dir_name = tools::file::get_file_list_from_dir_name(directory,true);
		tools::vec::add(&all_files_in_directories,file_list_from_dir_name);
    }
    tools::str::remove_duplicates(&all_files_in_directories);
	return true;
}

unordered_set<string> files_t::directory_list() 
{
	unordered_set<string> directory_list_p;
	if (files_list.size()==0) return {};
	for (auto& file:files_list)
	{
		directory_list_p.insert(tools::file::extract_directory_from_filename(file));
	}
	return directory_list_p;
}
























