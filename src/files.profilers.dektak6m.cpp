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

#include "files.hpp"

/*****************************************/
/*********    dektak6m_t  	**************/
/*****************************************/

files_::profilers_t::dektak6m_t::dektak6m_t(string& filename, const std::string& contents_string) :
    name(filename,"_",{".TXT",".txt",".profile"},{}), contents(filename, contents_string),
    logger(global_logger,__FILE__,"files_::profilers_t::dektak6m_t")
{
}


/*************************************************/
/*********  dektak6m_t::contents_t  **************/
/*************************************************/

files_::profilers_t::dektak6m_t::contents_t::contents_t(string& filename_with_path, const std::string& contents_string) :
    profiler_t::contents_t(filename_with_path,",",{"Scan Type","Stylus Type"},contents_string),
    logger(global_logger,__FILE__,"files_::profilers_t::dektak6m_t::contents_t")
{
}

crater_t::linescan_t files_::profilers_t::dektak6m_t::contents_t::linescan()
{
// 	if (!parse_data_and_header_tensors(",")) return false;
	if (raw_data_tensor().size()==0) 
		return {};
	if (raw_data_tensor()[0].size()==0) 
		return {};
	if (raw_data_tensor()[0][0].size()==0) 
		return {};

	vector<vector<double>> data_cols_lines = tools::mat::transpose_matrix(tools::mat::str_matrix_to_double_matrix(raw_data_tensor()[0]));
	
	
	
// 	linescan.xy.name="linescan_xy";
// 	linescan.xy.dimension="length";
// 	linescan.xy.unit="um";
// 	linescan.xy.data=data_cols_lines[0];
	quantity::quantity_t XY("length",data_cols_lines[0],units::prefixes::micro*units::SI::meter,quantity::dimensions::SI::length);
	quantity::depth_t D(data_cols_lines[1]);
// 	linescan.z.name="linescan_z";
// 	linescan.z.dimension="length";
// 	linescan.z.unit="nm";
// 	linescan.z.data=data_cols_lines[1];
	
	return crater_t::linescan_t(XY,D);
}
