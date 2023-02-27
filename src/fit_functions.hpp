
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
protected:
    class basics_t
    {
    private:
        class_logger_t class_logger;
    protected:
        double chisq_p=-1;
        double chisq0_p=-1;
        double gof_p=-1;
        bool fitted_p=false;
    public:
        basics_t();
        bool fitted() const;
        double chisq() const;
        double chisq0() const;
        double gof() const;
        virtual std::string to_string(std::string prefix="") const;
    };
public:

	static const std::map<double,double> data_1D_to_2D(std::vector<double> data_1D);
//    y = y0 +A [ 2m/Pi w/( 4*(x-x_c)^2 +w^2 ) + (1-m) sqrt(4ln(2)) / (4*sqrt(pi)) exp(-4ln(2)/w^2 (x-x_c)^2) ]
    class pseudo_voigt_t : public basics_t
    {
    private:
        class_logger_t class_logger;
        static void callback(const size_t iter, void *params, const gsl_multifit_nlinear_workspace *w);
        static int function(const gsl_vector * x, void *data, gsl_vector * f);
        std::vector<double> fit_parameters_to_std_vector() const;
        void vector_to_fit_parameters(const std::vector<double>& vec);
        std::vector<double> data_X;
        std::vector<double> data_Y;
        int info=-1;
    public:
        pseudo_voigt_t(double y0=0, double A=1, double m=0.25, double xc=1, double w=1);
        virtual ~pseudo_voigt_t();
        ///offset
        double y0=0;
        bool fit_y0=true;
        ///area
        double A=1;
        bool fit_A=true;
        ///gausian / lorentz linear mixture
        double m=0.25;
        bool fit_m=true;
        ///center
        double xc=1;
        bool fit_xc=true;
        ///width
        double w=1;
        bool fit_w=true;
        bool fit(const std::map<double, double>& data_XY);
        ///for large systems,... what is large?
        bool fit_large(const std::map<double, double>& data_XY);
        std::vector<double> y_data(const std::vector<double>& x) const;
        std::string to_string(std::string prefix="") const override;
        std::string stop_info() const;
    };

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
	    class_logger_t class_logger;
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
	    polynom_t(const std::vector<unsigned int> rank, const std::vector<double>& fit_parameters);
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
    class data_to_target_t
    {
    private:
//        void gsl_vector_to_fit_parameters(const gsl_vector * v, size_t v_start_idx);
        void gsl_vector_to_fit_parameters(const gsl_vector * v);
        gsl_vector* fit_parameters_to_gsl_vector() const;
        std::vector<double> fit_parameters_to_std_vector() const;
        const std::vector<double> fit_parameters_to_std_vector(const fit_functions::polynom_t& polynom) const;
        class_logger_t class_logger;

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
        data_to_target_t(const std::map<double,double>& target_map,
                       const polynom_t& x_polynom_s,
                       const polynom_t& y_polynom_s);
        bool fit_x_axis() const;
        const polynom_t& x_polynom() const;
        const polynom_t& y_polynom() const;
        ///tries to fit data to base_map
        bool fit(std::map<double,double> data);
    };
};

#endif // FIT_FUCTIONS
