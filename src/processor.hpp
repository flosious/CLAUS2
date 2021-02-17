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

#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include <chrono>
// #include "hash_functions.hpp"
#include <list>
#include <string>
#include <vector>
#include "tools.hpp"
#include "file.hpp"
#include "sample.hpp"
// #include "measurement_group.hpp"
// #include "measurement.hpp"
// #include <unordered_set>
// #include "quantity.hpp"

using namespace std;


class processor 
{
private:
	///feeds the typenames corresponding files_list --> use "load()" for all types at once
	///clears the recognized(loadable) filenames_with_path_s entry from the list
	template <typename T>
	void feed_files_list(vector<string>& filenames_with_path_s, list<T>& files_s)
	{
		for (int i=0;i<filenames_with_path_s.size();i++)
		{
			T file(filenames_with_path_s.at(i));
			if (file.name.is_correct_type() && file.contents.is_correct_type())
			{
				files_s.push_back(file);
				//remove from "filenames_with_path_s" vector
				filenames_with_path_s.at(i)="";
			}
		}
	}
	
	template <typename T>
	void feed_samples_list(list<T>& files,list<sample_t>& samples)
	{
		bool found_in_list;
		for (auto& file: files)
		{
			sample_t sample(file.name,file.contents);
			found_in_list=false;
			for (auto& s_in_list : samples)
			{
				if (s_in_list==sample)
				{
					found_in_list=true;
					s_in_list.filenames.insert(&file.name);
					s_in_list.filecontents.insert(&file.contents);
					samples.push_back(sample);
				}
			}
			if (!found_in_list) samples.push_back(sample);
		}
	}
public:
// 	list<file_t> files;
// 	list measurement_groups
// 	list samples
// 	vector<string> input_filenames;
	
	///samples across all MGs
// 	list<sample_t> samples;
	processor(vector<string> args_p);
};


#endif // PROCESSOR_HPP
