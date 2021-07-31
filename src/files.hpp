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

#ifndef FILES_HPP
#define FILES_HPP


#include <string>

#include "tools.hpp"
#include "quantity.hpp"
#include "cluster.hpp"
#include "element.hpp"
#include "ion.hpp"
#include "crater.hpp"
// #include "matrix.hpp"

class files_
{
public:
	class file_t
	{
	public:
		class name_t
		{
		private:
			static const vector<string> methods;
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
			vector<string> not_parseable_filename_parts_p;
			name_t(string& filename_with_path_s, const string delimiter_s,const set<string> OR_identifiers_s,const set<string> AND_identifiers_s);
		public:
			const string method() const;
			string to_string(const string del = ",");
			string delimiter;
			set<string> OR_identifiers_s;
			set<string> AND_identifiers_s;
			virtual const vector<string>& not_parseable_filename_parts();
			string filename_with_path;
			string filename_type_ending() const;
			const string filename() const;
			const string directory() const;
			string group();
			string lot();
			string lot_split();
			string monitor();
			string repetition();
			///empty if lot and wafer is set
			virtual const string simple_name();
			int chip_x();
			int chip_y();
			const int olcdb();
			int wafer();
			void to_screen(string prefix="");
			const bool is_correct_type(); 
			bool operator==(name_t& obj);
			bool operator!=(name_t& obj);
			bool operator<(name_t& obj);
		};
		
		class contents_t
		{
			friend class config_t;
		protected:
			///should be freed after parse_data_and_header_tensors, as it has no longer use
			string contents_p;
			///populates raw_data_tensor_p, raw_header_tensor_p, will clear *contents_p
			bool parse_data_and_header_tensors(vector<vector<vector<std::__cxx11::string> > >* raw_header_tensor, vector<vector<vector<std::__cxx11::string> > >* raw_data_tensor);
			vector<vector<vector<string>>> raw_data_tensor_p, raw_header_tensor_p;
			vector<vector<vector<string>>>& raw_data_tensor();
			vector<vector<vector<string>>>& raw_header_tensor();
			string value_by_key(string key);
			/// "matrix = 30Si5 70Ge85 Sn10"
			
			void to_screen(string prefix="");
			string delimiter;
			set<string> identifiers;
			const std::__cxx11::string& contents_string();
			string filename_with_path;
			/*ctors*/
			contents_t(string& filename_with_path,const string& delimiter,const set<string>& identifiers);
		public:
			string matrix();
			const bool is_correct_type(); 
			string to_string(const string del = ",");
			///this does not work as intended for some reason
			bool operator< (const contents_t& obj) const;
			bool operator== (const contents_t& obj) const;
			const string creation_date_time() const;
		};
	};
	
	class sims_t
	{
	public:
		class name_t : public file_t::name_t
		{
		private:
			string filename_without_crater_depths_s="";
			vector<total_sputter_depth_t> total_sputter_depths_p;
			bool parse_sputter_energy_element_polarity();
			string secondary_polarity_p="";
			energy_t sputter_energy_p;
			string sputter_element_p;
		protected:
			name_t(string& filename_with_path_s,const string delimiter_s,const set<string> OR_identifiers_s,const set<string> AND_identifiers_s);
		public:
			const vector<string>& not_parseable_filename_parts();
			const string simple_name();
			string to_string();
			const energy_t sputter_energy();
			element_t sputter_element();
			const string secondary_polarity();
			///parses crater depths and populates filename_wo_crater
			total_sputter_depth_t total_sputter_depths();
			const string filename_without_crater_depths();
			bool operator==(name_t& obj);
			bool operator!=(name_t& obj);
		};
		class contents_t : public file_t::contents_t
		{
		public:
			virtual vector<cluster_t> clusters();
			string to_string(const string del = ",");
			///content column
			class column_t
			{
			public:
				string unit;
				string cluster_name;
				string dimension;
				vector<double> data;
				void to_screen();
				string to_string() const;
			};
		protected:
			contents_t(string& filename_with_path,const string& delimiter,const set<string>& identifiers);
			vector<column_t> columns_s;
		};
	};
	
