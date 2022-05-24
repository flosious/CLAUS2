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


#include "sample.hpp"


/****************************/
/***       matrix_t       ***/
/****************************/


sample_t::matrix_t::matrix_t()
{
}

sample_t::matrix_t::matrix_t(vector<isotope_t> isotopes)
{
// 	cout << endl << "IN-matrix::isotopes.size()=" << isotopes.size() << endl;
// 	for (auto& i : isotopes)
// 		cout << "iso=" << i.substance_amount.to_string_detailed() << endl;
	///filter elements from isos and save in matrix
	while (isotopes.size()>0)
	{
		vector<unsigned int> vec_positions_to_erase;
		vector<isotope_t> isos_in_ele;
		for (int i=0;i<isotopes.size();i++)
		{
			isotope_t& I = isotopes.at(i);
			if (I.symbol=="") continue;
			if ((isos_in_ele.size()==0) || isos_in_ele.front().symbol==I.symbol)
			{
				isos_in_ele.push_back(I);
				vec_positions_to_erase.push_back(i);
			}
		}
		if (isos_in_ele.size()==0)
			break;
		elements.push_back({isos_in_ele});
		isotopes = tools::vec::erase(isotopes,vec_positions_to_erase);
	}
// 	cout << endl << "OUT-matrix::isotopes.size()=" << isotopes.size() << endl;
// 	for (auto& e : elements)
// 		for (auto& i : e.isotopes)
// 			cout << "iso=" << i.substance_amount.to_string() << endl;
}

sample_t::matrix_t::matrix_t(const string matrix_elements_s)
{
	*this = matrix_t{tools::str::get_strings_between_delimiter(matrix_elements_s," ")};
}

