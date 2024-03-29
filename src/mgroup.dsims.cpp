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

mgroups_::dsims_t::dsims_t(measurements_::dsims_t& dsims_measurements) : 
                sims_t(dsims_measurements), settings_p(dsims_measurements.settings),
                class_logger(__FILE__,"mgroups_::dsims_t")
{
	measurements_p.push_back(dsims_measurements);
//    export_location_p += (settings_p.to_string_short()) + PATH_DELIMITER;
}

mgroups_::dsims_t::dsims_t(std::vector< measurements_::dsims_t* > dsims_measurements) :
    sims_t(*(dsims_measurements.front())), settings_p(dsims_measurements.front()->settings),
    class_logger(__FILE__,"mgroups_::dsims_t")
{
    measurements_p = (tools::vec::pointers_to_values(dsims_measurements));
}

mgroups_::dsims_t::dsims_t(std::vector< measurements_::dsims_t >& dsims_measurements) : 
    sims_t(dsims_measurements.front()), settings_p(dsims_measurements.front().settings),
    class_logger(__FILE__,"mgroups_::dsims_t"), measurements_p(dsims_measurements)
{
//	dsims_measurements.pop_back();
//	for (auto DM=dsims_measurements.begin();DM!=dsims_measurements.end();DM++)
//	{
//		dsims_t MG(*DM);
//		if (*this != MG) continue;
//		measurements_p.push_back(*DM);
//		dsims_measurements.erase(DM);
//		DM--;
//	}
}

bool mgroups_::dsims_t::remove_measurement(const measurements_::dsims_t* measurement)
{
    return mgroup_t::remove_measurement(measurements_p, measurement);
}
int mgroups_::dsims_t::measurement_index(const measurements_::dsims_t* measurement)
{
    return mgroup_t::measurement_index(measurements_p,measurement);
}

bool mgroups_::dsims_t::operator==(const mgroups_::dsims_t& obj) const
{
	if (sims_t::operator!=(obj)) return false;
	if (settings_p != obj.settings_p) return false;
	return true;
}

bool mgroups_::dsims_t::operator!=(const mgroups_::dsims_t& obj) const
{
	return !operator==(obj);
}

std::string mgroups_::dsims_t::to_string(const std::string del)
{
	std::stringstream ss;
	ss << mgroup_t::to_string(del) << del;
	if (use_settings) ss << settings_p.to_string(del) << del;
	ss << "measurements: <" << measurements_p.size() << ">" << std::endl;
	ss << "{" << std::endl;
	int counter = 0;
	for (auto& M : measurements_p)
	{
		ss << "\t" << "[" << counter << "] " << M.to_string(del) << std::endl;
		counter++;
	}
	ss << "}" << std::endl;
	std::string out = ss.str();
// 	out.erase(out.end()-del.length(),out.end()); // remove the las del
	return out;
}

std::vector<measurements_::sims_t*> mgroups_::dsims_t::measurements()
{
	std::vector<measurements_::sims_t*> Ms(measurements_p.size());
	for (int i=0;i<measurements_p.size();i++)
		Ms.at(i)=&measurements_p.at(i);
	return Ms;
}

std::vector<measurements_::sims_t> mgroups_::dsims_t::measurements_copy()
{
	std::vector<measurements_::sims_t> Ms;
	for (int i=0;i<measurements_p.size();i++)
		Ms.push_back(measurements_p.at(i));
	return Ms;
}



const msettings::sims_t* mgroups_::dsims_t::settings() const
{
	return &settings_p;
}
