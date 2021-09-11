
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

using namespace std;

class fit_functions
{
public:
	/// like origin -> peak functions -> asym2sig
	class asym2sig_t
	{
	private:
		double chisq_p=-1;
		double chisq0_p=-1;
		double gof_p=-1;
		bool fitted_p=false;
		static void callback(const size_t iter, void *params, const gsl_multifit_nlinear_workspace *w);
// 		static int function_1st_derivative(const gsl_vector * x, void *data, gsl_matrix * J);
		static int function(const gsl_vector * x, void *data, gsl_vector * f);
	public:
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
		vector<double> fitted_y_data(vector<double> x);
		/// calculation of fit parameters
		bool fit(map<double, double> data_XY, double y0_s=NAN, double xc_s=NAN, double m_s=NAN, double A_s=NAN, double w1_s=NAN, double w2_s=NAN, double w3_s=NAN);
		bool fitted();
		void print_parameters();
		double chisq();
		double chisq0();
		double gof();
		string to_string(string prefix="");
	};
	/*
	 * Y = SUM_i ( c_j * x_j * rank_j )
	 */
	class polynom_t
	{
	private:
		double chisq_p=-1;
		/// parameters
		bool fitted_p=false;
		vector<double> Xdata;
		vector<double> fit_parameters_s;
		
	public:
		///degree means rank
		polynom_t(int degree);
		polynom_t(const vector<unsigned int> rank, vector<double> fit_parameters);
// 		polynom_t(vector<double> fit_parameters);
// 		polynom_t(const vector<unsigned int> rank);
		///rank of polynom: 0 means discard, and non-0 means use
		const vector<unsigned int> rank;
		int degree() const;
		polynom_t derivative(unsigned int derive=1);
		double chisq() const;
		const vector<double>& fit_parameters();
		///performs the actual fit
		bool fit(map<double,double> data_XY);
		///performs the actual fit
		bool fit(vector<double> Ydata);
		///was data successfully fitted?
		bool fitted() const;
		string to_string(string prefix="");
		vector<double> fitted_y_data(vector<double> x={}) const;
	};
	
	///y=m*x
	class linear_t
	{
	private:
		double chisq_p=-1;
		bool fitted_p=false;
	public:
		double cov;
		double slope;
		double chisq();
		bool fit(map<double,double> data_XY);
		bool fitted();
		vector<double> fitted_y_data(vector<double> x={});
	};
};

#endif // FIT_FUCTIONS