sample_t::matrix_t::matrix_t(const vector<string> elements_or_isotopes_s)
{
	/* 
	 * mole/abundance/concentration/atoms input values are asumed as absolutes, which are iternally normalized to 100at%: 
	 * sum of all elements must be 100 at% (will be enforced)
	 * input is something like: "Si Ge30" -> [28,29,30]Si70at% + [74,73,72,70]Ge30at% === [28,29,30]Si7mol + [74,73,72,70]Ge3mol
	 * single isotopes are also possible, for purified matrices: "28Si30 Ge60" -> [28]Si33at% [74,73,72,70]Ge66at%
	 * uncalculateable concentrations like "Si Ge" -> will lead to a warning, matrix isotopes are saved without abundance/substance amount
	 * indistinguishable isotopes like "29Si30 Si50 Ge10 Sn10" --> will lead to an error and aborting
	 * not recognized isotopes --> will lead to an error an aborting
	 */
	vector<isotope_t> isotopes;
	/*parsing*/
	smatch match;
	int nucleons;
	double amount;
	string symbol;
	double total_amount{0};
	int unknown_amounts{0};
	for (auto& ele_or_iso:elements_or_isotopes_s)
	{
		if (ele_or_iso=="") continue;
		if (regex_search(ele_or_iso,match,regex("^([0-9]{0,3})([a-zA-Z]{1,3})([0-9\\.]*)"))) 
		{
			/*records*/
			if (match[1]!="")
				nucleons = tools::str::str_to_int(match[1]);
			else 
				nucleons=-1;
			
			if (match[2]!="") 
				symbol = match[2];
			else
				symbol="";
			
			if (match[3]!="") 
				amount = tools::str::str_to_double(match[3]);
			else 
			{
				if (elements_or_isotopes_s.size()==1)
					amount = 1;
				else
					amount = -1;
			}
// 			cout << "amount="<< amount << endl;
			/*******/
			
			/*catch error*/
			if (PSE.element(symbol)==nullptr)
			{
				*this = matrix_t(); //make me empty
				logger::error("sample_t::matrix_t::matrix_t() symbol is not listed in the PSE, skipping use space as seperator for multiple symbols", ele_or_iso);
				return;
			}
			if (symbol=="")
			{
				*this = matrix_t(); //make me empty
				logger::error("sample_t::matrix_t::matrix_t() symbol not parseable, skipping", ele_or_iso);
				return;
			}
			if (nucleons>0 && PSE.element(symbol)->isotope_from_nucleons(nucleons)==nullptr)
			{
				*this = matrix_t(); //make me empty
				logger::error("sample_t::matrix_t::matrix_t() isotope is not listed in the PSE, skipping", ele_or_iso);
				return;
			}
			/*************/
			
			vector<isotope_t> new_isotopes_to_add;
			if (nucleons==-1)
			{
				//add all isotopes of the symbols element with natural abundance
				for (auto& pse_iso: PSE.element(symbol)->isotopes ) 
				{
					isotope_t iso{symbol,pse_iso.nucleons,pse_iso.abundance};
					iso.substance_amount = quantity::substance_amount_t({amount*pse_iso.abundance});
					new_isotopes_to_add.push_back(iso);
				}
				if (amount==-1)	unknown_amounts++;
			}
			else
			{
				isotope_t iso{symbol,nucleons};
				if (amount==-1)  // e.g. "29Si" = 29Si->-1mol
				{
					iso.substance_amount = quantity::substance_amount_t({-1});
					unknown_amounts++;
				}
				else // e.g. "29Si5" = 29Si->5mol
					iso.substance_amount = quantity::substance_amount_t({amount});
				new_isotopes_to_add.push_back(iso);
			}
			
			
			for (auto& new_iso : new_isotopes_to_add)
			{
				/*check for double entries before insertion!*/
				if (std::find( isotopes.begin(),isotopes.end(),new_iso)!=isotopes.end())
				{
					*this = matrix_t(); //make me empty
					logger::error("sample_t::matrix_t::matrix_t(): indistinguishable isotopes, skipping", tools::vec::combine_vec_to_string(elements_or_isotopes_s," "));
					return;
				}
				if (new_iso.substance_amount.is_set() && new_iso.substance_amount.data().at(0)>0) total_amount+=new_iso.substance_amount.data().at(0);
				/*add to matrix isotopes*/
				isotopes.push_back(new_iso);
			}
		}
		else
		{
			*this = matrix_t(); //make me empty
			logger::error("sample_t::matrix_t::matrix_t(): not parseable, skipping", tools::vec::combine_vec_to_string(elements_or_isotopes_s," "));
			return;
		}
	}
	
	if (unknown_amounts>1)
	{
// 		*this = matrix_t(); //make me empty
// 		logger::error("sample_t::matrix_t::matrix_t(): more than 1 unknown amount, skipping", tools::vec::combine_vec_to_string(elements_or_isotopes_s," "));
		logger::warning(3,"sample_t::matrix_t::matrix_t(): more than 1 unknown amount", tools::vec::combine_vec_to_string(elements_or_isotopes_s," "),"setting subsstance amount to unknown");
		for (auto& iso : isotopes)
			iso.substance_amount.clear();
		*this = matrix_t(isotopes);
		return;
	}
	
	int max_ = pow(10,ceil(log10(total_amount)));
	if (max_==10) max_ = 100; // Si Ge5 == Si95 Ge5
	
	/*calculate not given substance_amount*/
	for (auto& iso : isotopes)
	{
		if (iso.substance_amount.is_set() && iso.substance_amount.data().at(0)<0) 
			iso.substance_amount =  iso.substance_amount * (total_amount - max_ ) ;
	}
	
	/*calculate total substance_amount for each element/symbol*/
	map<string,double> symbol_to_total_amount;
	for (auto& iso : isotopes)
	{
		if (iso.abundance.data().at(0)<0)
		{
			if (!iso.substance_amount.is_set())
			{
				*this = matrix_t(); //make me empty
				logger::error("sample_t::matrix_t::matrix_t(): !iso.substance_amount.is_set(), skipping", tools::vec::combine_vec_to_string(elements_or_isotopes_s," "));
				return;
			}
			if (symbol_to_total_amount.find(iso.symbol)==symbol_to_total_amount.end())
				symbol_to_total_amount.insert(pair<string,double> (iso.symbol,iso.substance_amount.data().at(0)));
			else
				symbol_to_total_amount.find(iso.symbol)->second+=iso.substance_amount.data().at(0);
		}
	}
	
	/*calculate unset(-1) abundance*/
	for (auto& iso:isotopes)
	{
		if (iso.abundance.data().at(0)<0)
		{
			iso.abundance = quantity::abundance_t({iso.substance_amount.data().at(0)/symbol_to_total_amount.find(iso.symbol)->second});
		}
	}
	
	///calc relative substance_amount (at%)
	substance_amount_to_relative(isotopes);

	*this = matrix_t(isotopes);
	
	
	
	/*sum of all isos subtance_amounts may be high*/
// 	double sum=0, sum1=0;
// 	for (auto& iso : isotopes)
// 	{
// 		sum += iso.substance_amount.data.at(0);
// 		sum1 += iso.abundance.data.at(0);
// 		cout << iso.to_string() << iso.abundance.to_string() << "\tsum=" << sum<< "\tsum1=" << sum1 <<  endl;
// 	}
}

