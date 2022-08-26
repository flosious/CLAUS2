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

/*********************************/
/****       tofsims_t        *****/
/*********************************/

files_::tofsims_t::tofsims_t(string& filename, const std::string& contents_string) : name(filename),contents(filename, contents_string), logger(global_logger,__FILE__,"files_::tofsims_t")
{
}
files_::tofsims_t::tofsims_t(files_::tofsims_t::name_t& name_s, files_::tofsims_t::contents_t& contents_s) : 
            name(name_s), contents(contents_s), logger(global_logger,__FILE__,"files_::tofsims_t")
{
}

bool files_::tofsims_t::operator==(const tofsims_t& obj) const
{
	if (name.filename() != obj.name.filename()) return false;
	return true;
}

bool files_::tofsims_t::operator!=(const tofsims_t& obj) const
{
	return !operator==(obj);
}

bool files_::tofsims_t::operator<(const tofsims_t& obj) const
{
	if (name.filename() < obj.name.filename()) return true;
	if (name.filename() > obj.name.filename()) return false;
	
	return false;
}

