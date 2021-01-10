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
// #include "sample.hpp"
// #include "measurement.hpp"
// #include "measurement_group.hpp"

using namespace std;

namespace files
{
///parent class
class file_t
{
protected:
	static const string delimiter; 
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
	
public:
	file_t(string filename_with_path_s);
	vector<string> not_parseable_filename_parts() const;
	string filename_with_path() const;
	string type_ending() const;
	string filename() const;
	string directory() const;
	string load_contents() const;
	string group() const;
	string lot() const;
	string lot_split() const;
	string monitor() const;
	string repetition() const;
	int chip_x() const;
	int chip_y() const;
	int olcdb() const;
	int wafer() const;
	
	void to_screen(string prefix="") const;

	bool operator< (const file_t& fname) const;
	bool operator== (const file_t& fname) const;
	
// 	sample_t sample() const;
};

///parent class
class sims_t : file_t
{
protected:
	bool parse_crater_depth(string filename_part);
	bool parse_sputter_energy_element(string filename_part);
	element_t sputter_element_p;
	sputter_energy_t sputter_energy_p;
	vector<total_sputter_depth_t> total_sputter_depths_p;
public:
	string filename_without_crater_depths();
	element_t sputter_element();
	sputter_energy_t sputter_energy();
	///crater depths
	total_sputter_depth_t total_sputter_depths();
};

class dsims_t : sims_t
{
private:
public:
	static set<dsims_t> files(set<string>& filenames_with_path);
};

class tofsims_t : sims_t
{
private:
	bool parse_analysis_energy_element(string filename_part);
	element_t analysis_element_p;
	analysis_energy_t analysis_energy_p;
public:
	element_t analysis_element();
	analysis_energy_t analysis_energy();
	static set<tofsims_t> files(set<string>& filenames_with_path);
};
}
#endif // FILE_T_HPP

