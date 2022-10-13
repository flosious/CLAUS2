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

bool mgroups_::mgroup_t::use_olcdb=false;
bool mgroups_::mgroup_t::use_group=true;
bool mgroups_::mgroup_t::use_settings=true;

// mgroups_::mgroup_t::mgroup_t(filenames::filename_t& fn, files_::file_t& f, list<sample_t>& samples_list) : olcdb(fn.olcdb()), group(fn.group())
// {
// }

mgroups_::mgroup_t::mgroup_t(measurements_::measurement_t& measurement) : 
                            olcdb(measurement.olcdb), group_id(measurement.group_id),
                            logger(__FILE__,"mgroups_::mgroup_t")
{
}


std::string mgroups_::mgroup_t::to_string(const std::string del)
{
	std::stringstream ss;
	if (use_olcdb) ss << "olcdb: " << olcdb << del;
    if (use_group) ss << "group: " << group_id;
	return ss.str();
}

bool mgroups_::mgroup_t::operator==(const mgroups_::mgroup_t& obj) const
{
	if (use_olcdb)
		if (olcdb!=obj.olcdb) return false;
	
	if (use_group)
        if (group_id!=obj.group_id) return false;
		
	return true;
}

bool mgroups_::mgroup_t::operator!=(const mgroups_::mgroup_t& obj) const
{
	return !operator==(obj);
}
