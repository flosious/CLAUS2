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

#include "measurement.hpp"


measurements_::tofsims_t::tofsims_t(files_::tofsims_t& tofsims_file, 
									list<sample_t>& samples_list, 
									database_t& sql_wrapper, 
									vector<files_::jpg_t>* jpg_files, 
									vector<files_::profiler_t>* profiler_files) :
									settings(tofsims_file), 
									sims_t(tofsims_file.name,tofsims_file.contents,samples_list,"tofsims",sql_wrapper,jpg_files,profiler_files)
{
	crater.sputter_depth = tofsims_file.contents.sputter_depth();
	crater.sputter_time = tofsims_file.contents.sputter_time();
}

bool measurements_::tofsims_t::operator!=(const measurements_::tofsims_t& obj) const
{
	return !operator==(obj);
}

bool measurements_::tofsims_t::operator==(const measurements_::tofsims_t& obj) const
{
	if (measurement_t::operator!=(obj)) return false;
	if (settings!=obj.settings) return false;
	return true;
}
