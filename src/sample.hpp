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
#include "files.hpp"
#include "database_t.hpp"

using namespace std;


/*FORWARD DECLARE*/

/*****************/




class sample_t
{
	friend class config_t;
private:
// 	static const string db_tablename;
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

public:
	///just for implants at the moment
	class database
	{
	private:
		database_t& sql_wrapper;
		bool table_exists=false;
		static const string tablename;
		
		map<string,vector<string>> load_from_table(isotope_t isotope={});
		sample_t& sample;
	public:
		static bool create_table(database_t& sql_wrapper);
		database(sample_t& sample, database_t& sql_wrapper);
		matrix_t matrix();
		dose_t dose(const isotope_t isotope);
	};
	class chip_t
	{
	public:
		chip_t(int x=-1, int y=-1);
		int x=-1;
		int y=-1;
		bool operator==(const chip_t& obj) const;
		bool operator!=(const chip_t& obj) const;
		bool operator<(const chip_t& obj) const;
		bool operator>(const chip_t& obj) const;
		bool is_set() const;
		void to_screen(string prefix="");
		const string to_string(const string del=", ") const;
	};
	
// 	set<measurements::dsims_t*> dsims;
// 	set<measurements::tofsims_t*> tofsims;
// 	set<measurements::profiler_t*> profiler;
	database DB;
	
	sample_t(files_::file_t::name_t& fn,files_::file_t::contents_t& f, database_t& sql_wrapper);
	sample_t(files_::file_t::name_t& fn,database_t& sql_wrapper);
// 	sample_t(int& wafer, string& monitor, string& lot, string& lot_split, chip_t chip, string& simple_name, matrix_t& matrix);

	string to_string(const string del=", ");
	chip_t chip;
	int wafer;
	string lot;
	string lot_split;
	string monitor;
	string simple_name;
	string wafer_string();
	
	matrix_t& matrix();
	
	/*database stuff*/
	bool load_from_database();
	bool save_to_database();
// 	bool create_table();
	/****************/
	
	/*operators*/
	bool operator==(sample_t& obj) ;
	bool operator!=(sample_t& obj) ;
	bool operator<(sample_t& obj);
	bool operator>(sample_t& obj);
	/***********/
};

// extern database_t db;

#endif // SAMPLE_T_HPP
