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

#ifndef MATRIX_T_HPP
#define MATRIX_T_HPP

#include <algorithm>
#include <sstream>
#include <set>
#include <string>
#include <list>

#include "element.hpp"
#include "definitions.hpp"

using namespace std;

class matrix_t
{
private:
	///maybe loaded from DB if not set by ctor
	///isotope mapping to its absolute concentration in amount of atoms or mole
	/// OR relative concentration in at%; enforcing always 100at% within a matrix
// 	map<isotope_t,double> isotopes_amount;
	
public:
	matrix_t();
	matrix_t(const vector<string> elements_or_isotopes_s);
	matrix_t(const string matrix_elements_s);
	vector<isotope_t> isotopes;
	const bool is_set() const;
	const string to_string();
	///RELATIVE! in at%
	const concentration_t concentration(isotope_t& iso) const;
	bool operator==(matrix_t& obj) ;
	bool operator!=(matrix_t& obj) ;
	bool operator<(matrix_t& obj) ;
};
	



#endif // MATRIX_T_HPP
