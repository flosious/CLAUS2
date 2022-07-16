#include "unit.hpp"

/****************************************/
/*****  unit_t::base_exponents_t  *******/
/****************************************/

// unit_t units::SI::meter	=	({1,0,0,0,0,0,0},1);



const map<std::string,unit_t> unit_t::symbol_to_base_unit
{
	{"m",{units::SI::meter}},
	{"kg",{units::SI::kilogram}},
	{"s",{units::SI::second}},
	{"A",{units::SI::Ampere}},
	{"mol",{units::SI::mol}},
	{"K",{units::SI::Kelvin}},
	{"cd",{units::SI::Candela}}
};

const std::unordered_map<std::string, unit_t> unit_t::symbol_prefix_unit 
{
	{"d",{units::prefixes::deci}},
	{"c",{units::prefixes::centi}},
	{"m",{units::prefixes::milli}},
	{"u",{units::prefixes::micro}},
	{"n",{units::prefixes::nano}},
	{"p",{units::prefixes::pico}},
	{"f",{units::prefixes::femto}},
	{"a",{units::prefixes::atto}},
	{"z",{units::prefixes::zepto}},
	{"y",{units::prefixes::yocto}},
	
	{"da",{units::prefixes::deca}},
	{"h",{units::prefixes::hecto}},
	{"k",{units::prefixes::kilo}},
	{"M",{units::prefixes::mega}},
	{"G",{units::prefixes::giga}},
	{"T",{units::prefixes::terra}},
	{"P",{units::prefixes::peta}},
	{"E",{units::prefixes::exa}},
	{"Z",{units::prefixes::zetta}},
	{"Y",{units::prefixes::yotta}}
};

const map<std::string, unit_t> unit_t::symbol_to_unit 
{
	{"%",{units::suffixes::percent}},
	{"bar",{units::derived::bar}},
	{"cnt/s",{units::derived::counts/units::SI::second}},
	{"c/s",{units::derived::counts/units::SI::second}},
	{"e",{constants::elementary_charge}},
	{"V",{units::derived::volt}},
	{"eV",{units::derived::electron_volt}},
	/*atoms*/
	{"at",{units::derived::atoms}},
	{"at%",{units::derived::atom_percent}},
	{"nm/s",{units::SI::meter/units::SI::second*units::prefixes::nano}},
	{"Ang",{units::derived::Ang}},
	{"nm/min",{units::SI::meter/units::derived::min*units::prefixes::nano}},
	{"at/scm",{units::derived::atoms / (units::SI::meter.pow(2)*units::prefixes::centi.pow(2))}},
	{"at/ccm",units::derived::atoms_per_ccm},
// 	{"at/cm^3",{units::derived::atoms / (units::SI::meter.pow(3)/units::prefixes::centi.pow(3))}},
	{"at/cm^3",units::derived::atoms_per_ccm},
	{"atom/cm3",units::derived::atoms_per_ccm},
	{"atom/cm3/(c/s)",units::derived::atoms_per_ccm/(units::derived::counts/units::SI::second)},
	{"at/cm^2",{units::derived::atoms / (units::SI::meter.pow(2)/units::prefixes::centi.pow(2))}},
	/*time*/
	{"sec",{units::SI::second}},
	{"min",{units::derived::min}},
	{"h",{units::SI::second,60*60}},
	{"hour",{units::SI::second,60*60}},
	{"d",{units::SI::second,60*60*24}},
	{"day",{units::SI::second,60*60*24}},
	{"week",{units::SI::second,60*60*24*7}},
	{"y",{units::SI::second,60*60*24*365.25}},
	{"year",{units::SI::second,60*60*24*365.25}},
	{"w",{units::SI::second,60*60*24*7}}
};

bool unit_t::base_exponents_t::is_relative() const
{
	unsigned int sum = 0;
	sum += abs(meters);
	sum += abs(kilograms);
	sum += abs(seconds);
	sum += abs(amperes);
	sum += abs(moles);
	sum += abs(kelvins);
	sum += abs(candelas);
	if (sum==0)
		return true;
	return false;
}

bool unit_t::base_exponents_t::operator==(const unit_t::base_exponents_t& obj) const
{
	if (meters != obj.meters) return false;
	if (kilograms != obj.kilograms) return false;
	if (seconds != obj.seconds) return false;
	if (amperes != obj.amperes) return false;
	if (moles != obj.moles) return false;
	if (kelvins != obj.kelvins) return false;
	if (candelas != obj.candelas) return false;
	if ( (is_relative() != obj.is_relative())) return false;
	return true;
}

bool unit_t::base_exponents_t::operator!=(const unit_t::base_exponents_t& obj) const
{
	return !operator==(obj);
}

