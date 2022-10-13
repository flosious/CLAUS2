/*
	Copyright (C) 2021, 2022 Florian Bärwolf
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


class msettings
{
public:
	class sims_t
	{
    private:
        class_logger_t logger;
	public:
		sims_t();
		sims_t(files_::sims_t::name_t& filename);
		const std::string to_string(const std::string del = ", ") const;
		const std::string to_string_short() const;
		std::string secondary_polarity;
		quantity::energy_t sputter_energy;
		ion_t sputter_ion;
		bool operator==(const sims_t& obj) const;
		bool operator!=(const sims_t& obj) const;
        bool is_set() const;
	};

	class dsims_t : public sims_t
	{
    private:
        class_logger_t logger;
	public:
		dsims_t();
		dsims_t(files_::dsims_t::name_t& filename, files_::dsims_t::contents_t& filecontents);
		dsims_t(files_::dsims_t& file);
        const std::string to_string(const std::string del=", ") const;
		
		quantity::sputter_time_t total_sputter_time;
		quantity::sputter_time_t total_acquisition_time;
		quantity::rastersize_t sputter_rastersize;
		quantity::rastersize_t analyzed_area;
		quantity::quantity_t chamber_pressure;
		quantity::quantity_t egate;
		quantity::quantity_t mass_resolution;
		quantity::quantity_t field_aperture;
		quantity::quantity_t contrast_aperture;
		quantity::quantity_t entrance_slit;
		quantity::quantity_t exit_slit;
		quantity::quantity_t energy_window;
		quantity::quantity_t em_yield;
		quantity::quantity_t em_voltage;
		
		bool operator==(const dsims_t& obj) const;
		bool operator!=(const dsims_t& obj) const;
	};

	class tofsims_t : public sims_t
	{
    private:
        class_logger_t logger;
	public:
		tofsims_t(files_::tofsims_t::name_t& filename, files_::tofsims_t::contents_t& filecontents);
		tofsims_t(files_::tofsims_t& file);
		
		ion_t analysis_ion;
		quantity::energy_t analysis_energy;
		
		bool operator==(const tofsims_t& obj) const;
		bool operator!=(const tofsims_t& obj) const;
	};
};

#endif // MEASUREMENT_SETTINGS_T_HPP
