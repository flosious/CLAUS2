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


const std::map<double, double> fit_functions::data_1D_to_2D(std::vector<double> data_1D)
{
	std::map<double,double> data_XY;
	std::vector<double> X_d(data_1D.size());
	
	for (int x=0;x<X_d.size();x++)
		X_d.at(x)=x;
	tools::vec::combine_vecs_to_map(&X_d,&data_1D,&data_XY);
	return data_XY;
}


/*************
 * asym2sig_t*
 *************/

fit_functions::asym2sig_t::asym2sig_t() : logger(__FILE__,"fit_functions::asym2sig_t")
{

}

double fit_functions::asym2sig_t::gof() const
{
	return gof_p;
}


double fit_functions::asym2sig_t::chisq() const
{
	return chisq_p;
}

double fit_functions::asym2sig_t::chisq0() const
{
	return chisq0_p;
}


int fit_functions::asym2sig_t::function(const gsl_vector * x, void *data, gsl_vector * f)
{
  std::map<double, double> *data_XY = ((std::map<double, double> *)data);

  // y0,m,A,xc,w1,w2,w3
	double y0 = gsl_vector_get (x, 0);
	double m = gsl_vector_get (x, 1);
	double A = gsl_vector_get (x, 2);
	double xc = gsl_vector_get (x, 3);
	double w1 = gsl_vector_get (x, 4);
	double w2 = gsl_vector_get (x, 5);
	double w3 = gsl_vector_get (x, 6);

  size_t i=0;

	for (std::map<double,double>::iterator XY=data_XY->begin();XY!=data_XY->end();++XY)
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


bool fit_functions::asym2sig_t::fit(std::map<double, double> data_XY, double y0_s, double xc_s, double m_s, double A_s, double w1_s, double w2_s, double w3_s)
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
	
	std::vector<double> x_,y_;
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
	fdf.df = NULL;   /* std::set to NULL for finite-difference Jacobian */
	fdf.fvv = NULL;     /* not using geodesic acceleration */
	fdf.n = n;
	fdf.p = p;
// 	fdf.params = &d;
	fdf.params = &data_XY;

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
	std::cout << "y0 =" << y0 << std::endl;
	std::cout << "m =" << m << std::endl;
	std::cout << "A =" << A << std::endl;
	std::cout << "xc =" << xc << std::endl;
	std::cout << "w1 =" << w1 << std::endl;
	std::cout << "w2 =" << w2 << std::endl;
	std::cout << "w3 =" << w3 << std::endl;
}

std::vector<double> fit_functions::asym2sig_t::fitted_y_data(std::vector<double> x)
{
	if (!fitted_p) return {};
	std::vector<double> y_data(x.size());
	for (int i=0;i<x.size();i++)
		y_data[i] = y0 + m*x[i] + A / (1+exp(-((x[i]-xc+w1/2)/w2)))*( 1-1/(1+exp(-((x[i]-xc-w1/2)/w3))) );
	return y_data;
}

bool fit_functions::asym2sig_t::fitted() const
{
	return fitted_p;
}

std::string fit_functions::asym2sig_t::to_string(std::string prefix) const
{
	if (!fitted()) return "";
	std::stringstream ss;
	ss << prefix;
	ss <<"gof = " << gof() << "; chisq = " << chisq() << "; y0 = " << y0 << "; m = " << m << "; A = " << A << "; xc = " << xc << "; w1 = " << w1 << "; w2 = " << w2 << "; w3 = " << w3;
	return ss.str();
}


/************
 **POLYNOM***
 ************/

fit_functions::polynom_t::polynom_t(const std::vector<double>& fit_parameters) : fit_parameters_p(fit_parameters), logger(__FILE__,"fit_functions::polynom_t")
{
	successfully_fitted_p= true;
	rank_p.resize(fit_parameters.size(),0);
}

fit_functions::polynom_t::polynom_t(const std::vector<unsigned int> rank, 
									const std::vector<double>& fit_parameters, 
									const std::map<double, double>& data_XY) : 
                                    fit_parameters_p(fit_parameters), rank_p(rank),
                                    logger(__FILE__,"fit_functions::polynom_t")
{
	successfully_fitted_p = fit(data_XY); //fit the data only once
}

