/*
	Copyright (C) 2020 Florian Bärwolf
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
#include "fit_functions.hpp"


const map<double, double> fit_functions::data_1D_to_2D(vector<double> data_1D)
{
	map<double,double> data_XY;
	vector<double> X_d(data_1D.size());
	
	for (int x=0;x<X_d.size();x++)
		X_d.at(x)=x;
	tools::vec::combine_vecs_to_map(&X_d,&data_1D,&data_XY);
	return data_XY;
}


/*************
 * asym2sig_t*
 *************/

double fit_functions::asym2sig_t::gof()
{
	return gof_p;
}


double fit_functions::asym2sig_t::chisq()
{
	return chisq_p;
}

double fit_functions::asym2sig_t::chisq0()
{
	return chisq0_p;
}


int fit_functions::asym2sig_t::function(const gsl_vector * x, void *data, gsl_vector * f)
{
  map<double, double> *data_XY = ((map<double, double> *)data);

  // y0,m,A,xc,w1,w2,w3
	double y0 = gsl_vector_get (x, 0);
	double m = gsl_vector_get (x, 1);
	double A = gsl_vector_get (x, 2);
	double xc = gsl_vector_get (x, 3);
	double w1 = gsl_vector_get (x, 4);
	double w2 = gsl_vector_get (x, 5);
	double w3 = gsl_vector_get (x, 6);

  size_t i=0;

	for (map<double,double>::iterator XY=data_XY->begin();XY!=data_XY->end();++XY)
    {
      /* Model Yi = A * exp(-lambda * t_i) + b */
//       double Yi = A * exp (-lambda * t[i]) + b;
		double Yi = y0 + m*XY->first + A/(1+exp(-((XY->first-xc+w1/2)/w2)))*( 1-1/(1+exp(-((XY->first-xc-w1/2)/w3))) );
      gsl_vector_set (f, i, Yi - XY->second);
	  i++;
    }

  return GSL_SUCCESS;
}

void fit_functions::asym2sig_t::callback(const size_t iter, void *params, const gsl_multifit_nlinear_workspace *w)
{
  gsl_vector *f = gsl_multifit_nlinear_residual(w);
  gsl_vector *x = gsl_multifit_nlinear_position(w);
  double rcond;

  /* compute reciprocal condition number of J(x) */
  gsl_multifit_nlinear_rcond(&rcond, w);

  fprintf(stderr, "iter %2zu: A = %.4f, lambda = %.4f, b = %.4f, cond(J) = %8.4f, |f(x)| = %.4f\n",
          iter,
          gsl_vector_get(x, 0),
          gsl_vector_get(x, 1),
          gsl_vector_get(x, 2),
          1.0 / rcond,
          gsl_blas_dnrm2(f));
}


bool fit_functions::asym2sig_t::fit(map<double, double> data_XY, double y0_s, double xc_s, double m_s, double A_s, double w1_s, double w2_s, double w3_s)
{
	// maximum time for calculation?

	const gsl_multifit_nlinear_type *T = gsl_multifit_nlinear_trust;
	gsl_multifit_nlinear_workspace *w;
	gsl_multifit_nlinear_fdf fdf;
	gsl_multifit_nlinear_parameters fdf_params = gsl_multifit_nlinear_default_parameters();
	// number of data points to fit
	int N = data_XY.size();
	const size_t n = N;
	// number of fit parameters
	const size_t p = 7;

	gsl_vector *f;
	gsl_matrix *J;
	gsl_matrix *covar = gsl_matrix_alloc (p, p);
	double /* t[N], y[N],*/ weights[N];
// 	struct data d = { n, t, y };
	
	vector<double> x_,y_;
	tools::vec::split_map_to_vecs(data_XY,&x_,&y_);
	
	/* starting values */
// 	double xc_s,y0_s,A_s,m_s,w1_s;
	if (xc_s!=xc_s) xc_s = data_XY.rbegin()->first/2;
	if (y0_s!=y0_s) y0_s = y_[0];
	if (A_s!=A_s) A_s = y0+y_[y_.size()/2];
	if (m_s!=m_s) m_s = (y_.back()-y_[0])/(x_.back()-x_[0]);
	if (w1_s!=w1_s) w1_s = 0.5*x_.back();
	if (w2_s!=w2_s) w2_s = 10;
	if (w3_s!=w3_s) w3_s = 10;
			
	double x_init[p] = { y0_s, m_s, A_s, xc_s, w1_s,w2_s,w3_s}; 
	/******************/
	gsl_vector_view x = gsl_vector_view_array (x_init, p);
	gsl_vector_view wts = gsl_vector_view_array(weights, n);
	gsl_rng * r;
	int status, info;
	size_t i;
	const double xtol = 1e-8;
	const double gtol = 1e-8;
	const double ftol = 0.0;
	gsl_rng_env_setup();
	r = gsl_rng_alloc(gsl_rng_default);

	/* define the function to be minimized */
	fdf.f = function;
	fdf.df = NULL;   /* set to NULL for finite-difference Jacobian */
	fdf.fvv = NULL;     /* not using geodesic acceleration */
	fdf.n = n;
	fdf.p = p;
// 	fdf.params = &d;
	fdf.params = &data_XY;
	
	/* this is the data to be fitted */
	for (i = 0; i < n; i++)
		weights[i] = 1.0;


	/* allocate workspace with default parameters */
	w = gsl_multifit_nlinear_alloc (T, &fdf_params, n, p);
	/* initialize solver with starting point and weights */
	gsl_multifit_nlinear_winit (&x.vector, &wts.vector, &fdf, w);

	/* compute initial cost function */
	f = gsl_multifit_nlinear_residual(w);
	gsl_blas_ddot(f, f, &chisq0_p);

	/* solve the system with a maximum of 100 iterations */
	status = gsl_multifit_nlinear_driver(200, xtol, gtol, ftol, NULL, NULL, &info, w);

	/* compute covariance of best fit parameters */
	J = gsl_multifit_nlinear_jac(w);
	gsl_multifit_nlinear_covar (J, 0.0, covar);

	/* compute final cost */
	gsl_blas_ddot(f, f, &chisq_p);

	#define FIT(i) gsl_vector_get(w->x, i)
	#define ERR(i) sqrt(gsl_matrix_get(covar,i,i))
	
	/* save fit parameters*/
	y0 = FIT(0);
	m = FIT(1);
	A = FIT(2);
	xc = FIT(3);
	w1 = FIT(4);
	w2 = FIT(5);
	w3 = FIT(6);
	fitted_p = true;
	
	/*calc gof_p*/
// 	gof_p=0;
// 	for (auto& i:data_XY)
// 	{
// 		double Y = fitted_y_data({i.first})[0];
// 		gof_p = gof_p + abs((i.second-Y)/Y);
// 	}
// 	gof_p = gof_p/N;
// 	gof_p = 1-pow(gof_p,1/2)/N;
	
	gof_p = pow(statistics::get_correlation_Y1_Y2(y_,fitted_y_data(x_)),50);

	gsl_multifit_nlinear_free (w);
	gsl_matrix_free (covar);
	gsl_rng_free (r);

	return true;
}

