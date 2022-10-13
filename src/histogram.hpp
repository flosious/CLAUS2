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

#ifndef HISTOGRAM_HPP
#define HISTOGRAM_HPP

#include "quantity.hpp"
#include <vector>

class histogram_builder_t;

class histogram_t
{
public:
	class bin_t
	{
	private:
        class_logger_t logger;
		///quantities (vector) within the bin within the bin
		quantity::quantity_t quantities_in_bin_p;
		///range start of this bin
		quantity::quantity_t start_p;
		///range stop of this bin
		quantity::quantity_t stop_p;
	public:
		bin_t(const quantity::quantity_t& start, const quantity::quantity_t& stop);
		///range start of this bin
		const quantity::quantity_t& start() const;
		///range stop of this bin
		const quantity::quantity_t& stop() const;
		///quantities (vector) within the bin within the bin
		const quantity::quantity_t& quantities_in_bin() const;
		///returns 1 if added, 0 if not added
		bool operator<<(const double d);
		///returns the number of successfully added values in q
		int operator<<(const quantity::quantity_t& q);
	}; // bin_t
private:
    class_logger_t logger;
	///the actual histogram
	std::vector<bin_t> bins_p;
public:
	histogram_t(const std::vector<bin_t>& bins);
	unsigned int quantities_in_bins() const;
// 	histogram_t(const quantity::quantity_t& Qs);
	///tries to add a new quantity to the correct bin;
	histogram_t& add(const quantity::quantity_t& q);
	///tries to add a new quantity to the correct bin
	histogram_t& operator<<(const quantity::quantity_t& q);
	///the actual histogram
	const std::vector<bin_t> bins() const;
	std::string to_string() const;
	///range start
// 	const quantity::quantity_t& start() const;
	///range stop
// 	const quantity::quantity_t& stop() const;
	/// e.g. {155,bin[2]}, {10,bin[0]}, {5, bin[1]}
	const std::map<unsigned int, bin_t> quantity_data_size_to_its_bin() const;
	const histogram_builder_t rebuild() const;
	const histogram_builder_t rebuild(const bin_t& bin) const;
	const histogram_builder_t rebuild(const std::vector<bin_t>& bins) const;
};
	
class histogram_builder_t
{
private:
    class_logger_t logger;
	const quantity::quantity_t qs;
// 	const quantity::quantity_t start;
// 	const quantity::quantity_t stop;
public:
// 	histogram_builder_t(const quantity::quantity_t& start, const quantity::quantity_t& stop);
	///will generate start, stop from min and max values of qs
	histogram_builder_t(quantity::quantity_t qs);
	histogram_t equally_linearly_distanced_bins(unsigned int number_of_bins) const;
	histogram_t equally_log10_distanced_bins(unsigned int number_of_bins) const;
	/// bin_pos[i] = min() + q_summand*i
	histogram_t equally_summand_distanced_bins(quantity::quantity_t q_summand) const;
	/// bin_pos[i] = min() * q_factor^i
	histogram_t equally_factor_distanced_bins(quantity::quantity_t q_factor) const;
}; //histogram_t

#endif // QUANTITY_HPP
