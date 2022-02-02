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

#ifndef ION_T_HPP
#define ION_T_HPP


#include <sstream>
#include <vector>
#include <string>
#include "log.hpp"
#include "pse.hpp"
#include "quantity.hpp"
#include "definitions.hpp"
#include "element.hpp"



using namespace std;


class ion_t
{
private:
// 	const vector<element_t> elements_p;
// 	const quantity::electrical_charge_t electric_charge_p;
// 	vector< element_t > elements_p;
public:
	ion_t();
	ion_t(vector<element_t> elements_s, quantity::electrical_charge_t electric_charge_s);
	ion_t(element_t element_s, quantity::electrical_charge_t electric_charge_s);
	vector< element_t > elements;
	quantity::electrical_charge_t electric_charge;
	bool operator==(const ion_t& obj) const;
	bool operator!=(const ion_t& obj) const;
	string to_string(const string del="+") const;
	bool is_set() const;
};


#endif // ION_T_HPP
