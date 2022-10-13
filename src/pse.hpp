/*
	Copyright (C) 2020 Florian Bärwolf
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
#ifndef PSE_HPP
#define PSE_HPP

#include <string>
#include <map>
#include <vector>
#include "quantity.hpp"
#include "tools.hpp"
#include <regex>
#include "log.hpp"
#include "definitions.hpp"



class pse_t 
{
// 	friend class config_t;
private:
    static class_logger_t class_logger;
    static const std::string source_url;
//    const std::string filename="/home/florian/projects/claus2/build/pse.csv";
    static const std::string filename;
    static const std::string delimiter;;
	
	class pse_isotope_t
	{
    private:
        static class_logger_t class_logger;
	public:
		pse_isotope_t(const double abundance_s, const int nucleons_s, const double mass_s, const std::string symbol_alternative_s="");
		const double abundance;
		const int nucleons;
		const double mass;
		///H hydrogen
// 		const std::string symbol;
		///like D for 2H
        const std::string symbol_alternative;
        bool operator==(const pse_isotope_t& obj) const;
        bool operator<(const pse_isotope_t& obj) const;
	};
	class pse_element_t
	{
    private:
        static class_logger_t class_logger;
	public:
		pse_element_t(std::string symbol_s, int protons_s, std::vector<pse_isotope_t>& isotope_s);
		pse_element_t(std::string symbol_s, int protons_s, pse_isotope_t& isotope_s);
// 		pse_element_t(const std::set<pse_isotope_t>& isotope_s);
        const int protons;
        const std::string symbol;
        const std::vector<pse_isotope_t> isotopes;
// 		pse_element_t(const std::vector<isotope_t>& isotopes_s);
// 		const std::vector<isotope_t> isotopes;
		const pse_isotope_t* isotope_with_highest_abundance() const;
		const pse_isotope_t* isotope_from_nucleons(int nucleons) const;
        double mass() const;
        bool operator==(const pse_element_t& obj) const;
        bool operator<(const pse_element_t& obj) const;
		
	};
// 	std::map<std::string,element_t> symbol_to_element_p;
// 	std::vector<isotope_t> isotopes_p;
	
    static std::vector<pse_element_t> elements_p;
public:
    pse_t() = delete;
// 	pse_t(const std::vector<pse_element_t> elements_s);
    static const std::vector<pse_element_t>& elements();
    static const pse_element_t* element(std::string symbol);
	
    static const std::vector<pse_isotope_t>* isotopes(std::string symbol="");
    static const pse_isotope_t* isotope(std::string symbol, int nucleons);
	
    static bool load_file();
	///all natural(earth) elements within the PSE
	
	///an overview/console dump
    static void to_screen();
	///source URL, which feeded the pse.csv file in the year 2018
    static const std::string source();
};


#endif // PSE_HPP
