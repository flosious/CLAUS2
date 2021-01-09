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

#ifndef FILES_T_HPP
#define FILES_T_HPP

#include <iostream>
#include "tools.hpp"
#include <regex>
#include <vector>
#include <string>
#include <list>
#include <unordered_set>

using namespace std;

class files_t 
{
	friend class config_t;
private:
	static unordered_set<string> ignore_filename_substrings;
	static bool use_directory_files_list;
	static bool use_wildcards_in_filenames;
	static bool search_sub_directories;

	/// from the input args
    unordered_set<string> files_list;
	/// files that are ignored, defined in config_t
	vector<string> ignored_files;

	/// all the directories where the files from files_list are located in
    unordered_set<string> directory_list();

	/// a list of all files located in all directories from directory_list
    vector<string> all_files_in_directories;

	/// removes files that are ignored, defined in config_t
	void filter_ignored_files(vector<string>* files);

	bool get_all_files_in_directories();
	/// scans the input for wildcards (e.g. "*") and applies them
	bool add_filenames_from_wildcards();
	/// returns all files, asuming filename_regex is regulare expression
	unordered_set<string> wildcards_regex_match(string filename_w_wildcard);
	unordered_set<string> wildcards_substring_match(string filename_w_wildcard);
	/// returns all filenames, which contain a wildcard and removes them from the files_list
	unordered_set<string> filenames_with_wildcard();
public:
	
    /// returns a list of locations (dir+filename), given a filename (or just a part of a name)
    vector<string> get_locations_from_filename(string name);

	/// returns a list of filenames (without type ending), which have the least count of common chars over all filenames
    vector<string> get_filenames_with_maximum_common_chars();
	
    /// populates private parameters
    bool filter(vector<string>& files_list_p);
	bool filter(unordered_set<string>& files_list_p);
	
	
	// all filenames with path as vector
	vector<string> vec();
	
	unordered_set<string> filenames();
};

#endif // FILES_T_HPP
