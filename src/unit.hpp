/*
	Copyright (C) 2021 Florian Bärwolf
	floribaer@gmx.de

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef UNIT_HPP
#define UNIT_HPP

#include "tools.hpp"
#include "log.hpp"
#include <math.h>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <map>
#include <stdio.h>
// #include <gsl/gsl_const_mksa.h>

using namespace std;

/// just supporting the SI system at the moment, integer exponents
class unit_t
{
public:
	class base_exponents_t
	{
	private:
	public:
		enum base_t
		{
			Meter = 0,
			Kilogram = 1,
			Second = 2,
			Ampere = 3,
			Mole = 4,
			Kelvin = 5,
			Candela = 6
		};
		constexpr base_exponents_t(
				int meters,
				int kilograms,
				int seconds,
				int amperes,
				int moles,
				int kelvins,
				int candelas,
				bool relative=false
  								) : meters(meters), kilograms(kilograms), seconds(seconds), amperes(amperes),moles(moles), kelvins(kelvins), candelas(candelas), relative(relative)
		{};
		///returns the corresponding integer base from its enum
// 		int* get_base(base_t B);
		int meters;
		int kilograms;
		int seconds;
		int amperes;
		int moles;
		int kelvins;
		int candelas;
		bool relative=false;
		const string to_string() const;
		bool is_set() const;
		bool operator==(const base_exponents_t& obj) const;
		bool operator!=(const base_exponents_t& obj) const;
		base_exponents_t operator*(const base_exponents_t& obj) const;
		base_exponents_t operator/(const base_exponents_t& obj) const;
	};


// 	char* symbol = "";
// public:
private:
	static const map<std::string,unit_t> symbol_to_base_unit;
	static const std::unordered_map<std::string, unit_t> symbol_prefix_unit;
// 	static const std::unordered_map<std::string, unit_t> symbol_to_unit;
	static const map<std::string, unit_t> symbol_to_unit;
public:
	base_exponents_t base_units_exponents{0,0,0,0,0,0,0,false};
public:
	constexpr unit_t() {};
// 	constexpr unit_t(string symbols);
	constexpr unit_t(double multiplier) : multiplier(multiplier) {};
	constexpr unit_t(const unit_t& unit_, double multiplier) : base_units_exponents(unit_.base_units_exponents), multiplier(multiplier) {};
	constexpr unit_t(base_exponents_t unit_exponents, double multiplier) : base_units_exponents(unit_exponents),multiplier(multiplier) {};
	unit_t(string symbols);
	
// 	static unit_t string_to_unit(string unit_s);
// 	constexpr unit_t(unit_t unit, double multiplier);
// 	unit_t(string name_s) {};
	bool is_set() const;
	///return any matching symbol
	const string to_string() const;
	///return exact matching symbol, if not found -> fallback to any
// 	const string to_string(string symbol) const;
// 	double change_unit(unit_t new_unit);
	bool operator<(const unit_t& obj) const;
	bool operator==(const unit_t& obj) const;
	bool operator!=(const unit_t& obj) const;
	unit_t pow(int pot) const;
	unit_t operator*(const unit_t& obj) const;
	unit_t operator/(const unit_t& obj) const;
	double multiplier=1;
	/*tests*/
// 	signed int meter_ : 5;
	
// 	static const string symbol(const unit_t unit);
};

namespace units
{
	namespace SI
	{
		constexpr unit_t meter		({1,0,0,0,0,0,0},1);
		constexpr unit_t kilogram	({0,1,0,0,0,0,0},1);
		constexpr unit_t second		({0,0,1,0,0,0,0},1);
		constexpr unit_t Ampere		({0,0,0,1,0,0,0},1);
		constexpr unit_t mol		({0,0,0,0,1,0,0},1);
		constexpr unit_t Kelvin		({0,0,0,0,0,1,0},1);
		constexpr unit_t Candela	({0,0,0,0,0,0,1},1);
		constexpr unit_t one		({0,0,0,0,0,0,0},1);
	}
	
	namespace prefixes
	{
		constexpr unit_t deci	(1E-1);
		constexpr unit_t centi	(1E-2);
		constexpr unit_t milli	(1E-3);
		constexpr unit_t micro	(1E-6);
		constexpr unit_t nano	(1E-9);
		constexpr unit_t pico	(1E-12);
		constexpr unit_t femto	(1E-15);
		constexpr unit_t atto	(1E-18);
		constexpr unit_t zepto	(1E-21);
		constexpr unit_t yocto	(1E-24);
		
		constexpr unit_t deca	(1E1);
		constexpr unit_t hecto	(1E2);
		constexpr unit_t kilo	(1E3);
		constexpr unit_t mega	(1E6);
		constexpr unit_t giga	(1E9);
		constexpr unit_t terra	(1E12);
		constexpr unit_t peta	(1E15);
		constexpr unit_t exa	(1E18);
		constexpr unit_t zetta	(1E21);
		constexpr unit_t yotta	(1E24);
	}
	
