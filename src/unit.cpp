#include "unit.hpp"

const map<string,map<string,double>> unit_t::current_unit_to_new_unit=
{
	{"m", {
		{"A", 1E-10},
		{"nm", 1E-9},
		{"um", 1E-6}
	}},
	{"A", {
		{"nA", 1E-9},
		{"uA", 1E-6}
	}},
	{"J", {
		{"keV", 1E3/1.602177E-19},
		{"eV", 1/1.602177E-19},
		{"kJ", 1E3}
	}},
	{"amu", {
		{"kg", 1.660539066E-27},
		{"g", 1.660539066E-27*1E3}
	}}
};

double unit_t::change_unit(unit_t new_unit)
{
// 	if (current_unit_to_new_unit.find(name())==current_unit_to_new_unit.end()) return 1;
// 	if (current_unit_to_new_unit.at(name()).find(new_unit.name())==current_unit_to_new_unit.at(name()).end()) return 1;
	double factor = -1;
	double divisor = -1;
	
// 	///current unit is in the 1st map and new_unit in the 2nd map
// 	if (current_unit_to_new_unit.find(name())!=current_unit_to_new_unit.end())
// 	{
// 		map<string,double> unit_to_double=current_unit_to_new_unit.at(name());
// 		if (unit_to_double.find(new_unit.name()) != unit_to_double.end())
// 		{
// 			name_p = new_unit.name();
// 			return unit_to_double.at(new_unit.name());
// 		}
// 	}
// 		
// 	///current unit is in the 2nd map and new_unit in the 1st map
// 	if (current_unit_to_new_unit.find(new_unit.name())!=current_unit_to_new_unit.end())
// 	{
// 		map<string,double> unit_to_double=current_unit_to_new_unit.at(new_unit.name());
// 		if (unit_to_double.find(name()) != unit_to_double.end())
// 		{
// 			name_p = new_unit.name();
// 			return 1/unit_to_double.at(new_unit.name());
// 		}
// 	}
		
	///current unit and new_unit are both in the 2nd map
	for (auto& s : current_unit_to_new_unit)
	{
		if (s.first == name()) factor = 1;
		if (s.first == new_unit.name()) divisor = 1;
		for (auto& t : s.second)
		{
			if (factor>0 && divisor>0)
			{
				name_p = new_unit.name();
				return factor/divisor;
			}
			if (t.first == name()) factor = t.second;
			if (t.first == new_unit.name()) divisor = t.second;
		}
	}
	return 1;
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
