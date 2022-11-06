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

///physical quantities
namespace quantity
{
	
	///generic physical quantity;
	class quantity_t
	{
	private:
        class_logger_t logger;
		const std::map<double,double> data_XY_1D();
		static int get_value_index_position_in_strictly_monotonic_increasing_vector(const double value, const std::vector<double>& monotonic_vec);
	public:
		class dimension_t
		{
		private:

			static std::string string_builder(const std::string base,int exponent);
		public:
			constexpr dimension_t(
				int length=0,
				int mass=0,
				int time=0,
				int electic_current=0,
				int substance_amount=0,
				int absolute_temperature=0,
				int luminous_intensity=0
                )
                    : length(length),
                      mass(mass),
                      time(time),
                      electic_current(electic_current),
                      substance_amount(substance_amount),
                      absolute_temperature(absolute_temperature),
                      luminous_intensity(luminous_intensity)              {};
			int length;
			int mass;
			int time;
			int electic_current;
			int substance_amount;
			int absolute_temperature;
			int luminous_intensity;
			bool is_relative() const;
			///order: T=time L=length M=mass I=current Th=abs. temperature N=substance amount J=luminous intensity
			std::string to_string() const;
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
		std::vector<double> data_s;
		///contains all the operations already applied to this quantity before
		std::vector<std::string> operations_history_s;
		///name is changed by operation, e.g. "max(name)"
		std::string name_s;
		dimension_t dimension_s;
		unit_t unit_s;
	public:
		///{1,2,3,4,5,...data.size()}
		const std::vector<double> data_X_1D() const;
		bool is_relative() const;
		///the actual data
		const std::vector<double>& data() const;
		///contains all the operations already applied to this quantity before
		const std::vector<std::string>& operations_history() const;
		std::string operations_history_to_string() const;
		///name is changed by operation, e.g. "max(name)"
        const std::string& name() const;
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
		
		quantity_t(const quantity_t& quant,const unit_t& unit, std::string add_to_history="");
		quantity_t(const quantity_t& quant,const unit_t& unit, const dimension_t& dimension, std::string add_to_history="");
		
		quantity_t(const quantity_t& quant,const double& data, 			std::string add_to_history="");
		quantity_t(const quantity_t& quant,const std::vector<double>& data,	std::string add_to_history="");
		
// 		quantity_t(const std::vector<double>& data,unit_t& unit, 			std::string add_to_history="");
// 		quantity_t(const double& data,const unit_t& unit, 				std::string add_to_history="");
		
        quantity_t(const std::string& name,const std::vector<double>& data,const unit_t& unit);
        quantity_t(const std::string& name,const unit_t& unit, std::string add_to_history="");
        quantity_t(const std::string& name,const std::vector<double>& data,const unit_t& unit, const dimension_t& dimension);
        quantity_t(const std::string& name,const std::vector<double>& data,const unit_t& unit, const dimension_t& dimension, const std::vector<std::string>& operations_history,std::string add_to_history="");
		
		///returns a copy of itself
// 		quantity_t copy() const;
		
		///tries to change the current unit to its target unit; returns { } if unsuccessfull
		quantity_t change_unit(const unit_t& target_unit)  const;
		///tries to change the current unit to its target unit; returns { } if unsuccessfull
		quantity_t change_unit(std::string target_unit_string)  const;
        ///will change the resolution so all data-points are integer multiples of "new_res"
		quantity_t resolution(double new_res=0)  const;

        quantity_t change_resolution(quantity_t new_res) const;
        quantity_t change_resolution(double start, double step, double stop=-1) const;
		quantity_t change_resolution(unsigned int new_data_size) const;
// 		const unit_t& unit() const;
// 		const unit_t unit_copy() const;
		const std::string to_string_short() const;
		const std::string to_string() const;
		const std::string to_string_detailed() const;
		
		
		quantity_t log10() const;
		
		///returns the data point on given pos
		quantity_t at(unsigned int pos) const;
		///returns the data between start and end (including start + end)
		quantity_t at(unsigned int start_pos, unsigned int end_pos) const;
		
