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
#include <sstream>
#include "../quantity.hpp"
#include "../msettings/msettings.hpp"
// #include "sample.hpp"
#include "../definitions.hpp"
#include "../files/file.hpp"
#include "../measurements/measurement.hpp"


using namespace std;


/* FORWARD DECLARATIONS*/
namespace measurements
{
	class dsims_t;
	class measurement_t;
	class sims_t;
	class tofsims_t;
}

/************************/


namespace mgroups
{
	/*
	* there can not be different measurement methods in the same measurement_group.
	* that means: there can only be list<dsims_measurement_t> in dsims_measurement_group_t
	* this is a parent class
	*/
	class mgroup_t 
	{
	// 	friend class config_t;
	protected:
		static bool use_olcdb;
		static bool use_group;
		static bool use_settings;
	protected:
		///measurements belonging to this group
		
	public:
		string to_string();
		void insert_measurement(measurements::measurement_t* M_p);
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
		/*ctors*/
		mgroup_t(measurements::measurement_t* measurement);
		/*functions*/
		set<measurements::measurement_t*> measurements;
		/*static functions*/
		/*operators*/
		bool operator==(mgroup_t& obj);
		bool operator!=(mgroup_t& obj);
	};

	class sims_t: public mgroup_t
	{
	private:
	protected:
		
	public:
		string to_string();
		set<measurements::sims_t*> measurements;
		void insert_measurement(measurements::sims_t* M_p);
		sims_t(measurements::sims_t* measurement);
		msettings::sims_t settings();
		bool operator==(sims_t& obj);
		bool operator!=(sims_t& obj);
	};

	class dsims_t: public sims_t
	{
	private:
		
	public:
		string to_string();
		msettings::dsims_t settings();
		set<measurements::dsims_t*> measurements;
		void insert_measurement(measurements::dsims_t* M_p);
		dsims_t(measurements::dsims_t* measurement);
	};
	
	class tofsims_t: public sims_t
	{
	private:
		
	public:
		string to_string();
		set<measurements::tofsims_t*> measurements;
		void insert_measurement(measurements::tofsims_t* M_p);
		tofsims_t(measurements::tofsims_t* measurement);
	};
}


#endif // MEASUREMENT_GROUP_T_HPP