	class dsims_t
	{
	public:
		class name_t : public sims_t::name_t
		{
		public:
			name_t(string& filename_with_path_s);
		};
		class contents_t : public sims_t::contents_t
		{
		private:
			map<string,string> infos_and_settings_p;
			///will create a map: keys->values from raw_data_tensor()
			const map<string,string>& infos_and_settings();
			const vector<double> infos_and_settings_data(const string find_this); 
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
// 			class Ipr_t
// 			{
// 			public:
// 				sputter_current_t sputter_current;
// 				sputter_time_t sputter_time;
// 				sputter_depth_t sputter_depth;
// 				Ipr_t(sputter_current_t sputter_current_s={}, sputter_time_t sputter_time={}, sputter_depth_t sputter_depth_s={});
// 				const string to_string(const string del=", ") const;
// 			};
			vector<cluster_t> clusters();
			const tm start_date_time();
			const tm creation_date_time();
			///primary current (sputter_current vs time/depth)
			const crater_t::sputter_beam_t Ipr();
			const energy_t sputter_energy(const string find_this="Impact energy (eV)");
			const secondary_voltage_t secondary_voltage(const string find_this="Secondary ion energy (V)");
			const element_t sputter_element(const string find_this="Primary ions");
			const rastersize_t sputter_rastersize(const string find_this="Raster size (um)");
			const rastersize_t analyzed_area(const string find_this="Analyzed area size (um)");
			const string secondary_polarity(const string find_this="Secondary ion polarity");
			const quantity_t chamber_pressure(const string find_this="Analysis chamber press (mbar)");
			const quantity_t egate(const string find_this="EGate rate (%)");
			const quantity_t mass_resolution(const string find_this="mass_resolution");
			const quantity_t field_aperture(const string find_this="Field aperture (um)");
			const quantity_t contrast_aperture(const string find_this="Contrast aperture (um)");
			const quantity_t entrance_slit(const string find_this="Entrance slit (um)");
			const quantity_t exit_slit(const string find_this="Exit slit (um)");
			const quantity_t energy_window(const string find_this="Energy window (eV)");
			const quantity_t em_yield(const string find_this="EM yield (%)");
			const quantity_t em_voltage(const string find_this="EM HV (V)");
			const quantity_t total_sputtering_time(const string find_this="Total sputtering time (s)");
			const quantity_t total_acquisition_time(const string find_this="Total acquisition time (s)");
			const crater_t crater();
			void to_screen(string prefix="");
			contents_t(string& filename_with_path);
		};	
		dsims_t(string& filename);
		dsims_t(files_::dsims_t::name_t& name_s, files_::dsims_t::contents_t& contents_s);
		bool operator<(dsims_t& obj);
		name_t name;
		contents_t contents;
	};
	
	class tofsims_t
	{
	public:
		class name_t : public sims_t::name_t
		{
		private:
			bool parse_analysis_energy_element(string filename_part);
			string analysis_element_p;
		public:
			name_t(string& filename_with_path_s);
			const element_t analysis_element();
			const energy_t analysis_energy();
		};
		class contents_t : public sims_t::contents_t
		{
		protected:
			bool parse_analysis_energy_element(string filename_part);
			string analysis_element_p;
		public:
			const element_t analysis_element();
			const energy_t analysis_energy();
			contents_t(string& filename_with_path);
		};
		tofsims_t(string& filename);
		tofsims_t(files_::tofsims_t::name_t& name_s, files_::tofsims_t::contents_t& contents_s);
		name_t name;
		contents_t contents;
	};
	
	class profiler_t
	{
	public:	
		class name_t : public sims_t::name_t
		{
		public:
			///string primary_method will be set as a required identifier
			name_t(string& filename_with_path_s);
		};
		class contents_t : public sims_t::contents_t
		{
		public:
			crater_t::linescan_t linescan();
			contents_t(string& filename_with_path);
		};
		profiler_t(string& filename);
		profiler_t(files_::profiler_t::name_t& name_s, files_::profiler_t::contents_t& contents_s);
		bool operator<(profiler_t& obj);
		name_t name;
		contents_t contents;
	};
	
	class jpg_t
	{
	public:
		class name_t : public sims_t::name_t
		{
		public:
			name_t(string& filename_with_path_s);
		};
		jpg_t(string& filename);
		jpg_t(name_t& filename);
		bool operator<(jpg_t& obj);
		name_t name;
	};
};

#endif // FILES_HPP
