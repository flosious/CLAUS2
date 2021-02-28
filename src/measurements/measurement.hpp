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
#include "../sample.hpp"
#include "../crater.hpp"
#include "../files/file.hpp"
#include "../mgroups/mgroup.hpp"
#include "../msettings/msettings.hpp"


/* FORWARD DECLARATIONS*/
class sample_t;
class processor;
// class lists;

namespace measurements
{
	class sims_t;
	class dsims_t;
	class tofsims_t;
}

namespace mgroups
{
	class mgroup_t;
	class tofsims_t;
	class sims_t;
	class dsims_t;
}


/************************/


using namespace std;



namespace measurements
{
	
	class measurement_t
	{
	private:
		///use these figures to diistinguish one measurement from another
		static bool use_olcdb;
		static bool use_group;
		static bool use_settings;
		static bool use_repition;
		static bool use_sample;
	protected:
		sample_t* sample_p=nullptr;
		/*ctors*/
	public:
		string to_string();
		set<files::file_t*> files;
		measurement_t(files::file_t* file,sample_t* sample_p);
		/*functions*/
		void insert_file(files::file_t* file_p);
		///pointer to its corsseponding the sample
		sample_t* sample();
		int olcdb();
		string repetition();
		string group();
		/*static functions*/
		/*operators*/
		const bool operator==(measurement_t& obj);
		const bool operator!=(measurement_t& obj);
	};
	
	
	///standard sims template
	///can only be instanziated by childs
	class sims_t : public measurement_t
	{
	private:
		set<files::sims_t*> files;
		vector<cluster_t> clusters_p;
	protected:
		//measurement_settings
		crater_t crater_p;
		sims_t(files::sims_t* file, sample_t* sample);
		
	public:
		string to_string();
		msettings::sims_t settings();
		void insert_file(files::sims_t* file_p);
		crater_t& crater();
		///returns the cluster corresponding isotope
		isotope_t* isotope(cluster_t& cluster);
		///isotopes collected from clusters
		set<isotope_t*> isotopes();
		///copies filecontents to cluster_p
		set<cluster_t*> clusters();
		///returns the isotope corresponding clusterS (there can be more than one)
		///e.g. isotope(31P) --> cluster(74Ge 31P) & cluster(31P) & ...
		set<cluster_t*> clusters(isotope_t& isotope);
// 		const bool operator==(sims_t& obj);
// 		const bool operator!=(sims_t& obj);
	};

	class dsims_t : public sims_t 
	{
	private:
		set<files::dsims_t*> files;
		files::dsims_t::Ipr_t Ipr_p;
	public:
		files::dsims_t::Ipr_t* Ipr();
		string to_string();
		msettings::dsims_t settings();
		void insert_file(files::dsims_t* file_p);
		mgroups::dsims_t* mgroup=nullptr;
		dsims_t(files::dsims_t* file, sample_t* sample);
	};

	
	class tofsims_t : public sims_t 
	{
	private:
		set<files::tofsims_t*> files;
	public:
		string to_string();
		void insert_file(files::tofsims_t* file_p);
		mgroups::tofsims_t* mgroup=nullptr;
		tofsims_t(files::tofsims_t* file, sample_t* sample);
	};
	
	class profiler_t : public measurement_t
	{
	
	};
}

#endif // MEASUREMENT_T_HPP

