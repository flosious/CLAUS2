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


#include "msettings.hpp"

/********************/
/****  Dsims_t  *****/
/********************/


msettings::dsims_t::dsims_t(filenames::dsims_t& filename, files::dsims_t& file) : sims_t(filename)
{
	ion_t sputter_ion_from_file = ion_t(file.sputter_element(),{{1}}); // Cs + or O2 +
	/*overwrite values from filename with contents*/
	if (sims_t::secondary_polarity != file.secondary_polarity()) logger::warning("secondary_polarity in filename differs from file contents, taking contents",filename.filename_with_path());
	if (sims_t::sputter_energy != file.sputter_energy()) logger::warning("secondary_voltage in filename differs from file contents, taking contents",filename.filename_with_path());
	if (sims_t::sputter_ion != sputter_ion_from_file ) logger::warning("sputter_ion in filename differs from file contents, taking contents",filename.filename_with_path());
	secondary_polarity = file.secondary_polarity();
	sputter_energy = file.sputter_energy();
	sputter_ion = sputter_ion_from_file;
	
	sputter_rastersize = file.sputter_rastersize();
	analyzed_area = file.analyzed_area();
	chamber_pressure = file.chamber_pressure();
	egate = file.egate();
	mass_resolution = file.mass_resolution();
	field_aperture = file.field_aperture();
	contrast_aperture = file.contrast_aperture();
	entrance_slit = file.entrance_slit();
	exit_slit = file.exit_slit();
	energy_window = file.energy_window();
	em_yield = file.em_yield();
	em_voltage = file.em_voltage();
}

const std::__cxx11::string msettings::dsims_t::to_string(const string del) const
{
	stringstream ss;
// 	const string del = ",";
	ss << sims_t::to_string() << del;
	ss << chamber_pressure.to_string() << del;
	ss << egate.to_string() << del;
	ss << mass_resolution.to_string() << del;
	ss << field_aperture.to_string() << del;
	ss << contrast_aperture.to_string() << del;
	ss << entrance_slit.to_string() << del;
	ss << exit_slit.to_string() << del;
	ss << energy_window.to_string() << del;
	ss << em_yield.to_string() << del;
	ss << em_voltage.to_string() << del;
	return ss.str();
}

bool msettings::dsims_t::operator==(const msettings::dsims_t& obj) const
{
	if (sims_t::operator!=(obj)) return false;
	return true;
}

bool msettings::dsims_t::operator!=(const msettings::dsims_t& obj) const
{
	return !operator==(obj);
}
