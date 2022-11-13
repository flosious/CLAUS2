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
                            class_logger(__FILE__,"mgroups_::mgroup_t"), tool_name_p(measurement.tool_name)
{
    ///default
    export_location_p = tools::file::check_directory_string(tools::file::extract_directory_from_filename(measurement.filename_with_path));
    export_location_p += std::to_string(olcdb) + PATH_DELIMITER;
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

std::vector<measurements_::measurement_t*> mgroups_::mgroup_t::basic_measurements()
{
    return {};
}

std::string mgroups_::mgroup_t::to_string_short() const
{
    std::stringstream out;
    out  << olcdb << "_g"<<group_id << "_" << tool_name();
}

std::string mgroups_::mgroup_t::name() const
{
    if (name_p=="")
        return to_string_short();
    return name_p;
}

void mgroups_::mgroup_t::set_name(const std::string& new_name)
{
    name_p = new_name;
}

std::string mgroups_::mgroup_t::tool_name() const
{
    return tool_name_p;
}

void mgroups_::mgroup_t::set_export_location(std::string new_export_location)
{
    log_f;
//    if (new_export_location=="")
//    {
//        if (measurements().size()>0)
//            new_export_location = tools::file::check_directory_string(tools::file::extract_directory_from_filename(measurements().front()->filename_with_path));
//    }
//    logger.info("this").enter();
    new_export_location = tools::file::check_directory_string(new_export_location);
//    new_export_location += std::to_string(olcdb) + PATH_DELIMITER;
    export_location_p = new_export_location;
    std::vector<measurements_::measurement_t*> basic_measurements__ = basic_measurements();
    for (measurements_::measurement_t* M : basic_measurements__)
    {
        M->export_location = new_export_location + name();
        M->export_location = tools::file::check_directory_string(M->export_location);
//        logger.info("M->export_location").value(M->export_location,10,M->to_string_short());
    }
}

std::string mgroups_::mgroup_t::export_location() const
{
    return export_location_p;
}




























