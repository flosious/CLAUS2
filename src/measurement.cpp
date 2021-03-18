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

measurements_::measurement_t::measurement_t(files_::file_t::name_t& filename, files_::file_t::contents_t& filecontents, list<sample_t>& samples_list, string method) : 
												repetition(filename.repetition()), olcdb(filename.olcdb()),group(filename.group()), method(method), filename_with_path(filename.filename_with_path)
{
	sample_t s(filename,filecontents);
	sample = tools::find_in_V(s,samples_list); 
	if (sample==nullptr)
	{
		samples_list.push_back(s);
		sample = &samples_list.back();
	}

	if (!sample->matrix().is_set())
	{
// 		cout << "MATRIX = " << s.matrix().to_string() << endl;
		sample->matrix() = s.matrix(); 
	}
}

measurements_::measurement_t::measurement_t(files_::file_t::name_t& filename, list<sample_t>& samples_list, string method) : 
			repetition(filename.repetition()), olcdb(filename.olcdb()), group(filename.group()), method(method), filename_with_path(filename.filename_with_path)
{
	if (sample==nullptr)
	{
		sample_t s(filename);
		sample = tools::find_in_V(s,samples_list); 
		if (sample==nullptr)
		{
			samples_list.push_back(s);
			sample = &samples_list.back();
		}
	}
}


bool measurements_::measurement_t::operator!=(measurements_::measurement_t& obj)
{
	return !(operator==(obj));
}

bool measurements_::measurement_t::operator==(measurements_::measurement_t& obj)
{
	if (use_group)
		if (group!=obj.group) return false;
	if (use_olcdb)
		if (olcdb!=obj.olcdb) return false;
	if (use_repetition)
		if (repetition!=obj.repetition) return false;
	if (use_sample)
		if (*sample!=*obj.sample) return false; // pointer comparison
	if (sample==nullptr)
		logger::error("sample is null","this should never happen");
	
	return true;	
}

std::__cxx11::string measurements_::measurement_t::to_string(const std::__cxx11::string del) const
{
	stringstream ss;
	/*if (use_sample)*/ ss << "sample: " << sample->to_string(del) << del;
	/*if (use_repetition)*/ ss << "olcdb: " << olcdb << del;
	/*if (use_repetition)*/ ss << "group: " << group << del;
	/*if (use_repetition)*/ ss << "repetition: " << repetition ;
	return ss.str();
}

bool measurements_::measurement_t::operator<(measurements_::measurement_t& obj)
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
		if (*sample < *obj.sample) return true;
		if (*sample > *obj.sample) return false;
	}
	
	
	return false;
}

bool measurements_::measurement_t::operator>(measurements_::measurement_t& obj)
{
	if (!operator<(obj) && operator!=(obj)) return true;
	return false;
}


