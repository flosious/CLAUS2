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


#ifndef CLUSTER_T_HPP
#define CLUSTER_T_HPP

#include <string>
#include "element.hpp"
#include <vector>
#include "log.hpp"

using namespace std;

class cluster_t
{
private:
	string clustername_p;
	///each element contains the number of atoms
	vector<element_t> elements_p;
	concentration_t concentration_p;
	intensity_t intensity_p;
	sputter_time_t sputter_time_p;
	sputter_depth_t sputter_depth_p;
public:
	bool is_set() const;
	const concentration_t concentration() const;
	const intensity_t intensity() const;
	const sputter_time_t sputter_time() const;
	const sputter_depth_t sputter_depth() const;
	const vector<element_t> elements();
	cluster_t(string clustername);
	cluster_t(string clustername, sputter_time_t sputter_time_s,intensity_t intensity_s,sputter_depth_t sputter_depth_s={},concentration_t concentration_s={});
	cluster_t(vector<element_t> elements);
};

#endif // CLUSTER_T_HPP
