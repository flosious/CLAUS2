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

#ifndef CALC_T_HPP
#define CALC_T_HPP

#include "measurement.hpp"

using namespace std;


namespace calc
{
	class SR_c
	{
	private:
		const measurements_::sims_t& M;
	public:
		SR_c(const measurements_::sims_t& M);
		bool from_crater_depth();
		bool from_implant_max();
		bool from_database_parameters();
	};
}


#endif // CALC_T_HPP