	namespace suffixes
	{
		constexpr unit_t percent(1E-2);
		constexpr unit_t promill(1E-3);
	}
	
	namespace derived
	{
		constexpr unit_t amu({0,1,0,0,0,0,0},1/6.02214076E23/1E3);
		constexpr unit_t coulomb({0,0,1,1,0,0,0},1);
		constexpr unit_t newton({1,1,-2,0,0,0,0},1);
		constexpr unit_t joule({2,1,-2,0,0,0,0},1);
		constexpr unit_t electron_volt(joule,1.602176634E-19);
		constexpr unit_t watt({2,1,-3,0,0,0,0},1);
		constexpr unit_t volt({2,1,-3,-1,0,0,0},1); // kg m m /s /s / A /s = V
		constexpr unit_t atoms({0,0,0,0,1,0,0},1/6.02214076E23);
		constexpr unit_t counts(atoms);
		constexpr unit_t atom_percent({0,0,0,0,1,0,0,true},0.01);
		constexpr unit_t bar({-1,1,-2,0,0,0,0},1);
		constexpr unit_t min({0,0,1,0,0,0,0},60);
		constexpr unit_t atoms_per_ccm({-3,0,0,0,1,0,0},1E-6);
	}
}

namespace constants
{
	constexpr unit_t elementary_charge(units::derived::coulomb,1.602176634E-19);
	constexpr unit_t avogadro({0,0,0,0,-1,0,0},6.02214076E23);
}



// static const map<unit_t, std::string> base_unit_to_symbol
// {
// 	{units::SI::meter,"m"},
// 	{units::SI::kilogram,"kg"},
// 	{units::SI::second,"s"},
// 	{units::SI::Ampere,"A"},
// 	{units::SI::mol,"mol"},
// 	{units::SI::Kelvin,"K"},
// 	{units::SI::Candela,"Cd"}
// };
// 
// static const map<std::string,unit_t> symbol_to_base_unit
// {
// 	{"m",units::SI::meter},
// 	{"kg",units::SI::kilogram,},
// 	{"s",units::SI::second},
// 	{"A",units::SI::Ampere},
// 	{"mol",units::SI::mol},
// 	{"K",units::SI::Kelvin},
// 	{"Cd",units::SI::Candela}
// };
// 
// static const std::unordered_map<std::string, unit_t> symbol_prefix_unit 
// {
// 	{"d",units::prefixes::deci},
// 	{"c",units::prefixes::centi},
// 	{"m",units::prefixes::milli},
// 	{"u",units::prefixes::micro},
// 	{"n",units::prefixes::nano},
// 	{"p",units::prefixes::pico},
// 	{"f",units::prefixes::femto},
// 	{"a",units::prefixes::atto},
// 	{"z",units::prefixes::zepto},
// 	{"y",units::prefixes::yocto},
// 	
// 	{"da",units::prefixes::deca},
// 	{"h",units::prefixes::hecto},
// 	{"k",units::prefixes::kilo},
// 	{"M",units::prefixes::mega},
// 	{"G",units::prefixes::giga},
// 	{"T",units::prefixes::terra},
// 	{"P",units::prefixes::peta},
// 	{"E",units::prefixes::exa},
// 	{"Z",units::prefixes::zetta},
// 	{"Y",units::prefixes::yotta}
// };
// 
// static const std::unordered_map<std::string, unit_t> symbol_to_unit 
// {
// 	{"%",units::suffixes::percent},
// 	{"bar",units::derived::bar},
// 	{"cnt/s",units::derived::counts/units::SI::second},
// 	{"e",constants::elementary_charge},
// 	{"V",units::derived::volt},
// 	{"eV",units::derived::electron_volt},
// 	/*atoms*/
// 	{"at",units::derived::atoms},
// 	{"at%",units::derived::atom_percent},
// 	{"at/ccm",units::derived::atoms/units::SI::meter.pow(3)*units::prefixes::centi.pow(3)},
// 	{"at/scm",units::derived::atoms/units::SI::meter.pow(2)*units::prefixes::centi.pow(2)},
// 	{"at/cm^3",units::derived::atoms/units::SI::meter.pow(3)*units::prefixes::centi.pow(3)},
// 	{"at/cm^2",units::derived::atoms/units::SI::meter.pow(2)*units::prefixes::centi.pow(2)},
// 	/*time*/
// 	{"sec",{units::SI::second}},
// 	{"min",{units::SI::second,60}},
// 	{"h",{units::SI::second,60*60}},
// 	{"hour",{units::SI::second,60*60}},
// 	{"d",{units::SI::second,60*60*24}},
// 	{"day",{units::SI::second,60*60*24}},
// 	{"w",{units::SI::second,60*60*24*7}},
// 	{"week",{units::SI::second,60*60*24*7}},
// 	{"y",{units::SI::second,60*60*24*365.25}},
// 	{"year",{units::SI::second,60*60*24*365.25}}
// };

// static const map<unit_t, vector<string>> derived_units_symbols
// {
// 	{constants::elementary_charge,{"eV"}}
// };

#endif // UNIT_HPP
