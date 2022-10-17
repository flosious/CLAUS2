#include "isotope.hpp"

/*pse_t::singleton*/
// pse_t pse_t::

/************************************/
/*********     isotope_t       ******/
/************************************/

isotope_t::isotope_t() : symbol(""),nucleons(0), logger(__FILE__,"isotope_t")
{
}


isotope_t::isotope_t(std::string symbol_s, int nucleons_s, double abundance_s, double amount_s)
    : symbol(symbol_s), nucleons(nucleons_s), abundance({abundance_s}), substance_amount({amount_s}),
      logger(__FILE__,"isotope_t")
{
	if (abundance_s<0)
		abundance.clear();
	if (amount_s<0)
		substance_amount.clear();
}

isotope_t::isotope_t(std::string str, double abundance_s, double amount_s) : logger(__FILE__,"isotope_t")
{
    std::smatch match;
	if (regex_search(str,match,std::regex("^([0-9]{0,3})([a-zA-Z]{1,3})([0-9]*)$"))) /// 11B_fit
	{
		std::stringstream t;
		t << match[1]  << "\t"  << match[2] << "\t" << match[3];
        //logger::debug(21,"isotope_t::isotope_t()","str",str, t.str());
		if (match[1]!="") nucleons = tools::str::str_to_int(match[1]);
		else 
        {
            nucleons=pse_t::element(symbol)->isotope_with_highest_abundance()->nucleons;
		}
		if (match[2]!="") symbol = match[2];
		else
		{
            //logger::error("isotope_t::isotope_t()","symbol=''", "iso:'"+str + "'","returning");
			symbol="";
			return;
		}
		if (match[3]!="") amount_s = tools::str::str_to_double (match[3]);
			else amount_s = 1;
		*this = isotope_t{symbol,nucleons,abundance_s,amount_s};
	}
	else
    {
        //logger::error("isotope_t::isotope_t()","regex_search(iso_p,match,std::regex('^([0-9]{0,3})([a-zA-Z]{1,3})([0-9]*)')",str,"returning");
    }
}

const quantity::mass_t isotope_t::mass() const
{
	if (symbol=="")
	{
        //logger::error("isotope_t::mass() symbol not set", "this will likely cause calculation errors");
		return quantity::mass_t();
	}
	if (nucleons<=0)
	{
        //logger::error("isotope_t::mass() nucleons not std::set or found", "this will likely cause calculation errors");
		return quantity::mass_t();
	}
    if (pse_t::element(symbol)->isotope_from_nucleons(nucleons)==nullptr)
    {
        //logger::error("quantity::mass_t::abundance() isotope not found in pse_t::, "this will likely cause calculation errors");
        return quantity::mass_t();
    }
    return quantity::mass_t({pse_t::element(symbol)->isotope_from_nucleons(nucleons)->mass});
}

// const int isotope_t::nucleons() const
// {
// 	if (nucleons_p>0) return nucleons_p;
// 	if (symbol=="") 
// 	{
// 		logger::error("isotope_t::mass() symbol not set", "this will likely cause calculation errors");
// 		return nucleons_p;
// 	}
// 	return pse_t::element(symbol)->isotope_with_highest_abundance()->nucleons;
// }

bool isotope_t::is_set() const
{
	if (symbol=="")
		return false;
	if (nucleons<1)
		return false;
	return true;
}


const std::string isotope_t::to_string_short() const
{
	std::stringstream out;
	out << nucleons << symbol;
	return out.str();
}

const std::string isotope_t::to_string(const std::string del) const
{
	std::stringstream out;
	
	out << nucleons << symbol;
	if (substance_amount.is_set() && substance_amount.data().at(0) != 1)
		out << "" << substance_amount.data().at(0)/* << substance_amount.unit().to_string()*/;
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

void isotope_t::set_natural_abundance_from_PSE()
{
    if (!is_set())
        return;
    auto iso_in_pse = pse_t::isotope(symbol,nucleons);
    if (iso_in_pse == nullptr)
    {
        //logger::error("isotope_t::set_natural_abundance_from_pse_t::,"could not find isotope in pse_t:: " + to_string());
        return;
    }
    abundance = quantity::abundance_t({iso_in_pse->abundance});
}

