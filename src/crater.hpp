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

#ifndef CRATER_T_HPP
#define CRATER_T_HPP

#include "quantity.hpp"
#include "fit_functions.hpp"
// #include "filename.hpp"
// #include "filecontents.hpp"


class linescan_t
{
private:
	vector <double> fit_parameters;
	fit_functions::asym2sig_t asym2sig;
	bool fitted=false;
public:
	linescan_t();
	linescan_t(quantity_t xy,quantity_t z);
	fit_functions::asym2sig_t fit_params();
	/// the depth or height (z)
	quantity_t z;
	/// the surface (xy)
	quantity_t xy;
	/// calculates the depth from profile and returns it mean
	quantity_t depth();
	/// calculates roughness
	quantity_t roughness();
	/// origin -> peak-functions -> Asym2Sig
	quantity_t fit();
	string to_string(string prefix="");
	bool is_set();
};


class crater_t
{
public:
	total_sputter_depth_t total_sputter_depths;
	total_sputter_time_t total_sputter_time;
	vector<linescan_t> linescans;
};


#endif // CRATER_T_HPP
