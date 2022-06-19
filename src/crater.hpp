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
		fit_functions::asym2sig_t asym2sig;
		bool fitted=false;
	public:
		linescan_t();
		linescan_t(quantity::quantity_t xy,quantity::quantity_t z);
		const fit_functions::asym2sig_t& fit_params() const;
		/// the depth or height (z)
		quantity::quantity_t z;
		/// the surface (xy)
		quantity::quantity_t xy;
		/// calculates the depth from profile and returns it mean
		quantity::quantity_t depth();
		/// calculates roughness
		quantity::quantity_t roughness();
		/// origin -> peak-functions -> Asym2Sig
		quantity::quantity_t fit();
		string to_string(string prefix="");
		bool is_set() const;
		void plot_now(double sleep_sec);
	};
	class sputter_beam_t
	{
	public:
		quantity::current_t sputter_current;
		quantity::sputter_time_t sputter_time;
		quantity::sputter_depth_t sputter_depth;
		sputter_beam_t(quantity::current_t sputter_current_s={}, quantity::sputter_time_t sputter_time={}, quantity::sputter_depth_t sputter_depth_s={});
		const string to_string(const string del=", ") const;
	};
/***********/	
	friend class processor;
private:
	quantity::sputter_time_t total_sputter_time_s;
	///calculates common X points from all clusters and sets sputter_depth_s and/or sputter_time
	quantity::sputter_depth_t total_sputter_depths;
	vector<linescan_t> linescans;
	///called by common_sputter_depth or common_sputter_time
	quantity::quantity_t common_X_quantity(const vector<quantity::quantity_t>& X_quantities);
	
public:
	const string to_string(const string del=", ") const;
	crater_t();
	crater_t(const quantity::sputter_depth_t& sputter_depth);
	crater_t(const quantity::sputter_time_t& sputter_time);
	crater_t(const quantity::sputter_time_t& sputter_time, const quantity::sputter_depth_t& sputter_depth);
	
	///returns sputter_depth if set, or sputter_time, or nullptr, if nothing is set
	const quantity::quantity_t* X() const;
	sputter_beam_t sputter_beam;
	quantity::sputter_depth_t sputter_depth;
	quantity::sputter_time_t sputter_time;
	///resolution is 1s
	quantity::sputter_time_t unix_sputter_time(string date_time_start);
	quantity::current_t& sputter_current();
	quantity::sputter_time_t& total_sputter_time(vector<cluster_t>* clusters=nullptr);
	quantity::sputter_depth_t total_sputter_depth();
	quantity::SR_t SR;
	
	crater_t change_resolution(quantity::sputter_time_t sputter_time_res);
	crater_t change_resolution(quantity::sputter_depth_t sputter_depth_res);
	
	///finds a common minimal sputter_depth
	quantity::sputter_depth_t common_sputter_depth(vector<cluster_t>& clusters);
	///finds a common minimal sputter_time
	quantity::sputter_time_t common_sputter_time(vector<cluster_t>& clusters);
};


#endif // CRATER_T_HPP
