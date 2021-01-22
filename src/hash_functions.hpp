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

#ifndef HASH_FUNCTIONS_HPP
#define HASH_FUNCTIONS_HPP

#include "measurement_group.hpp"
#include "measurement.hpp"
#include "sample.hpp"
#include "quantity.hpp"
#include "element.hpp"

/*hash functions*/
namespace std
{
// 	template<> struct hash<unit_t>
// 	{
// 		size_t operator()(const unit_t & obj) const
// 		{
// 			size_t sum_hash = 0;
// 			sum_hash ^= (hash<string>()(obj.name()));
// 			return sum_hash;
// 		}
// 	};
// 	
// 	template<> struct hash<quantity_t>
// 	{
// 		size_t operator()(const quantity_t & obj) const
// 		{
// 			size_t sum_hash = 0;
// 			for (auto& d:obj.data)
// 				sum_hash ^= (hash<double>()(d));
// 			sum_hash ^= (hash<unit_t>()(obj.unit()));
// 			return sum_hash;
// 		}
// 	};
// 	
// 	template<> struct hash<element_t>
// 	{
// 		size_t operator()(const element_t & obj) const
// 		{
// 			size_t sum_hash = 0;
// 			sum_hash ^= (hash<string>()(obj.symbol()));
// 			return sum_hash;
// 		}
// 	};
// 	
// // 	template<> struct hash<dsims_settings_t>
// // 	{
// // 		size_t operator()(const dsims_settings_t & obj) const
// // 		{
// // 			size_t sum_hash = 0;
// // 			sum_hash ^= (hash<quantity_t>()(obj.sputter_energy()));
// // 			sum_hash ^= (hash<quantity_t>()(obj.sputter_raster_size()));
// // 			sum_hash ^= (hash<element_t>()(obj.sputter_element()));
// // 			return sum_hash;
// // 		}
// // 	};
// 	
// 	template<> struct hash<measurement_groups::dsims_t>
// 	{
// 		size_t operator()(const measurement_groups::dsims_t & obj) const
// 		{
// 			size_t sum_hash = 0;
// 			sum_hash ^= (hash<int>()(obj.id()));
// 			sum_hash ^= (hash<string>()(obj.measurement_tool_name()));
// 			return sum_hash;
// 		}
// 	};
// 	
// 	template<> struct hash<measurements::dsims_t>
// 	{
// 		size_t operator()(const measurements::dsims_t & obj) const
// 		{
// 			size_t sum_hash = 0;
// 			sum_hash ^= (hash<int>()(obj.olcdb()));
// 			sum_hash ^= (hash<string>()(obj.repetition()));
// 			return sum_hash;
// 		}
// 	};
// 	
// 	template<> struct hash<sample_t::chip_t>
// 	{
// 		size_t operator()(const sample_t::chip_t & obj) const
// 		{
// 			return (hash<int>()(obj.x())) ^ (hash<int>()(obj.y()));
// 		}
// 	};
// 	
// 	template<> struct hash<sample_t>
// 	{
// 		size_t operator()(const sample_t & obj) const
// 		{
// 			size_t sum_hash = 0;
// 			sum_hash ^= (hash<string>()(obj.lot_split()));
// 			sum_hash ^= (hash<int>()(obj.wafer_number()));
// 			sum_hash ^= (hash<string>()(obj.lot()));
// 			sum_hash ^= (hash<string>()(obj.simple_name()));
// 			sum_hash ^= (hash<sample_t::chip_t>()(obj.chip));
// 			return sum_hash;
// 		}
// 	};
}

#endif // HASH_FUNCTIONS_HPP


