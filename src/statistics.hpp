
#ifndef STATISTICS_HPP
#define STATISTICS_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "print.hpp"
#include "tools.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
// #include "plot.hpp"

#include <gsl/gsl_bspline.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlinear.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_multilarge.h>
#include <gsl/gsl_statistics_double.h>
#include <gsl/gsl_integration.h>
#include <gsl/gsl_min.h>
#include <gsl/gsl_movstat.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_sort_double.h>
#include <gsl/gsl_filter.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_bspline.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_statistics.h>
#include <gsl/gsl_histogram.h>

#include "persistence1d.hpp" // from https://www.csc.kth.se/~weinkauf/notes/persistence1d.html


class statistics {
private:  
	typedef int (* functiontype) (const gsl_movstat_end_t, const gsl_vector*, gsl_vector*, gsl_movstat_workspace* );
	static std::vector<double> get_moving_window_function(const std::vector<double>& Y, int window_size, functiontype function,const gsl_movstat_end_t gsl_movstat_end = GSL_MOVSTAT_END_TRUNCATE);
public:
	class histogram_t
	{
	private:
		class range_t
		{
		private:
			const std::vector<double>& data;
			unsigned int number_of_bins;
		public:
			range_t(const std::vector<double>& data, const unsigned int& number_of_bins);
			///breaks the min-max distance into equally linear distributed parts
			const std::vector<double> equal_distributed_log10() const;
			const std::vector<double> equal_distributed_linear() const;
		};
		const std::vector<double> data;
		const unsigned int number_of_bins;
		const range_t range;
		std::vector<unsigned int> fill_bins(const std::vector<double>& range) const;
	public:
		///1 dimension
		histogram_t(const std::vector<double>& data, unsigned int number_of_bins);
		///the actual histogram, with equally linear distributed bins
		std::vector<unsigned int> linear_bins() const;
		///the actual histogram, with equally log10 distributed bins
		std::vector<unsigned int> log10_bins() const;
	};
	static gsl_vector* get_gsl_vec(const std::vector<double>& Y);
	static std::vector<double> get_gsl_vec(gsl_vector* Y);
	///breaks data equally into number_of_bins and returns a std::vector of the bin position and the number of elements in that bin
// 	static std::vector<unsigned int> histogram_t(const std::vector<double>& data, unsigned int number_of_bins);
	/*TEST*/
	static void test();
	
	/*moving windows*/
	static std::vector<std::vector<double>> get_moving_window_statistics_on_dataX_YY(std::vector<std::vector<double>> *data_X_YY, int window_size, int column);
	static std::vector<std::vector<double>> get_moving_window_statistics_from_Y(std::vector<double> *Y, int window_size);
	static std::vector<double> get_moving_window_median_from_Y(const std::vector<double>& Y, int window_size);
	static std::vector<double> get_moving_window_mean_from_Y(const std::vector<double>& Y, int window_size);
	static std::vector<double> get_moving_window_sd_from_Y(const std::vector<double>& Y, int window_size);
	static std::vector<double> get_moving_window_MAD_from_Y(const std::vector<double>& Y, int window_size);
	///default q=0.25 = IQR (difference between the 75th and 25th percentiles of Y)
	static std::vector<double> get_moving_window_qqr_from_Y(const std::vector<double>& Y, int window_size, double q=0.25);
	static std::vector<double> get_moving_window_min_from_Y(const std::vector<double>& Y, int window_size);
	static std::vector<double> get_moving_window_max_from_Y(const std::vector<double>& Y, int window_size);
	static std::vector<double> get_moving_window_sum_from_Y(const std::vector<double>& Y, int window_size);
	
// 	static int get_moving_window_max_index_from_Y(std::vector<double> Y, int window_size);
	
	static std::vector<int> get_local_minima_indices_new(std::vector<double> Y, double Y_tolerance=0, int x_stepsize=1);
	
