/*
	Copyright (C) 2022 Florian Bärwolf
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

#pragma once

#ifndef MODEL_T_HPP
#define MODEL_T_HPP

#include <map>
#include "fit_functions.hpp"
#include "quantity.hpp"

class model_t
{
public:
	class polynomial_t
	{
	private:
		fit_functions::polynom_t X_polynom;
		fit_functions::polynom_t Y_polynom;
		quantity::map_t XY_ref_p;
	public:
		polynomial_t(const fit_functions::polynom_t& X_polynom, const fit_functions::polynom_t& Y_polynom);
		void set_reference(const quantity::map_t& XY_ref_s);
		const quantity::map_t& get_reference() const;
	};
};

#endif // MODEL_T_HPP
