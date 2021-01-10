#include "unit.hpp"

const map<string,map<string,double>> unit_t::current_unit_to_new_unit=
{
	{"nm", {
		{"A", 1E1}
	}},
	{"A", {
		{"nm", 1E-1}
	}},
	{"nm", {
		{"um", 1E-3}
	}},
	{"um", {
		{"nm", 1E3}
	}},
	{"eV", {
		{"keV", 1E-3}
	}},
	{"keV", {
		{"eV", 1E3}
	}}
};

double unit_t::change_unit(unit_t new_unit)
{
	if (current_unit_to_new_unit.find(name())==current_unit_to_new_unit.end()) return 1;
	if (current_unit_to_new_unit.at(name()).find(new_unit.name())==current_unit_to_new_unit.at(name()).end()) return 1;
	name_p = new_unit.name();
	return current_unit_to_new_unit.at(name()).at(new_unit.name());
}

unit_t::unit_t(std::__cxx11::string name_s)
{
	name_p = name_s;
}

std::__cxx11::string unit_t::name() const
{
	return name_p;
}

bool unit_t::operator==(const unit_t& obj) const
{
	if (name()!=obj.name()) return false;
	return true;
}

bool unit_t::operator!=(const unit_t& obj) const
{
	return !(operator==(obj));
}


unit_t unit_t::operator*(const unit_t& obj) const
{
	return unit_t{name() + "*" + obj.name()};
}

unit_t unit_t::operator/(const unit_t& obj) const
{
	return unit_t{name() + " / (" + obj.name()+")"};
}
