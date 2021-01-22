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

#ifndef ELEMENT_T_HPP
#define ELEMENT_T_HPP

#include "DEFINES.hpp"
#include <sstream>
#include <vector>
#include <string>
#include "pse.hpp"

using namespace std;


class element_t
{
protected:
	///-1 will use the nucleons from the element from PSE with highest abundance
	int nucleons_p=-1;
	int atoms_p=1;
	string symbol_p;
public:
	const bool is_set();
	mass_t mass() const;
	abundance_t abundance() const;
	///e.g. "30Si2" or "Si2"==28Si2
	element_t(int nucleons_s, string symbol_s);
	element_t(string element_str);
	element_t();
	///"Si" in 30Si2
	string symbol() const;
	///"2" in 30Si2
	int atoms() const;
	///"30" in 30Si2
	const int nucleons() const;
	string name() const;
	bool operator==(const element_t& obj) const;
	bool operator!=(const element_t& obj) const;
	bool operator<(const element_t& obj) const;
};


// class isotope_t : element_t
// {
// private:
//     
// public:
// 	///e.g. "30Si2" or "Si2"==28Si2
// 	isotope_t(string isotope_str);
// 	isotope_t();
// 	///"30" in 30Si2
// 	int nucleons() const;
// 	string to_string() const;
// 	bool operator==(const isotope_t& obj) const;
// 	bool operator!=(const isotope_t& obj) const;
// 	bool operator<(const isotope_t& obj) const;
// };




#endif // ELEMENT_T_HPP
