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


#include "element.hpp"

/*PSE singleton*/
pse_t PSE(global_logger);

/************************************/
/*********     element_t       ******/
/************************************/

element_t::element_t() : logger(global_logger,__FILE__,"element_t")
{
}

element_t::element_t(std::string symbol_s, double abs_amount, bool use_natural_abundance) :symbol(symbol_s),substance_amount({abs_amount}), logger(global_logger,__FILE__,"element_t")
{
	if (PSE.element(symbol_s)==nullptr)
	{
        if (symbol_s!="")
        {
            //logger::error("element_t::element_t symbol not in PSE",symbol_s);
        }
		return;
	}
	for (auto& iso:PSE.element(symbol_s)->isotopes)
	{
		if (use_natural_abundance)
			isotopes.push_back({symbol_s,iso.nucleons,iso.abundance,iso.abundance*abs_amount});
		else
			isotopes.push_back({symbol_s,iso.nucleons,-1,-1});
	}
	set_elemental_substance_amount();
}

element_t::element_t(isotope_t isotope_s) : element_t(std::vector<isotope_t>{isotope_s})
{
}

element_t::element_t(std::vector< isotope_t > isotopes_s) : 
													isotopes(isotopes_s),
                                                    symbol(isotopes_s.at(0).symbol),
                                                    logger(global_logger,__FILE__,"element_t")
{
// 	if (isotopes.size()==0) return;
// 	substance_amount=isotopes.front().substance_amount;
// 	auto start = (isotopes.begin());
// 	start++;
// 	for (auto I = start;I!=isotopes.end();I++)
// 	{
// 		if (!I->substance_amount.is_set())
// 		{
// 			substance_amount.clear();
// 			break;
// 		}
// 		substance_amount += I->substance_amount;
// 	}
	set_elemental_substance_amount();
}

void element_t::set_elemental_substance_amount()
{
	for (auto& iso : isotopes)
	{
		if (!iso.substance_amount.is_set())
			continue;
		if (!iso.abundance.is_set())
			continue;
		substance_amount = substance_amount / iso.abundance;
		return;
	}
	for (auto& iso : isotopes)
	{
		if (!iso.substance_amount.is_set())
		{
			substance_amount.clear();
			return;
		}
		substance_amount += iso.substance_amount;
	}
}


const int element_t::protons()
{
	if (symbol=="")
	{
        //logger::error("pse_t::element_t::protons() symbol is empty","abort");
		return -1;
	}
	return PSE.element(symbol)->protons;
}

// quantity::substance_amount_t& element_t::substance_amount() const
// {
// 	if (substance_amount.is_set())
// 		return substance_amount;
// // 	quantity::substance_amount_t sum;
// 	for (auto& iso : isotopes)
// 	{
// 		if (!iso.substance_amount.is_set())
// 			continue;
// 		if (!iso.abundance.is_set())
// 			continue;
// 		substance_amount = substance_amount / iso.abundance;
// 		return substance_amount;
// 	}
// 	for (auto& iso : isotopes)
// 	{
// 		if (!iso.substance_amount.is_set())
// 		{
// 			substance_amount.clear();
// 			return substance_amount;
// 		}
// 		substance_amount += iso.substance_amount;
// 	}
// 	return substance_amount();
// }

const quantity::mass_t element_t::mass()
{
	if (symbol!="" && isotopes.size()==0)
	{
		return quantity::mass_t({PSE.element(symbol)->mass()});
	}
	quantity::mass_t mass_s({0});
	for (int i=0;i<isotopes.size();i++)
		mass_s += (isotopes.at(i).mass() * isotopes.at(i).abundance);
	return mass_s;
}

bool element_t::operator==(const element_t& obj) const
{
	if (symbol!=obj.symbol) return false;
// 	if (isotopes.size()!=obj.isotopes.size()) return false;
// 	bool iso_same=false;
// 
// 	for (auto& iso:isotopes)
// 	{
// 		iso_same = false;
// 		for (auto& iso_obj : obj.isotopes)
// 		{
// 			if (iso == iso_obj)
// 			{
// 				iso_same = true;
// 				break;
// 			}
// 		}
// 		if (!iso_same) 
// 		{
// 			return false;
// 		}
// 	}
	
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

const std::string element_t::to_string() const
{
	std::stringstream out;
	const int size = isotopes.size();
	if (substance_amount.is_set())
		out << substance_amount.to_string() << "\t";
	if (size==0) return "";
	for (int i=0;i<size;i++)
	{
		out << isotopes.at(i).to_string();
		if (i<size-1)
			out << ",";
	}
	return out.str();
}


bool element_t::is_set()
{
	if (isotopes.size()>0) return true;
	return false;
}









