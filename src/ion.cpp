/*
	Copyright (C) 2021Florian Bärwolf
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


#include "ion.hpp"

/***************************/
/*******     ion_t     ********/
/***************************/
// ion_t::ion_t()
// {
// }

ion_t::ion_t(vector<element_t> elements_s,  electrical_charge_t electric_charge_s) : elements(elements_s), electric_charge(electric_charge_s)
{
}

ion_t::ion_t(element_t element_s,  electrical_charge_t electric_charge_s) : elements({element_s}), electric_charge(electric_charge_s)
{
}

bool ion_t::operator==(const ion_t& obj) const
{
	if (electric_charge!=obj.electric_charge) return false;
	bool same_elements=false;
	for (auto ele:elements)
	{
		same_elements = false;
		for (auto objele:obj.elements)
		{
			if (ele == objele)
			{
				same_elements=true;
				break;
			}
		}
		if (!same_elements) return false;
	}
	return true;
}

bool ion_t::operator!=(const ion_t& obj) const
{
	return !operator==(obj);
}


bool ion_t::is_set() const
{
	if (elements.size()==0) return false;
	if (!electric_charge.is_set()) return false;
	return true;
}

// vector<element_t>& ion_t::elements()
// {
// 	return elements_p;
// }



std::__cxx11::string ion_t::to_string(const std::__cxx11::string del) const
{
	if (!is_set()) return "";
	stringstream out;
	out << "(" ;
	const int size=elements.size(); 
	for (int i=0;i<size;i++)
	{
		out << elements.at(i).to_string();
		if (i<size-1)
			out << del;
	}
	out << ")"<< static_cast<int>(electric_charge.data.at(0));
	if (electric_charge.data.at(0)<0) out << "-";
	else out << "+";
	return out.str();
}
