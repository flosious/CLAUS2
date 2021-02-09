/*
	Copyright (C) 2021Florian Bärwolf
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

#ifndef ELEMENT_T_HPP
#define ELEMENT_T_HPP


#include <sstream>
#include <vector>
#include <string>
#include "log.hpp"
#include "pse.hpp"
#include "quantity.hpp"
#include "definitions.hpp"



using namespace std;



static pse_t PSE;


/*interface to PSE*/

class isotope_t
{
	friend class matrix_t;
	friend class cluster_t;
private:
	abundance_t abundance_p;
	const int nucleons_p;
// 	const string symbol_p;
	substance_amount_t substance_amount_p;
	concentration_t concentration_p;
	depth_t depth_p;
public:
	///nucleons_s=-1 -> isotope with highest natural abundance
	isotope_t(const string symbol_s, const int nucleons_s=-1, const double abundance_s=-1, const double amount_s=1);
// 	isotope_t(const string symbol_s, const int nucleons_s=-1, const abundance_t abundance_s=abundance_t({-1}), const substance_amount_t amount_s = substance_amount_t({1}));
	const abundance_t& abundance() const;
	const mass_t mass() const;
	const int nucleons() const;
	string to_string();
	const string symbol_alternative() const;
	const substance_amount_t& substance_amount();
	const concentration_t& concentration();
	const depth_t& depth();
	
	///NOT checking abundance
	const bool operator==(const isotope_t& obj) const;
	const bool operator<(const isotope_t& obj) const;
	
	const string symbol;
	
};

class element_t
{
	friend class matrix_t;
private:
	const string symbol_p="";
	vector<isotope_t> isotopes_p;
	substance_amount_t substance_amount_p;
public:
	element_t();
	element_t(string symbol_s, double abs_amount=1, bool use_naturale_abundance=false);
	element_t(const vector<isotope_t>& isotopes_s, double abs_amount=1);
// 	element_t(const isotope_t& isotope_s, double amount=1);
// 	const vector<isotope_t>* isotopes() const;
	vector<isotope_t>& isotopes();
// 	const substance_amount_t& substance_amount() const;
	
// 	const double abundance() const;
	const mass_t mass();
	const string symbol();
	const string to_string();
	const int protons();
	///some abosulte value: atoms, mole, particles, ...
	
// 	const isotope_t* isotope_with_highest_abundance() const;
// 	const isotope_t* isotope_from_nucleons(int nucleons) const;
	///checking isotopes but NOT their abundances
	const bool operator==(element_t& obj);
	const bool operator<(element_t& obj);
};

class ion_t
{
private:
// 	const vector<element_t> elements_p;
// 	const electrical_charge_t electric_charge_p;
	vector< element_t > elements_p;
public:
	ion_t(vector<element_t>& elements_s, electrical_charge_t electric_charge_s);
	vector< element_t >& elements();
	const electrical_charge_t electric_charge;
	const string to_string();
	const bool is_set();
};




// class isotope_t
// {
// private:
// // 		const int nucleons_p;
// //		const abundance_t abundance_p;
// public:
// 	isotope_t(const int nucleons_s, const string symbol_s, const abundance_t abundance_s);
// 	const string symbol;
// 	const int nucleons;
// 	/// 0 < abundance <= 1
// 	const abundance_t abundance;
// 	const mass_t mass() const;
// 	
// 	const bool operator==(const isotope_t& obj) const;
// };
// 
// class element_t
// {
// protected:	
// 	/// chemical symbol like "Si"
// // 	const string symbol_p="";
// public:
// 	element_t();
// 	element_t(const set<isotope_t> isotope_s);
// 	/// "Si" will add all 3 isotopes, according to their natural abundance
// 	/// "28Si" will add just the isotope 28Si with 100% abundance
// 	/// "28Si2" amount is 2
// 	/// "28Si0.922 29Si0.047 30Si0.031" is same as simply "Si"
// 	element_t(const string nucleons_symbol_amount);
// 	/// "{28Si0.922, 29Si0.047, 30Si0.031}" is same as simply "{Si}"
// 	element_t(const vector<string> nucleons_symbol_amount);
// 	
// 	set<isotope_t> isotopes;
// 	const bool is_set();
// 	mass_t mass() const;
// 	const string symbol() const;
// 	const string to_string() const;
// 	
// 	const bool operator==(const element_t& obj) const;
// 	const bool operator!=(const element_t& obj) const;
// 	const bool operator<(const element_t& obj) const;
// };



	
// class isotope_t : element_t
// {
// private:
//     
// public:
// 	///e.g. "30Si2" or "Si2"==28Si2
// 	isotope_t(string isotope_str);
// 	isotope_t();
// 	///"30" in 30Si2
// 	int nucleons() const;
// 	string to_string() const;
// 	bool operator==(const isotope_t& obj) const;
// 	bool operator!=(const isotope_t& obj) const;
// 	bool operator<(const isotope_t& obj) const;
// };




#endif // ELEMENT_T_HPP
