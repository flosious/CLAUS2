/*
	Copyright (C) 2020 Florian Bärwolf
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
#ifndef PSE_HPP
#define PSE_HPP

#include <string>
#include <map>
#include <vector>
#include "quantity.hpp"
#include "tools.hpp"
#include <regex>
#include "log.hpp"


using namespace std;



class pse_t 
{
// 	friend class config_t;
private:
	const string source_url="https://physics.nist.gov/cgi-bin/Compositions/stand_alone.pl";
	const string filename="build/pse.csv";
	const string delimiter = ",";
	
	class pse_isotope_t
	{
	public:
		pse_isotope_t(const double abundance_s, const int nucleons_s, const double mass_s, const string symbol_alternative_s="");
		const double abundance;
		const int nucleons;
		const double mass;
		///H hydrogen
// 		const string symbol;
		///like D for 2H
		const string symbol_alternative;
		const bool operator==(const pse_isotope_t& obj) const;
		const bool operator<(const pse_isotope_t& obj) const;
	};
	class pse_element_t
	{
	public:
		pse_element_t(string symbol_s, int protons_s, vector<pse_isotope_t>& isotope_s);
		pse_element_t(string symbol_s, int protons_s, pse_isotope_t& isotope_s);
// 		pse_element_t(const set<pse_isotope_t>& isotope_s);
		const int protons;
		const string symbol;
		const vector<pse_isotope_t> isotopes;
// 		pse_element_t(const vector<isotope_t>& isotopes_s);
// 		const vector<isotope_t> isotopes;
		const pse_isotope_t* isotope_with_highest_abundance() const;
		const pse_isotope_t* isotope_from_nucleons(int nucleons) const;
		const double mass() const;
		const bool operator==(const pse_element_t& obj) const;
		const bool operator<(const pse_element_t& obj) const;
		
	};
// 	map<string,element_t> symbol_to_element_p;
// 	vector<isotope_t> isotopes_p;
	
	vector<pse_element_t> elements_p;
public:
// 	pse_t(const vector<pse_element_t> elements_s);
	const vector<pse_element_t>& elements();
	const pse_element_t* element(string symbol);
	
	const vector<pse_isotope_t>* isotopes(string symbol="");
	const pse_isotope_t* isotope(string symbol, int nucleons);
	
	bool load_file();
	///all natural(earth) elements within the PSE
	
	///an overview/console dump
	void to_screen();
	///source URL, which feeded the pse.csv file in the year 2018
	const string source() const;
};

// extern const pse_t PSE;

#endif // PSE_HPP
