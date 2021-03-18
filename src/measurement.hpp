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
#include <mgl2/mgl.h>
#include <mgl2/fltk.h>
#include <unistd.h>
#include "origin.hpp"

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
		measurement_t(files_::file_t::name_t& filename, files_::file_t::contents_t& filecontents, list<sample_t>& samples_list, string method);
		measurement_t(files_::file_t::name_t& filename, list<sample_t>& samples_list, string method);
		
		string filename_with_path;
		string to_string(const string del = ", ") const;
		bool is_set() const;
		string group;
		string repetition;
		int olcdb;
		sample_t* sample=nullptr;
		/// dsims, tofsims, xps, profiler, ...
		string method;
		bool operator==(measurement_t& obj);
		bool operator!=(measurement_t& obj);
		bool operator<(measurement_t& obj);
		bool operator>(measurement_t& obj);
	};
	
	class profiler_t : public measurement_t
	{
	private:
		string primary_method_p="";
	public:
		const string& primary_method();
		total_sputter_depth_t total_sputter_depths;
		linescan_t linescan;
		profiler_t(files_::profiler_t& file, list<sample_t>& samples_list);
	};
	
	class sims_t : public measurement_t, public mglDraw
	{
	protected:
		void add_clusters(vector<cluster_t>& clusters_s);
	public: 
		set<cluster_t*> reference_clusters();
		bool load_reference();
		bool load_reference_parameters();
		///pointer to its measurement reference cluster
		cluster_t* reference_cluster=nullptr;
		
		//creates instantly a plot
		void plot_now(double sleep_sec=1);
		///origin ready for import
		void export_origin_ascii(string path="/tmp/", const string delimiter="\t");
		
		int Draw(mglGraph * gr) override;
		sims_t(files_::sims_t::name_t& filename, files_::sims_t::contents_t& filecontents, list<sample_t>& samples_list, string method, 
			   vector<files_::jpg_t>* jpg_files=nullptr,vector<files_::profiler_t>* profiler_files=nullptr);
		sims_t(files_::sims_t::name_t& filename, list<sample_t>& samples_list, string method);	
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
		dsims_t(files_::dsims_t& dsims_file, list<sample_t>& samples_list, vector<files_::jpg_t>* jpg_files=nullptr, vector<files_::profiler_t>* profiler_files=nullptr);
		///loads all matching files into the measurement and clears the elements from the list(s)
// 		dsims_t(vector<files_::dsims_t>& dsims_files, list<sample_t>& samples_list,vector<files_::jpg_t>* jpg_files=nullptr);
		bool operator==(dsims_t& obj);
		bool operator!=(dsims_t& obj);
// 		bool operator<(dsims_t& obj);
	};
	
	class tofsims_t : public sims_t
	{
	public:
// 		const msettings::tofsims_t settings;
// 		tofsims_t(vector<files_::tofsims_t>& dsims_files, list<sample_t>& samples_list,vector<files_::jpg_t>* jpg_files=nullptr);
	};
	
	
	
};

#endif // MEASUREMENT_T_HPP

