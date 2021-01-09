/*
	Copyright (C) 2020 Florian Bärwolf
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

#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include "hash_functions.hpp"
#include <list>
#include <string>
#include <vector>
#include "tools.hpp"
#include "file.hpp"
#include "sample.hpp"
#include "measurement_group.hpp"
#include "measurement.hpp"
#include <unordered_set>
#include "quantity.hpp"

using namespace std;


class processor 
{
private:
	// directory+filenames
// 	set<file_t> files;
// 	std::set<dsims_measurement_group_t> dsims_measurement_groups;
// 	std::set<dsims_measurement_t> dsims_measurements;
	std::set<sample_t> samples;
public:
// 	std::list<dsims_filename_t> dsims_filenames;
	
	processor(vector<string> args_p);
};


#endif // PROCESSOR_HPP
