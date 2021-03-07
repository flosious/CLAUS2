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

/**************/
/*** sims_t ***/
/**************/
files::sims_t::contents_t::contents_t(string& filename_with_path,const string& delimiter,const set<string>& identifiers) : files::file_t::contents_t(filename_with_path,delimiter,identifiers)
{
}


vector<cluster_t> files::sims_t::contents_t::clusters()
{
	logger::fatal("you should never be able to read this","virtual const vector<cluster_t> filecontents::sims_t::clusters()");
	return {};
}


void files::sims_t::contents_t::column_t::to_screen()
{
	cout << "unit="<<unit<<"\tcluster_name="<<cluster_name<<"\tdimension="<<dimension<<"\tdata.size()="<<data.size()<<endl;
}
