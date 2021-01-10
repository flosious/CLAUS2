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

#ifndef QUANTITY_HPP
#define QUANTITY_HPP

#include "tools.hpp"
#include <iostream>
#include <vector>
#include <string>
#include "unit.hpp"
#include "print.hpp"

using namespace std;


/*
* this is a template class
*/
class quantity_t
{
protected:
	string name_p="";
	unit_t unit_p{""};
	
public:
	bool is_set() const;
	quantity_t();
	quantity_t(string name_s,unit_t unit_s);
	quantity_t(string name_s,unit_t unit_s,vector<double> data_s);
	///tries to change the current unit to its target unit
	quantity_t change_unit(unit_t target_unit) const;
	unit_t unit() const;
	
	string name() const;

	vector<double> data;

	quantity_t mean() const;
	quantity_t median() const;

	bool operator==(const quantity_t& obj) const;
	bool operator!=(const quantity_t& obj) const;
	///appends obj.data to data vector
	void operator<<(const quantity_t& obj);
	quantity_t operator+(const quantity_t& quantity_p) const;
	quantity_t operator+(const double summand) const;
	quantity_t operator-(const quantity_t& quantity_p) const;
	quantity_t operator-(const double summand) const;
	quantity_t operator*(const quantity_t& quantity_p) const;
	quantity_t operator*(const double factor) const;
	quantity_t operator/(const quantity_t& quantity_p) const;
	quantity_t operator/(const double divisor) const;
	
// 	static quantity_t sputter_energy();
// 	static quantity_t sputter_rastersize();
// 	static quantity_t sputter_depth();
// 	static quantity_t total_sputter_depth();
// 	static quantity_t sputter_time();
// 	static quantity_t total_sputter_time();
// 	
// 	static quantity_t analysis_energy();
// 	static quantity_t analysis_rastersize();
// 	static quantity_t concentration();
// 	static quantity_t intensity();
// 	static quantity_t dose();
};

	class sputter_energy_t : public quantity_t
	{
	public:;
		sputter_energy_t(unit_t unit_s={"eV"},vector<double> data_s={});
		sputter_energy_t(quantity_t quantity_s);
	}; 
	
	class sputter_rastersize_t : public quantity_t
	{
	public:
		sputter_rastersize_t(unit_t unit_s={"um"},vector<double> data_s={});
		sputter_rastersize_t(quantity_t quantity_s);
	};
	
	class sputter_depth_t : public quantity_t
	{
	public:
		sputter_depth_t(unit_t unit_s={"nm"},vector<double> data_s={});
		sputter_depth_t(quantity_t quantity_s);
	};
	class total_sputter_depth_t : public quantity_t
	{
	public:
		total_sputter_depth_t(unit_t unit_s={"nm"},vector<double> data_s={});
		total_sputter_depth_t(quantity_t quantity_s);
	};
	
	class sputter_time_t : public quantity_t
	{
	public:
		sputter_time_t(unit_t unit_s={"s"},vector<double> data_s={});
		sputter_time_t(quantity_t quantity_s);
	};
	
	class total_sputter_time_t : public quantity_t
	{
	public:
		total_sputter_time_t(unit_t unit_s={"s"},vector<double> data_s={});
		total_sputter_time_t(quantity_t quantity_s);
	};
	
	class analysis_energy_t : public quantity_t
	{
	public:
		analysis_energy_t(unit_t unit_s={"eV"},vector<double> data_s={});
		analysis_energy_t(quantity_t quantity_s);
	};
	
	class analysis_rastersize_t : public quantity_t
	{
	public:
		analysis_rastersize_t(unit_t unit_s={"m"},vector<double> data_s={});
		analysis_rastersize_t(quantity_t quantity_s);
	};
	
	class concentration_t : public quantity_t
	{
	public:
		concentration_t(unit_t unit_s={"at/cm^3"},vector<double> data_s={});
		concentration_t(quantity_t quantity_s);
	};
	
	class intensity_t : public quantity_t
	{
	public:
		intensity_t(unit_t unit_s={"c/s"},vector<double> data_s={});
		intensity_t(quantity_t quantity_s);
	};
	
	class dose_t : public quantity_t
	{
	public:;
		dose_t(unit_t unit_s={"at/cm^2"},vector<double> data_s={});
		dose_t(quantity_t quantity_s);
	}; 
	
	class SF_t : public quantity_t
	{
	public:;
		SF_t(unit_t unit_s={"at/cm^3 / (c/s)"},vector<double> data_s={});
		SF_t(quantity_t quantity_s);
	}; 
	
	class RSF_t : public quantity_t
	{
	public:;
		RSF_t(unit_t unit_s={"at/cm^3"},vector<double> data_s={});
		RSF_t(quantity_t quantity_s);
	}; 
	
	class SR_t : public quantity_t
	{
	public:;
		SR_t(unit_t unit_s={"nm/s"},vector<double> data_s={});
		SR_t(quantity_t quantity_s);
	}; 
		

#endif // QUANTITY_HPP
