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


#include "msettings.hpp"

/**********************/
/****  tofsims_t  *****/
/**********************/

msettings::tofsims_t::tofsims_t(files_::tofsims_t::name_t& filename, files_::tofsims_t::contents_t& filecontents) : 
	sims_t(filename), analysis_ion(filename.analysis_ion()), analysis_energy(filename.analysis_energy())
{
}

msettings::tofsims_t::tofsims_t(files_::tofsims_t& file) : tofsims_t(file.name,file.contents)
{
}

bool msettings::tofsims_t::operator==(const tofsims_t& obj) const
{
	if (sims_t::operator!=(obj)) 
		return false;
	if (analysis_ion != obj.analysis_ion) 
		return false;
	if (analysis_energy != obj.analysis_energy) 
		return false;
	
	return true;
}

bool msettings::tofsims_t::operator!=(const tofsims_t& obj) const
{
	return !operator==(obj);
}
