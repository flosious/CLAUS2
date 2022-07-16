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

class mgroups_;

// namespace calc
// {
// 	class SR_c;
// }

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
		long long unsigned int memory_address() const;
		string filename_with_path;
		string to_string(const string del = ", ") const;
		bool is_set() const;
		string group;
		string repetition;
		int olcdb;
		sample_t* sample=nullptr;
		/// dsims, tofsims, xps, profiler, ...
		string method;
		bool operator==(const measurement_t& obj) const;
		bool operator!=(const measurement_t& obj) const;
		bool operator<(const measurement_t& obj) const;
		bool operator>(const measurement_t& obj) const;
	}; // measurement_t
	
	class profilers_t
	{
	public:
		class profiler_t : public measurement_t
		{
		protected:
			profiler_t(files_::file_t::name_t& filename, files_::file_t::contents_t& filecontents, const crater_t::linescan_t& linescan_s, list<sample_t>& samples_list, string method,database_t& sql_wrapper);
			crater_t::linescan_t linescan_p;
		public:
			crater_t::linescan_t& linescan();
			//creates instantly a plot
			void plot_now(double sleep_sec=1);
		}; //profiler_t
	public:
		class dektak6m_t : public profiler_t
		{
		public:
			dektak6m_t(files_::profilers_t::dektak6m_t& file, list<sample_t>& samples_list, database_t& sql_wrapper);
		};
		class P17_t : public profiler_t
		{
		public:
			P17_t(files_::profilers_t::dektak6m_t& file, list<sample_t>& samples_list, database_t& sql_wrapper);
		};
	}; // profilers_t
	
	class sims_t : public measurement_t
	{
		friend class processor;
	protected:
		///some general data filters
		class filter_t
		{
			
		private:
			const sims_t& M;
		public:
			filter_t(const sims_t& M) ;
			///filters out pulses/spikes/gaps in data
			sims_t impulses();
		};
	public:
		///calculation of everything within this measurement
		class calc_t
		{
			// 			friend class mgroups_::sims_t; // geht nicht
		public:
			sims_t& M;
			///overwrite with calculation results (even if they are empty); false -> just use value if no is already set
			bool overwrite = false;
		public:
			class SR_c
			{
			private:
				sims_t& M;
				calc_t& calc;
			public:
				SR_c(calc_t& calc);
				calc_t& from_crater_depths(bool overwrite=false);
				///mean from all available clusters; uses max cluster intensity or concentration
				calc_t& from_implant_max(bool overwrite=false);
				///from one particular cluster
				quantity::SR_t from_implant_max(cluster_t& C) const;
				///mean from all available clusters
				calc_t& from_ref_fit();
				///from one particular cluster
				calc_t& from_ref_fit(const cluster_t& cluster);
			};
			class SD_c
			{
			private:
				sims_t& M;
				calc_t& calc;
			public:
				SD_c(calc_t& calc);
				calc_t& from_SR(bool overwrite=false);
			};
			class SF_c
			{
			private:
				sims_t& M;
				calc_t& calc;
			public:
				SF_c(calc_t& calc);
				///for all available clusters
				calc_t& from_db_max(bool overwrite=false);
				///just for one particular cluster
				quantity::SF_t from_db_max(cluster_t& cluster);
				///for all available clusters
				calc_t& from_db_dose(bool overwrite=false);
				///just for one particular cluster
				quantity::SF_t from_db_dose(cluster_t& cluster);
				///for all available clusters; fitting to reference measurement(s)
				calc_t& from_ref_fit(bool overwrite=false);
				///just for one particular cluster; fitting to reference measurement(s)
				quantity::SF_t from_ref_fit(cluster_t& cluster);
				
				calc_t& from_RSF_mean_ref(bool overwrite=false);
				quantity::SF_t from_RSF_mean_ref(cluster_t& cluster);
				
				calc_t& from_RSF_median_ref(bool overwrite=false);
				quantity::SF_t from_RSF_median_ref(cluster_t& cluster);
				
				calc_t& from_SF_trimmed_mean_ref(bool overwrite=false);
				quantity::SF_t from_SF_trimmed_mean_ref(cluster_t& cluster);
				
				calc_t& from_RSF_pbp_ref(bool overwrite=false);
				quantity::SF_t from_RSF_pbp_ref(cluster_t& cluster);
				
				///reference_intensity could be mean, median, trimmed_mean, a vector (pbp), ...
				calc_t& from_RSF(const quantity::intensity_t& reference_intensity);
				quantity::SF_t from_RSF(cluster_t& cluster, const quantity::intensity_t& reference_intensity);
			};
			class RSF_c
			{
			private:
				sims_t& M;
				calc_t& calc;
			public:
				RSF_c(calc_t& calc);
				RSF_c(const quantity::SF_t& SF, const quantity::intensity_t& reference);
				
				calc_t& from_SF_mean_ref(bool overwrite=false);
				cluster_t::RSF_t from_SF_mean_ref(const cluster_t& cluster);
				
				calc_t& from_SF_median_ref(bool overwrite=false);
				cluster_t::RSF_t from_SF_median_ref(const cluster_t& cluster);
				
				calc_t& from_SF_trimmed_mean_ref(bool overwrite=false);
				cluster_t::RSF_t from_SF_trimmed_mean_ref(const cluster_t& cluster);
				
				calc_t& from_SF_pbp_ref(bool overwrite=false);
				cluster_t::RSF_t from_SF_pbp_ref(const cluster_t& cluster);
				
				calc_t& from_SF_percentile_ref(double percentile, bool overwrite=false);
				cluster_t::RSF_t from_SF_percentile_ref(const cluster_t& cluster, double percentile);
				
				///reference_intensity could be percentile, mean, median, trimmed_mean, a vector (pbp), or anything other ...
				calc_t& from_SF_ref(const quantity::intensity_t& reference_intensity, bool overwrite=false);
				cluster_t::RSF_t from_SF_ref(const cluster_t& cluster, const quantity::intensity_t& reference_intensity, bool overwrite=false);
			};
			class concentration_c
			{
			private:
				sims_t& M;
				calc_t& calc;
			public:
				concentration_c(calc_t& calc);
				calc_t& from_SF(bool overwrite=false);
				static quantity::concentration_t from_SF(const cluster_t& cluster);
			};
			class matrix_c
			{
			private:
				sims_t& M;
				calc_t& calc;
			public:
				///calculation of matrix concentrations
				matrix_c(calc_t& calc);
				calc_t& mean_const_from_db(bool overwrite = false);
				calc_t& median_const_from_db(bool overwrite = false);
				calc_t& median_const_from_reference_isotopes(bool overwrite = false);
				calc_t& trimmed_mean_const_from_db(bool overwrite = false, float trimm_factor=0.25);
			};
			class implant_c
			{
			public:
				class quantity_c
				{
				private:
					quantity::quantity_t Y;
					int minimum_index_position_p = -1;
					int maximum_index_position_p = -1;
				public:
					quantity_c(const quantity::quantity_t& Y);
					unsigned int minimum_index_position();
					quantity::quantity_t minimum_position();
					unsigned int maximum_index_position();
					quantity::quantity_t maximum_position();
					/// mean of 20 last data points
					quantity::quantity_t background(int last_points=20) const;
				};
				class map_c
				{
				private:
					quantity::map_t XY_map_p;
					int minimum_index_position_p = -1;
					int maximum_index_position_p = -1;
				public:
					map_c();
					map_c(const quantity::map_t& XY_map);
					map_c(const quantity::map_t& XY_map, unsigned int minimum_index_position_p, unsigned int maximum_index_position_p);
					//unmodified map (raw data)
					const quantity::map_t& XY_map() const; 
					//used as implant profile
					quantity::map_t XY_map_without_surface(); 
					quantity::quantity_t background_value(int last_points=20) const;
					quantity::quantity_t minimum_pos();
					quantity::quantity_t minimum_value();
					quantity::quantity_t maxmimum_pos();
					quantity::quantity_t maximum_value();
					///excluding surface
					quantity::quantity_t area();
					unsigned int minimum_index_position();
					unsigned int maximum_index_position();
					///plot 
					void to_screen(float seconds=0) const;
				};
			private:
				///the measurement
				sims_t& M;
				cluster_t& cluster;
				///a factor to change sputter_time resolution when determining maximum position
				double X_resolution_factor;
				int maximum_pos_index_s = -1;
				quantity::sputter_time_t sputter_time_at_maximum_s;
				quantity::intensity_t maximum_intensity_s;
				///populates maximum_intensity_s + sputter_time_at_maximum_s ;seconds_for_fit_plot < 0 no plot;
				void fit_maximum_intensity_val_and_pos(double seconds_for_fit_plot=-1);
				quantity::map_t fitted_curve_p;
				///saved calculation data
				map_c C_vs_SD_p;
				map_c C_vs_ST_p;
				map_c I_vs_SD_p;
				map_c I_vs_ST_p;
			public:
				implant_c(sims_t& measurement, cluster_t& cluster, double X_resolution_factor=1);
				///checks if the cluster is actually showing an implant profile
// 				bool has_implant_profile() const;
				static unsigned int minimum_index_position(quantity::quantity_t Y);
				static unsigned int minimum_index_position(vector<double> data);
				unsigned int minimum_index_position();
				int maximum_pos_index() const;
				quantity::intensity_t background_intensity();
				///quantity::sputter_time_t of maximum of intensity of implant
				const quantity::sputter_time_t& sputter_time_at_maximum() const;
				///maximum of intensity of implant
				const quantity::intensity_t& maximum_intensity() const;
				quantity::quantity_t minimum_starting_position() const;
				quantity::sputter_time_t minimum_sputter_time_position() const;
				quantity::depth_t minimum_sputter_depth_position() const;
				quantity::sputter_time_t sputter_time_from_minimum();
				quantity::depth_t sputter_depth_from_minimum();
				///calc dose from concentration and sputter_depth; returns {} if C or SD not set
				quantity::dose_t dose() const;
				quantity::SR_t SR() const;
				quantity::SF_t SF();
				quantity::SF_t SF_from_dose();
				quantity::SF_t SF_from_max();
				quantity::SF_t RSF();
				quantity::concentration_t concentration();
				const quantity::map_t& fitted_curve() const;
				double X_resolution_factor_() const;
				const map_c& C_vs_SD();
				const map_c& C_vs_ST();
				const map_c& I_vs_SD();
				const map_c& I_vs_ST();
			}; //implant_c
		private:
			map<cluster_t* const, implant_c> implants_s;
		public:
			implant_c& implant(cluster_t& cluster, double X_resolution_factor=0.1);
			///calculates SR and SF in one step from implant
			SR_c SR;
			SD_c SD;
			SF_c SF;
			RSF_c RSF;
			matrix_c matrix;
			concentration_c concentration;

			calc_t(sims_t& measurement,bool overwrite=false);
		};
	private:
		///adds more cluster to this measurement
		void add_clusters(vector<cluster_t>& clusters_s);
		cluster_t matrix_cluster_s;
	public: 
		sims_t(files_::sims_t::name_t& filename, files_::sims_t::contents_t& filecontents, list<sample_t>& samples_list, string method, database_t& sql_wrapper, vector<const quantity::total_sputter_depth_t*> total_sputter_dephs={});

		bool are_clusters_in_measurement(const vector<cluster_t>& clusters_s) const;
		bool are_clusters_in_measurement(const cluster_t& cluster_s) const;
		bool are_intensities_of_clusters_set(const vector<cluster_t>& clusters_s) const;
		bool are_intensities_of_clusters_set(const cluster_t& cluster_s) const;
		
		calc_t calc(bool overwrite=false);
		
		string to_string(const string del = ", ");
		const string to_string_short(const string del = ", ") const;
		///returns the measurement without surface artefacts
		filter_t filter() const;

		///e.g. isotope(31P) --> cluster(74Ge 31P) & cluster(31P) & ...
		set<cluster_t*> clusters_corresponding_to_isotope(const isotope_t& isotope);
		isotope_t isotope_corresponding_to_cluster(const cluster_t& cluster);
		///isotopes in matrix and references, e.g. in cluster 70Ge 11B, 70Ge is a ref isotope, but not included in sample matrix ; might be overwritten by mgroup
		vector<isotope_t> reference_isotopes;
		/*
		 * matrix cluster should contain only unique isotope for its cluster;
		 * that means e.g. SiGe matrix there can never be a SiGe cluster, just Si clusters and Ge clusters seperate
		 * SiGeB cluster or SiB cluster will be treatet as NON-matrix clusters, that means (implanted) isotopical cluster
		 */
		matrix_clusters_c matrix_clusters();
		
		//creates instantly a plot
		void plot_now(double sleep_sec=1);
		///origin ready for import
		void export_origin_ascii(string path="/tmp/", const string delimiter="\t");

		bool add(sims_t& measurement);
		
		//changes sputter_time axis and all others accordingly
		sims_t change_resolution(quantity::sputter_time_t sputter_time_res);
		sims_t change_resolution(quantity::sputter_depth_t sputter_depth_res);
		
		crater_t crater;
		vector<cluster_t> clusters;
		///returns pointer to the matching cluster within this measurement
		cluster_t* cluster(const cluster_t& cluster_s);
		///returns pointer to the matching cluster with the corresponding_isotope within this measurement
		cluster_t* cluster(const isotope_t& iso);
		const cluster_t* cluster(const cluster_t& cluster_s) const;
		///returns a copy of the RSF of the specified cluster
		cluster_t::RSF_t RSF(const cluster_t& cluster_s) const;
		///returns a copy of the SF of the specified cluster
		quantity::SF_t SF(const cluster_t& cluster_s) const;
		///returns a copy of the intensity of the specified cluster
		quantity::intensity_t intensity(const cluster_t& cluster_s) const;
		///returns a copy of the concentration of the specified cluster
		quantity::concentration_t concentration(const cluster_t& cluster_s) const;
		quantity::map_t concentration_vs_sputter_depth(const cluster_t& cluster) const;
		quantity::map_t concentration_vs_sputter_time(const cluster_t& cluster) const;
		quantity::map_t intensity_vs_sputter_depth(const cluster_t& cluster) const;
		quantity::map_t intensity_vs_sputter_time(const cluster_t& cluster) const;
	}; // sims_t
	
	class dsims_t : public sims_t
	{
	private:
		///for references
		class db_t
		{
		private:
			///loads all filenames counting as reference for this measurement M
			static vector<string> ref_filenames();
			///make this measurement M a reference
			static bool insert_into_table();
		public:
			static bool create_table();
			static dsims_t reference_measurement();
			static vector<dsims_t> reference_measurements();
		};
// 		vector<cluster_t> clusters;
	public:
		msettings::dsims_t settings;
		dsims_t(files_::dsims_t& dsims_file, list<sample_t>& samples_list, database_t& sql_wrapper, vector<const quantity::total_sputter_depth_t*> total_sputter_dephs={});
		bool operator==(const dsims_t& obj) const;
		bool operator!=(const dsims_t& obj) const;
	}; // dsims_t
	
	class tofsims_t : public sims_t
	{
	public:
		msettings::tofsims_t settings;
		tofsims_t(files_::tofsims_t& tofsims_file, list<sample_t>& samples_list, database_t& sql_wrapper, vector<const quantity::total_sputter_depth_t*> total_sputter_dephs={});

		bool operator==(const tofsims_t& obj) const;
		bool operator!=(const tofsims_t& obj) const;
	}; // tofsims_t
}; // measurements_

// extern database_t db;

#endif // MEASUREMENT_T_HPP

