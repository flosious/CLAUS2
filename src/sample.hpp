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


#include <unordered_set>
#include <vector>
#include <string>
#include <list>
#include <set>

#include "element.hpp"
#include "matrix.hpp"
#include "definitions.hpp"
// #include "measurement.hpp"
#include "file.hpp"

using namespace std;


/*FORWARD DECLARE*/

/*****************/




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
	
	matrix_t matrix_p;
	///all samples from all loaded files
// 	static vector<sample_t> samples_list_p;
protected:
	///local matrix its necessary, because we have to work in it (concentrations)
public:
	class chip_t
	{
	public:
		chip_t(int x=-1, int y=-1);
		int x=-1;
		int y=-1;
		bool operator==(const chip_t& obj) const;
		bool operator!=(const chip_t& obj) const;
		bool is_set();
		void to_screen(string prefix="");
		const string to_string(const string del=", ") const;
	};
	
// 	set<measurements::dsims_t*> dsims;
// 	set<measurements::tofsims_t*> tofsims;
// 	set<measurements::profiler_t*> profiler;
	
	
	sample_t(filenames::filename_t& fn,files::file_t& f);
// 	sample_t(set<filenames::filename_t*> filenames,set<files::file_t*> files);
	string to_string(const string del=", ");
	const chip_t chip;
	const int wafer;
	const string lot;
	const string lot_split;
	const string monitor;
	const string simple_name;
	
	// reads from database
	matrix_t& matrix();
	
// 	void to_screen(string prefix="");
	/*operators*/
	bool operator==(sample_t& obj) ;
	bool operator!=(sample_t& obj) ;
// 	bool operator<(sample_t& obj) ;
	
	///all samples from all loaded files
// 	list<sample_t> samples_list();
	
};



#endif // SAMPLE_T_HPP