unit_t::base_exponents_t unit_t::base_exponents_t::operator/(const unit_t::base_exponents_t& obj) const
{
	return {meters-obj.meters,kilograms-obj.kilograms,seconds-obj.seconds,amperes-obj.amperes,moles-obj.moles,kelvins-obj.kelvins,candelas-obj.candelas,relative};
}

unit_t::base_exponents_t unit_t::base_exponents_t::invert() const
{
	return {-1*meters,-1*kilograms,-1*seconds,-1*amperes,-1*moles,-1*kelvins,-1*candelas,relative};
}


unit_t::base_exponents_t unit_t::base_exponents_t::operator*(const unit_t::base_exponents_t& obj) const
{
	return {obj.meters+meters,obj.kilograms+kilograms,seconds+obj.seconds,obj.amperes+amperes,moles+obj.moles,kelvins+obj.kelvins,candelas+obj.candelas,relative};
}


bool unit_t::base_exponents_t::is_set() const
{
	if (meters!=0) return true;
	if (kilograms!=0) return true;
	if (seconds!=0) return true;
	if (amperes!=0) return true;
	if (moles!=0) return true;
	if (kelvins!=0) return true;
	if (candelas!=0) return true;
	return false;
}

const string unit_t::base_exponents_t::to_string() const
{
	stringstream out;
// 	vector<string> counter_, denominator;
	
	if (meters!=0)
	{
		logger::debug(21,"unit_t::base_exponents_t::to_string()","meters","set");
// 		out << base_unit_to_symbol.at(units::SI::meter);
		out << "m";
		if (meters!=1)
			out << "^" << meters << " ";
		else 
			out << " ";
	}
	if (kilograms!=0)
	{
		logger::debug(21,"unit_t::base_exponents_t::to_string()","kilograms","set");
// 		out << base_unit_to_symbol.at(units::SI::kilogram);
		out << "kg";
		if (kilograms!=1)
			out << "^" << kilograms << " ";
		else 
			out << " ";
	}
	if (seconds!=0)
	{
		logger::debug(21,"unit_t::base_exponents_t::to_string()","seconds","set");
// 		out << base_unit_to_symbol.at(units::SI::second);
		out << "s";
		if (seconds!=1)
			out << "^" << seconds << " ";
		else 
			out << " ";
	}
	if (amperes!=0)
	{
		logger::debug(21,"unit_t::base_exponents_t::to_string()","amperes","set");
// 		out << base_unit_to_symbol.at(units::SI::Ampere);
		out << "A";
		if (amperes!=1)
			out << "^" << amperes << " ";
		else 
			out << " ";
	}
	if (kelvins!=0)
	{
		logger::debug(21,"unit_t::base_exponents_t::to_string()","kelvins","set");
// 		out << base_unit_to_symbol.at(units::SI::Kelvin);
		out << "K";
		if (kelvins!=1)
			out << "^" << kelvins << " ";
		else 
			out << " ";
	}
	if (moles!=0)
	{
		logger::debug(21,"unit_t::base_exponents_t::to_string()","moles","set");
// 		out << base_unit_to_symbol.at(units::SI::mol);
		out << "mol";
		if (moles!=1)
			out << "^" << moles << " ";
		else 
			out << " ";
	}
	if (candelas!=0)
	{
		logger::debug(21,"unit_t::base_exponents_t::to_string()","candelas","set");
// 		out << base_unit_to_symbol.at(units::SI::Candela);
		out << "cd";
		if (candelas!=1)
			out << "^" << candelas << " ";
		else 
			out << " ";
	}
	string out_ = out.str();
// 	tools::str::remove_spaces_from_string_end(&out_);
	return out_.substr(0,out_.size()-1);
}

// int* unit_t::base_exponents_t::get_base(unit_t::base_exponents_t::base_t B)
// {
// 	switch (B)
// 	{
// 		case Meter: return &meters;
// 		case Kilogram: return &kilograms;
// 		case Second: return &seconds;
// 		case Ampere: return &amperes;
// 		case Mole: return &moles;
// 		case Kelvin: return &kelvins;
// 		case Candela: return &candelas;
// 		default: return nullptr;
// 	}
// 	return nullptr;
// }

/***********************/
/*****  unit_t   *******/
/***********************/


// unit_t::unit_t(unit_t unit_, string prefered_string) : unit_t(unit_)
// {
// 	prefered_output_string = prefered_string;
// }

