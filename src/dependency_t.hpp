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

#ifndef DEPENDENCY_T_HPP
#define DEPENDENCY_T_HPP

#include "quantity.hpp"

using namespace std;

class dependency_t
{
private:
	quantity_t X_p;
	quantity_t Y_p;
public:
	dependency_t(const quantity_t& X_s, const quantity_t& Y_s);
	///maps the data vecs to < X, *this>
	const map<double,double> data() const;

};

#endif // DEPENDENCY_T_HPP
