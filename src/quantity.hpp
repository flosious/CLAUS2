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

///physical quantities
namespace quantity
{
	///generic physical quantity; TODO implement dimensions
	class quantity_t
	{
	private:
		fit_functions::polynom_t polynom(unsigned int polynom_grade) const; 
		static int get_value_index_position_in_strictly_monotonic_increasing_vector(const double value, const vector<double>& monotonic_vec);
	public:
		unit_t unit_p;
	protected:
		string name_p="";
	public:
		vector<double> data;
		bool is_nan() const;
		bool is_inf() const;
		bool is_set() const;
		bool is_scalar() const;
		bool is_vector() const;
		quantity_t();
		quantity_t(const quantity_t& quant_s,const unit_t& unit_s);
		quantity_t(const quantity_t& quant_s,double data);
		quantity_t(const quantity_t& quant_s,vector<double> data);
		quantity_t(string name_s,unit_t unit_s);
		quantity_t(vector<double> data_s,unit_t unit_s);
		quantity_t(double data_s,unit_t unit_s);
		quantity_t(string name_s,vector<double> data_s,unit_t unit_s);
		///tries to change the current unit to its target unit; returns { } if unsuccessfull
		quantity_t change_unit(unit_t target_unit) const;
		///tries to change the current unit to its target unit; returns { } if unsuccessfull
		quantity_t change_unit(string target_unit_string) const;

		quantity_t resolution(double new_res=0) const;

		quantity_t change_resolution(quantity_t new_res) const;
		quantity_t change_resolution(unsigned int new_data_size) const;
		const unit_t& unit() const;
		const unit_t unit_copy() const;
		const string to_string() const;
		const string to_string_detailed() const;
		const string name() const;
		
		quantity_t log10() const;
		
		///returns the data point on fiven pos
		quantity_t at(unsigned int pos) const;
		
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
		
		quantity_t reverse() const;
		
		// moving windows
		quantity_t moving_window_qqr(int window_size=0, double q=0.25) const;
		quantity_t moving_window_iqr(int window_size=0) const;
		quantity_t moving_window_sum(int window_size=0) const;
		quantity_t moving_window_max(int window_size=0) const;
		quantity_t moving_window_min(int window_size=0) const;
		
		quantity_t moving_window_median(int window_size=0) const;
		quantity_t moving_window_mad(int window_size=0) const;
		quantity_t moving_window_mean(int window_size=0) const;
		quantity_t moving_window_sd(int window_size=0) const;
		
		//smoothing
		quantity_t bspline_smoothing(unsigned int breakpoints = 0, const size_t spline_order = 4) const;
		quantity_t bspline_smoothing(const quantity_t& Xdata, unsigned int breakpoints = 0, const size_t spline_order = 4) const;
		
		//skalars
		///
		quantity_t get_data_by_index(unsigned int start, unsigned int stop) const;
		quantity_t remove_data_by_index(unsigned int start, unsigned int stop) const;
		quantity_t remove_data_from_begin(unsigned int stop) const;
		quantity_t remove_data_from_begin(const quantity_t& remove_stop) const;
		quantity_t absolute() const;
		quantity_t invert() const;
		quantity_t abs_sum() const;
		///sums up all data in the data.vector
		quantity_t sum(int start=0, int stop = -1) const;
		quantity_t quantile(double percentile=0.75) const;
		quantity_t median() const;
		quantity_t mean() const;
		quantity_t geo_mean() const;
		quantity_t trimmed_mean(float alpha=0.25) const;
		quantity_t gastwirth() const;
		quantity_t sd() const;
		quantity_t mad() const;
		quantity_t max() const;
		quantity_t front() const;
		quantity_t back() const;
		/// returns x value at max(Y)
	// 	quantity_t max_at_x(const quantity_t& X,double lower_X_limit, double upper_X_limit) const;
		quantity_t max_at_x(const quantity_t& X) const;
		
		quantity_t min() const;
		/// returns x value at min(Y)
		quantity_t min_at_x(quantity_t& X,double lower_X_limit=0, double upper_X_limit=0) const;
	// 	quantity_t integrate(quantity_t& x_data,double lower_X_limit=0, double upper_X_limit=0);
		quantity_t integrate(const quantity_t& x_data, unsigned int lower_X_index, unsigned int upper_X_index = 0) const;
		quantity_t integrate(const quantity_t& x_data, const quantity_t& x_data_start={}, const quantity_t& x_data_stop={}) const;
		/// point by point integration
		quantity_t integrate_pbp(const quantity_t& x_data) const;
		
