#include "element.hpp"


/************************************/
/*********     isotope_t       ******/
/************************************/

isotope_t::isotope_t(const std::__cxx11::string symbol_s, const int nucleons_s, const double abundance_s) : symbol(symbol_s), 
																											nucleons_p(nucleons_s),
																											abundance_p({abundance_s})
{}

// const int isotope_t::amount_from_abundance() const
// {
// 	return pow(10,ceil(log10(abs(abundance_p))))*abundance_p;
// }


const abundance_t& isotope_t::abundance() const
{
	return abundance_p;
// 	if (abundance_p>0) return abundance_t({abundance_p});
// 	return abundance_t();
// 	if (symbol=="") 
// 	{
// 		logger::error("isotope_t::abundance() symbol not set", "this will likely cause calculation errors");
// 		return abundance_t();
// 	}
// 	if (nucleons()<=0)
// 	{
// 		logger::error("abundance_t::mass() nucleons not set or found", "this will likely cause calculation errors");
// 		return mass_t();
// 	}
// 	if (PSE.element(symbol)->isotope_from_nucleons(nucleons())==nullptr) 
// 	{
// 		logger::error("isotope_t::abundance() isotope not found in PSE", "this will likely cause calculation errors");
// 		return abundance_t();
// 	}
// 	return abundance_t({PSE.element(symbol)->isotope_from_nucleons(nucleons())->abundance});
}

const mass_t isotope_t::mass() const
{
	if (symbol=="")
	{
		logger::error("isotope_t::mass() symbol not set", "this will likely cause calculation errors");
		return mass_t();
	}
	if (nucleons()<=0)
	{
		logger::error("isotope_t::mass() nucleons not set or found", "this will likely cause calculation errors");
		return mass_t();
	}
	if (PSE.element(symbol)->isotope_from_nucleons(nucleons())==nullptr) 
	{
		logger::error("mass_t::abundance() isotope not found in PSE", "this will likely cause calculation errors");
		return mass_t();
	}
	return mass_t({PSE.element(symbol)->isotope_from_nucleons(nucleons())->mass});
}

const int isotope_t::nucleons() const
{
	if (nucleons_p>0) return nucleons_p;
	if (symbol=="") 
	{
		logger::error("isotope_t::mass() symbol not set", "this will likely cause calculation errors");
		return nucleons_p;
	}
	return PSE.element(symbol)->isotope_with_highest_abundance()->nucleons;
}



const bool isotope_t::operator==(const isotope_t& obj) const
{
	if (symbol!=obj.symbol) return false;
	if (nucleons()!=obj.nucleons()) return false;
	return true;
}

const bool isotope_t::operator<(const isotope_t& obj) const
{
	if (symbol < obj.symbol) return true;
	if (nucleons() < obj.nucleons()) return true;
	return false;
}

const std::__cxx11::string isotope_t::to_string() const
{
	stringstream out;
	out << nucleons() << symbol;
// 	if (abundance().is_set())
// 		out << abundance().data().at(0) << abundance().unit().to_string();
	if (substance_amount.is_set())
		out << substance_amount.data().at(0) << substance_amount.unit().to_string();
	return out.str();
}




/************************************/
/*********     element_t       ******/
/************************************/

element_t::element_t(std::__cxx11::string symbol_s, double abs_amount, bool use_natural_abundance) :symbol_p(symbol_s),substance_amount_p({abs_amount})
{
	for (auto& iso:PSE.element(symbol_s)->isotopes)
	{
		if (use_natural_abundance)
			isotopes_p.push_back({symbol_s,iso.nucleons,iso.abundance});
		else
			isotopes_p.push_back({symbol_s,iso.nucleons,-1});
	}
}

element_t::element_t(const std::vector< isotope_t >& isotopes_s, double abs_amount) : isotopes_p(isotopes_s),substance_amount_p({abs_amount})
{
}

const int element_t::protons() const
{
	if (symbol()=="")
	{
		logger::error("pse_t::element_t::protons() symbol is empty","abort");
		return -1;
	}
	return PSE.element(symbol())->protons;
}


const mass_t element_t::mass() const
{
	if (symbol()!="" && isotopes().size()==0)
	{
		return mass_t({PSE.element(symbol())->mass()});
	}
	mass_t mass_s({0});
	for (int i=0;i<isotopes().size();i++)
		mass_s += (isotopes().at(i).mass() * isotopes().at(i).abundance());
	return mass_s;
}

const std::__cxx11::string element_t::symbol() const
{
	if (symbol_p!="") return symbol_p;
	if (isotopes().size()==0)
	{
		logger::error("element_t::symbol isotopes().size()==0", "abort");
		return "";
	}
	return isotopes().at(0).symbol;
}

const bool element_t::operator==(const element_t& obj) const
{
	if (symbol()!=obj.symbol()) return false;
	if (isotopes().size()!=obj.isotopes().size()) return false;
	bool iso_same=false;
	for (auto& iso:isotopes())
	{
		iso_same = false;
		for (auto& iso_obj : obj.isotopes())
		{
			if (iso == iso_obj)
			{
				iso_same = true;
				break;
			}
			if (!iso_same) return false;
		}
	}
	return true;
}

const bool element_t::operator<(const element_t& obj) const
{
	return symbol()<obj.symbol();
}

const std::__cxx11::string element_t::to_string() const
{
	stringstream out;
	const int size = isotopes().size();
	for (int i=0;i<size;i++)
	{
		out << isotopes().at(i).to_string();
		if (i==size-1)
			out << ",";
	}
	return out.str();
}

const vector<isotope_t> & element_t::isotopes() const
{
	return isotopes_p;
}

// const substance_amount_t & element_t::substance_amount() const
// {
// 	return substance_amount_p;
// }
// 


// const vector<isotope_t> * element_t::isotopes() const
// {
// 	if (isotopes_p.size()>0)
// 		return &isotopes_p;
// 	if (symbol()!="")
// 	{
// 		vector<isotope_t> isos;
// 		for (auto& iso : PSE.element(symbol())->isotopes)
// 		{
// 			isos.push_back({symbol(),iso.nucleons,iso.abundance});
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




/***************************/
/*******     ion_t     ********/
/***************************/

ion_t::ion_t(vector<element_t>& elements_s,  electrical_charge_t electric_charge_s) : elements(elements_s), electric_charge(electric_charge_s)
{
}


const std::__cxx11::string ion_t::to_string() const
{
	if (!is_set()) return "";
	stringstream out;
	out << "(" ;
	const int size=elements.size(); 
	for (int i=0;i<size;i++)
	{
		out << elements.at(i).to_string();
		if (i==size-1)
			cout << ",";
	}
	cout << ")" << static_cast<int>(electric_charge.data().at(0));
	return out.str();
}

const bool ion_t::is_set() const
{
	if (elements.size()==0) return false;
	if (!electric_charge.is_set()) return false;
	return true;
}




