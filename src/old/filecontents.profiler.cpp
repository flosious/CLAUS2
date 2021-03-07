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


/**************************/
/***      profiler_t    ***/
/**************************/

filecontents::profiler_t::profiler_t(filenames::sims_t& name) : file_t(name,",",{"Scan Type,","Stylus Type"})
{
}

linescan_t filecontents::profiler_t::linescan()
{
	if (raw_data_tensor().size()==0) return linescan_t();
	vector<vector<double>> data_cols_lines = tools::mat::transpose_matrix(tools::mat::str_matrix_to_double_matrix(raw_data_tensor()[0]));
	if (data_cols_lines.size()!=2) return {};
	quantity_t xy("linescan_xy",data_cols_lines[0],{"um"});
	quantity_t z("linescan_z",data_cols_lines[1],{"nm"});

	return linescan_t(xy,z);
}

// filecontents::filecontents::dsims_profiler_t::dsims_profiler_t(filenames::sims_t& name) : profiler_t(name)
// {
// }
// 
// filecontents::filecontents::tofsims_profiler_t::tofsims_profiler_t(filenames::sims_t& name) : profiler_t(name)
// {
// }