		bool operator>(quantity_t obj) const;
        bool operator>=(quantity_t obj) const;
        bool operator<=(quantity_t obj) const;
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
		quantity_t get_data_by_index_rel(float start, float stop)  const;
		quantity_t remove_data_by_index(unsigned int start, unsigned int stop)  const;
		quantity_t remove_data_by_index(std::vector<unsigned int> remove_pos)  const;
		///deletes last entry
		quantity_t pop_back()  const;
		quantity_t remove_data_from_begin(unsigned int stop)  const;
		quantity_t remove_data_from_begin_rel(float stop_relative)  const;
// 		quantity_t remove_data_from_begin(quantity_t& remove_stop)  const;
		///removes data, which is bigger than Q.data[i]
		quantity_t remove_data_bigger_than(const quantity_t& Q) const;
		quantity_t remove_data_bigger_than(double Q) const;
		///removes data, which is identical to Q.data[i]
		quantity_t remove_data_equal_to(const quantity_t& Q) const;
		quantity_t remove_data_equal_to(double Q) const;
		///removes data, which is smaller than Q.data[i]
		quantity_t remove_data_smaller_than(const quantity_t& Q) const;
		quantity_t remove_data_smaller_than(double Q) const;
		quantity_t absolute()  const;
		quantity_t invert()  const;
		quantity_t abs_sum()  const;
		///sums up all data in the data.vector
		quantity_t sum(int start=0, int stop = -1)  const;
// 		quantity_t pbp_sum()  const;
		quantity_t quantile(double percentile=0.75)  const;
		quantity_t median()  const;
		quantity_t mean()  const;
		quantity_t geo_mean()  const;
		///statistically sorted; removes data at beginning and end
		quantity_t trimmed_mean(float alpha=0.25)  const;
		///statistically sorted; removes data at beginning and end
		quantity_t trimmed_median(float alpha=0.25)  const;
		///not sorted
		quantity_t cut_mean(float alpha=0.25)  const;
		///not sorted
		quantity_t cut_median(float alpha=0.25)  const;
		quantity_t gastwirth()  const;
		quantity_t sd()  const;
		quantity_t mad()  const;
		quantity_t max()  const;
        ///returns -1, on error
		int max_idx()  const;
		quantity_t min()  const;
        ///returns -1 on error
		int min_idx()  const;
		quantity_t front()  const;
		quantity_t back()  const;
		quantity_t x_at_max(const quantity_t& X)  const;
		
		quantity_t x_at_min(quantity_t& X)  const;
        quantity_t integrate(const quantity_t& x_data, unsigned int lower_X_index, unsigned int upper_X_index)  const;
        quantity_t integrate(const quantity_t& x_data, unsigned int lower_X_index=0)  const;
		quantity_t integrate(const quantity_t& x_data,const quantity_t& x_data_start,const quantity_t& x_data_stop) const;
		quantity_t integrate(const quantity_t& x_data,const quantity_t& x_data_start) const;
		/// point by point integration
        quantity_t integrate_pbp(const quantity_t& x_data);
        ///round mathimatically correct
        quantity_t round_(const unsigned int decimals=0) const;
        quantity_t round_to_multiple(double multiple) const;
        ///round up
        quantity_t ceil_(const unsigned int decimals=0) const;
        quantity_t ceil_to_multiple(double multiple) const;
        //round down
        quantity_t floor_(const unsigned int decimals=0) const;
        quantity_t floor_to_multiple(double multiple) const;
		quantity_t diff() const;
		quantity_t interp(const quantity_t& old_X, const quantity_t& new_X) const;
		quantity_t fit_polynom_by_x_data(const quantity_t& x_data, quantity_t new_x_data, int polynom_grade=17 ) const;
        fit_functions::polynom_t polynom(unsigned int polynom_grade) const;
		///polynomial interpolation from idx_start to idx_stop
        quantity_t polyfit(unsigned int polynom_grade) const;
        quantity_t polyfit(const fit_functions::polynom_t& polynom) const;
// 		quantity_t polyfit(fit_functions::polynom_t polynom_s) const;
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
		///lowest to highest
		quantity_t& sort();
		/// idx of std::vector is the number of the bin and the value of the vector.at(idx) is number of data (within the bin); 
		/// bins is the number of bins, all having same sizes
		std::vector<int> bin_data(int bins_count) const;
		/// idx of std::vector is the number of the bin and the value of the vector.at(idx) is number of data (within the bin); 
		/// returning the number of bins , where each idx of the result std::vector is containing the amount of data within the bin
		std::vector<int> bin_data(const std::vector<double>& bins) const;
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
		mass_t(std::vector<double> data_s={},unit_t unit_s=units::derived::amu, dimension_t dim_s=dimensions::SI::mass); // atomic mass units
		mass_t(const quantity_t& quantity_s);
	}; 