fit_functions::polynom_t::polynom_t(const std::vector<unsigned int> rank, 
									const std::vector<double>& fit_parameters, 
									const std::vector<double>& data) :
									polynom_t(rank,fit_parameters,fit_functions::data_1D_to_2D(data))
{
}

fit_functions::polynom_t::polynom_t(int degree, const std::map<double, double>& data_XY) : 
									polynom_t(std::vector<unsigned int>(degree+1,1),std::vector<double>(degree+1,1),data_XY)
{
}

fit_functions::polynom_t::polynom_t(int degree, const std::vector<double>& data) : 
									polynom_t(degree+1, fit_functions::data_1D_to_2D(data))
{
}

double fit_functions::polynom_t::gof() const
{
	return gof_p;
}

int fit_functions::polynom_t::degree() const
{
// 	int c=0;
// 	for (auto r : rank_p)
// 		if (r!=0)
// 			c++;
	return rank().size();
}



const std::vector<unsigned int>& fit_functions::polynom_t::rank() const
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
		std::vector<double> derived_parameters(p-1);
		for (int i=1;i<p;i++) {
			derived_parameters.at(i-1) = (i*deriv.fit_parameters()[i]);
		}
		deriv.fit_parameters_p = derived_parameters;
	}
	
	return deriv;
}

std::vector<double> fit_functions::polynom_t::y_data(const std::vector<double>& x_data) const
{
// 	if (!successfully_fitted()) 
// 		return {};
	std::vector<double> Y(x_data.size());
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
	std::stringstream out;
	if (!successfully_fitted())
        out << "fit: NOT successfull,";
    out << " gof: " << gof();
	out << " chisq_relative: " << chisq_relative();
	out << " chisq: " << chisq();
	out << " params: ";
	for (auto p : fit_parameters())
	{
		out << p<<", ";
	}
	return out.str();
}



double fit_functions::polynom_t::chisq() const
{
	return chisq_p;
}

double fit_functions::polynom_t::chisq_relative() const
{
	return chisq_relative_p;
}

const std::vector<double>& fit_functions::polynom_t::fit_parameters() const
{
	return fit_parameters_p;
}


bool fit_functions::polynom_t::fit(std::map<double,double> data_XY)
{
// 	std::cout << "rank.size(): " << rank.size() << std::endl;
// 	std::cout << "data_XY.size(): " << data_XY.size() << std::endl;
// 	print(data_XY);
	if (rank().size()==0) 
		return false;
	if (data_XY.size()==0)
		return false;
	
	/*catch a linear function*/
	if (rank().size()==2 && rank().at(0)==0 && data_XY.size()>0) // linear m*x
	{
		linear_t L(fit_parameters().at(1), data_XY);
		fit_parameters_p = {0,L.slope()};
		successfully_fitted_p = L.successfully_fitted();
		chisq_p = L.chisq();
		gof_p = L.gof();
		return L.successfully_fitted();
	}
	
	if (rank().size()>data_XY.size())
	{
		return false;
	}
	
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
// 		std::cout << "i="<<i<<"\tc=" << *c->data << std::endl;
	}
    
	std::vector<double> Ydata;
	std::vector<double> Xdata;
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
	
	std::vector<double> YY = y_data(Xdata);
	gof_p = statistics::round(pow(statistics::get_correlation_Y1_Y2(Ydata,YY),10),1);
	
	chisq_relative_p = 0;
	
// 	std::cout << std::endl << "chisq_relative_p: " << chisq_relative_p << std::endl;
	for (int i=0;i<YY.size();i++)
	{
		chisq_relative_p +=  (abs(YY.at(i)-Ydata.at(i)))/Ydata.at(i);
	}
	
// 	chisq_relative_p = chisq_relative_p / (Xdata.size());
	
	return true;
}


/**********************/
/****  linear_t  ******/
/**********************/

fit_functions::linear_t::linear_t(const double slope,const std::map<double,double>& data_XY) : slope_p(slope), logger(__FILE__,"fit_functions::linear_t")
{	
	successfully_fitted_p = fit(data_XY);
}