void sample_t::matrix_t::substance_amount_to_relative(vector<isotope_t>& isotopes)
{
	quantity::quantity_t sum;
	for (auto& iso : isotopes)
		sum << iso.substance_amount;
	sum = sum.sum();
	for (auto& iso : isotopes)
	{
// 		cout << "iso.substance_amount=" << iso.substance_amount.to_string_detailed() << endl;
// 		cout << "sum.substance_amount=" << sum.to_string_detailed() << endl;
// 		quantity::quantity_t Q = (iso.substance_amount/sum);
// 		cout << "Q.substance_amount=" << Q.to_string_detailed() << endl;
		auto Q = (iso.substance_amount/sum);
// 		cout << "Q.substance_amount=" << Q.to_string_detailed() << endl;
		iso.substance_amount = Q.change_unit("at%");
// 		cout << "AFTER - iso.substance_amount=" << iso.substance_amount.to_string_detailed() << endl;
// 		iso.substance_amount = quantity::substance_amount_t( quantity::quantity_t(iso.substance_amount/sum*100,units::derived::atom_percent));
// 		iso.substance_amount = quantity::quantity_t(iso.substance_amount/sum*100,units::derived::atom_percent) ;
	}
// 	cout << endl << "substance_amount_to_relative: sum:" << sum.to_string_detailed() << endl;
// 	for (auto& iso : isotopes)
// 		cout << iso.substance_amount.to_string_detailed() << endl;
}

bool sample_t::matrix_t::is_in(const isotope_t& iso) const
{
	const auto isos = isotopes();
	if (find(isos.begin(),isos.end(),iso)!=isos.end())
		return true;
	return false;
}

bool sample_t::matrix_t::is_in(const element_t& ele) const
{
	if (find(elements.begin(),elements.end(),ele)!=elements.end())
		return true;
	return false;
}

isotope_t* sample_t::matrix_t::isotope(isotope_t iso)
{
	for (auto& E : elements)
	{
		if (E.symbol != iso.symbol) continue;
		for (auto& I : E.isotopes)
			if (I == iso)
				return &I;
	}
	return nullptr;
}

element_t* sample_t::matrix_t::element(const element_t& ele)
{
	for (auto& E : elements)
	{
		if (E == ele)
			return &E;
	}
	return nullptr;
}

const vector<isotope_t> sample_t::matrix_t::isotopes() const
{
	vector<isotope_t> isotopes;
// 	cout << endl << "elements.size()="<< elements.size() << endl;
	for (auto& E : elements)
	{
// 		cout <<endl<< E.symbol << endl;
// 		for (auto& i : E.isotopes)
// 			cout << "iso=" << i.substance_amount.to_string_detailed() << endl;
		isotopes.insert(isotopes.end(), E.isotopes.begin(),E.isotopes.end());
	}
	return isotopes;
}

