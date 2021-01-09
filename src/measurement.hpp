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
namespace measurement_groups
{
	class dsims_t;
}
/************************/


using namespace std;

namespace measurements
{
/*
 * this is a parent class
 */
class measurement_t
{
protected:
	class definition_t
	{
	public:
		static bool olcdb;
		static bool repetition;
		static bool sample;
	} definition;
	
	int olcdb_p;
	string repetition_p;
	
public:
	int olcdb() const;
	string repetition() const;
	
	
	sample_t* sample;
	
	bool operator==(const measurement_t& obj) const;
	bool operator!=(const measurement_t& obj) const;
	bool operator<(const measurement_t& obj) const;
};

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

