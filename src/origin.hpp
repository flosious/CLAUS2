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

#ifndef ORIGIN_T_HPP
#define ORIGIN_T_HPP

#include <vector>
#include <string>
#include <regex>
#include "tools.hpp"
#include "quantity.hpp"
#include "cluster.hpp"

using namespace std;



class origin_t
{
private:
public:
	class column_t
	{
	public:
		string longname="";
		string unit="";
		string comment="";
		vector<string> data;
		vector<string> vec();
		column_t(const vector<double>& data_s, string longname="",string unit="", string comment="");
		column_t(const quantity_t& quantity, string prefix="", string suffix="");
		/// superscript "^30Si" --> "\+(30)Si"
		/// subscript "Si_2" --> "Si\-(2)"
		void apply_origin_conform_replacements(string& in_here);
	};
	origin_t(column_t cols);
	origin_t(vector<cluster_t>& clusters);
	string to_string();
};

#endif // ORIGIN_T_HPP
