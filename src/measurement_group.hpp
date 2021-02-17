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
// #include "sample.hpp"
#include "definitions.hpp"
#include "file.hpp"


using namespace std;


/* FORWARD DECLARATIONS*/
// class sample_t;
// class sample_t::chip_t;
class measurement_t;
namespace measurements
{
	class dsims_t;
}
/************************/


/*
 * there can not be different measurement methods in the same measurement_group.
 * that means: there can only be list<dsims_measurement_t> in dsims_measurement_group_t
 * this is a parent class
 */
class measurement_group_t 
{
// 	friend class config_t;
private:
	static bool use_olcdb;
	static bool use_group;
	static bool use_settings;
protected:
	///all loaded samples across all groups & measurements
// 	const list<sample_t>* samples_global;
	///all loaded measurements across all groups
	const list<measurement_t>* measurements_global;
	///measurements belonging to this group
	vector<measurement_t*> measurements_p;
public:
	/*const defitions*/
	///"51087" from filename: 51087_SJZ307#A#B_w17_X1Y5_mQ_13kVCs-_g5q.dp_rpc_asc
	const int olcdb() const;
	/// "17" from filename: 51087_SJZ307#A#B_w17_X1Y5_mQ_13kVCs-_g5q.dp_rpc_asc
// 	const int wafer;
	///"SJZ307" from filename: 51087_SJZ307#A#B_w17_X1Y5_mQ_13kVCs-_g5q.dp_rpc_asc
// 	const string lot;
	///"#A#B" from filename: 51087_SJZ307#A#B_w17_X1Y5_mQ_13kVCs-_g5q.dp_rpc_asc
// 	const string lot_split;
	/// X="1", Y="5" from filename: 51087_SJZ307#A#B_w17_X1Y5_mQ_13kVCs-_g5q.dp_rpc_asc
// 	const sample_t::chip_t chip;
	///"Q" from filename: 51087_SJZ307#A#B_w17_X1Y5_mQ_13kVCs-_g5q.dp_rpc_asc
// 	const string monitor;
	///multiple measurements of same sample use this: "q" from filename: 51087_SJZ307#A#B_w17_X1Y5_mQ_13kVCs-_g5q.dp_rpc_asc
// 	const string repetition;
	///measurement group identifier: "5" from filename: 51087_SJZ307#A#B_w17_X1Y5_mQ_13kVCs-_g5q.dp_rpc_asc
	const string group() const;
	///"d-sims", "tof-sims", "xps", "profiler", ...
	const string measurement_method_name;
	/*ctors*/
// 	measurement_group_t(const int& olcdbID_s, 
// 						const int& wafer_s,
// 						const string& lot_s, 
// 						const string& lod_split_s, 
// 						const sample_t::chip_t chip_s, 
// 						const string& monitor_s, 
// 						const string& repetition_s, 
// 						const string& groupID_s, 
// 						const list<measurement_t>* measurements_global, 
// 						const list<sample_t>* samples_global,
// 						const string measurement_method_name_s );
	measurement_group_t(const list<measurement_t>* measurements_global);
	/*functions*/
	const vector<measurement_t*>& measurements() const;
	/*static functions*/
	static list<measurement_group_t> measurement_groups(const list<files::file_t>& files_global, const list<measurement_t>& measurements_global);
	/*operators*/
	const bool operator==(const measurement_group_t& obj) const;
	const bool operator!=(const measurement_group_t& obj) const;
	const bool operator<(const measurement_group_t& obj) const;
};


namespace groups
{
	class dsims_t:measurement_group_t
	{
	private:
		
	public:
		dsims_t(const list<measurement_t>* measurements_global);
	};
}


#endif // MEASUREMENT_GROUP_T_HPP
