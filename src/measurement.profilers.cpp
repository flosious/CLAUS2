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

#include "measurement.hpp"


measurements_::profilers_t::profiler_t::profiler_t(files_::file_t::name_t& filename, 
												   files_::file_t::contents_t& filecontents, 
												   const crater_t::linescan_t& linescan_s,
												   list<sample_t>& samples_list, 
												   string method,
												   database_t& sql_wrapper) :
												   measurement_t(filename,filecontents,samples_list,method,sql_wrapper),
												   linescan_p(linescan_s)
{
}

crater_t::linescan_t& measurements_::profilers_t::profiler_t::linescan()
{
	return linescan_p;
}

/************************************************/
/******* 	 profiler_t::dektak6m_t    **********/
/************************************************/

measurements_::profilers_t::dektak6m_t::dektak6m_t(files_::profilers_t::dektak6m_t& file, list<sample_t>& samples_list, database_t& sql_wrapper) : 
	profiler_t(file.name,file.contents,file.contents.linescan(),samples_list,"dektak6m",sql_wrapper)
{
}
