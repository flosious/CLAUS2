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

#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include <chrono>
// #include "hash_functions.hpp"
#include <list>
#include <string>
#include <vector>

#include "tools.hpp"
// #include "sample.hpp"
// #include "lists.hpp"
#include "mgroup.hpp"
// #include "measurement_group.hpp"
// #include "measurement.hpp"
// #include <unordered_set>
// #include "quantity.hpp"
#include "files.hpp"

using namespace std;


class processor 
{
private:
	vector<string> filenames;
	
	/*SAMPLES*/
	list<sample_t> samples_list;
	
	/*FILES*/
	vector<files::jpg_t> dsims_jpg_files_p;
	vector<files::jpg_t> tofsims_jpg_files_p;
	
	vector<files::profiler_t> dsims_profiler_files_p;
	vector<files::profiler_t> tofsims_profiler_files_p;
	
	vector<files::dsims_t> dsims_files_p;
	/*MEASUREMENTS*/
	vector<measurements_::profiler_t> dsims_profiler_measurements_p;
	vector<measurements_::profiler_t> tofsims_profiler_measurements_p;
	vector<measurements_::dsims_t> dsims_measurements_p;
	
	/*MGROUPS*/
	vector<mgroups::dsims_t> dsims_mgroups_p;
	
	void populate_jpg_files();
	void populate_profiler_files();
public:
	processor(vector<string> args_p);
	/*FILENAMES*/
	vector<files::jpg_t>& dsims_jpg_files();
	vector<files::jpg_t>& tofsims_jpg_files();
	
	vector<files::profiler_t>& dsims_profiler_files();
	vector<files::profiler_t>& tofsims_profiler_files();
	/*MEASUREMENTS*/
	vector<measurements_::profiler_t>& dsims_profiler_measurements();
	vector<measurements_::profiler_t>& tofsims_profiler_measurements();
	
	/*DSIMS*/
	///removes matching filenames and populates dsims_files_p
	vector<files::dsims_t>& dsims_files();
	///removes matching dsims_files_p and populates dsims_measurements_p
	vector<measurements_::dsims_t>& dsims_measurements();
	
	/*MGROUPS*/
	vector<mgroups::dsims_t>& dsims_mgroups();
};


#endif // PROCESSOR_HPP
