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
#include <iostream>
#include <string>
// #include "quantity.hpp"
#include <vector>
#include <regex>
#include <map>
#include <set>
#include "../log.hpp"
#include "../definitions.hpp"
#include "../tools.hpp"
#include "../quantity.hpp"
#include "../element.hpp"
#include "../log.hpp"
#include "../cluster.hpp"
#include "../crater.hpp"
#include "../matrix.hpp"
#include "../filenames/filename.hpp"
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
		const string delimiter;
		const set<string> identifiers;
		const std::__cxx11::string& contents_string();
		
		/*ctors*/
		file_t(filenames::filename_t& name,const string& delimiter,const set<string>& identifiers);
		const string filename_with_path;
		
	public:
		matrix_t matrix();
		const bool is_correct_type(); 
		filenames::filename_t& name;
		string to_string();
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
// 		sims_t(const std::__cxx11::string& filename_with_path_s);
		sims_t(filenames::sims_t& name,const string& delimiter,const set<string>& identifiers);
	public:
		virtual const vector<cluster_t> clusters();
		filenames::sims_t& name;
		string to_string();
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
	};



	/**************************/
	/***      dsims_t       ***/
	/**************************/
	class dsims_t : public sims_t
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
		class Ipr_t
		{
		public:
			sputter_current_t sputter_current;
			sputter_time_t sputter_time;
			sputter_depth_t sputter_depth;
			Ipr_t(sputter_current_t sputter_current_s={}, sputter_time_t sputter_time_s={}, sputter_depth_t sputter_depth_s={});
			const string to_string(const string del=", ") const;
		};
		const vector<cluster_t> clusters();
		const tm start_date_time();
		const tm creation_date_time();
		///primary current (sputter_current vs time/depth)
		const Ipr_t Ipr();
		
		const energy_t sputter_energy();
		const secondary_voltage_t secondary_voltage();
		element_t sputter_element();
		const rastersize_t sputter_rastersize();
		const rastersize_t analyzed_area();
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
	
		
		dsims_t(filenames::dsims_t& name);
		filenames::dsims_t& name;
		void to_screen(string prefix="");
	};	


	/**************************/
	/***    tofsims_t   ***/
	/**************************/
	class tofsims_t : public sims_t
	{
	protected:
		bool parse_analysis_energy_element(string filename_part);
		string analysis_element_p;
	public:
		
		const element_t analysis_element();
		const energy_t analysis_energy();
	
		tofsims_t(filenames::tofsims_t& name);
		filenames::tofsims_t& name;
	};

}
#endif // FILE_T_HPP

