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

#ifndef LISTS_HPP
#define LISTS_HPP

#include <list>
#include "file.hpp"
#include "sample.hpp"
#include "measurement.hpp"
#include "measurement_group.hpp"

using namespace std;

/*
 * storing the actual data in lists, so their pointers are preserved
 * this is neccessary, because each measurement, group, sample 
 * and some of its members have to be able to access non-copies
 * of other measurements, samples or groups
 * in that way, CPU time and RAM is saved ... I think ...
 */
namespace lists
{
// 	static list<sample_t> samples;

	
	
	
// 	template <typename T>
// 	void feed_samples_list(T& file)
// 	{
// 		sample_t sample(file.name,file.contents);
// 		for (auto& s_in_list : samples)
// 		{
// 			if (s_in_list==sample)
// 			{
// 				s_in_list.filenames.insert(&file.name);
// 				s_in_list.filecontents.insert(&file.contents);
// 				samples.push_back(sample);
// 				return;
// 			}
// 		}
// 		samples.push_back(sample);
// 	}
	
	template <typename T>
	void feed_samples_list(list<T>& files)
	{
		for (auto& f: files)
			feed_samples_list(f);
	}

	///erase_filename will clear filename after successfull feeding
	template <typename T>
	void feed_files_list(string& filename_with_path_s, list<T>& files_s, bool erase_filename=false)
	{
		T file(filename_with_path_s);
		if (file.name.is_correct_type() && file.contents.is_correct_type())
		{
			files_s.push_back(file);
			if (erase_filename) filename_with_path_s="";
		}
	}
	
	///erase_filename will clear filename after successfull feeding
	template <typename T>
	void feed_files_list(vector<string>& filenames_with_path_s, list<T>& files_s, bool erase_filename=false)
	{
		for (auto& f : filenames_with_path_s)
			feed_files_list(f, files_s, erase_filename);
	}
}

#endif // LISTS_HPP
