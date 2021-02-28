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
#include "../quantity.hpp"
#include "../element.hpp"
#include "../files/file.hpp"

using namespace std;

namespace msettings
{

class beam_settings_t
{
public:
    const energy_t energy;
	const rastersize_t raster_size;
    const ion_t ion;
	beam_settings_t(energy_t energy_s, rastersize_t rastersize_s, ion_t ion_s);
};

/*
class sputter_beam_settings_t : public beam_settings_t
{
public:
	const sputter_current_t sputter_current;
	sputter_beam_settings_t(sputter_current_t sputter_current,energy_t energy_s, rastersize_t rastersize_s, ion_t ion_s);
};

class analysis_beam_settings_t : public beam_settings_t
{
public:
	const analysis_current_t analysis_current;
};
*/

class sims_t
{
protected:
	string secondary_polarity_p;
	ion_t sputter_ion_p;
	secondary_voltage_t secondary_voltage_p;
	set<files::sims_t*> files_p;
public:
	string to_string();
	virtual string& secondary_polarity();
	virtual secondary_voltage_t& secondary_voltage();
	virtual ion_t& sputter_ion();
	sims_t();
	sims_t(files::sims_t* file_p);
	sims_t(set<files::sims_t*> file_p);
	bool operator==(sims_t obj);
	bool operator!=(sims_t obj);
	bool is_set();
};

class dsims_t : public sims_t
{
protected:
	set<files::dsims_t*> files_p;
public:
	dsims_t();
	dsims_t(set<files::dsims_t*> files_p);
	string& secondary_polarity();
	secondary_voltage_t& secondary_voltage();
	ion_t& sputter_ion();
	string to_string();
	const rastersize_t sputter_rastersize();
	const rastersize_t analyzed_area();
	const quantity_t chamber_pressure();
	const quantity_t egate();
	const quantity_t mass_resolution();
	const quantity_t field_aperture();
	const quantity_t contrast_aperture();
	const quantity_t entrance_slit();
	const quantity_t exit_slit();
	const quantity_t energy_window();
	const quantity_t em_yield();
	const quantity_t em_voltage();
	bool operator==(dsims_t& obj);
	bool operator!=(dsims_t& obj);
};

class tofsims_t : public sims_t
{
protected:
	set<files::tofsims_t*> files_p;
public:
	tofsims_t();
// 	const beam_settings_t analysis_beam();
	tofsims_t(set<files::tofsims_t*> files_p);
	string to_string();
};
}
#endif // MEASUREMENT_SETTINGS_T_HPP
