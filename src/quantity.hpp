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


using namespace std;


/*
* this is a template class
*/
class quantity_t
{
protected:
	string name_p="unknown";
	unit_t unit_p{"","unkown"};
	
public:
	quantity_t();
	quantity_t(string name_s,unit_t unit_s);
	quantity_t(string name_s,vector<double> data_s ,unit_t unit_s);
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
	quantity_t operator+(const quantity_t& quantity_p);
	quantity_t operator*(const quantity_t& quantity_p);
	quantity_t operator*(const double factor);
	quantity_t operator/(const quantity_t& quantity_p);
	quantity_t operator/(const double divisor);
	
	static quantity_t sputter_energy();
	static quantity_t sputter_rastersize();
	static quantity_t sputter_depth();
	static quantity_t total_sputter_depth();
	static quantity_t sputter_time();
	static quantity_t total_sputter_time();
	
	static quantity_t analysis_energy();
	static quantity_t analysis_rastersize();
	static quantity_t concentration();
	static quantity_t intensity();
	static quantity_t dose();
};

	class sputter_energy_t : public quantity_t
	{
	public:;
		sputter_energy_t(unit_t unit_s={"","eV"});
		sputter_energy_t(quantity_t quantity_s);
	}; 
	
	class sputter_rastersize_t : public quantity_t
	{
	public:
		sputter_rastersize_t(unit_t unit_s={"u","m"});
	};
	
	class sputter_depth_t : public quantity_t
	{
	public:
		sputter_depth_t(unit_t unit_s={"n","m"});
	};
	class total_sputter_depth_t : public quantity_t
	{
	public:
		total_sputter_depth_t(unit_t unit_s={"n","m"});
	};
	
	class sputter_time_t : public quantity_t
	{
	public:
		sputter_time_t(unit_t unit_s={"","s"});
	};
	
	class total_sputter_time_t : public quantity_t
	{
	public:
		total_sputter_time_t(unit_t unit_s={"","s"});
	};
	
	class analysis_energy_t : public quantity_t
	{
	public:
		analysis_energy_t(unit_t unit_s={"k","eV"});
	};
	
	class analysis_rastersize_t : public quantity_t
	{
	public:
		analysis_rastersize_t(unit_t unit_s={"u","m"});
	};
	
	class concentration_t : public quantity_t
	{
	public:
		concentration_t(unit_t unit_s={"","at/cm^3"});
	};
	
	class intensity_t : public quantity_t
	{
	public:
		intensity_t(unit_t unit_s={"","c/s"});
	};
	

#endif // QUANTITY_HPP
