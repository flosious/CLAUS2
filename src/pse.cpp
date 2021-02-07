/*
	Copyright (C) 2020 Florian Bärwolf
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
#include "pse.hpp"



/************************************/
/******   pse_t::isotope_t   ********/
/************************************/

pse_t::pse_isotope_t::pse_isotope_t(const double abundance_s, 
									const int nucleons_s, 
									const double mass_s, 
									const std::__cxx11::string symbol_alternative_s) : 
									abundance(abundance_s), 
									nucleons(nucleons_s), 
									mass(mass_s), 
									symbol_alternative(symbol_alternative_s)
{}


const pse_t::pse_isotope_t * pse_t::isotope(std::__cxx11::string symbol, int nucleons)
{
	if (isotopes(symbol)==nullptr) return nullptr;
	for (int i=0;i<isotopes(symbol)->size();i++)
		if (isotopes(symbol)->at(i).nucleons==nucleons)
			return &isotopes()->at(i);
	return nullptr;
}



const vector<pse_t::pse_isotope_t> * pse_t::isotopes(std::__cxx11::string symbol)
{
	if (element(symbol)==nullptr) return nullptr;
	return &element(symbol)->isotopes;
}

const bool pse_t::pse_isotope_t::operator==(const pse_isotope_t& obj) const
{
	if (nucleons!=obj.nucleons) return false;
	return true;
}

const bool pse_t::pse_isotope_t::operator<(const pse_isotope_t& obj) const
{
	return nucleons<obj.nucleons;
}



/************************************/
/*****  pse_t::element_t   **********/
/************************************/

pse_t::pse_element_t::pse_element_t(string symbol_s, int protons_s, vector<pse_t::pse_isotope_t>& isotopes_s) : isotopes(isotopes_s), symbol(symbol_s), protons(protons_s)
{
}

pse_t::pse_element_t::pse_element_t(string symbol_s, int protons_s, pse_t::pse_isotope_t& isotopes_s) : isotopes({isotopes_s}), symbol(symbol_s), protons(protons_s)
{
}


const pse_t::pse_element_t * pse_t::element(std::__cxx11::string symbol)
{
	if (elements().size()==0) return nullptr;
	for (int i=0;i<elements().size();i++)
	{
		if (elements().at(i).symbol==symbol)
		{
			return &elements().at(i);
		}
	}
	return nullptr;
}

const vector<pse_t::pse_element_t>& pse_t::elements()
{
	if (elements_p.size()==0)
		load_file();
	return elements_p;
}


// pse_t::element_t::element_t(const vector<isotope_t>& isotopes_s) : isotopes(isotopes_s) {}
// 
const double pse_t::pse_element_t::mass() const
{
	double mass_s=0;
	for (auto& iso: isotopes)
	{
		mass_s += iso.mass * iso.abundance;
	}
	return mass_s;
}

const pse_t::pse_isotope_t* pse_t::pse_element_t::isotope_from_nucleons(int nucleons) const
{
	for (auto& iso:isotopes)
	{
		if (iso.nucleons == nucleons)
			return &iso;
	}
	return nullptr;
}


const pse_t::pse_isotope_t* pse_t::pse_element_t::isotope_with_highest_abundance() const
{
	const pse_t::pse_isotope_t* iso_w_max_abundance=nullptr;
	if (isotopes.size()==0)
	{
		logger::error("pse_t::element_t::isotope_with_highest_abundance() isotopes.size()==0","abort");
		return nullptr;
	}
	
// 	int max_entry = 0;
// 	double max_abundance = isotopes.at(max_entry).abundance;
	
	for (int i=1;i<isotopes.size();i++)
	{
		if (iso_w_max_abundance == nullptr || isotopes.at(i).abundance > iso_w_max_abundance->abundance)
		{
// 			max_entry = i;
// 			max_abundance = isotopes.at(i).abundance;
			iso_w_max_abundance = &isotopes.at(i);
		}	
	}
// 	return &isotopes.at(max_entry);
	return iso_w_max_abundance;
}


const bool pse_t::pse_element_t::operator==(const pse_t::pse_element_t& obj) const
{
	if (symbol != obj.symbol) return false;
	return true;
}

const bool pse_t::pse_element_t::operator<(const pse_t::pse_element_t& obj) const
{
	if (symbol < obj.symbol) return true;
	return false;
}




