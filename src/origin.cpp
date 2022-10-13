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


#include "origin.hpp"

/***************************/
/***  origin_t::column_t ***/
/***************************/

origin_t::column_t::column_t(const quantity::quantity_t& quantity, std::string prefix, std::string suffix) :
	column_t(quantity.data(),
					quantity.name(),
					quantity.unit().to_string(),
					prefix + longname + suffix)
{
}

origin_t::column_t::column_t(const std::vector<double>& data_s, std::string longname, std::string unit, std::string comment)
    :
	data(tools::vec::double_to_string(data_s)),
	longname(longname),
	unit(unit),
    comment(comment),
    logger(__FILE__,"origin_t::column_t")
{
}

std::vector<std::string> origin_t::column_t::vec()
{
	std::vector<std::string> vec;
// 	if (!populated) return vector;
	apply_origin_conform_replacements(longname);
	apply_origin_conform_replacements(unit);
	apply_origin_conform_replacements(comment);
	vec={longname,unit,comment};
	vec.insert(vec.end(),data.begin(),data.end());
	return vec;
}


void origin_t::column_t::apply_origin_conform_replacements(std::string& in_here)
{
	in_here = regex_replace(in_here,std::regex("\\^([0-9]+)"),"\\+($1)");
	in_here = regex_replace(in_here,std::regex("\\_([0-9]+)"),"\\-($1)");
}


/***************************/
/*****   origin_t   ********/
/***************************/
/*
origin_t::origin_t(origin_t::column_t cols, std::string path, std::string filename)
{
}

origin_t::origin_t(std::vector<cluster_t>& clusters, std::string path, std::string filename)
{
	std::vector<origin_t::column_t> cols;
	for (auto& C : clusters)
	{
		std::stringstream comment,longname;
		comment << "^" << C.to_string(" ^") << " ";
		longname << "^" << C.to_string(" ^") << " ";
		
		if (sample->simple_name!="")
			comment << sample->simple_name;
		else
			comment << sample->lot << sample->lot_split << "_w" << sample->wafer;
		
		if (C.sputter_time.is_set()) 
			cols.push_back({C.sputter_time.data,longname.str() + " sputter_time",C.sputter_time.unit().to_string(),comment.str()});
		if (C.intensity().is_set()) 
			cols.push_back({C.intensity().data,longname.str() + " intensity",C.intensity().unit().to_string(),comment.str()});
		if (C.sputter_depth().is_set())
			cols.push_back({C.sputter_depth().data,longname.str() + " sputter_depth",C.sputter_depth().unit().to_string(),comment.str()});
		if (C.concentration().is_set()) 
			cols.push_back({C.concentration().data,longname.str() + " concentration",C.concentration().unit().to_string(),comment.str()});
	}
}

*/
