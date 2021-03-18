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


measurements_::dsims_t::dsims_t(files_::dsims_t& dsims_file, 
								list<sample_t>& samples_list,
								vector<files_::jpg_t>* jpg_files,
								vector<files_::profiler_t>* profiler_files
   							) : 
				sims_t(dsims_file.name,dsims_file.contents,samples_list,"dsims",jpg_files,profiler_files), 
				settings(dsims_file.name,dsims_file.contents)
{
}

bool measurements_::dsims_t::operator!=(measurements_::dsims_t& obj)
{
	return !operator==(obj);
}

bool measurements_::dsims_t::operator==(measurements_::dsims_t& obj)
{
	if (measurement_t::operator!=(obj)) return false;
	if (settings!=obj.settings) return false;
	return true;
}

// bool measurements_::dsims_t::operator<(measurements_::dsims_t& obj)
// {
// 	if (measurement_t::operator<(obj)) return true;
// 	if (*this == obj)
// 		if (settings < obj.settings)
// 			return true;
// 	return false;
// }
// 
// 

/*needs sorted dsims_files input*/
// measurements_::dsims_t::dsims_t(vector<files_::dsims_t>& dsims_files, 
// 								list<sample_t>& samples_list, 
// 								vector<files_::jpg_t>* jpg_files) : 
// 				dsims_t(dsims_files.back(),samples_list,jpg_files)	
// {
// 	dsims_files.pop_back(); // --> is *this
// 	
// 	for (int i=dsims_files.size()-1;i>=0;i--)
// // 	for (vector<files_::dsims_t>::reverse_iterator df = dsims_files.rbegin();df!=dsims_files.rend();df++)
// 	{
// 		dsims_t M(dsims_files.at(i),samples_list,jpg_files);
// 		if (*this!=M)	
// 		{
// 			break; //sorted data
// // 			continue; //unsorted data
// 		}
// 		//copy useful information from M into this measurement
// 		if (M.crater.total_sputter_depths.is_set()) 
// 			crater.total_sputter_depths << M.crater.total_sputter_depths; 
// 		if (M.crater.linescans.size()>0)
// 			crater.linescans.insert(crater.linescans.end(),M.crater.linescans.begin(),M.crater.linescans.end());
// 		dsims_files.erase(dsims_files.begin()+i);
// 	}
// }
