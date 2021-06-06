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

#include "measurement.hpp"

measurements_::sims_t::calc_t::calc_t(const sims_t& measurement) : measurement(measurement)
{
}

/** STATICS **/
SR_t measurements_::sims_t::calc_t::sputter_rate(const total_sputter_time_t& total_sputter_time, const total_sputter_depth_t& total_sputter_depths)
{
	if (!total_sputter_time.is_set())
		return {};
	if (total_sputter_time.data.size()!=1)
		return {};
	if (!total_sputter_depths.is_set())
		return {};
	return SR_t(total_sputter_depths.mean() / total_sputter_time);
}

sputter_depth_t measurements_::sims_t::calc_t::sputter_depth(const SR_t& SR, const sputter_time_t& ST)
{
	if (!SR.is_set())
		return {};
	if (!ST.is_set())
		return {};
	return sputter_depth_t(SR * ST); // pbp
}

RSF_t measurements_::sims_t::calc_t::RSF(const SF_t& SF, const intensity_t& reference_intensity)
{
	if (!SF.is_set())
		return {};
	if (!reference_intensity.is_set())
		return {};
	return RSF_t(SF * reference_intensity); //point by point
	
}


/*************************/
/**      implant_t      **/
/*************************/

/*
 * lets keep it stupid simple:
 * 0)	asume the quantity.data are ordered from lowest to highest corresponding X
 * 1)	find the nearest local minimum, beginning from the end (highest X-axis value)
 */
unsigned int measurements_::sims_t::calc_t::implant_t::minimum_starting_position(quantity_t Y)
{
	if (!Y.is_set())
		return 0;
	vector<int> maxIdx, minIdx;
	Y = Y.filter_gaussian(5,4);
	if (!statistics::get_extrema_indices(maxIdx,minIdx,Y.data,(Y.mad()*2).data.at(0)))
	{
		logger::error("measurements_::sims_t::calc_t::implant_t::minimum_starting_position()","statistics::get_extrema_indices","false","returning 0");
		return 0;
	}
	for (auto& min : minIdx)
		cout << "min=" << min << endl;
	for (auto& max : maxIdx)
		cout << "max=" << max << endl;
	return 0;
}


SF_t measurements_::sims_t::calc_t::implant_t::SF(const dose_t& dose, const intensity_t& intensity, const sputter_depth_t& sputter_depth)
{
	if (!dose.is_set())
		return {};
	if (!intensity.is_set())
		return {};
	if (!sputter_depth.is_set())
		return {};
	
	return SF_t( dose / intensity.integrate(sputter_depth, minimum_starting_position(intensity)) );
}
