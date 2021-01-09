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
	if (atoms()!=obj.atoms()) return false;
	return true;
}
bool element_t::operator<(const element_t& obj) const
{
	if (symbol()<obj.symbol()) return true;
	if (atoms()<obj.atoms()) return true;
	return false;
}

std::__cxx11::string element_t::to_string(std::__cxx11::string prefix) const
{
	stringstream out;
	out << symbol() << atoms();
	return out.str();
}






/************************/







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
std::__cxx11::string isotope_t::to_string(std::__cxx11::string prefix) const
{
	stringstream out;
	out << nucleons() << symbol() << atoms();
	return out.str();
}
