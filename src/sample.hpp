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

// #include "file.hpp"
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


// template <typename T>
// set<sample_t> samples_from_files(set<T>& files)
// {
// 	set<sample_t> samples_s;
// 	for (auto f:files)
// 	{
// 		samples_s.insert(f.file());
// 	}
// 	return samples_s;
// }



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
public:
	
	class chip_t
	{
	public:
		chip_t(const int x, const int y);
		const int x;
		const int y;
		bool operator==(const chip_t& obj) const;
		bool operator!=(const chip_t& obj) const;
	};
// 	sample_t(const file_t& file_s);
// 	sample_t(files::dsims_dp_rpc_asc_t file_s);
	sample_t(string lot, string lot_split, int wafer,chip_t chip, string monitor,string simple_name);
	
	const set<const measurement_t*> measurements();
	
// 	const string group;
// 	const int olcdbid;
// 	const string repition;
	const chip_t chip;
	const int wafer;
	const string lot;
	const string lot_split;
	const string monitor;
	const string simple_name;
	
	// reads from database or file
	matrix_t matrix;
    
	void to_screen(string prefix="");
	/*operators*/
	bool operator==(const sample_t& obj) const;
	bool operator!=(const sample_t& obj) const;
	bool operator<(const sample_t& obj) const;
	
};



#endif // SAMPLE_T_HPP
