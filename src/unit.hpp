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

#ifndef UNIT_HPP
#define UNIT_HPP

#include <math.h>
#include <vector>
#include <string>
#include <sstream>
#include <map>

using namespace std;

/// just supporting the SI system at the moment
class unit_t
{
private:
	string name_p;
	static const map<string,map<string,double>> current_unit_to_new_unit;
public:
	unit_t(string name_s="");
	string name() const;
	double change_unit(unit_t new_unit);
	bool operator==(const unit_t& obj) const;
	bool operator!=(const unit_t& obj) const;
	unit_t operator*(const unit_t& obj) const;
	unit_t operator/(const unit_t& obj) const;
};


#endif // UNIT_HPP
