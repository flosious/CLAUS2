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


#ifndef FILENAME_T_HPP
#define FILENAME_T_HPP

#include <vector>
#include <string>
#include <regex>

#include "../element.hpp"
#include "../log.hpp"


using namespace std;

namespace filenames
{
	///standard IHP olc filename template: olcdb_lot#split_wafernumber_chip_monitor_settings_etc_group_craterdepth
	class filename_t
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
		filename_t(const string& filename_with_path_s, const string delimiter_s,const set<string> identifiers_s);
	public:
		string to_string();
		const string delimiter;
		const set<string> identifiers;
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
	
	class sims_t : public filename_t
	{
	private:
		vector<total_sputter_depth_t> total_sputter_depths_p;
		bool parse_sputter_energy_element_polarity();
		string secondary_polarity_p="";
		secondary_voltage_t secondary_voltage_p;
		string sputter_element_p;
	protected:
		sims_t(const string& filename_with_path_s,const string delimiter_s,const set<string> identifiers_s);
	public:
		string to_string();
		const secondary_voltage_t secondary_voltage();
		element_t sputter_element();
		const string secondary_polarity();
		const total_sputter_depth_t total_sputter_depths();
		const string filename_without_crater_depths();
	};	
	
	/*publicly applicable*/
	
	/************************/
	/** dsims_t **/
	/************************/
	class dsims_t : public sims_t
	{
	public:
		dsims_t(const string& filename_with_path_s);
	};
	
	/************************/
	/**      jpg_t         **/
	/************************/
	class sims_jpg_t : public sims_t
	{
	public:
		sims_jpg_t(const string& filename_with_path_s);
	};
	
	/************************/
	/**   tofsims_t    **/
	/************************/
	class tofsims_t : public sims_t
	{
	private:
		bool parse_analysis_energy_element(string filename_part);
		string analysis_element_p;
	public:
		tofsims_t(const string& filename_with_path_s);
		const element_t analysis_element();
		const energy_t analysis_energy();
	};
	
	/************************/
	/**   profiler_t       **/
	/************************/
	class profiler_t : public filename_t
	{
	public:
		profiler_t(const string& filename_with_path_s);
	};
}

#endif // FILENAME_T_HPP
