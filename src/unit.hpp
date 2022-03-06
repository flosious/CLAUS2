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
		bool is_relative() const;
		const string to_string() const;
		bool is_set() const;
		bool operator==(const base_exponents_t& obj) const;
		bool operator!=(const base_exponents_t& obj) const;
		base_exponents_t operator^(int pow) const;
		base_exponents_t operator*(const base_exponents_t& obj) const;
		base_exponents_t operator/(const base_exponents_t& obj) const;
		base_exponents_t invert() const;
	}; //base_exponents_t


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
	unit_t() {};
	unit_t(const unit_t& unit_) : base_units_exponents(unit_.base_units_exponents), multiplier(unit_.multiplier), prefered_output_string(unit_.prefered_output_string) {};
	unit_t(double multiplier) : multiplier(multiplier) {};
	unit_t(const unit_t& unit_, double multiplier, string prefered_unit_string = "" ) : base_units_exponents(unit_.base_units_exponents), multiplier(multiplier), prefered_output_string(prefered_unit_string) {};
	unit_t(const unit_t& unit_, string prefered_unit_string ) : base_units_exponents(unit_.base_units_exponents), multiplier(unit_.multiplier), prefered_output_string(prefered_unit_string) {};
	unit_t(base_exponents_t unit_exponents, double multiplier,string prefered_unit_string ="" ) : base_units_exponents(unit_exponents),multiplier(multiplier),prefered_output_string(prefered_unit_string) {};
	unit_t(string symbols,string prefered_string="");
	
// 	static unit_t string_to_unit(string unit_s);
// 	constexpr unit_t(unit_t unit, double multiplier);
// 	unit_t(string name_s) {};
	bool is_set() const;
	///return any matching symbol
	const string to_string() const;
	///forces the exact output string if unit_t is same
// 	const string to_string(const string output) const;
	///return exact matching symbol, if not found -> fallback to any
// 	const string to_string(string symbol) const;
// 	double change_unit(unit_t new_unit);
	bool operator<(const unit_t& obj) const;
	bool operator==(const unit_t& obj) const;
	bool operator!=(const unit_t& obj) const;
	
	unit_t operator*(const unit_t& obj) const;
	unit_t operator/(const unit_t& obj) const;
	unit_t pow(int pot) const;
	unit_t operator^(int exp) const;
	unit_t invert() const;
	double multiplier=1;
	/*tests*/
// 	signed int meter_ : 5;
	string prefered_output_string;
// 	static const string symbol(const unit_t unit},1};
};


namespace units
{
	namespace SI
	{
		const unit_t meter		=	{{1,0,0,0,0,0,0},1};
		const unit_t kilogram  =	{{0,1,0,0,0,0,0},1};
		const unit_t second		=	{{0,0,1,0,0,0,0},1};
		const unit_t Ampere		=	{{0,0,0,1,0,0,0},1};
		const unit_t mol		=	{{0,0,0,0,1,0,0},1};
		const unit_t Kelvin		=	{{0,0,0,0,0,1,0},1};
		const unit_t Candela	=	{{0,0,0,0,0,0,1},1};
		const unit_t one		=	{{0,0,0,0,0,0,0},1};
	}
	
	namespace prefixes
	{
		const unit_t deci	{1E-1};
		const unit_t centi	{1E-2};
		const unit_t milli	{1E-3};
		const unit_t micro	{1E-6};
		const unit_t nano	{1E-9};
		const unit_t pico	{1E-12};
		const unit_t femto	{1E-15};
		const unit_t atto	{1E-18};
		const unit_t zepto	{1E-21};
		const unit_t yocto	{1E-24};
		
		const unit_t deca	{1E1};
		const unit_t hecto	{1E2};
		const unit_t kilo	{1E3};
		const unit_t mega	{1E6};
		const unit_t giga	{1E9};
		const unit_t terra	{1E12};
		const unit_t peta	{1E15};
		const unit_t exa	{1E18};
		const unit_t zetta	{1E21};
		const unit_t yotta	{1E24};
	}
	
	namespace suffixes
	{
		const unit_t percent{1E-2};
		const unit_t promill{1E-3};
	}
	
	namespace derived
	{
		const unit_t amu 			= {SI::kilogram,1/6.02214076E23/1E3};
		const unit_t coulomb 		= {SI::Ampere * SI::second}; //{0,0,1,1,0,0,0};
		const unit_t newton 		= {SI::kilogram * SI::meter / SI::second / SI::second};  //{1,1,-2,0,0,0,0};
		const unit_t joule 			= {SI::meter * newton};
		const unit_t electron_volt 	= {joule,1.602176634E-19,"eV"};
		const unit_t watt 			= {joule/SI::second};
		const unit_t volt 			= {{2,1,-3,-1,0,0,0},1}; // kg m m /s /s / A /s = V
		const unit_t atoms			= {SI::mol,1/6.02214076E23,"at"};
		const unit_t counts 		= {atoms,1,"ct"};
		const unit_t atom_percent 	= {{0,0,0,0,0,0,0,true},0.01,"at%"};
		const unit_t bar 			= {newton / SI::meter / SI::meter,0.001,"bar"};
		const unit_t min 			= {SI::second,60};
// 		const unit_t atoms_per_ccm 	= {{-3,0,0,0,1,0,0},1E-6,"at/ccm"};
		const unit_t atoms_per_ccm 	= {atoms/((SI::meter*prefixes::centi)^3),"at/ccm"};
	}
}

namespace constants
{
	 const unit_t elementary_charge = {units::derived::coulomb,1.602176634E-19};
	 const unit_t avogadro = {{0,0,0,0,-1,0,0},6.02214076E23};
}


#endif // UNIT_HPP
