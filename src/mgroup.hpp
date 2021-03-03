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
#include "quantity.hpp"
#include "msettings.hpp"
// #include "sample.hpp"
#include "definitions.hpp"
#include "file.hpp"
#include "measurement.hpp"


using namespace std;


/* FORWARD DECLARATIONS*/
/************************/


class mgroups
{
public:
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
		mgroup_t(filenames::filename_t& fn, files::file_t& f, list<sample_t>& samples_list);
	public:
// 		mgroup_t();
		
// 		bool is_set() const;
// 		mgroup_t();
		string to_string(const string del=", ");
// 		void insert_measurement(measurements::measurement_t* M_p);
		/*const defitions*/
		///"51087" from filename: 51087_SJZ307#A#B_w17_X1Y5_mQ_13kVCs-_g5q.dp_rpc_asc
		const int olcdb;
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
		const string group;
		///"d-sims", "tof-sims", "xps", "profiler", ...
		/*ctors*/

		/*functions*/

		/*static functions*/
		/*operators*/
		bool operator==( const mgroup_t& obj) const;
		bool operator!=( const mgroup_t& obj) const;
	};

	class sims_t: public mgroup_t
	{
	private:
	protected:
		sims_t(filenames::sims_t& fn, files::sims_t& f, list<sample_t>& samples_list);
	public:
		
	};

	class dsims_t: public sims_t
	{
	private:
		
	public:
		string to_string(const string del=", ");
// 		void try_insert_measurement(measurements::dsims_t& M);
// 		void update(dsims_t& MG);
		vector<measurements_::dsims_t> measurements;
		const msettings::dsims_t settings;
		dsims_t(filenames::dsims_t& fn, files::dsims_t& f, list<sample_t>& samples_list);
		bool operator==(const dsims_t& obj) const;
		bool operator!=(const dsims_t& obj) const;
	};
	
// 	class tofsims_t: public sims_t
// 	{
// 	private:
// 		
// 	public:
// 		string to_string();
// 		set<measurements::tofsims_t*> measurements;
// 		void insert_measurement(measurements::tofsims_t* M_p);
// 		tofsims_t(measurements::tofsims_t* measurement);
// 	};
	
	template<typename Mt>
	static void try_insert_sources_into_target_vec(vector<Mt>& Ms,vector<Mt>& M_target)
	{
		for (auto& M : Ms)
		{
			Mt* M_p = tools::vec::find_in_vec(M,M_target);
			if (M_p == nullptr) // M  not  found in vec
			{
				M_target.push_back(M);
			}
		}
	}

	
};


#endif // MEASUREMENT_GROUP_T_HPP
