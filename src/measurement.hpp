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
// #include "mgroup.hpp"
#include "msettings.hpp"
#include <map>
#include "files.hpp"


/************************/


using namespace std;



class measurements_
{
public:	
	class measurement_t
	{
	protected:
		///use these figures to diistinguish one measurement from another
		static bool use_repetition;
		static bool use_group;
		static bool use_olcdb;
		static bool use_sample;
	public:
		measurement_t(files::file_t::name_t& filename, files::file_t::contents_t& filecontents, list<sample_t>& samples_list);
		measurement_t(files::file_t::name_t& filename, list<sample_t>& samples_list);
		
		string to_string(const string del = ", ") const;
		bool is_set() const;
		string group;
		string repetition;
		int olcdb;
		sample_t* sample=nullptr;
		bool operator==(measurement_t& obj);
		bool operator!=(measurement_t& obj);
	};
	
	class sims_t : public measurement_t
	{
	protected:
		void add_clusters(vector<cluster_t>& clusters_s);
	public:
		sims_t(files::sims_t::name_t& filename, files::sims_t::contents_t& filecontents, list<sample_t>& samples_list, 
			   vector<files::jpg_t>* jpg_files=nullptr,vector<files::profiler_t>* profiler_files=nullptr);
		sims_t(files::sims_t::name_t& filename, list<sample_t>& samples_list);	
		crater_t crater;
		string to_string(const string del = ", ") const;
		vector<cluster_t> clusters;
		///returns the cluster corresponding isotope
		isotope_t* isotope(cluster_t& cluster);
		///isotopes collected from clusters
		set<isotope_t*> isotopes();
		///returns the isotope corresponding clusterS (there can be more than one)
		///e.g. isotope(31P) --> cluster(74Ge 31P) & cluster(31P) & ...
		set<cluster_t*> clusters_from_iso(isotope_t& isotope);
	};
	
	class dsims_t : public sims_t
	{
	public:
		msettings::dsims_t settings;
		dsims_t(files::dsims_t& dsims_file, list<sample_t>& samples_list,vector<files::jpg_t>* jpg_files=nullptr,vector<files::profiler_t>* profiler_files=nullptr);
		///loads all matching files into the measurement and clears the elements from the list(s)
// 		dsims_t(vector<files::dsims_t>& dsims_files, list<sample_t>& samples_list,vector<files::jpg_t>* jpg_files=nullptr,vector<files::profiler_t>* profiler_files=nullptr);
	};
	
	class tofsims_t : public sims_t
	{
	public:
// 		const msettings::tofsims_t settings;
// 		tofsims_t(vector<files::tofsims_t>& dsims_files, list<sample_t>& samples_list,vector<files::jpg_t>* jpg_files=nullptr);
	};
	
	
	class profiler_t : public measurement_t
	{
	public:
		linescan_t linescan;
		profiler_t(files::profiler_t& file, list<sample_t>& samples_list);
// 		profiler_t(vector<files::profiler_t>& files, list<sample_t>& samples_list);
	};
};

#endif // MEASUREMENT_T_HPP