void fit_functions::asym2sig_t::print_parameters()
{
	cout << "y0 =" << y0 << endl;
	cout << "m =" << m << endl;
	cout << "A =" << A << endl;
	cout << "xc =" << xc << endl;
	cout << "w1 =" << w1 << endl;
	cout << "w2 =" << w2 << endl;
	cout << "w3 =" << w3 << endl;
}

vector<double> fit_functions::asym2sig_t::fitted_y_data(vector<double> x)
{
	if (!fitted_p) return {};
	vector<double> y_data(x.size());
	for (int i=0;i<x.size();i++)
		y_data[i] = y0 + m*x[i] + A / (1+exp(-((x[i]-xc+w1/2)/w2)))*( 1-1/(1+exp(-((x[i]-xc-w1/2)/w3))) );
	return y_data;
}

bool fit_functions::asym2sig_t::fitted()
{
	return fitted_p;
}

string fit_functions::asym2sig_t::to_string(string prefix)
{
	if (!fitted()) return "";
	stringstream ss;
	ss << prefix;
	ss <<"gof = " << gof() << "; chisq = " << chisq() << "; y0 = " << y0 << "; m = " << m << "; A = " << A << "; xc = " << xc << "; w1 = " << w1 << "; w2 = " << w2 << "; w3 = " << w3;
	return ss.str();
}


/************
 **POLYNOM***
 ************/

fit_functions::polynom_t::polynom_t(const vector<unsigned int> rank, 
									const vector<double>& fit_parameters, 
									const map<double, double>& data_XY) : 
									fit_parameters_p(fit_parameters), rank_p(rank)
{
	successfully_fitted_p = fit(data_XY); //fit the data only once
}

fit_functions::polynom_t::polynom_t(const vector<unsigned int> rank, 
									const vector<double>& fit_parameters, 
									const vector<double>& data) :
									polynom_t(rank,fit_parameters,fit_functions::data_1D_to_2D(data))
{
}

fit_functions::polynom_t::polynom_t(int degree, const map<double, double>& data_XY) : 
									polynom_t(vector<unsigned int>(degree+1,1),vector<double>(degree+1,1),data_XY)
{
}

fit_functions::polynom_t::polynom_t(int degree, const vector<double>& data) : 
									polynom_t(degree+1, fit_functions::data_1D_to_2D(data))
{
}

int fit_functions::polynom_t::degree() const
{
	return rank_p.size()-1;
}

const vector<unsigned int>& fit_functions::polynom_t::rank() const
{
	return rank_p;
}

const fit_functions::polynom_t fit_functions::polynom_t::derivative(unsigned int derive) const
{
	polynom_t deriv = *this;
	if (derive == 0) return deriv;
	if (derive >= fit_parameters().size())
	{
		deriv.fit_parameters_p={0};
		return deriv;
	}
	
	for (int j=0;j<derive;j++)
	{
		int p = deriv.fit_parameters().size();
		vector<double> derived_parameters(p-1);
		for (int i=1;i<p;i++) {
			derived_parameters.at(i-1) = (i*deriv.fit_parameters()[i]);
		}
		deriv.fit_parameters_p = derived_parameters;
	}
	
	return deriv;
}

