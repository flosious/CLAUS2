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

using namespace std;

/*
 *  parent class
 */
class element_t
{
private:
	int atoms_p=1;
	string symbol_p;
public:
	string symbol() const;
	int atoms() const;
	string to_string(string prefix="") const;
	bool operator==(const element_t& obj) const;
	bool operator!=(const element_t& obj) const;
	bool operator<(const element_t& obj) const;
};

class isotope_t : element_t
{
private:
    int nucleons_p;
public:
	int nucleons() const;
	string to_string(string prefix="") const;
	bool operator==(const isotope_t& obj) const;
	bool operator!=(const isotope_t& obj) const;
	bool operator<(const isotope_t& obj) const;
};




#endif // ELEMENT_T_HPP
