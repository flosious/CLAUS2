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

#ifndef CALC_HPP
#define CALC_HPP

#include "mgroup.hpp"

class calc
{
public:
	concentration_pbp(mgroups::mgroup_t& MG);
	concentration_mean(mgroups::mgroup_t& MG);
	concentration_quantil(mgroups::mgroup_t& MG);
	depth(mgroups::mgroup_t& MG);
	sputter_rate(mgroups::mgroup_t& MG);
	RSFs(mgroups::mgroup_t& MG);
};

#endif // CALC_HPP