	/*!
		populates vectors with indices for maxima and minima.
		starting with 0 index as global maximum resp. minimum
		and going on with 1 .... n in decreasing treshold order
	*/
	static bool get_extrema_indices(std::vector<int>& maxIdx, std::vector<int>& minIdx,const std::vector<double>& Y, double treshold=-1);
    ///\brief returns indices of all local maxima for Y, where Y>treshold
    static std::vector<int> get_local_maxima_indices(std::vector<double> Y, double treshold=-1, int stepsize=1);
	/// not tested!
	static std::vector<int> get_local_maxima_indices_by_triangulation(std::vector<double> Y, double treshold=0, int stepsize=1);
    /// not tested!
    static std::vector<int> get_local_minima_indices_by_triangulation(std::vector<double> Y, double treshold=0, int stepsize=1);
	static std::vector<int> get_local_minima_indices(std::vector<double> Y, double treshold=-1, int stepsize=1);
    ///\brief returns vectors {minima,maxima} of all local minima and maxima 
//     static std::vector<std::vector<int>> get_local_maxima_minima(std::vector<double> Y, double treshold);
    static std::vector<double> get_elements_lower_than_threshold(std::vector<double> Y, double treshold);
    static std::vector<int> get_elements_lower_than_threshold(std::vector<int> Y, int treshold);
    static std::vector<double> get_elements_higher_than_threshold(std::vector<double> Y, double treshold);
    static std::vector<int> get_elements_higher_than_threshold(std::vector<int> Y, int treshold);
    static std::vector<double> normalize_to_max(std::vector<double> Y);
    static std::vector<double> normalize_to_median(std::vector<double> Y);
    static std::vector<double> normalize_to_mean(std::vector<double> Y);
	/// percentile value is between 1 and 0 
	static double get_quantile_from_Y(std::vector<double> Y,double percentile=0.75);
    static double get_median_from_Y(std::vector<double> Y);
    static double get_mad_from_Y(std::vector<double> Y);
	static double get_sd_from_Y(std::vector<double> Y);
    static double get_mean_from_Y(std::vector<double> Y);
	
	/*!
	 * The trimmed mean, or truncated mean, discards a certain number of smallest and largest samples from the input vector
	 * before computing the mean of the remaining samples. The amount of trimming is specified by a factor alpha [0,0.5]. 
	 * Then the number of samples discarded from both ends of the input std::vector is floor(\alpha * n), 
	 * where n is the length of the input. So to discard 25% of the samples from each end, one would std::set alpha = 0.25.
	 */
	static double get_trimmed_mean_from_Y(std::vector<double> Y,float alpha=0.25);
	/*!
	 * something like "get_trimmed_mean_from_Y" 
	 * https://www.gnu.org/software/gsl/doc/html/statistics.html#robust-location-estimates
	 */
	static double get_gastwirth_estimator_from_Y(std::vector<double> Y);
    static int get_next_max_index(std::vector<double> Y, int skip_points=0);
    static int get_next_min_index(std::vector<double> Y, int skip_points=0);
    static double get_next_min(std::vector<double> Y, int skip_points=0);
    static double get_next_max(std::vector<double> Y, int skip_points=0);
    static std::vector<double> recursive_median_filter(std::vector<double> Y, int window_size);
    static std::vector<double> gaussian_filter(std::vector<double> Y, int window_size, double alpha);
    static std::vector<std::vector<double>> get_local_minima_XY(std::vector<double> X, std::vector<double> Y);
    static std::vector<double> impulse_filter(std::vector<double> Y, int window_size, float scale_factor);
    static std::vector<double> gsl_vec_to_std_vec(gsl_vector ** vec);
    static std::vector<double> fit_poly_XY_transposed(std::vector<std::vector<double>> XY, int poly_grad);
    static std::vector<double> get_poly_Y(std::vector<double> parameters, std::vector<double> X);
    static std::vector<double> derive_vec(std::vector<double> X, std::vector<double> Y);
    static int get_max_index_from_Y(std::vector<double> Y);
    static int get_min_index_from_Y(std::vector<double> Y);
	static double get_max_from_Y(std::vector<double> Y);
    static double get_min_from_Y(std::vector<double> Y);
    
//     static statis get_statistics(std::vector<double> Y);
    static bool std_mat_to_gsl_vec(std::vector<std::vector<double>> *std_mat, gsl_vector ** vec, int col);
    static bool std_vec_to_gsl_vec(std::vector<double> *std_vec, gsl_vector ** vec);
	static bool std_vec_to_gsl_vec(const std::vector<double>& std_vec, gsl_vector ** vec);
    ///\brief returns {X_min,X_max,Y_min,Y_max}
    static std::vector<double> get_minmax_from_dataXY_tensor(std::vector<std::vector<std::vector<double>>> dataXY_tensor);
    static int get_index_from_upper_limit_in_vec(std::vector<double> vec, double upper_limit);
    
