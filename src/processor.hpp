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
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <set>

#include <mgl2/mgl.h>
#include <mgl2/fltk.h>
// #include <omp.h>
#include "tools.hpp"
// #include "sample.hpp"
// #include "lists.hpp"
#include "mgroup.hpp"
// #include "measurement_group.hpp"
// #include "measurement.hpp"
// #include <unordered_set>
#include "quantity.hpp"
#include "files.hpp"
#include "plot.hpp"
#include "database_t.hpp"
#include "calc.hpp"

// #include "plc++demos.h"


using namespace std;


class processor 
{
private:
	sqlite3* sql = nullptr;
	database_t sql_wrapper;
	/*SAMPLES*/
	class camera_t
	{
	private:
		vector<string>& filenames;
		vector<files_::jpg_t> files_p;
		database_t& sql_wrapper;
	public:
		camera_t(vector<string>& filenames,database_t& sql_wrapper );
		vector<files_::jpg_t>& files();
	};
	
	class profiler_t
	{
	private:
		vector<string>& filenames;
		vector<files_::profiler_t> files_p;
		vector<measurements_::profiler_t> measurements_p;
		list<sample_t>& samples_list;
		database_t& sql_wrapper;
	public:
		profiler_t(vector<string>& filenames, list<sample_t>& samples_list,database_t& sql_wrapper);
		vector<files_::profiler_t>& files();
		vector<measurements_::profiler_t>& measurements();
	};
	
	class dsims_t
	{
	private:
		vector<string>& filenames;
		vector<files_::dsims_t> files_p;
		vector<measurements_::dsims_t> measurements_p;
		vector<mgroups_::dsims_t> mgroups_p;
		list<sample_t>& samples_list;
		profiler_t& profiler;
		camera_t& camera;
		database_t& sql_wrapper;
	public:
		dsims_t(vector<string>& filenames, list<sample_t>& samples_list, profiler_t& profiler, camera_t& cam, database_t& sql_wrapper);
		vector<files_::dsims_t>& files();
		vector<measurements_::dsims_t>& measurements();
		vector<mgroups_::dsims_t>& mgroups();
	};
	
	class tofsims_t
	{
	private:
		vector<string>& filenames;
		vector<files_::tofsims_t> files_p;
		vector<measurements_::tofsims_t> measurements_p;
		vector<mgroups_::tofsims_t> mgroups_p;
		list<sample_t>& samples_list;
		profiler_t& profiler;
		camera_t& camera;
		database_t& sql_wrapper;
	public:
		tofsims_t(vector<string>& filenames, list<sample_t>& samples_list, profiler_t& profiler, camera_t& cam, database_t& sql_wrapper);
		vector<files_::tofsims_t>& files();
		vector<measurements_::tofsims_t>& measurements();
		vector<mgroups_::tofsims_t>& mgroups();
	};
		
private:
	vector<string> filenames;
public:
	processor(vector<string> args_p);
};

// extern database_t db;

#endif // PROCESSOR_HPP