unit_t::unit_t(string symbols, string prefered_string) : prefered_output_string(prefered_string)
{
	tools::str::remove_spaces(&symbols);
	
	if (symbols=="" || symbols.length()==0)
	{
		logger::debug(11,"unit_t::unit_t()","symbols=="" || symbols.length()==0");
		return;
	}
	
	/*symbols is a derived unit*/
	if (symbol_to_unit.size()>0 && symbol_to_unit.find(symbols)!=symbol_to_unit.end())
	{
		logger::debug(21,"unit_t::unit_t","symbol_to_unit.at("+symbols+")",symbol_to_unit.at(symbols).base_units_exponents.to_string());
		*this={symbol_to_unit.at(symbols),symbols};
		logger::debug(21,"unit_t::unit_t","symbols is a derived unit",symbols,base_units_exponents.to_string());
		return;
	}
	/*symbols is a combination of prefix and derived unit*/
	for (auto& us : symbol_to_unit)
	{
		for (auto& prefix_symbol : symbol_prefix_unit)
		{
			if ((prefix_symbol.first + us.first) == symbols)
			{
				*this = {us.second*prefix_symbol.second,symbols};
				logger::debug(21,"unit_t::unit_t","symbols is a combination of prefix and derived unit",symbols,base_units_exponents.to_string());
				return;
			}
		}
	}
	
	/*symbols is a base unit*/
	if (symbol_to_base_unit.size()>0 && symbol_to_base_unit.find(symbols)!=symbol_to_base_unit.end())
	{
		logger::debug(21,"unit_t::unit_t","symbol_to_base_unit.at("+symbols+")",symbol_to_base_unit.at(symbols).base_units_exponents.to_string());
		*this={symbol_to_base_unit.at(symbols),symbols};
		logger::debug(21,"unit_t::unit_t","symbols is a base unit",symbols,base_units_exponents.to_string());
		return;
	}
	/*symbols is a combination of prefix and base unit*/
	for (auto& is : symbol_to_base_unit)
	{
		for (auto& prefix_symbol : symbol_prefix_unit)
		{
			if ((prefix_symbol.first + is.first) == symbols)
			{
// 				logger::debug("symbols is a combination of prefix and base unit",symbols);
				*this = {is.second*prefix_symbol.second,symbols};
				logger::debug(21,"unit_t::unit_t","symbols is a combination of prefix and base unit",symbols,base_units_exponents.to_string());
				return;
			}
		}
	}
	
	logger::error("unit_t::unit_t: symbols unknown","'"+symbols+"'");
}

// const string unit_t::to_string(string symbol) const
// {
// 	if (symbol=="")
// 		return to_string();
// 	
// 	if (unit_t(symbol)==*this)
// 		return symbol;
// 	//fallback to any symbol
// 	return to_string();
// }

// const string unit_t::to_string(const string output) const
// {
// 	if (is_set())
// 	{
// 		if (unit_t(output)==*this)
// 			return output;
// 	}
// 	return "";
// }

