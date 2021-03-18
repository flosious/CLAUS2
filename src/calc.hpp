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

#include <vector>
#include <string>
#include <map>

#include "measurement.hpp"
#include "quantity.hpp"
#include "isotope.hpp"
#include "cluster.hpp"

///calculation methods
class calc
{
public:
	///jiangs protocoll
	class jiang_t
	{
	public:
		jiang_t(vector<measurements_::sims_t>& Ms);
		///maps isotope -> depth profile (.first->depth, .second -> concentration)
		map<isotope_t,pair<sputter_depth_t,concentration_t>> depth_profiles(measurements_::sims_t& M);
		pair<sputter_depth_t,concentration_t> depth_profile(measurements_::sims_t& M, isotope_t& isotope);
		pair<sputter_depth_t,concentration_t> depth_profile(measurements_::sims_t& M, cluster_t& cluster);
	};
	///point by point calculation
	class pbp_t
	{
	public:
		pbp_t(vector<measurements_::sims_t>& Ms);
	};
	///constant reference calculation
	class const_ref_t
	{
	public:
		const_ref_t(vector<measurements_::sims_t>& Ms);
	};
	class test_t
	{
	public:
		test_t(vector<measurements_::sims_t>& Ms);
	};
};

#endif // CALC_HPP
