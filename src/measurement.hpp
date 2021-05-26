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
#include <map>
#include <mgl2/mgl.h>
#include <mgl2/fltk.h>
#include <unistd.h>

#include "sample.hpp"
#include "crater.hpp"
// #include "mgroup.hpp"
#include "msettings.hpp"
#include "files.hpp"
#include "origin.hpp"
#include "statistics.hpp"

#include "plot.hpp"
#include "database_t.hpp"
/************************/
// class plot_t;

using namespace std;


/************************/
// FORWARD DECLARATIONS //

namespace calc
{
	class SR_c;
}

/************************/


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
		database_t* sql_wrapper=nullptr;
	public:
		measurement_t(files_::file_t::name_t& filename, files_::file_t::contents_t& filecontents, list<sample_t>& samples_list, string method,database_t& sql_wrapper);
		measurement_t(files_::file_t::name_t& filename, list<sample_t>& samples_list, string method,database_t& sql_wrapper);
		
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
		crater_t::linescan_t linescan;
		profiler_t(files_::profiler_t& file, list<sample_t>& samples_list, database_t& sql_wrapper);
		//creates instantly a plot
		void plot_now(double sleep_sec=1);
	};
	
	class sims_t : public measurement_t
	{
	protected:
		///some general data filters
		class filter_t
		{
		private:
			const sims_t& M;
		public:
			filter_t(const sims_t& M);
			///filters out pulses/spikes/gaps in data
			sims_t impulses();
		};
		///determine sputter_equlibrium position (surface peak); maybe moved later on, when introducing other non cluster based measurement methods (e.g. XPS, AES)
		class equlibrium_t
		{
		private:
			const sims_t& M;
			///just for MATRIX/Reference cluster C -> here we know the content should be constant (most of the time)
			static const unsigned int equilibrium_start_index(const cluster_t& C);
			///just for quantity Q
			static const unsigned int equilibrium_start_index(const quantity_t& Q);
			quantity_t& erase_inequilibrium_data(quantity_t& Q);
			cluster_t& erase_inequilibrium_data(cluster_t& C);
			crater_t& erase_inequilibrium_data(crater_t& C);
			unsigned int& equilibrium_start_index_s;
		public:
			///if equilibrium_start_index_s>0 enforces to use this index
			equlibrium_t(const sims_t& M, unsigned int& equilibrium_start_index_s);
			///0 is equivalent to input measurement, so instant equilibrium
			const unsigned int equilibrium_start_index();
			sims_t measurement();
			vector<cluster_t> clusters();
		};
		class calc_t
		{
		protected:
			sims_t& measurement;
		public:
			class from_crater_t
			{
				SR_t SR() const;
			};
			class fit_to_ref_t
			{
				SR_t SR() const;
				SF_t SF() const;
				///SR and SF calculated once and returned within measurement
				sims_t measurement() const;
			};
			class from_database_t
			{
				SR_t SR();
				SF_t SF();
			};
			class SF_c
			{
			protected:
				sims_t& measurement;
			public:
				///sensitvity factor
				SF_c(sims_t& measurement);
				bool from_pbp_RSF();
				bool from_const_RSF();
				bool from_dose();
				bool from_implant_max();
				bool from_database_parameters();
			};
			calc_t(sims_t& measurement);
			calc::SR_c SR();
		};
		///adds more cluster to this measurement
		void add_clusters(vector<cluster_t>& clusters_s);
		///saved variable for faster recalculation of sputter_equlibrium
		unsigned int equilibrium_start_index_s=0;
	public: 
		calc_t calc();
		sims_t(files_::sims_t::name_t& filename, files_::sims_t::contents_t& filecontents, list<sample_t>& samples_list, string method, database_t& sql_wrapper,
			   vector<files_::jpg_t>* jpg_files=nullptr,vector<files_::profiler_t>* profiler_files=nullptr);
		sims_t(files_::sims_t::name_t& filename, list<sample_t>& samples_list, string method, database_t& sql_wrapper);	
		string to_string(const string del = ", ");
		///returns the measurement without surface artefacts
		sims_t sputter_equilibrium();
		filter_t filter() const;
		///returns the cluster corresponding isotope
// 		isotope_t* isotope(cluster_t& cluster);
		///isotopes collected from clusters
// 		set<isotope_t*> isotopes();
		///returns the isotope corresponding clusterS (there can be more than one)
		///e.g. isotope(31P) --> cluster(74Ge 31P) & cluster(31P) & ...
		set<cluster_t*> clusters_corresponding_to_isotope(isotope_t& isotope);
		isotope_t isotope_corresponding_to_cluster(cluster_t& cluster);
		///virtual cluster generated from all reference_clusters
		cluster_t virtual_matrix_cluster();
		///same as reference_clusters
		const set<const cluster_t*> matrix_clusters() const;
		///same as matrix_clusters
		const set<const cluster_t*> reference_clusters() const;
		///all isotopes from reference_clusters
		const set<isotope_t> reference_isotopes();
// 		bool load_reference();
// 		bool load_reference_parameters();
		///pointer to its measurement reference cluster
// 		cluster_t* reference_cluster=nullptr;
		
		//creates instantly a plot
		void plot_now(double sleep_sec=1);
		///origin ready for import
		void export_origin_ascii(string path="/tmp/", const string delimiter="\t");
		
// 		int Draw(mglGraph * gr) override;
		bool add(sims_t& measurement);
		
		//changes sputter_time axis and all others accordingly
		sims_t change_resolution(sputter_time_t sputter_time_res);
		sims_t change_resolution(sputter_depth_t sputter_depth_res);
		
		crater_t crater;
		vector<cluster_t> clusters;
	};
	
	class dsims_t : public sims_t
	{
	private:
		///for references
		class database
		{
		private:
// 			database_t& sql_wrapper;
			///reference to parent measurement
// 			dsims_t& M;
			///creates the table if not exists
			
			///loads all filenames counting as reference for this measurement M
			static vector<string> ref_filenames();
			///make this measurement M a reference
			static bool insert_into_table();
		public:
// 			database(dsims_t& M, database_t& sql_wrapper);
			static bool create_table();
			static dsims_t reference_measurement();
			static vector<dsims_t> reference_measurements();
		};
// 		vector<cluster_t> clusters;
	public:
		msettings::dsims_t settings;
		dsims_t(files_::dsims_t& dsims_file, list<sample_t>& samples_list, database_t& sql_wrapper, vector<files_::jpg_t>* jpg_files=nullptr, vector<files_::profiler_t>* profiler_files=nullptr);
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

// extern database_t db;

#endif // MEASUREMENT_T_HPP