const string unit_t::to_string() const
{
	if (!is_set()) return "";
	if (prefered_output_string!="") 
	{
// 		cout << endl << endl << "PREFERED STRING '" << prefered_output_string << "'" << endl;
		return prefered_output_string;
	}
	if (!base_units_exponents.is_set() )
	{
// 		logger::debug(21,"unit_t::to_string()","base_units_exponents not set");
		if (base_units_exponents.is_relative()  && multiplier==1)
			return "a.u.";
		else
			return "*"+tools::to_string(multiplier);
	}
	logger::debug(21,"unit_t::to_string()","base_units_exponents: "+ base_units_exponents.to_string(), "multiplier: " + tools::to_string(multiplier));
	/*check SI*/
	for (auto it=symbol_to_base_unit.begin();it!=symbol_to_base_unit.end();it++)
	{
		if (*this == it->second)
		{
			logger::debug(21,"unit_t::to_string()","just base",it->first);
			return it->first;
		}
		for (auto pre=symbol_prefix_unit.begin();pre!=symbol_prefix_unit.end();pre++)
		{
			if (pre->second*it->second==*this)
			{
				logger::debug(21,"unit_t::to_string()","prefix+base",it->first);
				return pre->first+it->first;
			}
		}
	}
	
	/*check known derived units*/
	for (auto it=symbol_to_unit.begin();it!=symbol_to_unit.end();it++)
	{
		if (it->second!=*this) continue;
		logger::debug(21,"unit_t::to_string()","just derived",it->first);
		return it->first;
	}
	/* just check base units and multiplier separately
	   and add prefix */
	for (auto it=symbol_to_unit.begin();it!=symbol_to_unit.end();it++)
	{
		if (it->second.base_units_exponents != base_units_exponents) continue;
		/*multipliers match, no need for prefix*/
		double pref_multiplier = multiplier / it->second.multiplier;
		if (pref_multiplier == 1)
		{
			logger::debug(21,"unit_t::to_string()","just derived(2)",it->first);
			return it->first;
		}
		for (auto const& prefix : symbol_prefix_unit)
		{
			if (pref_multiplier != prefix.second.multiplier) continue;
			logger::debug(21,"unit_t::to_string()","prefix+derived",prefix.first + it->first);
			return prefix.first + it->first;
		}
	}
	
	/*unknown SI derivative*/
	
	for (auto const& prefix : symbol_prefix_unit)
	{
		if (prefix.second != multiplier) continue;
		stringstream out;
		out << prefix.first << " ";
		out << base_units_exponents.to_string();
// 		logger::debug(21,"unit_t::to_string()","prefix+derived",prefix.first + it->first);
		logger::warning(3,"unit_t::to_string()","unknown SI derivate unit:",out.str());
		return out.str();
	}
	
	
// 	vector<string> counter, denominator;
// 	if (base_units_exponents.Ampere==1)			counter.push_back(units::SI::Ampere.to_string());
// 	else if (base_units_exponents.Ampere>0) 	counter.push_back(units::SI::Ampere.to_string() + string("^") + base_units_exponents.Ampere);
// 	else if (base_units_exponents.Ampere<0) 	denominator << units::SI::Ampere.to_string() << "^" << base_units_exponents.Ampere << " ";
	
// 	if (base_units_exponents.Candela==1)		counter << units::SI::Candela.to_string() << " ";
// 	else if (base_units_exponents.Candela>0) 	counter << units::SI::Candela.to_string() << "^" << base_units_exponents.Candela << " ";
		
// 	logger::debug(5,"unit_t::to_string()","not found","");

	logger::warning(3,"unit_t::to_string()","unknown SI unit: base=" + base_units_exponents.to_string()+ "\tmultiplier=" + tools::to_string(multiplier));
	stringstream out;
	out  << tools::to_string(multiplier) << " * "<< base_units_exponents.to_string();
	return out.str();
// 	exit (EXIT_FAILURE);
// 	return "unknown"; // will never be called
}

unit_t unit_t::operator^(int exp) const
{
	return pow(exp);
}
unit_t unit_t::pow(int pot) const
{
	unit_t powed = *this;
	powed.base_units_exponents.meters = pot * base_units_exponents.meters;
	powed.base_units_exponents.kilograms = pot * base_units_exponents.kilograms;
	powed.base_units_exponents.seconds = pot * base_units_exponents.seconds;
	powed.base_units_exponents.amperes = pot * base_units_exponents.amperes;
	powed.base_units_exponents.moles = pot * base_units_exponents.moles;
	powed.base_units_exponents.kelvins = pot * base_units_exponents.kelvins;
	powed.base_units_exponents.candelas = pot * base_units_exponents.candelas;
	
	powed.multiplier = std::pow(multiplier,pot);
	return powed;
}

bool unit_t::is_set() const
{
	if (base_units_exponents.is_set()) return true;
	if (multiplier!=0) return true;
	return false;
}

bool unit_t::operator==(const unit_t& obj) const
{
	if (base_units_exponents != obj.base_units_exponents) return false;
	if (multiplier != obj.multiplier) return false;
	return true;
}

bool unit_t::operator!=(const unit_t& obj) const
{
	return !(operator==(obj));
}

unit_t unit_t::operator*(const unit_t& obj) const
{
// 	if (!is_set() || !obj.is_set()) return {};
	return unit_t{base_units_exponents * obj.base_units_exponents, multiplier * obj.multiplier};
}

unit_t unit_t::operator/(const unit_t& obj) const
{
	if (*this == obj)
		return {{0,0,0,0,0,0,0,true},1};
	return {base_units_exponents / obj.base_units_exponents, multiplier / obj.multiplier};
}

unit_t unit_t::invert() const
{
	return {base_units_exponents.invert(), 1/multiplier};
}


bool unit_t::operator<(const unit_t& obj) const
{
	if (base_units_exponents.meters > obj.base_units_exponents.meters) return false;
	if (base_units_exponents.kilograms > obj.base_units_exponents.kilograms) return false;
	if (base_units_exponents.seconds > obj.base_units_exponents.seconds) return false;
	if (base_units_exponents.amperes > obj.base_units_exponents.amperes) return false;
	if (base_units_exponents.moles > obj.base_units_exponents.moles) return false;
	if (base_units_exponents.kelvins > obj.base_units_exponents.kelvins) return false;
	if (base_units_exponents.candelas > obj.base_units_exponents.candelas) return false;
// 	if (multiplier < obj.multiplier) return true;
	return true;
}
