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


measurements_::profiler_t::profiler_t(files::profiler_t&  file, std::__cxx11::list< sample_t >& samples_list) : measurement_t(file.name,file.contents,samples_list)
{
	linescan = file.contents.linescan();
// 	cout << ">>>inserting profiler_t: " << filename.filename_with_path << endl;
}
