#include "isotope.hpp"

/*PSE singleton*/
// pse_t PSE;

/************************************/
/*********     isotope_t       ******/
/************************************/

isotope_t::isotope_t(std::__cxx11::string symbol_s, int nucleons_s, double abundance_s, double amount_s) :							 symbol(symbol_s), 
																																   nucleons(nucleons_s),
																																   abundance({abundance_s}),
																																   substance_amount({amount_s})
{}

const mass_t isotope_t::mass() const
{
	if (symbol=="")
	{
		logger::error("isotope_t::mass() symbol not set", "this will likely cause calculation errors");
		return mass_t();
	}
	if (nucleons<=0)
	{
		logger::error("isotope_t::mass() nucleons not set or found", "this will likely cause calculation errors");
		return mass_t();
	}
	if (PSE.element(symbol)->isotope_from_nucleons(nucleons)==nullptr) 
	{
		logger::error("mass_t::abundance() isotope not found in PSE", "this will likely cause calculation errors");
		return mass_t();
	}
	return mass_t({PSE.element(symbol)->isotope_from_nucleons(nucleons)->mass});
}

// const int isotope_t::nucleons() const
// {
// 	if (nucleons_p>0) return nucleons_p;
// 	if (symbol=="") 
// 	{
// 		logger::error("isotope_t::mass() symbol not set", "this will likely cause calculation errors");
// 		return nucleons_p;
// 	}
// 	return PSE.element(symbol)->isotope_with_highest_abundance()->nucleons;
// }

const std::__cxx11::string isotope_t::to_string() const
{
	stringstream out;
	out << nucleons << symbol;
	if (abundance.is_set())
		out << ":" << abundance.data.at(0) << abundance.unit().to_string();
	if (substance_amount.is_set())
		out << ":" << substance_amount.data.at(0) << substance_amount.unit().to_string();
	return out.str();
}



/** OPERATORS **/
const bool isotope_t::operator==(const isotope_t& obj) const
{
	
	if (symbol!=obj.symbol) return false;
	if (nucleons!=obj.nucleons) return false;
	return true;
}

const bool isotope_t::operator!=(const isotope_t& obj) const
{
	return !operator==(obj);
}

const bool isotope_t::operator<(const isotope_t& obj) const
{
	if (symbol < obj.symbol) return true;
	if (symbol > obj.symbol) return false;
	if (nucleons < obj.nucleons) return true;
	if (nucleons > obj.nucleons) return false;
	return false;
}



