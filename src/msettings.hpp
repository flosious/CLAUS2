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

#ifndef MEASUREMENT_SETTINGS_T_HPP
#define MEASUREMENT_SETTINGS_T_HPP

#include <set>
#include <vector>
#include <string>
#include "quantity.hpp"
#include "element.hpp"
#include "files.hpp"
#include "log.hpp"
#include "ion.hpp"

using namespace std;

class msettings
{
public:
	class sims_t
	{
	public:
		sims_t(files::sims_t::name_t& filename);
		const std::__cxx11::string to_string(const std::__cxx11::string del = ", ") const;
		string secondary_polarity;
		energy_t sputter_energy;
		ion_t sputter_ion;
		bool operator==(const sims_t& obj) const;
		bool operator!=(const sims_t& obj) const;
		bool is_set();
	};

	class dsims_t : public sims_t
	{
	public:
		dsims_t(files::dsims_t::name_t& filename, files::dsims_t::contents_t& filecontents);
		dsims_t(files::dsims_t& file);
		const string to_string(const string del=", ") const;
		
		rastersize_t sputter_rastersize;
		rastersize_t analyzed_area;
		quantity_t chamber_pressure;
		quantity_t egate;
		quantity_t mass_resolution;
		quantity_t field_aperture;
		quantity_t contrast_aperture;
		quantity_t entrance_slit;
		quantity_t exit_slit;
		quantity_t energy_window;
		quantity_t em_yield;
		quantity_t em_voltage;
		
		bool operator==(const dsims_t& obj) const;
		bool operator!=(const dsims_t& obj) const;
	};

// 	class tofsims_t : public sims_t
// 	{
// 	public:
// 		tofsims_t();
// 	// 	const beam_settings_t analysis_beam();
// 		tofsims_t(set<files::tofsims_t*> files_p);
// 		string to_string();
// 	};
};
#endif // MEASUREMENT_SETTINGS_T_HPP