	///returns the biggest common std::vector from all vectors
	static std::vector<double> common_vector(std::vector<std::vector<double>> data_vectors);
    static std::vector<std::vector<double>> truncate_mat_by_limit_in_col(std::vector<std::vector<double>> *data_XY, double limit, int col);
    
    static int get_index_for_next_value_lower_than_treshold(std::vector<double> Y,double treshold, int start=0, int ende=0);
	static int get_index_for_next_value_higher_than_treshold(std::vector<double> Y,double treshold, int start=0, int ende=0);
    static int get_index_for_next_value_within_treshold(std::vector<double> Y,double treshold_bottom, double treshold_top, int start=0, int ende=0);
    
    static double get_correlation_from_dataXY(std::vector<std::vector<double>> *data_XY1, std::vector<std::vector<double>> *data_XY2);
	static double get_correlation_Y1_Y2(const std::vector<double>& Y1, const std::vector<double>& Y2);
//     static statis get_statistics_from_dataXY_transposed(std::vector<std::vector<double>> *data_XY_transposed);
    static double polynom_integrator (double x, void * params);
    static double get_area_under_dataXY_transposed(std::vector<std::vector<double>> data_XY_transposed,double lower_X, double upper_X);
    static double get_area_under_dataXY(std::vector<std::vector<double>> data_XY,double lower_X, double upper_X);
    static double get_area_under_polynom(std::vector<double> polynom_parameters, double lower_X, double upper_X);
    static std::vector<int> get_positions_of_zero_values(std::vector<std::vector<double>> matrix, double treshold);
    static std::vector<double> derive_poly(std::vector<double> parameters);
    static std::vector<double> integrate_poly(std::vector<double> parameters);
    static bool first_derivative_poly(std::vector<double> *parameters, std::vector<std::vector<double>> *data_XY_dY);
    static bool second_derivative_poly(std::vector<double> *parameters, std::vector<std::vector<double>> *data_XY_ddY);
    static std::vector<double> get_polynom_Y(std::vector<double> parameters, std::vector<double> *data_X);
    static double fit_poly (std::vector<std::vector<double>> *data_matrix, std::vector<double> *fit_parameters);
//     static bool polynomfit_XY(std::vector<std::vector<double>> data_XY,data *fitted_data, int polynom_grad=1, double interp_res=0);
	///using akima splines
	static std::vector<double> interpolate_data_XY_transposed(std::vector<std::vector<double>> data_XY, std::vector<double> X);
	///akima splines
	static std::vector<double> interpolate_dataXY_to_X(std::vector<std::vector<double>>& data_XY, std::vector<double>& X);
	
	/****************/
	///obsolet, use bspline_smooth instead
	static std::vector<double> interpolate_bspline(std::map<double, double>& data_XY,std::vector<double> X_new={}, int bspline_degree=3);
	/// akima splines
	static std::vector<double> interpolate_data_XY(const std::map<double,double>& data_XY,const std::vector<double>& X);
	
	///B-spline smoothing
	static std::vector<double> bspline_smooth(const std::vector<double>& Y, std::vector<double> Xdata={}, unsigned int breakpoints = 0, const size_t spline_order = 4);
	
	static double integrate(std::map<double,double>& data_XY, unsigned int start_idx=0, unsigned int stop_idx=0) ;
	
	static std::map<double, double> change_X_resolution(const std::map<double, double>& data_XY,const double new_resolution);
	
	static unsigned int factorial(const unsigned int& fac);
	static std::vector<unsigned int> factorial(const std::vector<unsigned int>& fac);
	///returns the absolute of each entry
	static std::vector<double> absolute(std::vector<double> data);
	static std::vector<double> round(std::vector<double> data_s ,unsigned int decimals);
	static double round(double data_s ,unsigned int decimals);
};

#endif // STATISTICS_HPP

