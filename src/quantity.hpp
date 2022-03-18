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
	
	///generic physical quantity;
	class quantity_t
	{
	private:
		///{1,2,3,4,5,...data.size()}
		const vector<double> data_X_1D() const;
		const std::map<double,double> data_XY_1D();
		fit_functions::polynom_t polynom(unsigned int polynom_grade) const; 
		static int get_value_index_position_in_strictly_monotonic_increasing_vector(const double value, const vector<double>& monotonic_vec);
	public:
		class dimension_t
		{
		private:
			static string string_builder(const string base,int exponent);
		public:
			constexpr dimension_t(
				int length=0,
				int mass=0,
				int time=0,
				int electic_current=0,
				int substance_amount=0,
				int absolute_temperature=0,
				int luminous_intensity=0
				) : length(length), mass(mass),time(time),electic_current(electic_current),substance_amount(substance_amount),absolute_temperature(absolute_temperature),luminous_intensity(luminous_intensity) {};
			int length;
			int mass;
			int time;
			int electic_current;
			int substance_amount;
			int absolute_temperature;
			int luminous_intensity;
			bool is_relative() const;
			///order: T=time L=length M=mass I=current Th=abs. temperature N=substance amount J=luminous intensity
			string to_string() const;
			dimension_t(const dimension_t& dim);
			bool operator==(const dimension_t& obj) const;
			bool operator!=(const dimension_t& obj) const;
			dimension_t operator^(int pow) const;
			dimension_t operator*(const dimension_t& obj) const;
			dimension_t operator/(const dimension_t& obj) const;
			dimension_t invert() const;
		}; //quantity_t::dimension_t
	private: 
		static dimension_t get_dimension_from_unit(const unit_t& unit);
	protected:
		///the actual data
		vector<double> data_s;
		///contains all the operations already applied to this quantity before
		vector<string> operations_history_s;
		///name is changed by operation, e.g. "max(name)"
		string name_s;
		dimension_t dimension_s;
		unit_t unit_s;
	public:
		bool is_relative() const;
		///the actual data
		const vector<double>& data() const;
		///contains all the operations already applied to this quantity before
		const vector<string>& operations_history() const;
		string operations_history_to_string() const;
		///name is changed by operation, e.g. "max(name)"
		const string& name() const;
		const dimension_t& dimension() const;
		const unit_t& unit() const;
		
		/*operations*/
				
		bool is_nan() const;
		bool is_inf() const;
		bool has_data() const;
		bool is_set() const;
		bool is_scalar() const;
		bool is_vector() const;
		/**CTORS*/
		quantity_t();
// 		quantity_t(const quantity_t& Q);
		
		quantity_t(const quantity_t& quant,const unit_t& unit, string add_to_history="");
		quantity_t(const quantity_t& quant,const unit_t& unit, const dimension_t& dimension, string add_to_history="");
		
		quantity_t(const quantity_t& quant,const double& data, 			string add_to_history="");
		quantity_t(const quantity_t& quant,const vector<double>& data,	string add_to_history="");
		
// 		quantity_t(const vector<double>& data,unit_t& unit, 			string add_to_history="");
// 		quantity_t(const double& data,const unit_t& unit, 				string add_to_history="");
		
		quantity_t(const string& name,const vector<double>& data,const unit_t& unit);
		quantity_t(const string& name,const unit_t& unit, string add_to_history="");
		quantity_t(const string& name,const vector<double>& data,const unit_t& unit, const dimension_t& dimension);
		quantity_t(const string& name,const vector<double>& data,const unit_t& unit, const dimension_t& dimension, const vector<string>& operations_history,string add_to_history="");
		
		///returns a copy of itself
// 		quantity_t copy() const;
		
		///tries to change the current unit to its target unit; returns { } if unsuccessfull
		quantity_t change_unit(const unit_t& target_unit)  const;
		///tries to change the current unit to its target unit; returns { } if unsuccessfull
		quantity_t change_unit(string target_unit_string)  const;

		quantity_t resolution(double new_res=0)  const;

		quantity_t change_resolution(quantity_t new_res) const;
		quantity_t change_resolution(unsigned int new_data_size) const;
// 		const unit_t& unit() const;
// 		const unit_t unit_copy() const;
		const string to_string() const;
		const string to_string_detailed() const;
		
		
		quantity_t log10() const;
		
		///returns the data point on given pos
		quantity_t at(unsigned int pos) const;
		///returns the data between start and end (including start + end)
		quantity_t at(unsigned int start_pos, unsigned int end_pos) const;
		
		bool operator>(quantity_t obj) const;
		bool operator<(quantity_t obj) const;
		bool operator==(quantity_t obj) const;
		bool operator!=(quantity_t obj) const;
		bool operator==(double number) const;
		bool operator!=(double number) const;
		///appends obj.data (if_set) to data vector; or overwrites it if not set
		quantity_t& operator<<(const quantity_t& obj);
		quantity_t operator+(quantity_t quantity) const;
		quantity_t operator+(const double summand) const;
		void operator+=(quantity_t quantity);
		void operator+=(const double summand);
// 		quantity_t& operator=(const quantity_t& quantity);
		quantity_t operator-(quantity_t quantity) const;
		quantity_t operator-(const double summand)  const;
		quantity_t operator*(quantity_t quantity) const ;
		quantity_t operator*(const double factor) const ;
		quantity_t operator/(quantity_t quantity) const ;
		quantity_t operator/(const double divisor) const ;
		quantity_t operator^(const int exp) const ;
		
		/*statistics*/
		
		quantity_t reverse() const;
		
		// moving windows
		quantity_t moving_window_qqr(int window_size=0, double q=0.25) const;
		quantity_t moving_window_iqr(int window_size=0)  const;
		quantity_t moving_window_sum(int window_size=0)  const;
		quantity_t moving_window_max(int window_size=0)  const;
		quantity_t moving_window_min(int window_size=0)  const;
		
		quantity_t moving_window_median(int window_size=0)  const;
		quantity_t moving_window_mad(int window_size=0)  const;
		quantity_t moving_window_mean(int window_size=0)  const;
		quantity_t moving_window_sd(int window_size=0)  const;
		
		//smoothing
		quantity_t bspline_smoothing(unsigned int breakpoints = 0, const size_t spline_order = 4)  const;
		quantity_t bspline_smoothing(const quantity_t& Xdata, unsigned int breakpoints = 0, const size_t spline_order = 4)  const;
		
		//skalars
		///removes all data which are lower or greater than the limits
		quantity_t get_data_within_limits(double lower_limit, double upper_limit)  const;
		quantity_t get_data_by_index(unsigned int start, unsigned int stop)  const;
		quantity_t remove_data_by_index(unsigned int start, unsigned int stop)  const;
		quantity_t remove_data_by_index(vector<unsigned int> remove_pos)  const;
		///deletes last entry
		quantity_t pop_back()  const;
		quantity_t remove_data_from_begin(unsigned int stop)  const;
		quantity_t remove_data_from_begin(quantity_t& remove_stop)  const;
		quantity_t absolute()  const;
		quantity_t invert()  const;
		quantity_t abs_sum()  const;
		///sums up all data in the data.vector
		quantity_t sum(int start=0, int stop = -1)  const;
		quantity_t quantile(double percentile=0.75)  const;
		quantity_t median()  const;
		quantity_t mean()  const;
		quantity_t geo_mean()  const;
		///statistically sorted
		quantity_t trimmed_mean(float alpha=0.25)  const;
		///not sorted
		quantity_t cut_mean(float alpha=0.25)  const;
		///not sorted
		quantity_t cut_median(float alpha=0.25)  const;
		quantity_t gastwirth()  const;
		quantity_t sd()  const;
		quantity_t mad()  const;
		quantity_t max()  const;
		quantity_t front()  const;
		quantity_t back()  const;
		quantity_t x_at_max(const quantity_t& X)  const;
		quantity_t min()  const;
		quantity_t x_at_min(quantity_t& X)  const;
		quantity_t integrate(const quantity_t& x_data, unsigned int lower_X_index, unsigned int upper_X_index)  const;
		quantity_t integrate(const quantity_t& x_data, unsigned int lower_X_index)  const;
		quantity_t integrate(const quantity_t& x_data,const quantity_t& x_data_start,const quantity_t& x_data_stop) const;
		quantity_t integrate(const quantity_t& x_data,const quantity_t& x_data_start) const;
		/// point by point integration
		quantity_t integrate_pbp(const quantity_t& x_data) const;
		quantity_t round_() const;
		quantity_t diff() const;
		quantity_t interp(const quantity_t& old_X, const quantity_t& new_X) const;
		quantity_t fit_polynom_by_x_data(const quantity_t& x_data, quantity_t new_x_data, int polynom_grade=17 ) const;
		///polynomial interpolation from idx_start to idx_stop
		quantity_t polyfit(unsigned int polynom_grade) const;
		///deviation = 1 -> 1st deviation; 0 no deviation == polyfit
		quantity_t polyfit_derivative(unsigned int polynom_grade, unsigned int derivative=1) const;
		quantity_t extrapolate(unsigned int polynom_grade, const quantity_t& X) const;
		quantity_t filter_recursive_median(int window_size=0)  const;
		quantity_t filter_impulse(int window_size=0, float factor=4)  const;
		quantity_t filter_gaussian(int window_size=0, double alpha=1)  const;
		quantity_t remove_inf()  const;
		quantity_t remove_nan()  const;
		///clears all data: data={}
		quantity_t& clear();
	};
	
	
	namespace dimensions
	{
		namespace SI
		{
			const quantity_t::dimension_t length				(1,0,0,0,0,0,0);
			const quantity_t::dimension_t mass					(0,1,0,0,0,0,0);
			const quantity_t::dimension_t time					(0,0,1,0,0,0,0);
			const quantity_t::dimension_t electic_current		(0,0,0,1,0,0,0);
			const quantity_t::dimension_t substance_amount		(0,0,0,0,1,0,0);
			const quantity_t::dimension_t absolute_temperature	(0,0,0,0,0,1,0);
			const quantity_t::dimension_t luminous_intensity	(0,0,0,0,0,0,1);
			const quantity_t::dimension_t relative				(0,0,0,0,0,0,0);
		}
		namespace derived
		{
			const quantity_t::dimension_t concentration(dimensions::SI::substance_amount / (dimensions::SI::length^3));
			const quantity_t::dimension_t energy				(2,1,-2,0,0,0,0);
			const quantity_t::dimension_t voltage				(2,1,-3,-1,0,0,0);
		}
	} // dimensions

	class mass_t : public quantity_t
	{
	public:
		mass_t(vector<double> data_s={},unit_t unit_s=units::derived::amu, dimension_t dim_s=dimensions::SI::mass); // atomic mass units
		mass_t(const quantity_t& quantity_s);
	}; 
	class abundance_t : public quantity_t
	{
	public:
		abundance_t(vector<double> data_s={},unit_t unit_s={}, dimension_t dim_s=dimensions::SI::relative);
		abundance_t(const quantity_t& quantity_s);
	}; 
	
	class energy_t : public quantity_t
	{
	public:
		energy_t(vector<double> data_s={},unit_t unit_s=units::derived::electron_volt, dimension_t dim_s=dimensions::derived::energy);
		energy_t(const quantity_t& quantity_s);
	}; 
	
	class secondary_voltage_t : public quantity_t
	{
	public:
		secondary_voltage_t(vector<double> data_s={},unit_t unit_s=units::derived::volt, dimension_t dim_s=dimensions::derived::voltage);
		secondary_voltage_t(const quantity_t& quantity_s);
	}; 
	
	class rastersize_t : public quantity_t
	{
	public:
		rastersize_t(vector<double> data_s={},unit_t unit_s=units::SI::meter*units::prefixes::micro, dimension_t dim_s=dimensions::SI::length);
		rastersize_t(const quantity_t& quantity_s);
	};
	
	class depth_t : public quantity_t
	{
	public:
		depth_t(vector<double> data_s={},unit_t unit_s=units::SI::meter*units::prefixes::nano, dimension_t dim_s=dimensions::SI::length);
		depth_t(const quantity_t& quantity_s);
	};
	class sputter_depth_t : public quantity_t
	{
	public:
		sputter_depth_t(vector<double> data_s={},unit_t unit_s=units::SI::meter*units::prefixes::nano, dimension_t dim_s=dimensions::SI::length);
		sputter_depth_t(const quantity_t& quantity_s);
	};
	class total_sputter_depth_t : public quantity_t
	{
	public:
		total_sputter_depth_t(vector<double> data_s={},unit_t unit_s=units::SI::meter*units::prefixes::nano, dimension_t dim_s=dimensions::SI::length);
		total_sputter_depth_t(const quantity_t& quantity_s);
	};
	
	class sputter_time_t : public quantity_t
	{
	public:
		sputter_time_t(vector<double> data_s={},unit_t unit_s=units::SI::second, dimension_t dim_s=dimensions::SI::time);
		sputter_time_t(const quantity_t& quantity_s);
	};
	
	class total_sputter_time_t : public quantity_t
	{
	public:
		total_sputter_time_t(vector<double> data_s={},unit_t unit_s=units::SI::second, dimension_t dim_s=dimensions::SI::time);
		total_sputter_time_t(const quantity_t& quantity_s);
	};
	
	class concentration_t : public quantity_t
	{
	public:
		concentration_t(vector<double> data_s={},unit_t unit_s=units::derived::atoms / ((units::SI::meter*units::prefixes::centi).pow(3)), dimension_t dim_s=dimensions::derived::concentration);
		concentration_t(const quantity_t& quantity_s);
	};
	
	class intensity_t : public quantity_t
	{
	public:
		intensity_t(vector<double> data_s={},unit_t unit_s=units::derived::counts / units::SI::second, dimension_t dim_s=dimensions::SI::substance_amount/dimensions::SI::time);
		intensity_t(const quantity_t& quantity_s);
	};
	
	class current_t : public quantity_t
	{
	public:
		current_t(vector<double> data_s={},unit_t unit_s=units::prefixes::nano*units::SI::Ampere, dimension_t dim_s=dimensions::SI::electic_current);
		current_t(const quantity_t& quantity_s);
	};
	
	class sputter_current_t : public quantity_t
	{
	public:
		sputter_current_t(vector<double> data_s={},unit_t unit_s=units::prefixes::nano*units::SI::Ampere, dimension_t dim_s=dimensions::SI::electic_current);
		sputter_current_t(const quantity_t& quantity_s);
	};
	
	class analysis_current_t : public quantity_t
	{
	public:
		analysis_current_t(vector<double> data_s={},unit_t unit_s=units::prefixes::nano*units::SI::Ampere, dimension_t dim_s=dimensions::SI::electic_current);
		analysis_current_t(const quantity_t& quantity_s);
	};
	
	class dose_t : public quantity_t
	{
	public:
		dose_t(vector<double> data_s={},unit_t unit_s=units::derived::atoms / ((units::SI::meter*units::prefixes::centi).pow(2)), dimension_t dim_s=dimensions::SI::substance_amount/((dimensions::SI::length)^2));
		dose_t(const quantity_t& quantity_s);
	}; 
	
	class SF_t : public quantity_t
	{
	public:
		SF_t(vector<double> data_s={},unit_t unit_s=units::derived::atoms/ ((units::SI::meter*units::prefixes::centi).pow(3)) / (units::derived::counts/units::SI::second) , dimension_t dim_s=dimensions::SI::substance_amount/((dimensions::SI::length)^3) / (dimensions::SI::substance_amount/dimensions::SI::time) );
		SF_t(const quantity_t& quantity_s);
	}; 
	///matrix RSF
