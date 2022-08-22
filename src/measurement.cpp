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

#include "measurement.hpp"

bool measurements_::measurement_t::use_repetition=true;
bool measurements_::measurement_t::use_sample=true;
bool measurements_::measurement_t::use_olcdb=true;
bool measurements_::measurement_t::use_group=true;

measurements_::measurement_t::measurement_t(files_::file_t::name_t& filename, files_::file_t::contents_t& filecontents, string method, database_t& database) :
                                                repetition(filename.repetition()), olcdb(filename.olcdb()),group(filename.group()), method(method), filename_with_path(filename.filename_with_path), database(&database),
                                                sample(filename,database), logger(global_logger,__FILE__,"measurements_::measurement_t")
{
//    sample_t s(filename,filecontents,database);
//	sample = tools::find_in_V(s,samples_list);
//	if (sample==nullptr)
//	{
//		samples_list.push_back(s);
//		sample = &samples_list.back();
//	}

//	if (!sample.matrix().is_set())
//	{
//		*sample = s;
//	}
}

measurements_::measurement_t::measurement_t(files_::file_t::name_t& filename, string method, database_t& database) :
            repetition(filename.repetition()), olcdb(filename.olcdb()), group(filename.group()), method(method), filename_with_path(filename.filename_with_path), database(&database),
            sample(filename,database), logger(global_logger,__FILE__,"measurements_::measurement_t")
{
//	if (sample==nullptr)
//	{
//        sample_t s(filename,database);
//		sample = tools::find_in_V(s,samples_list);
//		if (sample==nullptr)
//		{
//			samples_list.push_back(s);
//			sample = &samples_list.back();
//		}
//	}
}

long long unsigned int measurements_::measurement_t::memory_address() const
{
	stringstream out;
	out << this;
	const auto address = tools::math::hexTOint(out.str());
	return address;
}


bool measurements_::measurement_t::operator!=(const measurements_::measurement_t& obj) const
{
	return !(operator==(obj));
}

bool measurements_::measurement_t::operator==(const measurements_::measurement_t& obj) const
{
//	if (use_group)
//		if (group!=obj.group) return false;
//	if (use_olcdb)
//		if (olcdb!=obj.olcdb) return false;
//	if (use_repetition)
//		if (repetition!=obj.repetition) return false;
//	if (use_sample)
//		if (*sample!=*obj.sample) return false; // pointer comparison
//	if (sample==nullptr)
    if (filename_with_path != obj.filename_with_path)
        return false;
// 		logger::error("sample is null","this should never happen");
        //logger::error("measurements_::measurement_t::operator==","sample==nullptr",to_string(),"returning TRUE");
	
	return true;	
}

string measurements_::measurement_t::to_string(const string del) const
{
	stringstream ss;
    ss << "olcdb: " << olcdb;
    ss << del << sample.to_name(del);
//    ss << del << sample;
	if (group!="") ss << del << "group: " << group;
	if (repetition!="") ss << del << "repetition: " << repetition ;
	return ss.str();
}

bool measurements_::measurement_t::operator<(const measurements_::measurement_t& obj) const
{
	if (use_olcdb)
	{
		if (olcdb < obj.olcdb) return true;
		if (olcdb > obj.olcdb) return false;
	}
	if (use_group)
	{
		if (group < obj.group) return true;
		if (group > obj.group) return false;
	}
	if (use_repetition)
	{
		if (repetition < obj.repetition) return true;
		if (repetition > obj.repetition) return false;
	}
	if (use_sample)
	{
        if (sample < obj.sample) return true;
        if (sample > obj.sample) return false;
	}
	
	
	return false;
}

bool measurements_::measurement_t::operator>(const measurements_::measurement_t& obj) const
{
	if (!operator<(obj) && operator!=(obj)) return true;
	return false;
}


