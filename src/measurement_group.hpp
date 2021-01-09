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

#ifndef MEASUREMENT_GROUP_T_HPP
#define MEASUREMENT_GROUP_T_HPP



#include <set>
#include <vector>
#include <string>
#include <list>
// #include "measurement.hpp"
#include "quantity.hpp"
#include "measurement_settings.hpp"
#include "measurement.hpp"
#include "sample.hpp"

using namespace std;


/* FORWARD DECLARATIONS*/
class sample_t;
namespace measurements
{
	class dsims_t;
}
/************************/

namespace measurement_groups
{

/*
 * there can not be different measurement methods in the same measurement_group.
 * that means: there can only be list<dsims_measurement_t> in dsims_measurement_group_t
 * this is a parent class
 */
class measurement_group_t 
{
protected:
	class definition_t
	{
	public:
		static bool id;
	} definition;
	int id_p;
	string measurement_tool_name_p = "";
public:
// // 	measurement_group_t(measurement_t &measurement_p);
// 	static list<measurement_group_t> measurement_groups(list<measurement_t> &measurements_p);
	int id() const;
    string measurement_tool_name() const;
// 	bool operator==(const measurement_group_t& obj) const;
	// hash function operator
// 	size_t operator()(const measurement_group_t & obj) const;
	bool operator==(const measurement_group_t& obj) const;
	bool operator!=(const measurement_group_t& obj) const;
	bool operator<(const measurement_group_t& obj) const;
};


class sims_t : public measurement_group_t
{
private:
    
public:
};

class dsims_t : public sims_t
{
private:
    string measurement_tool_name_p = "dsims";
	settings::dsims_t settings;
public:
    set<measurements::dsims_t*> measurements;
// 	dsims_measurement_group_t(unordered_set<dsims_measurement_t>& dsims_measurements_p);
// 	static unordered_set<dsims_measurement_group_t> measurement_groups(unordered_set<dsims_measurement_group_t>& MG_set,
// 																	   unordered_set<dsims_measurement_t>& M_set, 
// 																	   unordered_set<sample_t>& sample_set,
// 																	   unordered_set<string>& filenames);
	
	
};

}

#endif // MEASUREMENT_GROUP_T_HPP