double fit_functions::linear_t::gof() const
{
	return gof_p;
}

double fit_functions::linear_t::chisq_relative() const
{
	return chisq_relative_p;
}

double fit_functions::linear_t::cov() const
{
	return cov_p;
}

double fit_functions::linear_t::slope() const
{
	return slope_p;
}

double fit_functions::linear_t::chisq() const
{
	return chisq_p;
}

bool fit_functions::linear_t::successfully_fitted() const
{
	return successfully_fitted_p;
}

std::vector<double> fit_functions::linear_t::y_data(std::vector<double> x_data)
{
	std::vector<double> Y(x_data.size());
	for (int i=0;i<x_data.size();i++)
	{
		Y[i] = x_data[i] * slope();
	}
	return Y;
}

bool fit_functions::linear_t::fit(std::map<double, double> data_XY)
{
// 	double cov01, cov11;
	size_t n = data_XY.size();
	if (n==0) 
		return false;

	double x[n], y[n];
	int i=0;
	for (auto& xy : data_XY)
    {
		x[i] = xy.first;
		y[i] = xy.second;
		i++;
	}
	gsl_fit_mul(x,1,y,1,n,&slope_p,&cov_p,&chisq_p);
	successfully_fitted_p = true;
	
	std::vector<double> Ydata;
	std::vector<double> Xdata;
	tools::vec::split_map_to_vecs(data_XY,&Xdata,&Ydata);
	std::vector<double> YY = y_data(Xdata);
	gof_p = statistics::round(pow(statistics::get_correlation_Y1_Y2(Ydata,YY),10),1);
	chisq_relative_p = 0;
	
	for (int i=0;i<YY.size();i++)
	{
		chisq_relative_p +=  (abs(YY.at(i)-Ydata.at(i)))/Ydata.at(i);
	}
	return true;
}



/****************************/
/****  data_to_data_t  ******/
/****************************/

void fit_functions::data_to_data_t::callback(const size_t iter, void *params, const gsl_multifit_nlinear_workspace *w)
{
    gsl_vector *f = gsl_multifit_nlinear_residual(w);
    gsl_vector *x = gsl_multifit_nlinear_position(w);
    double rcond;

    /* compute reciprocal condition number of J(x) */
    gsl_multifit_nlinear_rcond(&rcond, w);

    fprintf(stderr, "iter %2zu: A, cond(J) = %8.4f, |f(x)| = %.4f\n",
            iter,
            1.0 / rcond,
            gsl_blas_dnrm2(f));
}

std::vector<double> fit_functions::data_to_data_t::y_data()
{
    auto shifted_data_X = x_polynom().y_data(data_X); // transform into target_X space
    auto shifted_data_Y = y_polynom().y_data(data_Y); // transform into target_Y space
    std::cout << "shifted_data_X\shifted_data_Y"   << std::endl;
    for (int i=0;i<shifted_data_X.size();i++)
        std::cout << shifted_data_X.at(i) << "\t" << shifted_data_Y.at(i) << std::endl;
    std::cout << "shifted_data_X.size(): " << shifted_data_X.size() << std::endl;
    std::cout << "shifted_data_Y.size(): " << shifted_data_Y.size() << std::endl;
    return statistics::interpolate_data_XY(shifted_data_X,shifted_data_Y,target_X); // map to target_X
}

fit_functions::data_to_data_t::data_to_data_t(const std::map<double,double>& target_map,
                                              const polynom_t& x_polynom_s,
                                              const polynom_t& y_polynom_s) :
    x_polynom_p(x_polynom_s),y_polynom_p(y_polynom_s), log_c
{
    tools::vec::split_map_to_vecs(target_map,&target_X,&target_Y);
}


const fit_functions::polynom_t& fit_functions::data_to_data_t::x_polynom()
{
    return x_polynom_p;
}

const fit_functions::polynom_t& fit_functions::data_to_data_t::y_polynom()
{
    return y_polynom_p;
}

/*************************************/
/*******   data_to_data_t  ***********/
/*************************************/


