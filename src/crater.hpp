/*
	Copyright (C) 2020 Florian Bärwolf
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

#ifndef CRATER_HPP
#define CRATER_HPP

#include <vector>
#include <string>
#include "quantity.hpp"

using namespace std;

class lineprofile_t
{
	
};

class crater_t
{
private:
	vector<lineprofile_t> lineprofiles;
	sputter_current_t sputter_current_p;
	sputter_time_t sputter_time_p;
	sputter_depth_t sputter_depth_p;
public:
	///multiple crater depths
	sputter_depth_t depths();
	/// sputter_depth to sputter_time vector
	const sputter_depth_t sputter_depth();
	
};


#endif // CRATER_HPP
