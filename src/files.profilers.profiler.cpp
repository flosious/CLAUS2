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

#include "files.hpp"

files_::profilers_t::profiler_t::contents_t::contents_t(string& filename_with_path,const string& delimiter,const set<string>& identifiers, const std::string& contents_string) :
    files_::file_t::contents_t(filename_with_path, delimiter, identifiers,{}, contents_string)
{
}

crater_t::linescan_t files_::profilers_t::profiler_t::contents_t::linescan()
{
    //logger::error("files_::profilers_t::profiler_t::contents_t::linescan()","virtual function called, this should never happen, tell florian");
	return {};
}
