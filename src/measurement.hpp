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



namespace measurements
{
	class measurement_t
	{
// 		friend class processor;
	private:
		///use these figures to diistinguish one measurement from another
		static bool use_olcdb;
		static bool use_group;
		static bool use_settings;
		static bool use_repition;
		static bool use_sample;
		
		set<files::file_t::name_t*> filenames;
		set<files::file_t::contents_t*> filecontents;
	protected:
		list<sample_t>* samples_list;
		sample_t* sample_p=nullptr;
		/*ctors*/
	public:
		measurement_t(files::file_t::name_t& filename_p, files::file_t::contents_t& filecontents_p, list<sample_t>& samples_list_p);
	public:
		/*functions*/
		///pointer to its corsseponding the sample
		const sample_t* sample();
		const int olcdb() const;
		const string repitition() const;
		const string group() const;
		/*static functions*/
		/*operators*/
		const bool operator==(measurement_t& obj);
		const bool operator!=(measurement_t& obj);
	};

	
	class profiler_t : public measurement_t
	{
	
	};
	
	
	///standard sims template
	///can only be instanziated by friends
	class sims_t : public measurement_t
	{
	private:
		set<files::sims_t::name_t*> filenames;
		set<files::sims_t::contents_t*> filecontents;
		vector<cluster_t> clusters_p;
	protected:
		//measurement_settings
		crater_t crater_p;
		sims_t(files::sims_t::name_t& filename_p, files::sims_t::contents_t& filecontents_p, list<sample_t>& samples_list_p);
	public:
		crater_t& crater();
		///returns the cluster corresponding isotope
		isotope_t* isotope(cluster_t& cluster);
		///isotopes collected from clusters
		vector<isotope_t*> isotopes();
		///copies filecontents to cluster_p
		vector<cluster_t*> clusters();
		///returns the isotope corresponding clusterS (there can be more than one)
		///e.g. isotope(31P) --> cluster(74Ge 31P) & cluster(31P) & ...
		vector<cluster_t*> clusters(isotope_t& isotope);
		const bool operator==(sims_t& obj);
		const bool operator!=(sims_t& obj);
	};


	class dsims_t : public sims_t 
	{
		dsims_t(files::dsims_dp_rpc_asc_t::name_t& filename_p, files::dsims_dp_rpc_asc_t::contents_t& filecontents_p, list<sample_t>& samples_list_p);
		dsims_t(files::jpg_t::name_t& filename_p, list<sample_t>& samples_list_p);
	};

	
	class tofsims_t : public sims_t 
	{
	
	};
}

#endif // MEASUREMENT_T_HPP

