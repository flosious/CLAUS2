/*
	Copyright (C) 2022 Florian Bärwolf
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

mgroups_::tofsims_t::tofsims_t(measurements_::tofsims_t& tofsims_measurements) : 
                sims_t(tofsims_measurements) , settings_p(tofsims_measurements.settings),
                logger(__FILE__,"mgroups_::tofsims_t")
{
	measurements_p.push_back(tofsims_measurements);
}

mgroups_::tofsims_t::tofsims_t(std::vector< measurements_::tofsims_t* > tofsims_measurements) :
    sims_t(*(tofsims_measurements.front())), settings_p(tofsims_measurements.front()->settings),
    logger(__FILE__,"mgroups_::tofsims_t")
{
    measurements_p = (tools::vec::pointers_to_values(tofsims_measurements));
}

mgroups_::tofsims_t::tofsims_t(std::vector< measurements_::tofsims_t >& tofsims_measurements) : 
    sims_t(tofsims_measurements.front()), settings_p(tofsims_measurements.front().settings),
    logger(__FILE__,"mgroups_::tofsims_t"), measurements_p(tofsims_measurements)
{
//	tofsims_measurements.pop_back();
//	for (auto DM=tofsims_measurements.begin();DM!=tofsims_measurements.end();DM++)
//	{
//		tofsims_t MG(*DM);
//		if (*this != MG) continue;
//		measurements_p.push_back(*DM);
//		tofsims_measurements.erase(DM);
//		DM--;
//	}
}

mgroups_::tofsims_t::~tofsims_t()
{
	
}

bool mgroups_::tofsims_t::remove_measurement(const measurements_::tofsims_t* measurement)
{
    return mgroup_t::remove_measurement(measurements_p, measurement);
}
int mgroups_::tofsims_t::measurement_index(const measurements_::tofsims_t* measurement)
{
    return mgroup_t::measurement_index(measurements_p,measurement);
}

bool mgroups_::tofsims_t::operator==(const mgroups_::tofsims_t& obj) const
{
	if (sims_t::operator!=(obj)) return false;
// 	if (settings_p != obj.settings_p) return false;
	return true;
}

bool mgroups_::tofsims_t::operator!=(const mgroups_::tofsims_t& obj) const
{
	return !operator==(obj);
}

std::string mgroups_::tofsims_t::to_string(const std::string del)
{
	std::stringstream ss;
	ss << mgroup_t::to_string(del) << del;
// 	if (use_settings) ss << settings_p.to_string(del) << del;
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

std::vector<measurements_::sims_t*> mgroups_::tofsims_t::measurements()
{
	std::vector<measurements_::sims_t*> Ms(measurements_p.size());
	for (int i=0;i<measurements_p.size();i++)
		Ms.at(i)=&measurements_p.at(i);
	return Ms;
}

std::vector<measurements_::sims_t> mgroups_::tofsims_t::measurements_copy()
{
	std::vector<measurements_::sims_t> Ms;
	for (int i=0;i<measurements_p.size();i++)
		Ms.push_back(measurements_p.at(i));
	return Ms;
}



const msettings::sims_t* mgroups_::tofsims_t::settings() const
{
	return &settings_p;
}
