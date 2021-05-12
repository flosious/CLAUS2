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

#ifndef QUANTITY_HPP
#define QUANTITY_HPP


#include <math.h>
#include <iostream>
#include <vector>
#include <string>
#include "unit.hpp"
#include "print.hpp"
#include "log.hpp"
#include "statistics.hpp"
#include "fit_functions.hpp"
#include "tools.hpp"

using namespace std;


/*
* this is a template class
*/
class quantity_t
{
// 	friend class matrix_t;
protected:
	string name_p="";
	unit_t unit_p{};
// 	double resolution_p=-1;
// 	vector<double> data_p;
public:
// 	///maps the data vecs to < X, *this>
// 	const map<double,double> to_X(const quantity_t& X) const;
// 	///maps the data vecs to < *this, Y>
// 	const map<double,double> to_Y(const quantity_t& Y) const;
	vector<double> data;
// 	const vector<double> data() const;
	bool is_set() const;
	quantity_t();
	quantity_t(const quantity_t& quant_s,const unit_t& unit_s);
	quantity_t(string name_s,unit_t unit_s);
	quantity_t(vector<double> data_s,unit_t unit_s);
	quantity_t(double data_s,unit_t unit_s);
	quantity_t(string name_s,vector<double> data_s,unit_t unit_s);
	///tries to change the current unit to its target unit
	quantity_t change_unit(unit_t target_unit) const;
	/// resolution <= 0 will use raw data, no interpolation
// 	void change_resolution(double resolution_s);
// 	void reset_resolution();
	///0 returns the current resolution, otherwise returns quantity with new resolution
	quantity_t resolution(double new_res=0) const;
	///returns quantity with new resolution, performs unit transformation if neccessary
	quantity_t resolution(quantity_t new_res) const;
	unit_t unit() const;
	const string to_string() const;
	const string name() const;
	
	
	bool operator>(const quantity_t& obj) const;
	bool operator<(const quantity_t& obj) const;
	bool operator==(const quantity_t& obj) const;
	bool operator!=(const quantity_t& obj) const;
	///appends obj.data to data vector
	void operator<<(quantity_t obj);
	quantity_t operator+(const quantity_t& quantity_p) const;
	quantity_t operator+(const double summand) const;
	void operator+=(const quantity_t& quantity_p);
	void operator+=(const double summand);
	quantity_t operator-(const quantity_t& quantity_p) const;
	quantity_t operator-(const double summand) const;
	quantity_t operator*(const quantity_t& quantity_p) const;
	quantity_t operator*(const double factor) const;
	quantity_t operator/(const quantity_t& quantity_p) const;
	quantity_t operator/(const double divisor) const;
	
	/*statistics*/
	
	// moving windows
	quantity_t moving_window_median(int window_size=0) const;
	quantity_t moving_window_mad(int window_size=0) const;
	quantity_t moving_window_mean(int window_size=0) const;
	quantity_t moving_window_sd(int window_size=0) const;
	
	
	//skalars
	///
	quantity_t absolute() const;
	quantity_t invert() const;
	quantity_t sum() const;
	quantity_t quantile(double percentile=0.75) const;
	quantity_t median() const;
	quantity_t mean() const;
	quantity_t geo_mean() const;
	quantity_t trimmed_mean(float alpha=0.25) const;
	quantity_t gastwirth() const;
	quantity_t sd() const;
	quantity_t mad() const;
	quantity_t max() const;
	/// returns x value at max(Y)
	quantity_t max_at_x(quantity_t& X,double lower_X_limit, double upper_X_limit) const;
	quantity_t max_at_x(quantity_t& X) const;
	
	quantity_t min() const;
	/// returns x value at min(Y)
	quantity_t min_at_x(quantity_t& X,double lower_X_limit=0, double upper_X_limit=0) const;
// 	quantity_t integrate(quantity_t& x_data,double lower_X_limit=0, double upper_X_limit=0);
	quantity_t integrate(quantity_t& x_data, double lower_X_limit = 0, double upper_X_limit = 0) const;
	/// point by point integration
	quantity_t integrate_pbp(const quantity_t& x_data) const;
	
	quantity_t round_() const;
	quantity_t diff(bool preserve_size=true) const;
	
	quantity_t interp(const quantity_t& old_X, const quantity_t& new_X) const;
	quantity_t fit_polynom_by_x_data(quantity_t& x_data, quantity_t new_x_data, int polynom_grade=-1 ) const;
	quantity_t polyfit(int polynom_grade=-1) const;
	
	
	quantity_t filter_recursive_median(int window_size=0) const;
	quantity_t filter_impulse(int window_size=0, float factor=4) const;
	quantity_t filter_gaussian(int window_size=0, double alpha=1) const;
	