// 	class mRSF_t : public quantity_t
// 	{
// 	public:
// 		mRSF_t(vector<double> data_s={},unit_t unit_s=units::SI::one,dimension_t dim_s=dimensions::SI::relative);
// 		mRSF_t(const quantity_t& quantity_s);
// 	}; 
	
	class SR_t : public quantity_t
	{
	public:
		SR_t(vector<double> data_s={},unit_t unit_s=units::SI::meter*units::prefixes::nano / units::SI::second, dimension_t dim_s=dimensions::SI::length / dimensions::SI::time);
		SR_t(const quantity_t& quantity_s);
	};  
	
	class substance_amount_t : public quantity_t
	{
	public:
		substance_amount_t(vector<double> data_s={},unit_t unit_s=units::SI::mol, dimension_t dim_s=dimensions::SI::substance_amount);
		substance_amount_t(const quantity_t& quantity_s);
	}; 
	
	class electrical_charge_t : public quantity_t
	{
	public:
		electrical_charge_t(vector<double> data_s={},unit_t unit_s={units::derived::coulomb,-1}, dimension_t dim_s=dimensions::SI::electic_current*dimensions::SI::time);
		electrical_charge_t(const quantity_t& quantity_s);
	};

	///maps quantity X to quantity Y
	class map_t
	{
	private:
		
	public:
		map_t(const quantity_t& X, const quantity_t& Y);
		const quantity_t X;
		const quantity_t Y;
		///returns new_Y using akima splines
		const map_t map_interp_akima(const quantity_t new_X) const;
		///removes pairwise all infinity values from the map
		const map_t remove_inf() const;
		const map_t remove_nan() const;
		///remove map entry by given X position
		const map_t remove_by_X(quantity_t X_s) const;
		unsigned int fitable_data_size() const;
		///delete last element
		const map_t pop_back() const;
		bool is_set() const;
		const map<double,double> data_map() const;
		string to_string() const;
		string to_string_detailed() const;
		
		fit_functions::polynom_t polynom(const vector<unsigned>& rank, const vector<double>& polynom_start_parameters) const;
		fit_functions::polynom_t polynom(unsigned int polynom_grade) const;
		quantity_t polyfit(const quantity_t& x_vals, fit_functions::polynom_t polynom_s) const;
		quantity_t polyfit(const quantity_t& x_vals, unsigned int polynom_grade) const;
		quantity_t polyfit(const quantity_t& x_vals, const vector<unsigned>& rank, const vector<double>& polynom_start_parameters) const;
		
	}; // map_t
	
	
}

#endif // QUANTITY_HPP