int fit_functions::data_to_data_t::function(const gsl_vector * x, void *data, gsl_vector * f)
{
    data_to_data_t* data_to_data = static_cast<data_to_data_t*>(data);

    //fit parameters
//    double y0 = gsl_vector_get (x, 0);
//    double m = gsl_vector_get (x, 1);
//    double A = gsl_vector_get (x, 2);
//    double xc = gsl_vector_get (x, 3);
//    double w1 = gsl_vector_get (x, 4);
//    double w2 = gsl_vector_get (x, 5);
//    double w3 = gsl_vector_get (x, 6);
    data_to_data->gsl_vector_to_fit_parameters(x);
    auto new_Y = data_to_data->y_data();
    for (size_t i=0;i<new_Y.size();i++)
    {
//        gsl_vector_set (f, i, log10(new_Y.at(i)) - log10(data_to_data->target_Y.at(i)));
        gsl_vector_set (f, i, new_Y.at(i) - data_to_data->target_Y.at(i));
    }

  return GSL_SUCCESS;
}

void fit_functions::data_to_data_t::gsl_vector_to_fit_parameters(const gsl_vector *x)
{
    const size_t X = x_polynom().fit_parameters().size();
    const size_t Y = y_polynom().fit_parameters().size();

    std::vector<double> fit_params_x(X);
    for (size_t i=0; i < X; i++)
    {
        fit_params_x.at(i) = gsl_vector_get(x,i);
    }
    x_polynom_p = polynom_t(fit_params_x);

    std::vector<double> fit_params_y(Y);
    for (size_t i=X; i < Y; i++)
    {
        fit_params_y.at(i-X) = gsl_vector_get(x,i);
    }
    y_polynom_p = polynom_t(fit_params_y);
}

gsl_vector fit_functions::data_to_data_t::fit_parameters_to_gsl_vector() const
{
    gsl_vector x;
}

bool fit_functions::data_to_data_t::fit(std::map<double,double> data)
{
    tools::vec::split_map_to_vecs(data,&data_X,&data_Y);
    const gsl_multifit_nlinear_type *T = gsl_multifit_nlinear_trust;
    gsl_multifit_nlinear_workspace *w;
    gsl_multifit_nlinear_fdf fdf;
    gsl_multifit_nlinear_parameters fdf_params = gsl_multifit_nlinear_default_parameters();
    const size_t n = data.size();
    const size_t p = y_polynom().degree() + x_polynom().degree(); //sum of x_polynom + y_polynom parameters

    gsl_vector *f;
    gsl_matrix *J;
    gsl_matrix *covar = gsl_matrix_alloc (p, p);
    double t[n], y[n], weights[n];
//      struct data d = { n, t, y };
    std::vector<double> xy_init = x_polynom().fit_parameters();
    std::vector<double> y_init = y_polynom().fit_parameters();
    xy_init.insert(xy_init.end(),y_init.begin(),y_init.end());
//      double x_init[3] = { 1.0, 1.0, 0.0 }; /* starting values */
    gsl_vector_view x = gsl_vector_view_array (&xy_init[0], p);
    gsl_vector_view wts = gsl_vector_view_array(weights, n);
    gsl_rng * r;
    double chisq, chisq0;
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
    fdf.params = this;
    for (i = 0; i < n; i++)
        weights[i] = 1.0;
    /* allocate workspace with default parameters */
    w = gsl_multifit_nlinear_alloc (T, &fdf_params, n, p);
    /* initialize solver with starting point and weights */
    gsl_multifit_nlinear_winit (&x.vector, &wts.vector, &fdf, w);

    /* compute initial cost function */
    f = gsl_multifit_nlinear_residual(w);
    gsl_blas_ddot(f, f, &chisq0);
    /* solve the system with a maximum of 100 iterations */
    status = gsl_multifit_nlinear_driver(100, xtol, gtol, ftol,
                                         callback, NULL, &info, w);

    /* compute covariance of best fit parameters */
    J = gsl_multifit_nlinear_jac(w);
    gsl_multifit_nlinear_covar (J, 0.0, covar);
    /* compute final cost */
    gsl_blas_ddot(f, f, &chisq);

    gsl_vector_to_fit_parameters(w->x);


    gsl_multifit_nlinear_free (w);
    gsl_matrix_free (covar);
    gsl_rng_free (r);

    return 0;
}




