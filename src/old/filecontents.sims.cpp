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

#include "filecontents.hpp"

/**************/
/*** sims_t ***/
/**************/
filecontents::sims_t::sims_t(filenames::sims_t& name,const string& delimiter,const set<string>& identifiers) : file_t(name,delimiter,identifiers), name(name)
{
}


vector<cluster_t> filecontents::sims_t::clusters()
{
	logger::fatal("you should never be able to read this","virtual const vector<cluster_t> filecontents::sims_t::clusters()");
	return {};
}
