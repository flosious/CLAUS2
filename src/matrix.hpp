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

#include "DEFINES.hpp"
#include <sstream>
#include <set>
#include <string>
#include <list>
#include "element.hpp"

using namespace std;

class matrix_t
{
private:
public:
	string to_string();
	set<element_t> elements;
	set<element_t> isotopes;
	bool operator==(const matrix_t& obj) const;
	bool operator!=(const matrix_t& obj) const;
};



#endif // MATRIX_T_HPP
