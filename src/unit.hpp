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

#include <math.h>
#include <vector>
#include <string>
#include <sstream>
#include <map>

using namespace std;

/// just supporting the SI system at the moment
class unit_t
{
private:
// 	const string name_p;
	///the "^m" in cm^2 or at in at%
	string base_p;
	///the "c" in cm^2
// 	string prefix_p;
	///the "%" in at%
	string suffix_p;
	///the "2" in cm^2
// 	int exponent_p;
	class prefix_t 
	{
	private:
		///minimizes the prefix, appying a prefix unit
		int minimize_prefix_exponent(int factor) ;
		string name_p="";
	public:
		bool operator==(const prefix_t& obj) const;
		bool operator!=(const prefix_t& obj) const;
		double operator /(const prefix_t& obj) ;
		double operator *(const prefix_t& obj) ;
		static const map<string,int> prefix_to_exponent;
		string name() const;
		prefix_t(string name_s);
	} prefix_p;
public:
	
	string base() const;
	string prefix() const;
	string suffix() const;
// 	int exponent() const;
	
// 	unit_t(string name_s="");
	unit_t(string prefix_s, string base_s, string suffix_s="");
	string name() const;
	
	bool operator==(const unit_t& obj) const;
	bool operator!=(const unit_t& obj) const;
	
	
};


#endif // UNIT_HPP
