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

#include <vector>
#include <string>
#include "quantity.hpp"
#include "element.hpp"

using namespace std;

namespace settings
{

/*
 * parent class
 */
class beam_settings_t
{
public:
    const energy_t energy;
	const rastersize_t raster_size;
    const ion_t ion;
	beam_settings_t(energy_t energy_s, rastersize_t rastersize_s, ion_t ion_s);
};

class sims_t
{
public:
	const string secondary_polarity;
	const beam_settings_t sputter_beam;
	sims_t(beam_settings_t sputter_beam_s, string secondary_polarity_s);
};

/*
 * direct copy of its parent
 */
class dsims_t : public sims_t
{
public:
	dsims_t(beam_settings_t sputter_beam_s, string secondary_polarity_s);
	///from secondary voltage?
	const string secondary_polarity() const;
	const secondary_voltage_t secondary_voltage();
	
	const quantity_t chamber_pressure;
	const quantity_t egate;
	const quantity_t mass_resolution;
	const quantity_t field_aperture;
	const quantity_t contrast_aperture;
	const quantity_t entrance_slit;
	const quantity_t exit_slit;
	const quantity_t energy_window;
	const quantity_t em_yield;
	const quantity_t em_voltage;
	
};

class tofsims_t : public sims_t
{
public:
	const beam_settings_t analysis_beam;
	tofsims_t(beam_settings_t sputter_beam_s, string secondary_polarity_s, beam_settings_t analysis_beam_s);
};
}
#endif // MEASUREMENT_SETTINGS_T_HPP