	class abundance_t : public quantity_t
	{
	public:
		abundance_t(std::vector<double> data_s={},unit_t unit_s={}, dimension_t dim_s=dimensions::SI::relative);
		abundance_t(const quantity_t& quantity_s);
	}; 
	
	class energy_t : public quantity_t
	{
	public:
		energy_t(std::vector<double> data_s={},unit_t unit_s=units::derived::electron_volt, dimension_t dim_s=dimensions::derived::energy);
		energy_t(const quantity_t& quantity_s);
	}; 
	
	class secondary_voltage_t : public quantity_t
	{
	public:
		secondary_voltage_t(std::vector<double> data_s={},unit_t unit_s=units::derived::volt, dimension_t dim_s=dimensions::derived::voltage);
		secondary_voltage_t(const quantity_t& quantity_s);
	}; 
	
	class rastersize_t : public quantity_t
	{
	public:
		rastersize_t(std::vector<double> data_s={},unit_t unit_s=units::SI::meter*units::prefixes::micro, dimension_t dim_s=dimensions::SI::length);
		rastersize_t(const quantity_t& quantity_s);
	};
	
	class depth_t : public quantity_t
	{
	public:
		depth_t(std::vector<double> data_s={},unit_t unit_s=units::SI::meter*units::prefixes::nano, dimension_t dim_s=dimensions::SI::length);
		depth_t(const quantity_t& quantity_s);
	};
	class sputter_depth_t : public quantity_t
	{
	public:
		sputter_depth_t(std::vector<double> data_s={},unit_t unit_s=units::SI::meter*units::prefixes::nano, dimension_t dim_s=dimensions::SI::length);
		sputter_depth_t(const quantity_t& quantity_s);
	};
	class total_sputter_depth_t : public quantity_t
	{
	public:
		total_sputter_depth_t(std::vector<double> data_s={},unit_t unit_s=units::SI::meter*units::prefixes::nano, dimension_t dim_s=dimensions::SI::length);
		total_sputter_depth_t(const quantity_t& quantity_s);
	};
	
	class sputter_time_t : public quantity_t
	{
	public:
		sputter_time_t(std::vector<double> data_s={},unit_t unit_s=units::SI::second, dimension_t dim_s=dimensions::SI::time);
		sputter_time_t(const quantity_t& quantity_s);
	};
	
	class total_sputter_time_t : public quantity_t
	{
	public:
		total_sputter_time_t(std::vector<double> data_s={},unit_t unit_s=units::SI::second, dimension_t dim_s=dimensions::SI::time);
		total_sputter_time_t(const quantity_t& quantity_s);
	};
	
    class concentration_t : public quantity_t
    {
    public:
        concentration_t(std::vector<double> data_s={},unit_t unit_s=units::derived::atoms / ((units::SI::meter*units::prefixes::centi).pow(3)), dimension_t dim_s=dimensions::derived::concentration);
        concentration_t(const quantity_t& quantity_s);
    };
    ///relative concentration
    class concentration_rel_t : public quantity_t
    {
    public:
        concentration_rel_t(std::vector<double> data_s={},unit_t unit_s=units::derived::atom_percent, dimension_t dim_s=dimensions::SI::relative);
        concentration_rel_t(const quantity_t& quantity_s);
    };
	
	class intensity_t : public quantity_t
	{
	public:
		intensity_t(std::vector<double> data_s={},unit_t unit_s=units::derived::counts / units::SI::second, dimension_t dim_s=dimensions::SI::substance_amount/dimensions::SI::time);
		intensity_t(const quantity_t& quantity_s);
	};
	
	class current_t : public quantity_t
	{
	public:
		current_t(std::vector<double> data_s={},unit_t unit_s=units::prefixes::nano*units::SI::Ampere, dimension_t dim_s=dimensions::SI::electic_current);
		current_t(const quantity_t& quantity_s);
	};
	
