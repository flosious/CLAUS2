/*
	Copyright (C) 2021Florian Bärwolf
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


#include "element.hpp"

/*PSE singleton*/
pse_t PSE;

/************************************/
/*********     element_t       ******/
/************************************/

element_t::element_t(std::__cxx11::string symbol_s, double abs_amount, bool use_natural_abundance) :symbol(symbol_s),substance_amount({abs_amount})
{
	if (PSE.element(symbol_s)==nullptr)
	{
		if (symbol_s!="") logger::error("element_t::element_t symbol not in PSE",symbol_s);
		return;
	}
	for (auto& iso:PSE.element(symbol_s)->isotopes)
	{
		if (use_natural_abundance)
			isotopes.push_back({symbol_s,iso.nucleons,iso.abundance,iso.abundance*abs_amount});
		else
			isotopes.push_back({symbol_s,iso.nucleons,-1,-1});
	}
}

element_t::element_t(std::vector< isotope_t > isotopes_s, double abs_amount) : isotopes(isotopes_s),substance_amount({abs_amount}),symbol(isotopes_s.at(0).symbol)
{
}

const int element_t::protons()
{
	if (symbol=="")
	{
		logger::error("pse_t::element_t::protons() symbol is empty","abort");
		return -1;
	}
	return PSE.element(symbol)->protons;
}


const mass_t element_t::mass()
{
	if (symbol!="" && isotopes.size()==0)
	{
		return mass_t({PSE.element(symbol)->mass()});
	}
	mass_t mass_s({0});
	for (int i=0;i<isotopes.size();i++)
		mass_s += (isotopes.at(i).mass() * isotopes.at(i).abundance);
	return mass_s;
}

bool element_t::operator==(const element_t& obj) const
{
	if (symbol!=obj.symbol) return false;
	if (isotopes.size()!=obj.isotopes.size()) return false;
	bool iso_same=false;

	for (auto& iso:isotopes)
	{
		iso_same = false;
		for (auto& iso_obj : obj.isotopes)
		{
			if (iso == iso_obj)
			{
				iso_same = true;
				break;
			}
		}
		if (!iso_same) 
		{
			return false;
		}
	}
	
	return true;
}

bool element_t::operator!=(const element_t& obj) const
{
	return !(operator==(obj));
}


bool element_t::operator<(const element_t& obj) const
{
	return symbol<obj.symbol;
}

const std::__cxx11::string element_t::to_string() const
{
	stringstream out;
	const int size = isotopes.size();
	
	if (size==0) return "";
	for (int i=0;i<size;i++)
	{
		out << isotopes.at(i).to_string();
		if (i<size-1)
			out << ",";
	}
	return out.str();
}

// vector<isotope_t> & element_t::isotopes
// {
// 	return isotopes;
// }

bool element_t::is_set()
{
	if (isotopes.size()>0) return true;
	return false;
}
/*
substance_amount_t& element_t::substance_amount()
{
	return substance_amount;
}*/


// const substance_amount_t & element_t::substance_amount() const
// {
// 	return substance_amount;
// }
// 


// const vector<isotope_t> * element_t::isotopes const
// {
// 	if (isotopes.size()>0)
// 		return &isotopes;
// 	if (symbol!="")
// 	{
// 		vector<isotope_t> isos;
// 		for (auto& iso : PSE.element(symbol)->isotopes)
// 		{
// 			isos.push_back({symbol,iso.nucleons,iso.abundance});
// 		}
// 		*this = element_t(isos);
// 	}
// 	return nullptr;
// }


// set<element_t> element_t::elements(vector<isotope_t>& isotopes)
// {
// 	set<element_t> elements;
// 	set<string> symbols;
// 	for (auto& iso : isotopes)
// 		symbols.insert(iso.symbol);
// 	
// 	for (auto& symbol : symbols)
// 	{
// 		vector<isotope_t> isotopes_in_ele;
// 		for (auto& iso : isotopes)
// 		{
// 			if (iso.symbol==symbol)
// 				isotopes_in_ele.push_back(iso);
// 		}
// 		if (isotopes_in_ele.size()>0) elements.insert(isotopes_in_ele);
// 	}
// 	return elements;
// }


















