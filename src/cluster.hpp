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
// #include "filecontents.hpp"
#include "element.hpp"
// #include "sample.hpp"
// #include "crater.hpp"
#include <mgl2/fltk.h>

using namespace std;



class cluster_t : public mglDraw
{
	friend class crater_t;
private:
	static vector<isotope_t> parse_clustername(const string clustername);
	int equilibrium_starting_position=-1;
// 	map<isotope_t,int> isotopes_amount_p;
// 	vector<isotope_t> isotopes_p;
	concentration_t concentration_p;
	intensity_t intensity_p;
	sputter_time_t sputter_time_p;
	sputter_depth_t sputter_depth_p;

// 	const crater_t* crater();
// 	const sample_t::matrix_t* matrix();
// 	const cluster_t* reference_cluster();
	///put this in the measurement_t class
// 	vector<element_t>* matrix_elements; 

	
public:
	int Draw(mglGraph * gr) override;
	/// 28Si2 Ge2
	cluster_t(string clustername, 
			  sputter_time_t sputter_time={},
			  intensity_t intensity={},
			  sputter_depth_t sputter_depth={},
			  concentration_t concentration={});
	cluster_t(set<isotope_t> isotopes, 
			  sputter_time_t sputter_time={},
			  intensity_t intensity={},
			  sputter_depth_t sputter_depth={},
			  concentration_t concentration={});
	cluster_t();
	cluster_t(const vector<isotope_t>& isotopes_s);
	string name() const;
	string to_string(const string del=" ") const;
	const bool is_set() const;
	///isotopes which define this cluster-
	concentration_t& concentration();
	intensity_t& intensity();
	sputter_time_t& sputter_time();
	sputter_depth_t& sputter_depth();
// 	SR_t SR;
	SF_t SF;
	RSF_t RSF;
	///cahnges resolution of all
// 	cluster_t change_sputter_depth_resolution(sputter_depth_t sputter_depth_res);
// 	cluster_t change_sputter_time_resolution(sputter_time_t sputter_time_res);
// 	cluster_t interpolate(sputter_time_t& new_sputter_time, sputter_time_t& old_sputter_time);
	cluster_t interpolate(quantity_t& new_Q, quantity_t& old_Q) const;
	
	///sputter equilibrium state of this cluster
// 	cluster_t equilibrium();
	// 	const map<isotope_t,int>& isotopes_amount() const;
	vector<isotope_t> isotopes;
	///pointer to the isotope within this cluster, where matrix isotopes have been removed
	const isotope_t* corsseponding_isotope() const; 
	bool operator==(const cluster_t& obj) const;
	bool operator<(const cluster_t& obj) const;
	bool operator!=(const cluster_t& obj) const;
	
	/*TEST*/
// 	calc_t calc(RSF,reference_intensity/ies)
// 	calc_t calc(SF)
//	cluster -> model.concentration
//	cluster -> model.SR
//	cluster -> model.depth
};

#endif // CLUSTER_T_HPP
