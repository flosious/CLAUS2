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


#ifndef FILE_T_HPP
#define FILE_T_HPP

#include <iterator>
#include <algorithm>
#include "log.hpp"
#include "definitions.hpp"
#include <iostream>
#include <string>
// #include "quantity.hpp"
#include <vector>
#include <regex>
#include <map>
#include <set>
#include "tools.hpp"
#include "quantity.hpp"
#include "element.hpp"
#include "log.hpp"
#include "cluster.hpp"
#include "crater.hpp"
#include "measurement_settings.hpp"
#include "matrix.hpp"
// #include "sample.hpp"
// #include "measurement.hpp"
// #include "measurement_group.hpp"

using namespace std;


/*
 * overview:
 * 
 * files::
 * 		->file_t 								(contents_t ,name_t)
 * 				-> sims_t 						(contents_t:file_t_contents ,name_t:file_t::name_t)
 * 						-> dsims_dp_rpc_asc_t 	(contents_t:sims_t_contents ,name_t:sims_t::name_t)
 * 						-> tofsims_t 			(contents_t:sims_t_contents ,name_t:sims_t::name_t)
 * 				-> xps_t						(contents_t:file_t_contents ,name_t:file_t::name_t)
 * 		
 * 
 */

/******************************/
/****  namespace files   ******/
/******************************/
namespace files
{


	/**********************/
	/****     file_t    ***/
	/**********************/


	///standard IHP olc file template
	class file_t
	{
		friend class config_t;	
	public:
		class contents_t
		{
		private:
			const string filename_with_path_p;
			///should be freed after parse_data_and_header_tensors, as it has no longer use
			string contents_p;
			///populates raw_data_tensor_p, raw_header_tensor_p, will clear *contents_p
			bool parse_data_and_header_tensors(vector<vector<vector<std::__cxx11::string> > >* raw_header_tensor, vector<vector<vector<std::__cxx11::string> > >* raw_data_tensor);
		protected:
			vector<vector<vector<string>>> raw_data_tensor_p, raw_header_tensor_p;
			vector<vector<vector<string>>>& raw_data_tensor();
			vector<vector<vector<string>>>& raw_header_tensor();
			string value_by_key(string key);
		public:
			/// "matrix = 30Si5 70Ge85 Sn10"
			matrix_t matrix();
			void to_screen(string prefix="");
			const string delimiter;
			const set<string> identifiers;
			const std::__cxx11::string& contents_string();
			const bool is_correct_type(); 
			///give me contents_s if already loaded somewhere
			contents_t(const string& filename_with_path_s,const string delimiter_s,const set<string> identifiers_s);
		};
		///standard IHP olc filename template
		class name_t
		{
		private:
			const string filename_with_path_p;
			vector<string> not_parseable_filename_parts_p;
			string group_p="";
			string lot_p="";
			string lot_split_p="";
			string monitor_p="";
			string repetition_p="";
			int wafer_p=-1;
			int chip_x_p=-1;
			int chip_y_p=-1;
			int olcdb_p=-1;
			
			void parse_all_parts_at_once();
			///order of parsing the parts
			void parse_filename_parts();
			
			bool parse_olcdb(string filename_part);
			bool parse_lot(string filename_part);
			bool parse_wafer(string filename_part);
			bool parse_chip(string filename_part);
			bool parse_monitor(string filename_part);
			bool parse_group(string filename_part);
			bool parse_repetitor(string filename_part);
			bool parsed_filename_parts = false;
		protected:
			
		public:
			const string delimiter;
			const set<string> identifiers;
			name_t(const string& filename_with_path_s, const string delimiter_s,const set<string> identifiers_s);
			const vector<string>& not_parseable_filename_parts();
			string filename_with_path() const;
			string filename_type_ending() const;
			const string filename() const;
			const string directory() const;
			string group();
			string lot();
			string lot_split();
			string monitor();
			string repetition();
			///empty if lot and wafer is set
			const string simple_name();
			int chip_x();
			int chip_y();
			const int olcdb();
			int wafer();
			void to_screen(string prefix="");
			const bool is_correct_type(); 
		};
		/*ctors*/
		file_t(const std::__cxx11::string& filename_with_path_s);
		const string filename_with_path;
		
	public:
		///this does not work as intended for some reason
		bool operator< (const file_t& obj) const;
		bool operator== (const file_t& obj) const;
		const string creation_date_time() const;
		
	};


	/**********************/
	/****     sims_t    ***/
	/**********************/

	///standard sims template
	class sims_t : public file_t
	{
	protected:
		sims_t(const std::__cxx11::string& filename_with_path_s);
	public:
		///content column
		class column_t
		{
		public:
			string unit;
			string cluster_name;
			string dimension;
			vector<double> data;
			void to_screen();
		};
		class name_t : public file_t::name_t
		{
		private:
			vector<total_sputter_depth_t> total_sputter_depths_p;
			bool parse_sputter_energy_element_polarity();
			string secondary_polarity_p="";
			energy_t sputter_energy_p;
			string sputter_element_p;
		public:
			name_t(const string& filename_with_path_s,const string delimiter_s,const set<string> identifiers_s);
			const energy_t sputter_energy();
			element_t sputter_element();
			const string secondary_polarity();
			const total_sputter_depth_t total_sputter_depths();
			const string filename_without_crater_depths();
		};	
		class contents_t : public file_t::contents_t
		{
		public:
			contents_t(const string& filename_with_path_s,const string delimiter_s,const set<string> identifiers_s);
		};
	};