vector<double> fit_functions::polynom_t::y_data(const vector<double>& x_data) const
{
	if (!successfully_fitted()) return {};
	vector<double> Y(x_data.size());
	for (int i=0;i<x_data.size();i++)
	{
		Y[i]=fit_parameters()[0];
		for (int p=1;p<fit_parameters().size();p++)
		{
			Y[i]+=fit_parameters()[p]*pow(x_data[i],p);
		}
	}
	return Y;
}

bool fit_functions::polynom_t::successfully_fitted() const
{
	return successfully_fitted_p;
}

std::string fit_functions::polynom_t::to_string(std::string prefix) const
{
	stringstream out;
	if (!successfully_fitted())
		return "unsecesfull fit";
	out << "chisq: " << chisq();
	out << " params: ";
	for (auto p : fit_parameters())
	{
		out << p<<", ";
	}
	return out.str();
}



const double fit_functions::polynom_t::chisq() const
{
	return chisq_p;
}

double fit_functions::polynom_t::chisq_relative() const
{
	return chisq_relative_p;
}

const vector<double>& fit_functions::polynom_t::fit_parameters() const
{
	return fit_parameters_p;
}


bool fit_functions::polynom_t::fit(map<double,double> data_XY)
{
// 	cout << "rank.size(): " << rank.size() << endl;
// 	cout << "data_XY.size(): " << data_XY.size() << endl;
// 	print(data_XY);
	if (rank().size()==0) 
		return false;
	if (data_XY.size()==0)
		return false;
	if (rank().size()>data_XY.size()) 
		return false;
	
	
	
	int i, n;
	gsl_matrix *X, *cov;
	gsl_vector *y, *w, *c;
	const int p = rank().size();
    
	n = data_XY.size();
    
	X = gsl_matrix_alloc (n, p);
	y = gsl_vector_alloc (n);
	w = gsl_vector_alloc (n);
	cov = gsl_matrix_alloc (p, p);
	c = gsl_vector_alloc (p);

	for (int i=0;i<fit_parameters_p.size();i++)
	{
		gsl_vector_set (c, i, fit_parameters_p.at(i));
// 		cout << "i="<<i<<"\tc=" << *c->data << endl;
	}
    
	vector<double> Ydata;
	vector<double> Xdata;
	tools::vec::split_map_to_vecs(data_XY,&Xdata,&Ydata);
	
	chisq_p=-1;
	
	for (i = 0; i < n; i++)
    {
		gsl_vector_set (y, i, Ydata[i]);
		for (int ii=0;ii<p;ii++)  // create reference data (polynom)
		{
			if (rank().at(ii)==0)
				gsl_matrix_set (X, i, ii, 0);
			else
				gsl_matrix_set (X, i, ii, pow(Xdata[i],ii));
		}
    }

	{
		gsl_multifit_linear_workspace * work = gsl_multifit_linear_alloc (n, p);
		gsl_multifit_linear (X, y, c, cov, &chisq_p, work);
		gsl_multifit_linear_free (work);
	}

#define C(i) (gsl_vector_get(c,(i)))
#define COV(i,j) (gsl_matrix_get(cov,(i),(j)))
    for (int ii=0;ii<p;ii++) {
        fit_parameters_p.at(ii)=C(ii);
    }
    gsl_matrix_free (X);
    gsl_vector_free (y);
    gsl_vector_free (w);
    gsl_vector_free (c);
    gsl_matrix_free (cov);
	
	chisq_relative_p =-1;
	vector<double> YY = y_data(Xdata);
	for (int i=0;i<YY.size();i++)
	{
		chisq_relative_p +=  (abs(YY.at(i)-Ydata.at(i)))/Ydata.at(i);
	}
	chisq_relative_p = chisq_relative_p / (Xdata.size());
	
	return true;
}


/**********************/
/****  linear_t  ******/
/**********************/

const double fit_functions::linear_t::cov() const
{
	return cov_p;
}

const double fit_functions::linear_t::slope() const
{
	return slope_p;
}

double fit_functions::linear_t::chisq() const
{
	return chisq_p;
}

bool fit_functions::linear_t::fitted() const
{
	return fitted_p;
}

vector<double> fit_functions::linear_t::Y(vector<double> x)
{
	if (!fitted_p) return {};
	vector<double> Y(x.size());
	for (int i=0;i<x.size();i++)
	{
		Y[i] = x[i] * slope();
	}
	return Y;
}

bool fit_functions::linear_t::fit(map<double, double> data_XY)
{
// 	double cov01, cov11;
	size_t n = data_XY.size();
	double x[n], y[n];
	int i=0;
	for (auto& xy : data_XY)
    {
		x[i] = xy.first;
		y[i] = xy.second;
		i++;
	}
	gsl_fit_mul(x,1,y,1,n,&slope_p,&cov_p,&chisq_p);
	fitted_p = true;
	return true;
}

