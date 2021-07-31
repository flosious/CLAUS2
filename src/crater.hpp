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

#ifndef CRATER_T_HPP
#define CRATER_T_HPP

#include "quantity.hpp"
#include "fit_functions.hpp"
#include "cluster.hpp"

// #include "filename.hpp"
// #include "filecontents.hpp"


/*data storage regarding everything about the sputter crater*/
class crater_t
{
	friend class measurements_;
// 	friend class processor;
/*NESTED CLASSES*/
public:
	class linescan_t
	{
	private:
		vector <double> fit_parameters;
		fit_functions::asym2sig_t asym2sig;
		bool fitted=false;
	public:
		linescan_t();
		linescan_t(quantity_t xy,quantity_t z);
		fit_functions::asym2sig_t fit_params();
		/// the depth or height (z)
		quantity_t z;
		/// the surface (xy)
		quantity_t xy;
		/// calculates the depth from profile and returns it mean
		quantity_t depth();
		/// calculates roughness
		quantity_t roughness();
		/// origin -> peak-functions -> Asym2Sig
		quantity_t fit();
		string to_string(string prefix="");
		bool is_set();
	};
	class sputter_beam_t
	{
	public:
		sputter_current_t sputter_current;
		sputter_time_t sputter_time;
		sputter_depth_t sputter_depth;
		sputter_beam_t(sputter_current_t sputter_current_s={}, sputter_time_t sputter_time={}, sputter_depth_t sputter_depth_s={});
		const string to_string(const string del=", ") const;
	};
	class calc_t
	{
// 		SR_c
// 		ST_c
// 		SD_c
	};
/***********/	
	friend class processor;
private:
// 	sputter_depth_t sputter_depth_s;
// 	sputter_time_t sputter_time;
// 	sputter_current_t sputter_current_s;
	total_sputter_time_t total_sputter_time_s;
	///calculates common X points from all clusters and sets sputter_depth_s and/or sputter_time
	total_sputter_depth_t total_sputter_depths;
	vector<linescan_t> linescans;
	///called by common_sputter_depth or common_sputter_time
	quantity_t common_X_quantity(const vector<quantity_t>& X_quantities);
	
public:
	const string to_string(const string del=", ") const;
	crater_t();
	crater_t(const vector<cluster_t>& clusters, const sputter_beam_t& sputter_beam);
	///returns sputter_depth if set, or sputter_time, or nullptr, if nothing is set
	const quantity_t* X() const;
	sputter_beam_t sputter_beam;
	sputter_depth_t sputter_depth;
	sputter_time_t sputter_time;
	///resolution is 1s
	sputter_time_t unix_sputter_time(string date_time_start);
	sputter_current_t& sputter_current();
	total_sputter_time_t& total_sputter_time(vector<cluster_t>* clusters=nullptr);
	total_sputter_depth_t total_sputter_depth();
	SR_t SR;
	
	crater_t change_resolution(sputter_time_t sputter_time_res);
	crater_t change_resolution(sputter_depth_t sputter_depth_res);
	
	///finds a common minimal sputter_depth
	sputter_depth_t common_sputter_depth(vector<cluster_t>& clusters);
	///finds a common minimal sputter_time
	sputter_time_t common_sputter_time(vector<cluster_t>& clusters);
};


#endif // CRATER_T_HPP
