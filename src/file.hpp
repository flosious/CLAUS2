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
#include "DEFINES.hpp"
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
// #include "sample.hpp"
// #include "measurement.hpp"
// #include "measurement_group.hpp"

using namespace std;

namespace files
{
///parent class
class file_t
{
	friend class config_t;
protected:

	
	vector<vector<vector<string>>> raw_data_tensor_p, raw_header_tensor_p;
	///populates raw_data_tensor_p, raw_header_tensor_p
	bool parse_data_and_header_tensors(vector<vector<vector<std::__cxx11::string> > >* raw_header_tensor, vector<vector<vector<std::__cxx11::string> > >* raw_data_tensor);
	vector<vector<vector<string>>> raw_data_tensor();
	vector<vector<vector<string>>> raw_header_tensor();
	vector<string> error_messages_p;
	bool error=false;
	bool correct_file_type=false;
	string tool_name="";
	set<string> filename_type_ids_p;
	set<string> file_content_ids_p;
	string delimiter; 
	bool parsed=false;
	bool parse_filename_parts();
	bool parse_all_parts_at_once();
	bool parse_olcdb(string filename_part);
	bool parse_lot(string filename_part);
	bool parse_wafer(string filename_part);
	bool parse_chip(string filename_part);
	bool parse_monitor(string filename_part);
	bool parse_group(string filename_part);
	bool parse_repetitor(string filename_part);
	
	const bool is_correct_file_type(/*const set<string>& filename_type_ids,const set<string>& file_content_ids*/); 
	
	vector<string> not_parseable_filename_parts_p;
	string filename_with_path_p="";
	string group_p="";
	string lot_p="";
	string lot_split_p="";
	string monitor_p="";
	string repetition_p="";
	int wafer_p=-1;
	int chip_x_p=-1;
	int chip_y_p=-1;
	int olcdb_p=-1;
	
	string contents_p;
	
	class name_t
	{
	private:
		const string& name_with_path_p;
	public:
		name_t(string& name_with_path_s);
		string name();
		string name_with_path();
	};
public:
	///returns the file_t parent
	file_t file() const;
	file_t(string filename_with_path_s);
	vector<string> not_parseable_filename_parts();
	string filename_with_path() const;
	string filename_type_ending() const;
	string filename() const;
	string directory() const;
	string load_contents();
	string group();
	string lot();
	string lot_split();
	string monitor();
	string repetition();
	int chip_x();
	int chip_y();
	int olcdb();
	int wafer();
	
	name_t name;
	
	void to_screen(string prefix="");
	
	bool operator< (const file_t& fname) const;
	bool operator== (const file_t& fname) const;
	
};

///parent and child class
class sims_t : public file_t
{
protected:
	class column_t
	{
	public:
		string unit;
		string cluster_name;
		string dimension;
		vector<double> data;
		void to_screen();
	};
	sputter_time_t sputter_time_p;
	sputter_depth_t sputter_depth_p;
	vector<cluster_t> clusters_p;
	
	
	string polarity_p="";
	sputter_energy_t sputter_energy_p;
	element_t sputter_element_p;
	sputter_rastersize_t sputter_rastersize_p;
	vector<total_sputter_depth_t> total_sputter_depths_p;
	const bool parse_sputter_energy_element_polarity();
// 	const vector<column_t> columns();
	class name_t : file_t::name_t
	{
	public:
		name_t(string& name_with_path_s);
		string test();
	};
	
public:
// 	name_t test;
	sims_t(string filename_with_path_s);
	const string polarity();
	
	const sputter_energy_t sputter_energy();
	const element_t sputter_element();
	const sputter_rastersize_t sputter_rastersize();
	const string filename_without_crater_depths();
	const total_sputter_depth_t total_sputter_depths();
	
// 	const vector<cluster_t> clusters();
};

class dsims_dp_rpc_asc_t : public sims_t
{
private:
// 	const settings::dsims_t settings_p;
	
	
	vector<string> units_p;
	vector<string> dimensions_p;
	vector<string> cluster_names_p;
	///e.g. < sputter_time > = get_quantity_from_dimension_and_clustername("Time","Ipr")
	const quantity_t get_quantity_from_dimension_and_clustername(const string col_dimension, const string ignore_clustername="");
	
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
	/// primary current
	const vector<string> units();
	const vector<string> dimensions();
	const vector<string> cluster_names();
	const vector<column_t> columns();
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
	};
	
// 	class settings_t
// 	{
// 		
// 	};
public:
	dsims_dp_rpc_asc_t(string filename_with_path_s);
	const Ipr_t Ipr();
// 	const settings_t settings();
	static set<dsims_dp_rpc_asc_t> files(set<string>& filenames_with_path);
	
	const sputter_time_t greatest_common_sputter_time();
	const sputter_depth_t greatest_common_sputter_depth();
	const vector<cluster_t> clusters();
};

class tofsims_t : public sims_t
{
private:
	bool parse_analysis_energy_element(string filename_part);
// 	element_t analysis_element_p;
// 	analysis_energy_t analysis_energy_p;
public:
	tofsims_t(string filename_with_path_s);
	const element_t analysis_element();
	const analysis_energy_t analysis_energy();
	static set<tofsims_t> files(set<string>& filenames_with_path);
	///identifiers within the filename ending
// 	static const set<string> filename_type_ids_p;
	///identifiers within the file contents
// 	static const set<string> file_content_ids_p;
// 	const bool is_tofsims() const;
};

class profiler_t : public file_t 
{
public:
	profiler_t(string filename_with_path_s);
	///identifiers within the filename ending
// 	static const set<string> filename_type_ids_p;
	///identifiers within the file contents
// 	static const set<string> file_content_ids_p;
// 	const bool is_profile() const;
};

}
#endif // FILE_T_HPP

