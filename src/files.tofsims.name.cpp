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
files_::tofsims_t::name_t::name_t(std::string& filename_with_path_s) :
                                        files_::sims_t::name_t(filename_with_path_s,"_",{".TXT"},{}),
                                        logger(global_logger,__FILE__,"files_::tofsims_t::name_t")
{
	parse_analysis_energy_element();
}

bool files_::tofsims_t::name_t::parse_analysis_energy_element()
{
	std::smatch match;
	std::regex reg;
// 	std::cout << "IN" << std::endl;
// 	std::cout << "not_parseable_filename_parts_p: " << not_parseable_filename_parts_p.size() << std::endl;
// 	std::cout << to_string() << std::endl;
	for (std::vector<std::string>::iterator FNp = not_parseable_filename_parts_p.begin();FNp!=not_parseable_filename_parts_p.end();FNp++)
	{
		reg = ("^([0-9]{1,5})(V|eV|kV|keV)(Mn|Mn\\+|Bi|Bi\\+|Bi\\+\\+)$"); 
		if (regex_search(*FNp,match,reg)) 
		{	
			std::string element_p = (match[3]);
			if (match[2]=="kV" || match[2]=="keV")
				analysis_energy_p = quantity::energy_t{{tools::str::str_to_double(match[1])*1000},{"eV"}};
			else
				analysis_energy_p = quantity::energy_t{{tools::str::str_to_double(match[1])},{"eV"}};
			if (element_p=="Bi++")
				analysis_ion_p = ion_t(element_t{tools::str::remove_chars_from_string(element_p,"+")},quantity::electrical_charge_t({2}));
			else
				analysis_ion_p = ion_t(element_t{tools::str::remove_chars_from_string(element_p,"+")},quantity::electrical_charge_t({1}));
			not_parseable_filename_parts_p.erase(FNp);
			FNp--;
			return true;
		}
		else
		{
// 			std::cout << "not recognized: " << *FNp << std::endl;
		}
	}
	return false;
}

const ion_t& files_::tofsims_t::name_t::analysis_ion() const
{
// 	if (!analysis_ion_p.is_set())
// 		parse_analysis_energy_element();
	return analysis_ion_p;
}

const quantity::energy_t& files_::tofsims_t::name_t::analysis_energy() const
{
// 	if (!analysis_energy_p.is_set())
// 		parse_analysis_energy_element();
	return analysis_energy_p;
}
