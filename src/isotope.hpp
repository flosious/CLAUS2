/*
	Copyright (C) 2021Florian Bärwolf
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

#ifndef ISOTOPE_T_HPP
#define ISOTOPE_T_HPP


#include <sstream>
#include <vector>
#include <string>
#include "log.hpp"
#include "pse.hpp"
#include "quantity.hpp"
#include "definitions.hpp"



using namespace std;



class isotope_t
{
private:

public:
	isotope_t();
	///nucleons_s=-1 -> isotope with highest natural abundance
	isotope_t(string symbol_s, int nucleons_s, double abundance_s=-1, double amount_s=1);
	isotope_t(string str, double abundance_s=-1, double amount_s=1);
	const mass_t mass() const;
	const string symbol_alternative() const;
	///isotopical signature; isotopenverhältnis; Häufigkeit eines Isotopes eines Elements
	abundance_t abundance;
	substance_amount_t substance_amount;
	int nucleons;
	string symbol;
	const string to_string(const string del=" ") const;
	
	///NOT checking abundance or substance_amount
	const bool operator==(const isotope_t& obj) const;
	const bool operator!=(const isotope_t& obj) const;
	const bool operator<(const isotope_t& obj) const;
};




#endif // ISOTOPE_T_HPP
