#include "element.hpp"


/************************************/
/*********     isotope_t       ******/
/************************************/

isotope_t::isotope_t(const std::__cxx11::string symbol_s, const int nucleons_s, const double abundance_s) : symbol(symbol_s), 
																											nucleons_p(nucleons_s),
																											abundance_p(abundance_s)
{}

// const int isotope_t::amount_from_abundance() const
// {
// 	return pow(10,ceil(log10(abs(abundance_p))))*abundance_p;
// }


const abundance_t isotope_t::abundance() const
{
	if (abundance_p>0) return abundance_t();
	if (symbol=="") 
	{
		logger::error("isotope_t::abundance() symbol not set", "this will likely cause calculation errors");
		return abundance_t();
	}
	if (nucleons()<=0)
	{
		logger::error("abundance_t::mass() nucleons not set or found", "this will likely cause calculation errors");
		return mass_t();
	}
	if (PSE.element(symbol)->isotope_from_nucleons(nucleons())==nullptr) 
	{
		logger::error("isotope_t::abundance() isotope not found in PSE", "this will likely cause calculation errors");
		return abundance_t();
	}
	return abundance_t({PSE.element(symbol)->isotope_from_nucleons(nucleons())->abundance});
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
	if (abundance().is_set())
		out << " " << abundance().to_string();
	return out.str();
}




/************************************/
/*********     element_t       ******/
/************************************/



element_t::element_t(const vector<isotope_t>& isotopes_s) : isotopes(isotopes_s)
{
}

element_t::element_t(const isotope_t& isotope_s) : isotopes({isotope_s})
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
	if (symbol()!="" && isotopes.size()==0)
	{
		return mass_t({PSE.element(symbol())->mass()});
	}
	mass_t mass_s({0});
	for (int i=0;i<isotopes.size();i++)
		mass_s += (isotopes.at(i).mass() * isotopes.at(i).abundance());
	return mass_s;
}

const std::__cxx11::string element_t::symbol() const
{
	if (symbol_p!="") return symbol_p;
	if (isotopes.size()==0)
	{
		logger::error("element_t::symbol isotopes.size()==0", "abort");
		return "";
	}
	return isotopes.at(0).symbol;
}

const bool element_t::operator==(const element_t& obj) const
{
	if (symbol()!=obj.symbol()) return false;
	return true;
}

const bool element_t::operator<(const element_t& obj) const
{
	return symbol()<obj.symbol();
}

const std::__cxx11::string element_t::to_string() const
{
	stringstream out;
	for (int i=0;i<isotopes.size();i++)
	{
		out << isotopes.at(i).to_string() << "\t";
	}
	return out.str();
}

element_t::element_t(const string symbol_s) : symbol_p(symbol_s)
{
}

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




