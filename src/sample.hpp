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
#include "definitions.hpp"
#include <set>
#include "measurement.hpp"

using namespace std;

class measurement_t;
class dsims_measurement_t;
class sample_t;





class sample_t
{
	friend class config_t;
private:
	static bool use_lot;
	static bool use_lot_split;
	static bool use_wafer;
	static bool use_monitor;
	static bool use_chip;
	static bool use_simple_name;
	static bool use_matrix;
	///all samples from all loaded files
	static vector<sample_t> samples_list_p;
public:
	///files corresponding to this sample
	set<file_t*> files;
	class chip_t
	{
	public:
		chip_t(int x=-1, int y=-1);
		int x;
		int y;
		bool operator==(const chip_t& obj) const;
		bool operator!=(const chip_t& obj) const;
		bool is_set();
		void to_screen(string prefix="");
	};
	sample_t(file_t& file_s);
	sample_t(file_t* file_s);
// 	sample_t(string lot, string lot_split, int wafer,chip_t chip, string monitor,string simple_name);
	
	chip_t chip();
	int wafer();
	string lot();
	string lot_split();
	string monitor();
	string simple_name();
	
	// reads from database or file
	matrix_t matrix();
	
	void to_screen(string prefix="");
	/*operators*/
	bool operator==( sample_t& obj) ;
	bool operator!=( sample_t& obj) ;
	bool operator<( sample_t& obj) ;
	
	///all samples from all loaded files
	static vector<sample_t>* samples_list();
};



#endif // SAMPLE_T_HPP
