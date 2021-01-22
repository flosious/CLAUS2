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
// #include "isotope.hpp"

using namespace std;




class pse_t 
{
// 	friend class config_t;
private:
	static vector<string> file_locations;
	const string url="https://physics.nist.gov/cgi-bin/Compositions/stand_alone.pl";
	
	const string default_filename="pse.csv";
	const string delimiter = ",";
	
	/// maps an isotope to its abundance "30Si" -> 0.03...
	map <string, abundance_t> isotopes_abundance;
	/// maps an isotope to its mass "30Si" -> 29.973...
	map <string, mass_t> isotopes_mass;
	/// maps an element to its isotopes "Si" -> {28Si,29Si,30Si}
	map <string, vector<string>> elements_isotopes;
	/// lists all elements within the PSE
	vector<string> list_elements;
	/// lists all isotopes within the PSE: starting from 1H, 2H, ...30Si,...70Ge,..
	vector<string> list_isotopes;
	void add_isotopes_with_highest_abundance_to_maps();
	
public:
	
	/// parses all pse files
	int load();
	/// parses the pse filename with delimiter (loads the pse)
	int load_file(string filename);
	/// lists all elements within the PSE
	const vector<string> get_all_elements();
	/// lists all isotopes within the PSE: starting from 1H, 2H, ...30Si,...70Ge,..
	const vector<string> get_all_isotopes();
	/// return all isotopes as element_t data structure
// 	vector<element_t> get_all();
	/// returns the isotope with the highest abundance for a specific element: 28Si for Si
	const string get_isotope_with_highest_abundance(string element);
	/// returns all isotopes with highest abundance for each element within the PSE
	const vector<string> get_all_isotopes_with_highest_abundance();
	
	
	const abundance_t get_abundance_from_isotope(string isotope);
	const mass_t get_mass_from_isotope(string isotope);
	const vector<string> get_isotopes_from_element(string element);
	
	void print_t(string isotope);
	void print_t();
	
};

extern pse_t PSE;

#endif // PSE_HPP
