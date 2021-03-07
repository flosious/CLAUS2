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
#include <chrono>

measurements_::dsims_t::dsims_t(files::dsims_t& dsims_file, 
								list<sample_t>& samples_list,
								vector<files::jpg_t>* jpg_files,
								vector<files::profiler_t>* profiler_files) : 
				sims_t(dsims_file.name,dsims_file.contents,samples_list,jpg_files, profiler_files), 
				settings(dsims_file.name,dsims_file.contents)
{
}

// measurements_::dsims_t::dsims_t(vector<files::dsims_t>& dsims_files, 
// 								list<sample_t>& samples_list, 
// 								vector<files::jpg_t>* jpg_files, 
// 								vector<files::profiler_t>* profiler_files) : 
// 				dsims_t(dsims_files.back(),samples_list,jpg_files)
// {
// // 	vector<files::dsims_t> not_selected(dsims_files.size()-1);
// 	dsims_files.pop_back(); // --> is *this
// 	//starting from end of vector maybe more efficient (if last element will be erased)
// // 	for (vector<files::dsims_t>::reverse_iterator df = dsims_files.rbegin();df!=dsims_files.rend();df++)
// 	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
// 	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
// 	vector<dsims_t> Ms;
// 	for (auto& F : dsims_files)
// 		Ms.push_back({F,samples_list});
// 	
// 	
// 	for (vector<files::dsims_t>::iterator df = dsims_files.begin();df!=dsims_files.end();df++)
// 	{
// 		end = std::chrono::steady_clock::now();
// 		std::cout << "\t\tM deltaT\t" << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
// 		begin = std::chrono::steady_clock::now();
// 		dsims_t M(*df,samples_list);
// 		if (*this!=M)	
// 		{
// 			continue;
// 		}
// 		//copy useful information from M into this measurement
// 		crater.total_sputter_depths << M.crater.total_sputter_depths; 
// 		dsims_files.erase(df);
// 		df--; // or df++ ?
// 	}
// // 	dsims_files = not_selected;
// }
