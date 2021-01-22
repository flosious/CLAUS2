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

#ifndef MEASUREMENT_SETTINGS_T_HPP
#define MEASUREMENT_SETTINGS_T_HPP

#include <vector>
#include <string>
#include "quantity.hpp"
#include "element.hpp"
// #include "file.hpp"

using namespace std;

namespace settings
{
	
/*
 * parent class
 */
class measurement_settings_t
{
private:
public:
};


/*
 * parent class
 */
class sims_t : measurement_settings_t
{
protected:
//     sputter_energy_t sputter_energy_p;
// 	sputter_rastersize_t sputter_raster_size_p;
//     element_t sputter_element_p;
public:
    const sputter_energy_t sputter_energy() const;
	const sputter_rastersize_t sputter_raster_size() const;
    const element_t sputter_element() const;
	sims_t(sputter_energy_t sputter_energy_s, sputter_rastersize_t sputter_raster_size_s, element_t sputter_element_s);
};




class dsims_t : public sims_t
{
private:
    
public:
	dsims_t(sputter_energy_t sputter_energy_s, sputter_rastersize_t sputter_raster_size_s, element_t sputter_element_s);
};

class tofsims_t : public sims_t
{
private:
//     analysis_energy_t analysis_energy_p;
// 	analysis_rastersize_t analysis_raster_size_p;
//     element_t analysis_element_p;
public:
    const analysis_energy_t analysis_energy() const;
	const analysis_rastersize_t analysis_raster_size() const;
    const element_t analysis_element() const;
	tofsims_t(sputter_energy_t sputter_energy_s, sputter_rastersize_t sputter_raster_size_s, element_t sputter_element_s, 
			  sputter_energy_t analysis_energy_s, sputter_rastersize_t analysis_rastersize_s, element_t analysis_element_s);
};
}
#endif // MEASUREMENT_SETTINGS_T_HPP