	class sputter_current_t : public quantity_t
	{
	public:
		sputter_current_t(std::vector<double> data_s={},unit_t unit_s=units::prefixes::nano*units::SI::Ampere, dimension_t dim_s=dimensions::SI::electic_current);
		sputter_current_t(const quantity_t& quantity_s);
	};
	
	class analysis_current_t : public quantity_t
	{
	public:
		analysis_current_t(std::vector<double> data_s={},unit_t unit_s=units::prefixes::nano*units::SI::Ampere, dimension_t dim_s=dimensions::SI::electic_current);
		analysis_current_t(const quantity_t& quantity_s);
	};
	
	class dose_t : public quantity_t
	{
	public:
		dose_t(std::vector<double> data_s={},unit_t unit_s=units::derived::atoms / ((units::SI::meter*units::prefixes::centi).pow(2)), dimension_t dim_s=dimensions::SI::substance_amount/((dimensions::SI::length)^2));
		dose_t(const quantity_t& quantity_s);
	}; 
	
	class SF_t : public quantity_t
	{
	public:
		SF_t(std::vector<double> data_s={},unit_t unit_s=units::derived::atoms/ ((units::SI::meter*units::prefixes::centi).pow(3)) / (units::derived::counts/units::SI::second) , dimension_t dim_s=dimensions::SI::substance_amount/((dimensions::SI::length)^3) / (dimensions::SI::substance_amount/dimensions::SI::time) );
		SF_t(const quantity_t& quantity_s);
	}; 
	///matrix RSF
// 	class mRSF_t : public quantity_t
// 	{
// 	public:
// 		mRSF_t(std::vector<double> data_s={},unit_t unit_s=units::SI::one,dimension_t dim_s=dimensions::SI::relative);
// 		mRSF_t(const quantity_t& quantity_s);
// 	}; 
	
	class SR_t : public quantity_t
	{
	public:
		SR_t(std::vector<double> data_s={},unit_t unit_s=units::SI::meter*units::prefixes::nano / units::SI::second, dimension_t dim_s=dimensions::SI::length / dimensions::SI::time);
		SR_t(const quantity_t& quantity_s);
	};  
	
	class substance_amount_t : public quantity_t
	{
	public:
		substance_amount_t(std::vector<double> data_s={},unit_t unit_s=units::SI::mol, dimension_t dim_s=dimensions::SI::substance_amount);
		substance_amount_t(const quantity_t& quantity_s);
	}; 
	
	class electrical_charge_t : public quantity_t
	{
	public:
		electrical_charge_t(std::vector<double> data_s={},unit_t unit_s={units::derived::coulomb}, dimension_t dim_s=dimensions::SI::electic_current*dimensions::SI::time);
		electrical_charge_t(const quantity_t& quantity_s);
	};

