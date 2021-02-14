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


namespace measurements
{
	class sims_t;
	class dsims_t;
	class tofsims_t;
}

namespace measurement_groups
{
	class dsims_t;
	class tofsims_t;
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
	protected:
		/*ctors*/
	measurement_t(const list<file_t>& files, const measurement_group_t* MG_p);
public:
	
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
	///standard sims template
	///can only be instanziated by friends
	class sims_t : public measurement_t
	{
	private:
		crater_t crater;
	public:
	};

	class profiler_t : public measurement_t
	{
	private:
		static vector<profiler_t> measurements_p;
	public:
		static vector<profiler_t>* measurements();
	};


	class dsims_t : public sims_t 
	{
	private:
		static vector<dsims_t> measurements_p;
	public:
		measurement_groups::dsims_t* measurement_group;
		void test();
		static vector<dsims_t>* measurements();
	};

	class tofsims_t : public sims_t 
	{
	private:
		static vector<tofsims_t> measurements_p;
	public:
		measurement_groups::tofsims_t* measurement_group;
		void test();
		static vector<tofsims_t>* measurements();
	};
}

#endif // MEASUREMENT_T_HPP

