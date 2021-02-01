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
class sample_t;


template <typename T>
set<sample_t> samples_from_files(set<T>& files)
{
	set<sample_t> samples_s;
	for (auto f:files)
	{
		samples_s.insert(f.file());
	}
	return samples_s;
}
	

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
public:
	class matrix_t
	{
	private:
		set<element_t> elements_p;
	public:
		matrix_t(set<element_t> elements_s);
		const string to_string();
		const set<element_t>& elements();
		bool operator==(matrix_t& obj) ;
		bool operator!=(matrix_t& obj) ;
	};
	class chip_t
	{
	public:
		chip_t(const int x, const int y);
		const int x;
		const int y;
		bool operator==(const chip_t& obj) const;
		bool operator!=(const chip_t& obj) const;
	};
	sample_t(string lot_s, int wafer_number_s ,string lot_split_s="", string monitor_s="", int chip_x_s=-1, int chip_y_s=-1 ,string simple_name_s="",set<element_t> elements_s={});
	sample_t(files::file_t file_s);
	sample_t(files::dsims_dp_rpc_asc_t file_s);

	const chip_t chip;
	const int wafer_number;
	const string lot;
	const string lot_split;
	const string monitor;
	const string simple_name;
	// reads from database
	matrix_t matrix;
    
	void to_screen(string prefix="");
	bool operator==(const sample_t& obj) const;
	bool operator!=(const sample_t& obj) const;
	bool operator<(const sample_t& obj) const;
	
};



#endif // SAMPLE_T_HPP
