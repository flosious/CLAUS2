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

files_::dsims_t::dsims_t(std::string& filename_s, const std::string& contents_string) : name(filename_s),contents(name, contents_string), logger(__FILE__,"files_::dsims_t")
{
}

files_::dsims_t::dsims_t(files_::dsims_t::name_t& name_s, files_::dsims_t::contents_t& contents_s) : 
            name(name_s), contents(contents_s),
            logger(__FILE__,"files_::dsims_t")
{
}

bool files_::dsims_t::operator<(const files_::dsims_t& obj) const
{
	if (name.filename() < obj.name.filename()) return true;
	if (name.filename() > obj.name.filename()) return false;
	
	return false;
}
