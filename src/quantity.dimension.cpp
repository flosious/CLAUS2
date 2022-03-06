#include "quantity.hpp"

quantity::quantity_t::dimension_t::dimension_t(const dimension_t& dim)
{
	*this = dim;
}

quantity::quantity_t::dimension_t quantity::quantity_t::dimension_t::operator/(const dimension_t& obj) const 
{
	return {length-obj.length,
			mass - obj.mass,
			time - obj.time,
			electic_current - obj.electic_current,
			substance_amount - obj.substance_amount,
			absolute_temperature - obj.absolute_temperature,
			luminous_intensity - obj.luminous_intensity	};
}

quantity::quantity_t::dimension_t quantity::quantity_t::dimension_t::operator*(const dimension_t& obj) const 
{
	return {length + obj.length,
			mass + obj.mass,
			time + obj.time,
			electic_current + obj.electic_current,
			substance_amount + obj.substance_amount,
			absolute_temperature + obj.absolute_temperature,
			luminous_intensity + obj.luminous_intensity	};
}

quantity::quantity_t::dimension_t quantity::quantity_t::dimension_t::operator^(int pow) const
{
	if (pow==0)
		return {};
	
	return {length*pow,
			mass*pow,
			time*pow,
			electic_current*pow,
			substance_amount*pow,
			absolute_temperature*pow,
			luminous_intensity*pow
	};
}

quantity::quantity_t::dimension_t quantity::quantity_t::dimension_t::invert() const
{
	return *this^(-1);
}

bool quantity::quantity_t::dimension_t::operator==(const dimension_t& obj) const
{
	if (length != obj.length) 	return false;
	if (mass != obj.mass) 	return false;
	if (time != obj.time) 	return false;
	if (electic_current != obj.electic_current) 	return false;
	if (substance_amount != obj.substance_amount) 	return false;
	if (absolute_temperature != obj.absolute_temperature) 	return false;
	if (luminous_intensity != obj.luminous_intensity) 	return false;
	return true;
}

bool quantity::quantity_t::dimension_t::operator!=(const dimension_t& obj) const
{
	return !operator==(obj);
}

string quantity::quantity_t::dimension_t::string_builder(const string base, int exponent)
{
	stringstream ss;
	if (exponent!=0)
	{
		ss << base;
		if (exponent!=1)
			ss << exponent;
	}
	return ss.str();
}

bool quantity::quantity_t::dimension_t::is_relative() const
{
	if (time!=0) return false;
	if (length!=0) return false;
	if (mass!=0) return false;
	if (electic_current!=0) return false;
	if (absolute_temperature!=0) return false;
	if (substance_amount!=0) return false;
	if (luminous_intensity!=0) return false;
	return true;
}

string quantity::quantity_t::dimension_t::to_string() const
{
	if (is_relative())
		return "relative";
	stringstream ss;
	ss << string_builder("T",time);
	ss << string_builder("L",length);
	ss << string_builder("M",mass);
	ss << string_builder("I",electic_current);
	ss << string_builder("Th",absolute_temperature);
	ss << string_builder("N",substance_amount);
	ss << string_builder("J",luminous_intensity);
	return ss.str();
}


