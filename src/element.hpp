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

#ifndef ELEMENT_T_HPP
#define ELEMENT_T_HPP


#include <sstream>
#include <vector>
#include <string>
#include "log.hpp"
#include "pse.hpp"
#include "quantity.hpp"
#include "definitions.hpp"
#include "isotope.hpp"


class element_t
{
// 	friend class matrix_t;
private:
    class_logger_t logger;
// 	std::vector<isotope_t> isotopes_p;
	void set_elemental_substance_amount();
// 	quantity::substance_amount_t substance_amount_p;
public:
	element_t();
	element_t(std::string symbol_s, double abs_amount=1, bool use_naturale_abundance=true);
	element_t(std::vector<isotope_t> isotopes_s);
	element_t(isotope_t isotope_s);

	std::vector<isotope_t> isotopes;
	quantity::substance_amount_t substance_amount;
	
	const quantity::mass_t mass();
	std::string symbol;
	const std::string to_string() const;
	const int protons();
	///some abosulte value: atoms, mole, particles, ...
	
// 	const isotope_t* isotope_with_highest_abundance() const;
// 	const isotope_t* isotope_from_nucleons(int nucleons) const;
	///checking isotopes but NOT their abundances
	bool operator==(const element_t& obj) const;
	bool operator!=(const element_t& obj) const;
	bool operator<(const element_t& obj) const;
	bool is_set();
};
extern Logger global_logger;

#endif // ELEMENT_T_HPP
