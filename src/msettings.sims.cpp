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

msettings::sims_t::sims_t(files_::sims_t::name_t& filename) : 
		sputter_ion(ion_t(filename.sputter_element(),{{1}})), secondary_polarity(filename.secondary_polarity()), sputter_energy(filename.sputter_energy())
{
// 	secondary_polarity = filename.secondary_polarity();
// 	secondary_voltage = filename.secondary_voltage();
// 	sputter_ion = ion_t(filename.sputter_element(),{{1}});
}

bool msettings::sims_t::operator==(const msettings::sims_t& obj) const
{
	if (secondary_polarity != obj.secondary_polarity) return false;
	if (sputter_energy != obj.sputter_energy) return false;
	if (sputter_ion != obj.sputter_ion) return false;
	return true;
}

bool msettings::sims_t::operator!=(const msettings::sims_t& obj) const
{
	return !operator==(obj);
}

bool msettings::sims_t::is_set()
{
	if (secondary_polarity=="") return false;
	if (!sputter_energy.is_set()) return false;
	if (!sputter_ion.is_set()) return false;
	return true;
}

const string msettings::sims_t::to_string(const string del) const
{
	stringstream ss;
	ss << sputter_energy.to_string() << del;
	ss << sputter_ion.to_string() << del;
	ss << "secondary_polarity: " << secondary_polarity;
	return ss.str();
}

const string msettings::sims_t::to_string_short() const
{
	if (!sputter_energy.is_scalar() || !sputter_ion.is_set()) 
		return "";
	quantity::energy_t SE;
	stringstream ss;
	if (sputter_energy.data.at(0)>=1000)
		SE =sputter_energy.change_unit(units::prefixes::kilo * sputter_energy.unit());
	else
		SE = sputter_energy;
	
// 	element_t E = sputter_ion.elements.at(0);
// 	PSE.isotope()
	
	ss << SE.data.at(0) << SE.unit().to_string();
	
	for (auto& ele : sputter_ion.elements)
		ss << ele.isotopes.at(0).symbol;
	
	ss << secondary_polarity;
	return ss.str();
}
