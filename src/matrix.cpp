#include "matrix.hpp"


/****************************/
/***  matrix_t  ***/
/****************************/


matrix_t::matrix_t()
{
}

matrix_t::matrix_t(const std::__cxx11::string matrix_elements_s)
{
	*this = matrix_t{tools::str::get_strings_between_delimiter(matrix_elements_s," ")};
}


matrix_t::matrix_t(const vector<std::__cxx11::string> elements_or_isotopes_s)
{
	/* 
	 * mole/abundance/concentration/atoms input values are asumed as absolutes, which are iternally normalized to 100at%: 
	 * sum of all elements must be 100 at%
	 * input is something like: "Si Ge30" -> [28,29,30]Si70at% + [74,73,72,70]Ge30at% === [28,29,30]Si7mol + [74,73,72,70]Ge3mol
	 * single isotopes are also possible, for purified matrices: "28Si30 Ge60" -> [28]Si33at% [74,73,72,70]Ge66at%
	 * uncalculateable concentrations like "Si Ge" -> will lead to an error and aborting
	 * indistinguishable isotopes like "29Si30 Si50 Ge10 Sn10" --> will lead to an error and aborting
	 */
	
	/*parsing*/
	smatch match;
	int nucleons;
	double amount;
	string symbol;
	double total_amount{0};
// 	double total_neg_abundance{0};
	int unknown_amounts{0};
	for (auto& ele_or_iso:elements_or_isotopes_s)
	{
		if (regex_search(ele_or_iso,match,regex("^([0-9]{0,3})([a-zA-Z]{1,3})([0-9]*)"))) 
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
				amount = tools::str::str_to_int(match[3]);
			else 
				amount = -1;
			
			/*******/
			
			/*catch error*/
			if (symbol=="")
			{
				*this = matrix_t(); //make me empty
				logger::error("matrix_t::matrix_t() symbol not parseable, skipping", ele_or_iso);
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
					cout << "abundance=" << pse_iso.abundance << " ,amount*abundance=" << amount*pse_iso.abundance << endl;
					iso.substance_amount = substance_amount_t({amount*pse_iso.abundance});
					new_isotopes_to_add.push_back(iso);
				}
				if (amount==-1)	unknown_amounts++;
			}
			else
			{
				isotope_t iso{symbol,nucleons};
				if (amount==-1)  // e.g. "29Si" = 29Si->-1mol
				{
					iso.substance_amount = substance_amount_t({-1});
					unknown_amounts++;
				}
				else // e.g. "29Si5" = 29Si->5mol
					iso.substance_amount = substance_amount_t({amount});
				new_isotopes_to_add.push_back(iso);
			}
			
			
			for (auto& new_iso : new_isotopes_to_add)
			{
				/*check for double entries before insertion!*/
				if (std::find( isotopes.begin(),isotopes.end(),new_iso)!=isotopes.end())
				{
					*this = matrix_t(); //make me empty
					logger::error("matrix_t::matrix_t(): indistinguishable isotopes, skipping", tools::vec::combine_vec_to_string(elements_or_isotopes_s," "));
					return;
				}
// 				if (new_iso.abundance().is_set() && new_iso.abundance().data().at(0)<0) total_neg_abundance-= new_iso.abundance().data().at(0);
				if (new_iso.substance_amount.is_set() && new_iso.substance_amount.data().at(0)>0) total_amount+=new_iso.substance_amount.data().at(0);
				/*add to matrix isotopes*/
				isotopes.push_back(new_iso);
			}
		}
	}
	
	if (unknown_amounts>1)
	{
		*this = matrix_t(); //make me empty
		logger::error("matrix_t::matrix_t(): more than 1 unknown amount, skipping", tools::vec::combine_vec_to_string(elements_or_isotopes_s," "));
		return;
	}
	
	const int max_ = pow(10,ceil(log10(total_amount)));
	cout << "max_=" << max_ << endl;
	cout << "total_amount=" << total_amount << endl;
	
	for (auto& iso : isotopes)
	{
		if (iso.substance_amount.is_set() && iso.substance_amount.data().at(0)<0) 
			iso.substance_amount =  iso.substance_amount * (total_amount - max_ ) ;
	}
}


const bool matrix_t::is_set() const
{
	if (isotopes.size()==0) return false;
	return true;
}

bool matrix_t::operator==(matrix_t& obj)
{
	if (isotopes.size()!=obj.isotopes.size()) return false;
	bool same_iso;
	for (auto& iso: isotopes)
	{
		same_iso = false;
		for (auto& iso_obj: obj.isotopes)
		{
			if (iso == iso_obj)
			{
				same_iso = true;
				break;
			}
		}
		if (!same_iso) return false;
	}
	return true;
}

bool matrix_t::operator!=(matrix_t& obj)
{
	return !operator==(obj);
}

const std::__cxx11::string matrix_t::to_string()
{
	stringstream out;
	if (!is_set()) return "";
	for (int i=0;i<isotopes.size();i++)
	{
		out << isotopes.at(i).to_string();
		if (i<isotopes.size()-1) out << ",";
	}
	return out.str();
}


/*
sample_t::matrix_t::matrix_t(const std::__cxx11::string matrix_elements_s)
{
	vector<string> isos_or_eles =  tools::str::get_strings_between_delimiter(matrix_elements_s, " ");
	smatch match;
	int nucleons;
	double concentration;
	string symbol;
	for (auto& iso_p : isos_or_eles)
	{
		if (regex_search(iso_p,match,regex("^([0-9]{0,3})([a-zA-Z]{1,3})([0-9]*)"))) 
		{
			if (match[1]!="") nucleons = tools::str::str_to_int(match[1]);
			else 
			{
				vector<isotope_t> isos;
				for (auto& iso : PSE.element(symbol)->isotopes)
					isos.push_back({symbol,iso.nucleons,iso.abundance});
				//TODO
			}
			if (match[2]!="") symbol = match[2];
			else
			{
				logger::error("cluster_t::cluster_t() symbol not parseable, skipping", iso_p);
				continue;
			}
			if (match[3]!="") concentration = tools::str::str_to_int(match[3]);
			else concentration = 1;
		}
		
	}
// 	isotopes_concentration_p.insert(pair<isotope_t,concentration_t> ());
	///TODO
}

sample_t::matrix_t::matrix_t(const set<element_t> elements_s) : elements_p(elements_s)
{
}


bool sample_t::matrix_t::operator!=(matrix_t& obj) 
{
	return !(operator==(obj));
}
const string sample_t::matrix_t::to_string()
{
	stringstream out;
	out << "elements:";
	for (auto& element:elements())
		out << "\t" << element.to_string() ;
	return out.str();
}*/
