#include "element.hpp"

int element_t::atoms() const
{
	return atoms_p;
}

std::__cxx11::string element_t::symbol() const
{
	return symbol_p;
}
bool element_t::operator!=(const element_t& obj) const
{
	return !(operator==(obj));
}
bool element_t::operator==(const element_t& obj) const
{
	if (symbol()!=obj.symbol()) return false;
	if (nucleons()!=obj.nucleons()) return false;
	return true;
}
bool element_t::operator<(const element_t& obj) const
{
	if (symbol()<obj.symbol()) return true;
	if (nucleons()<obj.nucleons()) return true;
	return false;
}

std::__cxx11::string element_t::name() const
{
	stringstream out;
	out << nucleons() << symbol();
	return out.str();
}

const int element_t::nucleons() const
{
	
	return nucleons_p;
}


element_t::element_t(std::__cxx11::string element_str)
{
	smatch match;
	regex reg ("^([0-9]{0,3})([a-zA-Z]{1,2})([0-9]*)$"); 
	if (regex_search(element_str,match,reg)) 
	{
		if (match[1].length()>0) nucleons_p = tools::str::str_to_int(match[1]);
		symbol_p = match[2];
		if (match[3].length()>0) atoms_p = tools::str::str_to_int(match[3]);
		if (nucleons_p<1)
		{
			nucleons_p = element_t(PSE.get_isotope_with_highest_abundance(symbol())).nucleons();
		}
	}
}


element_t::element_t::element_t()
{

}

element_t::element_t::element_t(int nucleons_s, string symbol_s)
{
	nucleons_p = nucleons_s;
	symbol_p = symbol_s;
	if (nucleons_p<1)
	{
		nucleons_p = element_t(PSE.get_isotope_with_highest_abundance(symbol())).nucleons();
	}
}

const bool element_t::is_set()
{
	if (symbol()=="") return false;
	if (PSE.get_isotopes_from_element(symbol()).size()==0) return false;
	for (auto& nucleon : PSE.get_isotopes_from_element(symbol()))
	{
		if (tools::str::str_to_int(nucleon)==nucleons()) return true;
	}
	return false;
}


/************************/



/*



int isotope_t::nucleons() const
{
	return nucleons_p;
}
bool isotope_t::operator!=(const isotope_t& obj) const
{
	return !(operator==(obj));
}
bool isotope_t::operator==(const isotope_t& obj) const
{
	if (symbol()!=obj.symbol()) return false;
	if (atoms()!=obj.atoms()) return false;
	if (nucleons()!=obj.nucleons()) return false;
	return true;
}
bool isotope_t::operator<(const isotope_t& obj) const
{
	if (symbol()<obj.symbol()) return true;
	if (atoms()<obj.atoms()) return true;
	if (nucleons()<obj.nucleons()) return true;
	return false;
}
std::__cxx11::string isotope_t::to_string() const
{
	stringstream out;
	out << nucleons() << symbol() << atoms();
	return out.str();
}

isotope_t::isotope_t(std::__cxx11::string isotope_str)
{
	smatch match;
	regex reg ("^([0-9]{1,3})([a-zA-Z]{1,2})([0-9]*)$"); 
	if (regex_search(isotope_str,match,reg)) 
	{
		nucleons_p = tools::str::str_to_int(match[1]);
		symbol_p = match[2];
		if (match[3].length()>0) atoms_p = tools::str::str_to_int(match[3]);
	}
}


isotope_t::isotope_t()
{
}*/
