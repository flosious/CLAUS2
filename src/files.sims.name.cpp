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


/*********************/
/****   sims_t   *****/
/*********************/

files_::sims_t::name_t::name_t(string& filename_with_path_s, 
						  const string delimiter_s, 
						  const std::set< string > OR_identifiers_s, 
						  const std::set< string > AND_identifiers_s) : files_::file_t::name_t(filename_with_path_s,delimiter_s,OR_identifiers_s,AND_identifiers_s)
{
}

total_sputter_depth_t files_::sims_t::name_t::total_sputter_depths()
{
	filename_without_crater_depths_s = filename();
	if (total_sputter_depths_p.size()==0)
	{
// 		for (auto& filename_part : not_parseable_filename_parts())
		for (vector<string>::iterator FNp=not_parseable_filename_parts_p.begin();FNp!=not_parseable_filename_parts_p.end();FNp++)
		{
			smatch match;
			regex reg ("^([0-9]{2,})(nm|A)$"); 
			if (regex_search(*FNp,match,reg)) 
			{
				string value = match[1];
				string unit = match[2];
				total_sputter_depths_p.push_back({{tools::str::str_to_double(value)},unit});
				not_parseable_filename_parts_p.erase(FNp);
				FNp--;
				tools::str::remove_substring_from_mainstring(&filename_without_crater_depths_s,"_"+value+unit);
				tools::str::remove_substring_from_mainstring(&filename_without_crater_depths_s,value+unit+"_");
			}
		}
	}
	total_sputter_depth_t tspd;
	for (total_sputter_depth_t& t : total_sputter_depths_p)
		tspd << t;
	return tspd;
}

const string files_::sims_t::name_t::filename_without_crater_depths()
{
	if (filename_without_crater_depths_s=="") 
		total_sputter_depths();
	return filename_without_crater_depths_s;
// 	string filename_wo_crater_depths  = tools::file::extract_filename(filename_with_path);
// 	stringstream remove;
// 	for (double total_sputter_depth:total_sputter_depths().data)
// 	{
// 		remove.str("");
// 		remove << total_sputter_depth << total_sputter_depths().unit().to_string();
// 		tools::str::remove_substring_from_mainstring(&filename_wo_crater_depths ,remove.str() + delimiter);
// 		tools::str::remove_substring_from_mainstring(&filename_wo_crater_depths ,delimiter+ remove.str());
// 	}
// 	return filename_wo_crater_depths;
}

bool files_::sims_t::name_t::parse_sputter_energy_element_polarity()
{
	smatch match;
	regex reg;
// 	for (auto& filename_part : not_parseable_filename_parts())
	for (vector<string>::iterator FNp = not_parseable_filename_parts_p.begin();FNp!=not_parseable_filename_parts_p.end();FNp++)
	{
		reg = ("^([0-9]{1,2})(kV|keV)(O|Cs)(\\+|-)$"); 
		if (regex_search(*FNp,match,reg)) 
		{	
			sputter_energy_p = energy_t{{tools::str::str_to_double(match[1])*1000},{"eV"}};
			sputter_element_p = (match[3]);
			secondary_polarity_p = match[4];
			not_parseable_filename_parts_p.erase(FNp);
			FNp--;
			return true;
		}
	
		reg = ("^([0-9]{2,5})(V|eV)(O|Cs)(\\+|-)$"); 
		if (regex_search(*FNp,match,reg)) 
		{
			sputter_energy_p = energy_t{{tools::str::str_to_double(match[1])},{"eV"}};
			sputter_element_p = (match[3]);
			secondary_polarity_p = match[4];
			not_parseable_filename_parts_p.erase(FNp);
			FNp--;
			return true;
		}
	}
	return false;
}

const energy_t files_::sims_t::name_t::sputter_energy()
{
	if (!sputter_energy_p.is_set()) 
		parse_sputter_energy_element_polarity();
	return sputter_energy_p;
}

element_t files_::sims_t::name_t::sputter_element()
{
	if (sputter_element_p=="") 
	{
		parse_sputter_energy_element_polarity();
	}
	return sputter_element_p;
}

const string files_::sims_t::name_t::secondary_polarity()
{
	if (secondary_polarity_p=="") 
		parse_sputter_energy_element_polarity();
	return secondary_polarity_p;
}

const vector<string>& files_::sims_t::name_t::not_parseable_filename_parts()
{
	files_::file_t::name_t::not_parseable_filename_parts();
	sputter_energy(); // parse_sputter_energy_element_polarity();
	total_sputter_depths();
	return not_parseable_filename_parts_p;
}

const string files_::sims_t::name_t::simple_name()
{
	string simple_name_p;
	if ((lot()=="") && not_parseable_filename_parts().size()>0) 
		simple_name_p=tools::vec::combine_vec_to_string(not_parseable_filename_parts(), delimiter);
	else if ((wafer()<0)) 
		simple_name_p=lot() +lot_split() + tools::vec::combine_vec_to_string(not_parseable_filename_parts(), delimiter);
	else 
		simple_name_p="";
	return simple_name_p;
}

bool files_::sims_t::name_t::operator==(name_t& obj)
{
	if (filename_without_crater_depths()==obj.filename_without_crater_depths()) return true;
	if (files_::file_t::name_t::operator!=(obj)) return false;
	if (secondary_polarity()!=obj.secondary_polarity()) return false;
	if (sputter_energy()!=obj.sputter_energy()) return false;
	if (sputter_element()!=obj.sputter_element()) return false;
	return true;
}

bool files_::sims_t::name_t::operator!=(name_t& obj)
{
	return !operator==(obj);
}

string files_::sims_t::name_t::to_string()
{
	stringstream out;
	out << file_t::name_t::to_string();
	if (sputter_energy().is_set())
		out << "_" << sputter_energy().data.at(0) << sputter_energy().unit().to_string();
	if (sputter_element().is_set())
		out << sputter_element().to_string() << secondary_polarity();
	return out.str();
}


