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


#ifndef CLUSTER_T_HPP
#define CLUSTER_T_HPP

#include <string>

#include <vector>
#include "log.hpp"
#include "quantity.hpp"
#include "regex"
#include <map>
// #include "file.hpp"
#include "element.hpp"
// #include "sample.hpp"
// #include "crater.hpp"

using namespace std;

namespace files
{
	class dsims_dp_rpc_asc_t;
};

class cluster_t
{
	friend class files::dsims_dp_rpc_asc_t;
private:
	int equilibrium_starting_position=-1;
	map<isotope_t,int> isotopes_amount_p;
	concentration_t concentration_p;
	intensity_t intensity_p;
	sputter_time_t sputter_time_p;
	sputter_depth_t sputter_depth_p;
// 	const crater_t* crater();
// 	const sample_t::matrix_t* matrix();
	const cluster_t* reference_cluster();
	///put this in the measurement_t class
// 	vector<element_t>* matrix_elements; 

	
public:
	const map<isotope_t,int>& isotopes_amount() const;
	///pointer to the isotope within this cluster, where matrix isotopes have been removed
	const isotope_t* corsseponding_isotope() const; 
	/// 28Si2 Ge2
	cluster_t(string clustername);
	cluster_t(vector<string> clustername_parts);
// 	cluster_t(string clustername, sputter_time_t sputter_time_s,intensity_t intensity_s,sputter_depth_t sputter_depth_s={},concentration_t concentration_s={});
	cluster_t(vector<isotope_t>& isotopes_s);
	cluster_t(map<isotope_t,int>& isotopes_amount_s);
	
	const string to_string() const;
	const bool is_set() const;
	///isotopes which define this cluster
	const concentration_t* concentration();
	const intensity_t* intensity();
	const sputter_time_t* sputter_time();
	const sputter_depth_t* sputter_depth();
	const SF_t SF();
	///sputter equilibrium state of this cluster
	cluster_t equilibrium();
};

#endif // CLUSTER_T_HPP
