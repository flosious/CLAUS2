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

#ifndef SAMPLE_T_HPP
#define SAMPLE_T_HPP

#include "file.hpp"
#include <unordered_set>
#include <vector>
#include <string>
#include <list>
#include "element.hpp"
#include "matrix.hpp"
#include "measurement.hpp"

using namespace std;

class dsims_measurement_t;

class sample_t
{
private:
	friend class config_t;
	class definition_t
	{
	public:
		static bool wafer_number;
		static bool lot;
		static bool lot_split;
		static bool monitor;
		static bool chip;
		static bool simple_name;
	} definition;
	int wafer_number_p=-1;
	string lot_p="";
	string lot_split_p="";
	string monitor_p="";
	string simple_name_p="unknown_sample_name";
	matrix_t matrix_p;
public:
	sample_t(string lot_s, int wafer_number_s ,string lot_split_s="", string monitor_s="", int chip_x_s=-1, int chip_y_s=-1 ,string simple_name_s="");
	sample_t(files::file_t file_s);
	sample_t(files::sims_t file_s);
	sample_t(files::dsims_dp_rpc_asc_t file_s);
	sample_t(files::tofsims_TXT_t file_s);
	
	
	class chip_t
	{
		friend class sample_t;
	private:
		int x_p=-1;
		int y_p=-1;
	public:
		int x() const;
		int y() const;
		bool operator==(const chip_t& obj) const;
		bool operator!=(const chip_t& obj) const;
	} chip;
	int wafer_number() const;
	string lot() const;
	string lot_split() const;
	string monitor() const;
	//used, when lot or wafer is empty
	string simple_name() const;
// 	sample_t(string& lot_s, string& lot_split_s,int& wafer_s,string& monitor_s, chip_t& chip_s, string& simple_name_s);
// 	sample_t(files::file_t& file);
	// reads from database
	matrix_t matrix() const;
    unordered_set<dsims_measurement_t*> dsims_measurements;
	void to_screen(string prefix="") const;
	bool operator==(const sample_t& obj) const;
	bool operator!=(const sample_t& obj) const;
	bool operator<(const sample_t& obj) const;
	
};



#endif // SAMPLE_T_HPP