	///maps quantity X to quantity Y
	class map_t
	{
	private:
        class_logger_t logger;
		std::vector<std::pair<double,double>> XYdata_pairs() const;
		static bool sort_by_x(const std::pair<double,double>& P1, const std::pair<double,double>& P2);
		static bool sort_by_y(const std::pair<double,double>& P1, const std::pair<double,double>& P2);
		quantity_t X_p;
		quantity_t Y_p;
	public:
		map_t();
		map_t(const quantity_t& X, const quantity_t& Y);
		///overwrites X and Y quantities by XYdata_pairs
		map_t(const quantity_t& X, const quantity_t& Y, const std::vector<std::pair<double,double>>& XYdata_pairs);
		const quantity_t& X() const;
		const quantity_t& Y() const;
		///returns new_Y using akima splines
		const map_t map_interp_akima(const quantity_t new_X) const;
		///removes pairwise all infinity values from the map
		const map_t remove_inf() const;
		const map_t remove_nan() const;
		///remove std::map entry by given X position
		const map_t remove_by_X(quantity_t X_s) const;
		///delete last element
		const map_t pop_back() const;
		const map_t back() const;
		bool is_set() const;
		//scalar vs scalar
		bool is_single_point() const;
		const std::map<double,double> data_map() const;
		std::string to_string() const;
		std::string to_string_detailed() const;
		std::string to_string_list() const;
		///removes outliners until chisqr_relative is below treshold (default 10%)
		map_t remove_outliners(fit_functions::polynom_t polynom_s,float gof_treshold=0.1) const;
		map_t remove_worst_outliner(const fit_functions::polynom_t& polynom_s) const;
		fit_functions::polynom_t polynom(const std::vector<unsigned>& rank, const std::vector<double>& polynom_start_parameters) const;
		fit_functions::polynom_t polynom(unsigned int polynom_grade) const;
		map_t swap_X_Y() const;
		map_t at(unsigned int idx_pos) const;
		///max of Y
		map_t max() const;
		///min of Y
		map_t min() const;
		///absolute of Y
		map_t absolute() const;
		quantity_t integral() const;
		map_t sort_X_from_low_to_high() const;
		map_t sort_Y_from_low_to_high() const;
		map_t sort_X_from_high_to_low() const;
		map_t sort_Y_from_high_to_low() const;
		/// Y1(X1) ... Yn(Xn) will be inversed: Yn(Xn) ... Y1(X1)
		map_t reverse_order() const;
		///difference of Y and polynomial fitted Y -> the error
		map_t delta_y(const fit_functions::polynom_t& polynom_s) const;
		///difference of Y and Y_ref
		map_t delta_y(const map_t& Y_ref) const;
		///difference of Y and Y_ref
		map_t delta_y(const quantity_t& Y_ref) const;
		///the polynomial fitted std::map (X(),Y_fitted())
		map_t polyfit(fit_functions::polynom_t polynom_s) const;
		map_t polyfit(fit_functions::polynom_t polynom_s,const unsigned int number_of_points) const;
		map_t polyfit(fit_functions::polynom_t polynom_s, quantity_t new_X) const;
		///the polynomial fitted std::map (X(),Y_fitted())
		map_t polyfit(unsigned int polynom_grade) const;
		///the polynomial fitted std::map (X(),Y_fitted())
		map_t polyfit(const std::vector<unsigned>& rank, const std::vector<double>& polynom_start_parameters) const;
		quantity_t polyfit(const quantity_t& x_vals, fit_functions::polynom_t polynom_s) const;
		quantity_t polyfit(const quantity_t& x_vals, unsigned int polynom_grade) const;
		quantity_t polyfit(const quantity_t& x_vals, const std::vector<unsigned>& rank, const std::vector<double>& polynom_start_parameters) const;
		///using radius (X^2 + Y^2)
		quantity_t distance() const;
		/// idx of std::vector is the number of the bin and the value of the vector.at(idx) is number of data (within the bin); 
		/// bins is the number of bins, all having same sizes
		///using radius (X^2 + Y^2)
		std::vector<int> bin_data(int bins_count) const;
		/// idx of std::vector is the number of the bin and the value of the vector.at(idx) is number of data (within the bin); 
		/// returning the number of bins , where each idx of the result std::vector is containing the amount of data within the bin
		///using radius (X^2 + Y^2)
		std::vector<int> bin_data(const std::vector<double>& bins) const;
		/// returns the number of filled data bins
		int filled_data_bins(int bins_count) const;
		int size() const;
		map_t get_data_by_index(unsigned int start, unsigned int stop) const;
		const map_t change_resolution(const quantity_t new_X_resolution) const;
		/*********OPERATORS*********/
		///appends obj to the map, if quantitys dimension and units allow it
		map_t& operator<<(map_t obj);
		///returns {X() , Y()+obj.Y()}, if quantitys dimension and units allow it
		map_t operator+(map_t obj) const;
		map_t operator+(quantity_t Y_o) const;
		///returns {X() , Y()-obj.Y()}, if quantitys dimension and units allow it
		map_t operator-(map_t obj) const;
		map_t operator-(quantity_t Y_o) const;
		///returns {X() , Y()*obj.Y()}, if quantitys dimension and units allow it
		map_t operator*(map_t obj) const;
		map_t operator*(quantity_t Y_o) const;
		///returns {X() , Y()/obj.Y()}, if quantitys dimension and units allow it
		map_t operator/(map_t obj) const;
		map_t operator/(quantity_t Y_o) const;
	}; // map_t
	class table_t
	{
    private:
        class_logger_t logger;
	public:
		class column_t
		{
		private:
            class_logger_t logger;
			///line_id -> quantity_entry; each entry can be a vector
			std::map<long long unsigned int, quantity_t> quantity_entries_p;
		public:
			std::string name;
			std::string comment;
			column_t();
			column_t(long long unsigned int line_id,const quantity_t& q, std::string name="", std::string comment="");
			//quantities, have same unit, dimension and name; just their data might be different
			const std::map<long long unsigned int, quantity_t>& quantity_entries() const;
			///header quantity; all q-entries must have same dimension, unit and name
			const quantity_t header() const;
			///at least one quantity entry line is be set
			bool is_set() const;
			std::string to_string() const;
			///true on success
			bool add_quantity_entry(long long unsigned int line_id, quantity_t q);
			///adds new lines/quantities to this column or replaces not_set ones; true on success
			bool add_entries(const column_t& col);
			///all line_ids with match with q
			std::vector<long long unsigned int> get_line_ids(quantity_t q) const;
			//true on success; sets all entries to {}, where q is identical
			bool unset_quantities(const quantity_t& q);
			//true on success
			bool unset_quantity_entry(long long unsigned int line_id);
			//true on success
			bool erase_quantity_entry(long long unsigned int line_id);
			//true on success; replace Line_id with q_replacement
			bool replace_quantity_entry(long long unsigned int line_id, const quantity_t& q_replacement);
			///compares name and quantity.name + quantity.dimension + quantity.unit
			bool operator==(const column_t& col) const;
			///compares name and quantity.name + quantity.dimension + quantity.unit
			bool operator!=(const column_t& col) const;
			///quantity is not set, if line_id does not exist
			quantity_t get_quantity(long long unsigned int line_id) const;
			///column is transformed into 1 std::vector quantity; just possible if column entries are scalars
			quantity_t get_quantity() const;
			///calculates for each column entry its median
			column_t median() const;
			///for each column entry!
			column_t mean()  const;
			///for each column entry!
			column_t geo_mean()  const;
			///statistically sorted; for each column entry!
			column_t trimmed_mean(float alpha=0.25)  const;
			///not sorted; for each column entry!
			column_t cut_mean(float alpha=0.25)  const;
			///not sorted; for each column entry!
			column_t cut_median(float alpha=0.25)  const;
		}; // table_t::column_t
	private:
		///combination of col+name is unique
		std::vector<column_t> columns_p;
		///generates a random line_id, which is not used yet
		long long unsigned int get_new_random_line_id() const;
		///generates a sequential line_id, which is not used yet
		long long unsigned int get_new_sequential_line_id() const;
		column_t* column(const column_t& col);
	public:
        table_t();
		/// line_id --> line_name
		std::map<long long unsigned int,std::string> line_names;
		bool add(const table_t& table);
		bool add(const std::vector<column_t>& cols);
		bool add(const column_t& col);
		///using get_new_sequential_line_id to generate a line_id
		bool add(const quantity_t& Q, std::string col_name);
		bool add(const quantity_t& Q, std::string col_name, long long unsigned int line_id);
		table_t& erase_line(long long unsigned int line_id);
		table_t& erase_lines(std::set<long long unsigned int> line_ids);
		///returns the corssponding column within the table
		const column_t* get_col(const column_t& col) const;
		///returns the corssponding column within the table
		const column_t* get_col(const quantity_t& col_header, std::string col_name) const;
		///returns only quantitie, which are set
		map_t get_map(const column_t& col_X, const column_t& col_Y) const;
		///returns 0th and 1st column idxs
		map_t get_map() const;
		map_t get_map(int X_col_idx, int Y_col_idx) const;
		const std::vector<column_t>& columns() const;
		const std::vector<column_t> line(long long unsigned int line_id) const;
		const std::set<long long unsigned int> line_ids() const;
		std::string to_string() const;
		///removes all lines, with at least 1 entry, where quantity is not set
		table_t& remove_empty_lines();
		table_t& remove_empty_columns();
		///at least 1 col in table is_set
		bool is_set() const;
	}; //table_t
	

}


#endif // QUANTITY_HPP
