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
#include <map>

#include "element.hpp"
// #include "matrix.hpp"
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
	static const int logger_verbosity_offset = 0;
public:
	class matrix_t
	{
	private:
		///maybe loaded from DB if not set by ctor
		///isotope mapping to its absolute concentration in amount of atoms or mole
		/// OR relative concentration in at%; enforcing always 100at% within a matrix
		void substance_amount_to_relative(vector<isotope_t>& isotopes);
		static const int logger_verbosity_offset = 11;
	public:
		matrix_t();
		/* 
		* mole/abundance/concentration/atoms input values are asumed as absolutes, which are iternally normalized to 100at%: 
		* sum of all elements must be 100 at% (will be enforced)
		* input is something like: "Si Ge30" -> [28,29,30]Si70at% + [74,73,72,70]Ge30at% === [28,29,30]Si7mol + [74,73,72,70]Ge3mol
		* single isotopes are also possible, for purified matrices: "28Si30 Ge60" -> [28]Si33at% [74,73,72,70]Ge66at%
		* uncalculateable concentrations like "Si Ge" -> will lead to a warning, matrix isotopes are saved without abundance/substance amount
		* indistinguishable isotopes like "29Si30 Si50 Ge10 Sn10" --> will lead to an error and aborting
		* not recognized isotopes --> will lead to an error an aborting
		*/
		matrix_t(const vector<string> elements_or_isotopes_s);
		matrix_t(const string matrix_elements_s);
		matrix_t(vector<isotope_t> isotopes);
		
		const vector<isotope_t> isotopes() const;
		///points to the corresponding isotope of this iso
		isotope_t* isotope(isotope_t iso);
		const isotope_t* isotope(isotope_t iso) const;
		///returns the pointer to the element within this matrix
		element_t* element(const element_t& ele);
		const element_t* element(const element_t& ele ) const;
		vector<element_t> elements;
		bool is_in(const isotope_t& iso) const;
		bool is_in(const element_t& ele) const;
		const bool is_set() const;
		const string to_string() const;
		///RELATIVE! in at%
		const quantity::concentration_t concentration(isotope_t iso) const;
		const quantity::concentration_t concentration(element_t ele) const;
		bool operator==(const matrix_t& obj) const;
		bool operator!=(const matrix_t& obj) const;
		bool operator<(const matrix_t& obj) const;
	};
private:

	static bool use_lot;
	static bool use_lot_split;
	static bool use_wafer;
	static bool use_monitor;
	static bool use_chip;
	static bool use_simple_name;
	
	matrix_t matrix_p;
	
	const database_t* sql_wrapper=nullptr;
	///all samples from all loaded files
public:
	class implant_s
	{
	private:
		static const int logger_verbosity_offset = 0;
	public:
		quantity::dose_t dose;
		quantity::concentration_t concentration_maximum;
		quantity::depth_t depth_at_concentration_maxium;
		implant_s(quantity::dose_t dose={}, quantity::concentration_t concentration_maximum={}, quantity::depth_t depth_at_concentration_maxium={});
		string to_string() const;
	};
private:
	map<isotope_t,implant_s> implants_p;
	///populates implants and matrix_p
	void load_from_database();
protected:

public:
	class db_t
	{
	private:
		static const int logger_verbosity_offset = 10;
		const database_t& sql_wrapper;
		static const string tablename;
		///saved load_from_table entries
// 		map<string,vector<string>> table_entries_s;
		map<isotope_t,implant_s> implants_s;
		matrix_t matrix_s;
		bool load_from_table();
		const sample_t& sample;
	public:
		bool insert(const implant_s implant, const string comment="");
		///load entries from old db into this(new)
		static bool migrate_claus1_db(database_t& sql_wrapper, const string filename = "migrate.database.sqlite3");
		static bool create_table(database_t& sql_wrapper);
		db_t(const sample_t& sample, const database_t& sql_wrapper);
		matrix_t& matrix();
		implant_s implant(const isotope_t& isotope);
		const map<isotope_t,implant_s>& implants();
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
	
	implant_s implant(const isotope_t& isotope);
	const map<isotope_t,implant_s>& implants() const;
	///checks if there is at least 1 isotope implanted
	bool is_implanted();
	bool is_implanted(const isotope_t& isotope);
	sample_t(files_::file_t::name_t& fn,files_::file_t::contents_t& f, database_t& sql_wrapper);
	sample_t(files_::file_t::name_t& fn,database_t& sql_wrapper);
// 	sample_t(int& wafer, string& monitor, string& lot, string& lot_split, chip_t chip, string& simple_name, matrix_t& matrix);

	string to_string(const string del=", ");
	///with out matrix
	string to_name(const string del=", ") const;
	chip_t chip;
	int wafer;
	string lot;
	string lot_split;
	string monitor;
	string simple_name;
	const string wafer_string() const;
	
	matrix_t& matrix();
	
	
	/*database stuff*/
	bool save_to_database();
// 	bool create_table();
	/****************/
	
	/*operators*/
	bool operator==(const sample_t& obj) const;
	bool operator!=(const sample_t& obj) const;
	bool operator<(sample_t& obj);
	bool operator>(sample_t& obj);
	/***********/
};

// extern database_t db;

#endif // SAMPLE_T_HPP
