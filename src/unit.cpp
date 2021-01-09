#include "unit.hpp"




const map<string,int> unit_t::prefix_t::prefix_to_exponent = 
{
	{"Y",24},
	{"Z",21},
	{"E",18},
	{"P",15},
	{"T",12},
	{"G",9},
	{"M",6},
	{"k",3},
	{"h",2},
	{"da",1},
	{"",0},
	{"d",-1},
	{"c",-2},
	{"m",-3},
	{"u",-6},
	{"n",-9},
	{"p",-12},
	{"f",-15},
	{"a",-18},
	{"z",-21},
	{"y",-24}
};

unit_t::prefix_t::prefix_t(std::__cxx11::string name_s)
{
	name_p = name_s;
}

std::__cxx11::string unit_t::prefix_t::name() const
{
	return name_p;
}

bool unit_t::prefix_t::operator==(const unit_t::prefix_t& obj) const
{
	if (name() != obj.name()) return false;
	return true;
}

bool unit_t::prefix_t::operator!=(const unit_t::prefix_t& obj) const
{
	return !(operator==(obj));
}

int unit_t::prefix_t::minimize_prefix_exponent(int prefix_factor) 
{
	/*minimize prefix_factor to exponent*/
	string prefix_s="";
	int exponent = prefix_factor / 3;
	if (exponent > 24)
		exponent = 24;
	else if (exponent < -24)
		exponent = -24;
	
	for (auto& pre: prefix_to_exponent)
	{
		if (pre.second == exponent)
		{
			prefix_s = pre.first;
			break;
		}
	}
	if (prefix_s=="") return 0;
	name_p = prefix_s;
	return prefix_factor % 3;
}


double unit_t::prefix_t::operator*(const unit_t::prefix_t& obj) 
{
	if (prefix_to_exponent.find(name())==prefix_to_exponent.end()) return 1;
	if (prefix_to_exponent.find(obj.name())==prefix_to_exponent.end()) return 1;
	int factor = prefix_to_exponent.at(name()) * prefix_to_exponent.at(obj.name());
	name_p = "";
	return pow(10,minimize_prefix_exponent(factor));
}


/******************************/





std::__cxx11::string unit_t::name() const
{
	stringstream name_s;
	name_s << suffix_p << base_p << suffix_p;
	return name_s.str();
}

std::__cxx11::string unit_t::base() const
{
	return base_p;
}

std::__cxx11::string unit_t::prefix() const
{
	return prefix_p.name();
}
std::__cxx11::string unit_t::suffix() const
{
	return suffix_p;
}

unit_t::unit_t(std::__cxx11::string prefix_s, std::__cxx11::string base_s, std::__cxx11::string suffix_s)
{
	base_p = base_s;
	prefix_p = prefix_s;
	suffix_p = suffix_s;
}

bool unit_t::operator==(const unit_t& obj) const
{
	if (base()!=obj.base()) return false;
	if (prefix()!=obj.prefix()) return false;
	if (suffix()!=obj.suffix()) return false;
	return true;
}

bool unit_t::operator!=(const unit_t& obj) const
{
	return !(operator==(obj));
}