const bool sample_t::matrix_t::is_set() const
{
	if (isotopes().size()==0)
		return false;
	for (auto& iso : isotopes())
		if (!iso.abundance.is_set())
			return false;
	return true;
}

const quantity::concentration_t sample_t::matrix_t::concentration(element_t ele) const
{
	if (!is_in(ele))
		return quantity::concentration_t({0},units::derived::atom_percent,quantity::dimensions::SI::relative);
	quantity::substance_amount_t sum;
	for (const auto& e : elements)
	{
		if (!e.substance_amount.is_set())
			continue;
		if (e == ele)
			ele.substance_amount = e.substance_amount;
		sum += e.substance_amount;
	}
	quantity::concentration_t T(ele.substance_amount/sum);
	return T.change_unit(units::derived::atom_percent);
}

const quantity::concentration_t sample_t::matrix_t::concentration(isotope_t iso) const
{
	if (!is_in(iso))
		return quantity::concentration_t({0},units::derived::atom_percent,quantity::dimensions::SI::relative);
	quantity::substance_amount_t sum;
	for (const auto& i : isotopes())
	{
		if (i == iso) 
			iso.substance_amount = i.substance_amount;
		if (!sum.is_set())
			sum = i.substance_amount;
		else
			sum += i.substance_amount;
	}
	quantity::concentration_t result(iso.substance_amount / sum);
	return result.change_unit(units::derived::atom_percent);
}

bool sample_t::matrix_t::operator==(const matrix_t& obj) const
{
	logger::debug(logger_verbosity_offset+6,"sample_t::matrix_t::operator==()","entering");
	if (isotopes().size()!=obj.isotopes().size()) 
	{
		logger::debug(logger_verbosity_offset+5,"sample_t::matrix_t::operator==()","isotopes.size()!=obj.isotopes.size()","returning FALSE");
		return false;
	}
	bool same_iso;
	for (auto& iso: isotopes())
	{
		same_iso = false;
		for (auto& iso_obj: obj.isotopes())
		{
			// iso == iso_obj will not check abundance and substance_amount!
			if (iso == iso_obj && iso.substance_amount == iso_obj.substance_amount && iso.abundance == iso_obj.abundance) 
			{
				same_iso = true;
				break;
			}
		}
		if (!same_iso) 
		{
			logger::debug(logger_verbosity_offset+5,"sample_t::matrix_t::operator==()","!same_iso","returning FALSE");
			return false;
		}
	}
	logger::debug(logger_verbosity_offset+6,"sample_t::matrix_t::operator==()","returning true");
	return true;
}

bool sample_t::matrix_t::operator!=(const matrix_t& obj) const
{
	return !operator==(obj); 
}

bool sample_t::matrix_t::operator<(const matrix_t& obj) const
{
	logger::debug(logger_verbosity_offset+6,"sample_t::matrix_t::operator<()","entering");
	if (isotopes().size()<obj.isotopes().size()) 
	{
		logger::debug(logger_verbosity_offset+5,"sample_t::matrix_t::operator<()","returning true");
		return true;
	}
	if (isotopes().size()>obj.isotopes().size())
	{
		logger::debug(logger_verbosity_offset+5,"sample_t::matrix_t::operator<()","returning FALSE");
		return false;
	}
	
	if (operator==(obj))
		return false;
// 	set<isotope_t> iso(isotopes.begin(),isotopes.end());
// 	set<isotope_t> obj_iso(obj.isotopes.begin(),obj.isotopes.end());
	return true;
}

const string sample_t::matrix_t::to_string() const
{
	stringstream out;
	if (!is_set()) return "";
	for (int i=0;i<isotopes().size();i++)
	{
		out << isotopes().at(i).nucleons << isotopes().at(i).symbol;
		if (isotopes().at(i).substance_amount.is_set()) 
			out << isotopes().at(i).substance_amount.data().at(0);
// 		out << isotopes.at(i).to_string();
		if (i<isotopes().size()-1) out << " ";
	}
	return out.str();
}