	/**************************/
	/*** dsims_dp_rpc_asc_t ***/
	/**************************/
	class dsims_dp_rpc_asc_t : public sims_t
	{
		friend class dsims_jpg_t;
	private:
		class Ipr_t
		{
		private:
			sputter_current_t sputter_current_p;
			sputter_time_t sputter_time_p;
			sputter_depth_t sputter_depth_p;
		public:
			Ipr_t(sputter_current_t sputter_current_s, sputter_time_t sputter_time_s, sputter_depth_t sputter_depth_s={});
			const sputter_time_t sputter_time() const;
			const sputter_depth_t sputter_depth() const;
			const sputter_current_t sputter_current() const;
			const string to_string() const;
		};
// 	protected:
	public:
		class contents_t : public files::sims_t::contents_t
		{	
		private:
			map<string,string> infos_and_settings_p;
			///will create a map: keys->values from raw_data_tensor()
			const map<string,string>& infos_and_settings();
			vector<string> units_p;
			vector<string> dimensions_p;
			vector<string> cluster_names_p;
			/// returns the corrrupted lines (last line of each column,except Ipr), which should be removed
			set<int> corrupted_lines();
			/// removes the lines, where some columns are empty
			void remove_corrupted_lines();
			/// parses the raw_header_tensor into vectors of dimensions, isotopes and units
			bool parse_units_dimensions_clusternames();
			/// corrects tab stops for correct alignment and parsing
			bool ipr_shift_correction();
			/// return true if raw_data_mat contains Ipr (primary current)
			bool check_Ipr();
			/// true, if Ipr was detected; false if no Ipr
			bool is_Ipr;
			///e.g. < sputter_time > = get_quantity_from_dimension_and_clustername("Time","Ipr")
			const quantity_t get_quantity_from_dimension_and_clustername(const string col_dimension, const string ignore_clustername="");
			const vector<string>& units();
			const vector<string>& dimensions();
			const vector<column_t> columns();
			const vector<string>& cluster_names();
		public:
			contents_t(const string& filename_with_path_s);
			const vector<cluster_t> clusters();
			const tm start_date_time();
			const tm creation_date_time();
			///primary current (sputter_current vs time/depth)
			const Ipr_t Ipr();
			
			const energy_t sputter_energy();
			const secondary_voltage_t secondary_voltage();
			element_t sputter_element();
			const rastersize_t sputter_rastersize();
			const rastersize_t analysis_rastersize();
			const string secondary_polarity();
			const quantity_t chamber_pressure();
			const quantity_t egate();
			const quantity_t mass_resolution();
			const quantity_t field_aperture();
			const quantity_t contrast_aperture();
			const quantity_t entrance_slit();
			const quantity_t exit_slit();
			const quantity_t energy_window();
			const quantity_t em_yield();
			const quantity_t em_voltage();
		};	
		class name_t : public files::sims_t::name_t
		{
		public:
			name_t(const std::__cxx11::string& name_with_path_s);
		};
	
		dsims_dp_rpc_asc_t(const string& filename_with_path_s);
		name_t name;
		contents_t contents;
		void to_screen(string prefix="");
	};	


	/**************************/
	/***    tofsims_TXT_t   ***/
	/**************************/
	class tofsims_TXT_t : public sims_t
	{
	protected:
		class contents_t : public sims_t::contents_t
		{
		private:
			bool parse_analysis_energy_element(string filename_part);
			string analysis_element_p;
		public:
			contents_t(const string& filename_with_path_s);
			const element_t analysis_element();
			const energy_t analysis_energy();
		};
// 		class name_t : public sims_t::name_t
// 		{
// 		private:
// 		public:
// 			name_t(const string& filename_with_path_s);
// 		};
	public:
		tofsims_TXT_t(const string& filename_with_path_s);
		name_t name;
		contents_t contents;
	};

	/**********************/
	/**** dsims_jpg_t ****/
	/**********************/

	class jpg_t : public file_t
	{
// 		class name_t : dsims_dp_rpc_asc_t::name_t
// 		{
// 		public:
// 			name_t(const string& filename_with_path_s);
// 		};
	public:
		name_t name;
// 		contents_t contents;
		jpg_t(const string& filename_with_path_s);
	};

	/**************************/
	/****     profiler_t    ******/
	/**************************/
	class profiler_t : public file_t 
	{
	private:
// 		class contents_t : file_t::contents_t
// 		{
// 		private:
// 			bool parse_analysis_energy_element(string filename_part);
// 		public:
// 			contents_t(const string& filename_with_path_s,string contents_s="");
// 		};
// 		class name_t : file_t::name_t
// 		{
// 		private:
// 		public:
// 			name_t(const string& name_with_path_s);
// 		};	
	public:
		name_t name;
		contents_t contents;
		profiler_t(const string& filename_with_path_s);
	};

}
#endif // FILE_T_HPP