/************************************/
/*********    pse_t    	   **********/
/************************************/


// const std::vector< std::__cxx11::string > pse_t::get_all_isotopes_with_highest_abundance()
// {
// 	vector<string> isotopes_with_highest_abundance;
// 	for (auto const& element:get_all_elements())
// 	{
// 		isotopes_with_highest_abundance.push_back(get_isotope_with_highest_abundance(element));
// 	}
// 	return isotopes_with_highest_abundance;
// }


// vector<pse_t::element_t>& pse_t::elements()
// {
// 	vector<pse_t::element_t> elements_p;
// 	for ()
// }



void pse_t::to_screen()
{
	for (auto& element : elements())
	{
		cout << element.protons << ":";
		for (auto& isotope : element.isotopes)
		{
// 			const pse_isotope_t iso = it->isotopes()->at(i);
			cout << "\t" << isotope.nucleons <<  element.symbol;  
			if (isotope.symbol_alternative!="") cout << "," << isotope.symbol_alternative;
			if (isotope.abundance>0) cout << isotope.abundance;
		}
		cout << endl;
	}
}


bool pse_t::load_file() 
{
	vector<vector<string>> contents = tools::file::load(filename,delimiter);
	if (contents.size()==0)
	{
		logger::fatal("PSE file could not be loaded", "dying...");
		return false;
	}
// 	tools::mat::remove_empty_cols_from_matrix(&contents);
// 	tools::mat::remove_empty_lines_from_matrix(&contents);
// 	tools::str::remove_spaces(&contents);
// 	tools::file::write_mat_to_file(filename,&contents,false,delimiter,"\n");
	
// 	isotopes_p.reserve(contents.size());
	string symbol, nuclide, mass, abundance, symbol_alternative, protons;
	map<string,vector<pse_element_t>> symbols_to_eles; //a list where each element contains just 1 isotop
	for (int i=0;i<contents.size();i++) 
	{
		if (contents[i].size()>4)
		{
			symbol_alternative="";
			if (contents[i][0]!="") 
			{
				protons=contents[i][0];
				symbol=contents[i][1];
				tools::str::remove_spaces(&symbol);
				tools::str::remove_spaces(&protons);
			}
			if (contents[i][0]=="" && contents[i][1]!="") 	symbol_alternative=contents[i][1];
			nuclide = contents[i][2];
			mass = contents[i][3];
			abundance = contents[i][4];
			abundance = tools::str::get_strings_between_delimiter(abundance,"(")[0]; // just take the mean part and ignore uncertainty
			if (abundance=="") abundance = "0";
			if (mass == "")
			{
				logger::error("pse_t::load_file() could not parse line: ", i+1);
				continue;
			}
			 pse_isotope_t iso(	 tools::str::str_to_double(abundance),
								 tools::str::str_to_int(nuclide),
								 tools::str::str_to_double(mass),
								 symbol_alternative);
			 pse_element_t ele(symbol,tools::str::str_to_int(protons),iso);
			if (symbols_to_eles.find(symbol)==symbols_to_eles.end())
				symbols_to_eles.insert(pair<string,vector<pse_element_t>> (symbol,{ele}));
			else 
				symbols_to_eles.at(symbol).push_back(ele);
			
// 			if (symbol_alternative!="")
// 				symbols_to_eles.insert(pair<string,set<pse_isotope_t>> (symbol_alternative,{iso}));
		}
		else
			logger::error("pse_t::load_file() could not read line (to less entries): ", i+1);
	}
	set<pse_element_t> elements_s;
	for (auto& symbol_to_eles : symbols_to_eles)
	{
		set<pse_isotope_t> isotopes_s;
		for (auto& isotopes_in_ele : symbol_to_eles.second)
		{
			isotopes_s.insert(isotopes_in_ele.isotopes.begin(),isotopes_in_ele.isotopes.end());
		}
		vector<pse_isotope_t> iso_vec(isotopes_s.begin(),isotopes_s.end());
		pse_element_t element(symbol_to_eles.second.begin()->symbol, symbol_to_eles.second.begin()->protons,iso_vec);
		elements_s.insert(element);
	}
	elements_p = {elements_s.begin(),elements_s.end()};
// 	vector<pse_element_t> elements_vec (elements_s.begin(),elements_s.end());
// 	*this = pse_t(elements_vec);
	
	logger::info("PSE loaded");
	return true;
}