		quantity_t round_() const;
		quantity_t diff(bool preserve_size=true) const;
		
		quantity_t interp(const quantity_t& old_X, const quantity_t& new_X) const;
		quantity_t fit_polynom_by_x_data(quantity_t& x_data, quantity_t new_x_data, int polynom_grade=-1 ) const;
		///polynomial interpolation from idx_start to idx_stop
		quantity_t polyfit(unsigned int polynom_grade) const;
		///deviation = 1 -> 1st deviation; 0 no deviation == polyfit
		quantity_t polyfit_derivative(unsigned int polynom_grade, unsigned int derivative=1) const;
		quantity_t extrapolate(unsigned int polynom_grade, const quantity_t& X) const;
		
		
		quantity_t filter_recursive_median(int window_size=0) const;
		quantity_t filter_impulse(int window_size=0, float factor=4) const;
		quantity_t filter_gaussian(int window_size=0, double alpha=1) const;
		
		///clears all data in the cluster
		void clear();
	};

	class mass_t : public quantity_t
	{
	public:
		mass_t(vector<double> data_s={},unit_t unit_s=units::derived::amu); // atomic mass units
		mass_t(quantity_t quantity_s);
	}; 
	class abundance_t : public quantity_t
	{
	public:
		abundance_t(vector<double> data_s={},unit_t unit_s={});
		abundance_t(quantity_t quantity_s);
	}; 
	
	class energy_t : public quantity_t
	{
	public:
		energy_t(vector<double> data_s={},unit_t unit_s=units::derived::electron_volt);
		energy_t(quantity_t quantity_s);
	}; 
	
	class secondary_voltage_t : public quantity_t
	{
	public:
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
	public:
		dose_t(vector<double> data_s={},unit_t unit_s=units::derived::atoms/units::SI::meter.pow(2)/units::prefixes::centi.pow(2));
		dose_t(quantity_t quantity_s);
	}; 
	
	class SF_t : public quantity_t
	{
	public:
		SF_t(vector<double> data_s={},unit_t unit_s=units::derived::atoms/units::SI::meter.pow(3)/units::prefixes::centi.pow(3) / (units::derived::counts/units::SI::second) );
		SF_t(quantity_t quantity_s);
	}; 
	
	class RSF_t : public quantity_t
	{
	public:
		RSF_t(vector<double> data_s={},unit_t unit_s=units::derived::atoms/units::SI::meter.pow(3)/units::prefixes::centi.pow(3));
		RSF_t(quantity_t quantity_s);
	}; 
	
	class SR_t : public quantity_t
	{
	public:
		SR_t(vector<double> data_s={},unit_t unit_s=units::SI::meter*units::prefixes::nano / units::SI::second);
		SR_t(quantity_t quantity_s);
	};  
	
	class substance_amount_t : public quantity_t
	{
	public:
		substance_amount_t(vector<double> data_s={},unit_t unit_s=units::SI::mol);
		substance_amount_t(quantity_t quantity_s);
	}; 
	
	class electrical_charge_t : public quantity_t
	{
	public:
		electrical_charge_t(vector<double> data_s={},unit_t unit_s={units::derived::coulomb,-1});
		electrical_charge_t(quantity_t quantity_s);
	};

	///maps quantity Y to quantity X
	class map_t
	{
	public:
		map_t(const quantity_t& X, const quantity_t& Y);
		///returns new_Y using akima splines
		quantity_t quantity_interp_akima(const quantity_t new_X) const;
		map_t map_interp_akima(const quantity_t new_X) const;
		void plot(double time=0) const;
	protected:
		const quantity_t X;
		const quantity_t Y;
	private:
	};
	
	///maps vector of quantities Ys to same quantity X
	class multi_map_t
	{
	public:
		multi_map_t(const quantity_t& X, const quantity_t& Y);
		multi_map_t(const quantity_t& X, const vector<quantity_t>& Ys);
		///if X does not match (is not regular) then Y will try to interpolate using akima splines
		bool add(const quantity_t& X, const quantity_t& Y);
		///returns new_Y using akima splines
		quantity_t quantity_interp_akima(const quantity_t new_X) const;
		map_t map_interp_akima(const quantity_t new_X) const;
		void plot(double time=0) const;
	protected:
		const quantity_t X;
		const vector<quantity_t> Ys;
	private:
	};
}

#endif // QUANTITY_HPP