	///clears all data in the cluster
	void clear();
};

	class mass_t : public quantity_t
	{
	public:;
		mass_t(vector<double> data_s={},unit_t unit_s=units::derived::amu); // atomic mass units
		mass_t(quantity_t quantity_s);
	}; 
	class abundance_t : public quantity_t
	{
	public:;
		abundance_t(vector<double> data_s={},unit_t unit_s={});
		abundance_t(quantity_t quantity_s);
	}; 
	
	class energy_t : public quantity_t
	{
	public:;
		energy_t(vector<double> data_s={},unit_t unit_s=units::derived::electron_volt);
		energy_t(quantity_t quantity_s);
	}; 
	
	class secondary_voltage_t : public quantity_t
	{
	public:;
		secondary_voltage_t(vector<double> data_s={},unit_t unit_s=units::derived::volt);
		secondary_voltage_t(quantity_t quantity_s);
	}; 
	
	class rastersize_t : public quantity_t
	{
	public:
		rastersize_t(vector<double> data_s={},unit_t unit_s=units::SI::meter*units::prefixes::micro);
		rastersize_t(quantity_t quantity_s);
	};
	
	class depth_t : public quantity_t
	{
	public:
		depth_t(vector<double> data_s={},unit_t unit_s=units::SI::meter*units::prefixes::nano);
		depth_t(quantity_t quantity_s);
	};
	class sputter_depth_t : public quantity_t
	{
	public:
		sputter_depth_t(vector<double> data_s={},unit_t unit_s=units::SI::meter*units::prefixes::nano);
		sputter_depth_t(quantity_t quantity_s);
	};
	class total_sputter_depth_t : public quantity_t
	{
	public:
		total_sputter_depth_t(vector<double> data_s={},unit_t unit_s=units::SI::meter*units::prefixes::nano);
		total_sputter_depth_t(quantity_t quantity_s);
	};
	
	class sputter_time_t : public quantity_t
	{
	public:
		sputter_time_t(vector<double> data_s={},unit_t unit_s=units::SI::second);
		sputter_time_t(quantity_t quantity_s);
	};
	
	class total_sputter_time_t : public quantity_t
	{
	public:
		total_sputter_time_t(vector<double> data_s={},unit_t unit_s=units::SI::second);
		total_sputter_time_t(quantity_t quantity_s);
	};
	
// 	class analysis_energy_t : public quantity_t
// 	{
// 	public:
// 		analysis_energy_t(vector<double> data_s={},unit_t unit_s={"eV"});
// 		analysis_energy_t(quantity_t quantity_s);
// 	};
	
	class concentration_t : public quantity_t
	{
	public:
		concentration_t(vector<double> data_s={},unit_t unit_s=units::derived::atoms/units::SI::meter.pow(3)/units::prefixes::centi.pow(3));
		concentration_t(quantity_t quantity_s);
	};
	
	class intensity_t : public quantity_t
	{
	public:
		intensity_t(vector<double> data_s={},unit_t unit_s=units::derived::counts / units::SI::second);
		intensity_t(quantity_t quantity_s);
	};
	
	class current_t : public quantity_t
	{
	public:
		current_t(vector<double> data_s={},unit_t unit_s=units::prefixes::nano*units::SI::Ampere);
		current_t(quantity_t quantity_s);
	};
	
	class sputter_current_t : public quantity_t
	{
	public:
		sputter_current_t(vector<double> data_s={},unit_t unit_s=units::prefixes::nano*units::SI::Ampere);
		sputter_current_t(quantity_t quantity_s);
	};
	
	class analysis_current_t : public quantity_t
	{
	public:
		analysis_current_t(vector<double> data_s={},unit_t unit_s=units::prefixes::nano*units::SI::Ampere);
		analysis_current_t(quantity_t quantity_s);
	};
	
	class dose_t : public quantity_t
	{
	public:;
		dose_t(vector<double> data_s={},unit_t unit_s=units::derived::atoms/units::SI::meter.pow(2)/units::prefixes::centi.pow(2));
		dose_t(quantity_t quantity_s);
	}; 
	
	class SF_t : public quantity_t
	{
	public:;
		SF_t(vector<double> data_s={},unit_t unit_s=units::derived::atoms/units::SI::meter.pow(3)/units::prefixes::centi.pow(3) / (units::derived::counts/units::SI::second) );
		SF_t(quantity_t quantity_s);
	}; 
	
	class RSF_t : public quantity_t
	{
	public:;
		RSF_t(vector<double> data_s={},unit_t unit_s=units::derived::atoms/units::SI::meter.pow(3)/units::prefixes::centi.pow(3));
		RSF_t(quantity_t quantity_s);
	}; 
	
	class SR_t : public quantity_t
	{
	public:;
		SR_t(vector<double> data_s={},unit_t unit_s=units::SI::meter*units::prefixes::nano / units::SI::second);
		SR_t(quantity_t quantity_s);
	}; 
	
// 	class analysis_rastersize_t : public quantity_t
// 	{
// 	public:
// 		analysis_rastersize_t(vector<double> data_s={},unit_t unit_s={"m"});
// 		analysis_rastersize_t(quantity_t quantity_s);
// 	}; 
	
	class substance_amount_t : public quantity_t
	{
	public:
		substance_amount_t(vector<double> data_s={},unit_t unit_s=units::SI::mol);
		substance_amount_t(quantity_t quantity_s);
// 		const substance_amount_t round_to_full_integer_data() const;
	}; 
	
	class electrical_charge_t : public quantity_t
	{
	public:
		electrical_charge_t(vector<double> data_s={},unit_t unit_s={units::derived::coulomb,-1});
		electrical_charge_t(quantity_t quantity_s);
	};

#endif // QUANTITY_HPP
