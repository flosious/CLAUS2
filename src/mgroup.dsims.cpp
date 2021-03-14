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


#include "mgroup.hpp"

mgroups::dsims_t::dsims_t(measurements_::dsims_t& dsims_measurements) : 
				sims_t(dsims_measurements), settings(dsims_measurements.settings)
{
	measurements.push_back(dsims_measurements);
}

mgroups::dsims_t::dsims_t(std::vector< measurements_::dsims_t >& dsims_measurements) : 
									dsims_t(dsims_measurements.back())
{
	dsims_measurements.pop_back();
	for (vector<measurements_::dsims_t>::iterator DM=dsims_measurements.begin();DM!=dsims_measurements.end();DM++)
	{
		dsims_t MG(*DM);
		if (*this != MG) continue;
		measurements.push_back(*DM);
		dsims_measurements.erase(DM);
		DM--;
	}
}

bool mgroups::dsims_t::operator==(const mgroups::dsims_t& obj) const
{
	if (sims_t::operator!=(obj)) return false;
	if (settings != obj.settings) return false;
	return true;
}

bool mgroups::dsims_t::operator!=(const mgroups::dsims_t& obj) const
{
	return !operator==(obj);
}

std::__cxx11::string mgroups::dsims_t::to_string(const string del)
{
	stringstream ss;
	ss << mgroup_t::to_string(del) << del;
	if (use_settings) ss << settings.to_string(del) << del;
	ss << "measurements: <" << measurements.size() << ">" << endl;
	ss << "{" << endl;
	int counter = 0;
	for (auto& M : measurements)
	{
		ss << "\t" << "[" << counter << "] " << M.to_string(del) << endl;
		counter++;
	}
	ss << "}" << endl;
	string out = ss.str();
// 	out.erase(out.end()-del.length(),out.end()); // remove the las del
	return out;
}
