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

#ifndef MEASUREMENT_T_HPP
#define MEASUREMENT_T_HPP

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include "sample.hpp"
#include "crater.hpp"
#include "file.hpp"
#include "measurement_group.hpp"


/* FORWARD DECLARATIONS*/
class sample_t;
class measurement_group_t;

namespace measurement_groups
{
	class dsims_t;
}
/************************/


using namespace std;

class measurement_t
	{
// 	friend class config_t;
private:
	///use these figures to diistinguish one measurement from another
	static bool use_olcdb;
	static bool use_group;
	static bool use_settings;
	static bool use_repition;
	static bool use_sample;
	
	///measurements are distinguished by their group -> always true
	///measurements are distinguished by their sample -> always true
	///measurements are distinguished by their reptition ID, that means 2 measurements can not be the same, when they differ in repition ID
	///user specified definition from config file input
// 	static definitions_t user_definitions;
	///all samples loaded across all measurement_groups and measurements
	list<sample_t>* sample_list_global;
	///all files, belonging to this measurement
	list<file_t*> files_p;
public:
	/*ctors*/
	measurement_t(const list<file_t>& files, const measurement_group_t* MG_p);
	/*vars*/
	const measurement_group_t* MG;
	/*functions*/
	///pointer to its corsseponding the sample
	const sample_t* sample() const;
	const list<file_t*>* files() const;
	const int olcdb() const;
	const string repitition() const;
	const string group() const;
	/*static functions*/
	///measurements
	static list<measurement_t> measurements(const list<file_t>& files_s, const measurement_group_t* MG_s=nullptr);
	/*operators*/
	const bool operator==(const measurement_t& obj) const;
	const bool operator!=(const measurement_t& obj) const;
	const bool operator<(const measurement_t& obj) const;
};

namespace measurements
{

class profiler_t : public measurement_t
{
private:
public:
	static list<measurements::profiler_t> measurements(unordered_set<string>* filenames);
};


class sims_t : public measurement_t
{
private:
	crater_t crater;
public:
};


class dsims_t : public measurements::sims_t 
{
private:
public:
    measurement_groups::dsims_t* measurement_group;
	static list<measurements::dsims_t> measurements(unordered_set<string>* filenames);
	static list<measurements::dsims_t> measurements(unordered_set<string>* filenames,set<measurements::profiler_t>& proflier_measurements);
    void test();
};

}
#endif // MEASUREMENT_T_HPP

