
#ifndef FIT_FUCTIONS_HPP
#define FIT_FUCTIONS_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
// #include <cmath>

#include "tools.hpp"
#include "statistics.hpp"


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
#include <gsl/gsl_bspline.h>
#include <gsl/gsl_minmax.h>
#include <gsl/gsl_fit.h>
#include "log.hpp"

class fit_functions
{
public:

	static const std::map<double,double> data_1D_to_2D(std::vector<double> data_1D);
	/// like origin -> peak functions -> asym2sig
	class asym2sig_t
	{
	private:
        class_logger_t logger;
		double chisq_p=-1;
		double chisq0_p=-1;
		double gof_p=-1;
		bool fitted_p=false;
		static void callback(const size_t iter, void *params, const gsl_multifit_nlinear_workspace *w);
// 		static int function_1st_derivative(const gsl_vector * x, void *data, gsl_matrix * J);
		static int function(const gsl_vector * x, void *data, gsl_vector * f);
	public:
        asym2sig_t();
		//fitparameters
		/// offset
		double y0;
		/// anstieg
		double m;
		/// amplitude
		double A;
		/// crater centre
		double xc;
		/// crater width
		double w1;
		double w2,w3;
		/// y(x) values
		std::vector<double> fitted_y_data(std::vector<double> x);
		/// calculation of fit parameters
		bool fit(std::map<double, double> data_XY, double y0_s=NAN, double xc_s=NAN, double m_s=NAN, double A_s=NAN, double w1_s=NAN, double w2_s=NAN, double w3_s=NAN);
		bool fitted() const;
		void print_parameters();
		double chisq() const;
		double chisq0() const;
		double gof() const;
		std::string to_string(std::string prefix="") const;
	};
	/*
	 * Y = SUM_i ( c_j * x_j * rank_j )
	 */
	class polynom_t
	{
	private:
        class_logger_t logger;
		bool successfully_fitted_p=false;
		double chisq_p=-1;
		double chisq_relative_p=-1;
		std::vector<double> fit_parameters_p;
		///perform the actual fit
		bool fit(std::map<double,double> data_XY);
		std::vector<unsigned int> rank_p;
		double gof_p;
// 		const std::vector<double> x_data_p;
	public:
		///degree means rank; 2D data
		polynom_t(int degree, const std::map<double,double>& data_XY);
		///degree means rank; 1D data
		polynom_t(int degree, const std::vector<double>& data);
		///fitting 2D data
		polynom_t(const std::vector<unsigned int> rank, const std::vector<double>& fit_parameters, const std::map<double,double>& data_XY);
		///fitting 1D data
		polynom_t(const std::vector<unsigned int> rank, const std::vector<double>& fit_parameters, const std::vector<double>& data);
		///no fit is done; use "fit_parameters" directly 
		polynom_t(const std::vector<double>& fit_parameters);
		///rank of polynom: 0 means discard, and non-0 means use
		const std::vector<unsigned int>& rank() const;
        double chisq() const;
		///
		double chisq_relative() const;
		const std::vector<double>& fit_parameters() const;
		///the minimum / necessary rank
		int degree() const;
		const polynom_t derivative(unsigned int derive=1) const;
		std::string to_string(std::string prefix="") const;
		bool successfully_fitted() const;
		double gof() const;
		///returns the fitted Y_data using X_data and the successfully interpolated fit_parameters;
		///returns empty if interpolation was not successfully
		std::vector<double> y_data(const std::vector<double>& x_data) const;
	};
	
	/// y = m*x
	/// n == 0
	class linear_t
	{
	private:
        class_logger_t logger;
		double chisq_p=-1;
		bool successfully_fitted_p=false;
		double cov_p;
		double chisq_relative_p=-1;
		double slope_p;
		double gof_p;
		bool fit(std::map<double,double> data_XY);
	public:
		linear_t(const double slope,const std::map<double,double>& data_XY);
		double chisq_relative() const;
        double cov() const;
		double slope() const;
		double chisq() const;
		bool successfully_fitted() const;
		double gof() const;
		std::vector<double> y_data(std::vector<double> x_data);
	};

    ///fits a data map to a target data map, using x_polynom for x direction and y_polynom for y direction
    class data_to_data_t
    {
    private:
//        class polynoms
//        {
//        private:
//            polynom_t X_p;
//            polynom_t Y_p;
//        public:
//            polynoms(const polynom_t& X, const polynom_t Y);
//            const polynom_t& get_X() const;
//            const polynom_t& get_Y() const;
//        };
        void gsl_vector_to_fit_parameters(const gsl_vector * x);
        gsl_vector fit_parameters_to_gsl_vector() const;
        class_logger_t class_logger;
//        std::map<double,double> target_map_p;
        std::vector<double> target_X;
        std::vector<double> target_Y;
        std::vector<double> data_Y;
        std::vector<double> data_X;
        polynom_t x_polynom_p;
        polynom_t y_polynom_p;
        static int function(const gsl_vector * x, void *data, gsl_vector * f);
        static void callback(const size_t iter, void *params,
                             const gsl_multifit_nlinear_workspace *w);
        ///returns y for given data, but within the x_range of base_map and x_resolution of target_map;
        ///applying x_polynom and y_polynom
        std::vector<double> y_data();
    public:
        ///polynom_s may include some start parameters
        data_to_data_t(const std::map<double,double>& target_map,
                       const polynom_t& x_polynom_s,
                       const polynom_t& y_polynom_s);
        const polynom_t& x_polynom();
        const polynom_t& y_polynom();
        ///tries to fit data to base_map
        bool fit(std::map<double,double> data);
    };
};

#endif // FIT_FUCTIONS
