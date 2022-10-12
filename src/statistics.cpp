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
#include "statistics.hpp"

void statistics::test()
{
	int N = 2000;
	int NCOEFFS = 200;
	int NBREAK = NCOEFFS -2 ;
  const size_t n = N;
  const size_t ncoeffs = NCOEFFS;
  const size_t nbreak = NBREAK;
  size_t i, j;
  gsl_bspline_workspace *bw;
  gsl_vector *B;
  double dy;
  gsl_rng *r;
  gsl_vector *c, *w;
  gsl_vector *x, *y;
  gsl_matrix *X, *cov;
  gsl_multifit_linear_workspace *mw;
  double chisq, Rsq, dof, tss;

  gsl_rng_env_setup();
  r = gsl_rng_alloc(gsl_rng_default);

  /* allocate a cubic bspline workspace (k = 4) */
  bw = gsl_bspline_alloc(4, nbreak);
  B = gsl_vector_alloc(ncoeffs);

  x = gsl_vector_alloc(n);
  y = gsl_vector_alloc(n);
  X = gsl_matrix_alloc(n, ncoeffs);
  c = gsl_vector_alloc(ncoeffs);
  w = gsl_vector_alloc(n);
  cov = gsl_matrix_alloc(ncoeffs, ncoeffs);
  mw = gsl_multifit_linear_alloc(n, ncoeffs);
// 	double TT=(15.0 / (N - 1))+0.05;
  double TT = 0.1;
  /* this is the data to be fitted */
  for (i = 0; i < n; ++i)
// 		i=0;
// 	for (double xi = 0.0; xi <= 15.0; xi += 0.1)
    {
// 		std::cout << "A" << std::endl;
      double sigma;
//       double xi = (15.0 / (N - 1)) * i;
	  double xi = TT*i;
      double yi = cos(xi) * exp(-0.1 * xi);

      sigma = 0.1 * yi;
      dy = gsl_ran_gaussian(r, sigma);
      yi += dy;

//       gsl_vector_set(x, i, xi);
	  gsl_vector_set(x, i, i);
      gsl_vector_set(y, i, yi);
      gsl_vector_set(w, i, 1.0 / (sigma * sigma));
// 		i++;
      printf("%f %f\n", xi, yi);
    }
    /************************/
	std::vector<double> Xin, Yin;
	Xin = get_gsl_vec(x);
	Yin = get_gsl_vec(y);
	/************************/
	
  /* use uniform breakpoints on [0, 15] */
  gsl_bspline_knots_uniform(Xin.front(), Xin.back(), bw);

  /* construct the fit matrix X */
  for (i = 0; i < n; ++i)
    {
      double xi = gsl_vector_get(x, i);
// 		std::cout << i << std::endl;
      /* compute B_j(xi) for all j */
      gsl_bspline_eval(xi, B, bw);

      /* fill in row i of X */
      for (j = 0; j < ncoeffs; ++j)
        {
          double Bj = gsl_vector_get(B, j);
          gsl_matrix_set(X, i, j, Bj);
        }
    }
// 	std::cout << "fit inc" << std::endl;
  /* do the fit */
  gsl_multifit_wlinear(X, w, y, c, cov, &chisq, mw);
// 	std::cout << "fit done" << std::endl;
  dof = n - ncoeffs;
  tss = gsl_stats_wtss(w->data, 1, y->data, 1, y->size);
  Rsq = 1.0 - chisq / tss;

//   fprintf(stderr, "chisq/dof = %e, Rsq = %f\n",
//                    chisq / dof, Rsq);
// 
//   printf("\n\n");

  
  
  /* output the smoothed curve */
  std::vector<double> Yfitted;
  std::vector<double> Xfitted;
    double xi, yi, yerr;

//     for (xi = 0.0; xi < 15.0; xi += 0.1)
	for (int j=0;j<Xin.size();j++)
      {
		  double xi = Xin.at(j);
        gsl_bspline_eval(xi, B, bw);
        gsl_multifit_linear_est(B, c, cov, &yi, &yerr);
        printf("%f %f\n", xi, yi);
		Yfitted.push_back(yi);
		Xfitted.push_back(xi);
      }
  
// 	plot_t plot(false,false);
// 	plot.Y1.range(-1.2,1.2,false);
// 	plot.Y2.range(-1.2,1.2,false);
// 	plot.Y1.add_curve(Xin,Yin);
// 	plot.Y2.add_curve(Xfitted,Yfitted);
// 	std::cout << "Yin.size()=" << Yin.size() << "\t" << "Yfitted.size()=" << Yfitted.size() << std::endl;
// 	plot.to_screen("",0);
	
  gsl_rng_free(r);
  gsl_bspline_free(bw);
  gsl_vector_free(B);
  gsl_vector_free(x);
  gsl_vector_free(y);
  gsl_matrix_free(X);
  gsl_vector_free(c);
  gsl_vector_free(w);
  gsl_matrix_free(cov);
  gsl_multifit_linear_free(mw);

}

bool statistics::get_extrema_indices(std::vector<int>& maxIdx, std::vector<int>& minIdx, const std::vector<double>& Y, double treshold)
{
	std::vector<int> local_extrema;
	
// 	std::vector<float> data(Y.size());
// 	for (int i=0;i<Y.size();i++)
// 		Y[i]=Y[i];
	
	//Run persistence on data - this is the main call.
    p1d::Persistence1D p;
    p.RunPersistence(Y);
	
	//Get all extrema with a persistence larger than 10.
    std::vector< p1d::TPairedExtrema > Extrema;
    if (treshold < 0) treshold =  get_mad_from_Y(Y);
	if (!p.GetPairedExtrema(Extrema, treshold)) return false;
	
	maxIdx.clear();
	minIdx.clear();
	maxIdx.resize(Extrema.size()+1);
	minIdx.resize(Extrema.size()+1);
	
	maxIdx[0]=get_max_index_from_Y(Y);
	minIdx[0]=p.GetGlobalMinimumIndex();
	
	int i=0;
	int s = minIdx.size();
	for(std::vector< p1d::TPairedExtrema >::iterator it = Extrema.begin(); it != Extrema.end(); it++)
	{
		i++;
		minIdx[s-i]=(*it).MinIndex;
		maxIdx[s-i]=(*it).MaxIndex;
	}
	
	 //Print all found pairs - pairs are sorted ascending wrt. persistence.
//     for(std::vector< p1d::TPairedExtrema >::iterator it = Extrema.begin(); it != Extrema.end(); it++)
//     {
//         std::cout << "Persistence: " << (*it).Persistence
// 			 << std::endl
//              << " minimum index: " << (*it).MinIndex << "\tvalue = " << Y[(*it).MinIndex]
//              << std::endl 
//              << " maximum index: " << (*it).MaxIndex << "\tvalue = " << Y[(*it).MaxIndex]
//              << std::endl;
//     }
	
    //Also, print the global minimum.
//     std::cout << "Global minimum index: " << p.GetGlobalMinimumIndex()
//          << " Global minimum value: " << p.GetGlobalMinimumValue() << std::endl;
		 
	return true;
}

int statistics::get_index_for_next_value_within_treshold(std::vector<double> Y, double treshold_bottom, double treshold_top, int start, int ende)
{
	if (ende==0) ende=Y.size();
	int index=0;

	for (int i=start;i<ende;i++)
	{
		index=i;
		if (Y[i]<treshold_top && Y[i]>=treshold_bottom) break;
	}
	return index;
}


int statistics::get_index_for_next_value_lower_than_treshold(std::vector<double> Y,double treshold, int start, int ende)
{
	if (ende==0) ende=Y.size();
	int index=0;
	for (int i=start;i<ende;i++)
	{
		index=i;
		if (Y[i]<treshold) break;
	}
	return index;
}

int statistics::get_index_for_next_value_higher_than_treshold(std::vector<double> Y,double treshold, int start, int ende)
{
	if (ende==0) ende=Y.size();
	int index=0;
	for (int i=start;i<ende;i++)
	{
		index=i;
		if (Y[i]>treshold) break;
	}
	return index;
}

std::vector<int> statistics::get_local_maxima_indices_by_triangulation(std::vector<double> Y, double treshold, int stepsize)
{
	if (Y.size()<3*stepsize) return {};
	std::vector<int> local_max_indices;
    std::vector<double> Y_max;
	for (int x=2*stepsize;x<Y.size();x=x+stepsize)
	{
		if (Y[x-stepsize]>Y[x-2*stepsize]+treshold && Y[x-stepsize]>Y[x]+treshold) local_max_indices.push_back(x-stepsize);
	}
	return local_max_indices;
}

std::vector<int> statistics::get_local_minima_indices_by_triangulation(std::vector<double> Y, double treshold, int stepsize)
{
	if (Y.size()<3*stepsize) return {};
	std::vector<int> local_min_indices;
    std::vector<double> Y_max;
	for (int x=3*stepsize;x<Y.size();x=x+stepsize)
	{
		if (Y[x-stepsize]<Y[x-2*stepsize]-treshold && Y[x-stepsize]<Y[x]-treshold) local_min_indices.push_back(x-stepsize);
	}
	return local_min_indices;
}

std::vector<int> statistics::get_local_maxima_indices(std::vector<double> Y, double treshold, int stepsize) {
    std::vector<int> local_max_indices;
    std::vector<double> Y_max;
    int Y_max_start_i;
    
    if (treshold==-1) treshold=statistics::get_median_from_Y(Y)+1.5*statistics::get_mad_from_Y(Y);

    for (int i=0;i<Y.size();i+=stepsize) { 
        if (Y[i]>=treshold ) { /*record entries*/
            if (Y_max.size()==0) Y_max_start_i=i;
            Y_max.push_back(Y[i]);
        } else { // outside of treshold
            if (Y_max.size()>0) { // record and reset
                int local_max_index = statistics::get_max_index_from_Y(Y_max) + Y_max_start_i;
                local_max_indices.push_back(local_max_index);
                Y_max.clear();
            } 
        }
    }
    return local_max_indices;
}

std::vector<int> statistics::get_local_minima_indices_new(std::vector<double> Y, double Y_tolerance, int x_stepsize)
{
	std::vector<int> local_minima_indices;
	if (Y.size()==0) return local_minima_indices;
	
// 	bool last_max=false;
	bool last_min=false;
	int last_min_pos=0;
	for (int x=x_stepsize;x<Y.size();x=x+x_stepsize)
	{
		if (Y[x]<Y[x-x_stepsize]) 
		{
			last_min=false;
		}
		else if (Y[x]>+Y_tolerance+Y[x-x_stepsize] && !last_min) 
		{
			last_min=true;
			local_minima_indices.push_back(x-x_stepsize/2);
		}
		
	}
	return local_minima_indices;
}

std::vector<int> statistics::get_local_minima_indices(std::vector<double> Y, double treshold, int stepsize) {
    std::vector<int> local_min_indices;
    std::vector<double> Y_min;
    int Y_min_start_i;
    
    if (treshold==-1) treshold=statistics::get_median_from_Y(Y)-1.5*statistics::get_mad_from_Y(Y);
    
    for (int i=0;i<Y.size();i+=stepsize) { 
        if (Y[i]<=treshold ) { /*record entries*/
            if (Y_min.size()==0) Y_min_start_i=i;
            Y_min.push_back(Y[i]);
        } else { // outside of treshold
            if (Y_min.size()>0) { // record and reset
                int local_min_index = statistics::get_min_index_from_Y(Y_min) + Y_min_start_i;
                local_min_indices.push_back(local_min_index);
                Y_min.clear();
            } 
        }
    }
    return local_min_indices;
}

std::vector<double> statistics::get_elements_higher_than_threshold(std::vector<double> Y, double treshold) {
    std::vector<double> Y_res;
    for (int i=0;i<Y.size();i++) { 
        if (Y[i]>treshold) Y_res.push_back(Y[i]);
    }
    return Y_res;
}

std::vector<int> statistics::get_elements_higher_than_threshold(std::vector<int> Y, int treshold) {
    std::vector<int> Y_res;
    for (int i=0;i<Y.size();i++) { 
        if (Y[i]>treshold) Y_res.push_back(Y[i]);
    }
    return Y_res;
}


std::vector<double> statistics::get_elements_lower_than_threshold(std::vector<double> Y, double treshold) {
    std::vector<double> Y_res;
    for (int i=0;i<Y.size();i++) {
        if (Y[i]<treshold) Y_res.push_back(Y[i]);
    }
    return Y_res;
}
std::vector<int> statistics::get_elements_lower_than_threshold(std::vector<int> Y, int treshold) {
    std::vector<int> Y_res;
    for (int i=0;i<Y.size();i++) {
        if (Y[i]<treshold) Y_res.push_back(Y[i]);
    }
    return Y_res;
}

std::vector<double> statistics::normalize_to_max(std::vector<double> Y) {
    double max_index = get_max_index_from_Y(Y);
    for (int i=0;i<Y.size();i++) Y[i]=Y[i]/Y[max_index];
    return Y;
}

std::vector<double> statistics::normalize_to_median(std::vector<double> Y) {
    double median = get_median_from_Y(Y);
    for (int i=0;i<Y.size();i++) Y[i]=Y[i]/median;
    return Y;
}

std::vector<double> statistics::normalize_to_mean(std::vector<double> Y) {
    double mean = get_mean_from_Y(Y);
    for (int i=0;i<Y.size();i++) Y[i]=Y[i]/mean;
    return Y;
}

std::vector<double> statistics::recursive_median_filter(std::vector<double> Y, int window_size) {
    std::vector<double> Y_filtered;
    
    const size_t N = Y.size();                        /* length of time series */
    const size_t K = window_size;                         /* window size */
    gsl_vector *x = gsl_vector_alloc(N);         /* input std::vector */
    gsl_vector *y = gsl_vector_alloc(N);        /* filtered output std::vector for alpha1 */
    gsl_filter_rmedian_workspace  *w = gsl_filter_rmedian_alloc(K);

//     statistics::std_vec_to_gsl_vec(&Y,&x);
    for (size_t i=0;i<Y.size();i++)gsl_vector_set (x, i, Y.at(i));
    /* apply filters */
    gsl_filter_rmedian(GSL_FILTER_END_TRUNCATE, x, y, w);
//     Y_filtered = statistics::gsl_vec_to_std_vec(&y);
    for (int i=0;i<y->size;i++) Y_filtered.push_back(y->data[i]);
    
    gsl_vector_free(x);
    gsl_vector_free(y);
    gsl_filter_rmedian_free(w);

    return Y_filtered;
}

std::vector<double> statistics::impulse_filter(std::vector<double> Y, int window_size, float scale_factor) {
    std::vector<double> Y_filtered;                                   /* result std::vector */
    const size_t N = Y.size();                               /* length of time series */
    const size_t K = window_size;                                 /* window size */
    const double t = scale_factor;                                /* number of scale factors for outlier detection */
    gsl_vector *x = gsl_vector_alloc(N);                 /* input std::vector */
    gsl_vector *y = gsl_vector_alloc(N);                 /* output (filtered) std::vector */
    gsl_vector *xmedian = gsl_vector_alloc(N);           /* window medians */
    gsl_vector *xsigma = gsl_vector_alloc(N);            /* window scale estimates */
    gsl_vector_int *ioutlier = gsl_vector_int_alloc(N);  /* outlier detected? */
    gsl_filter_impulse_workspace * w = gsl_filter_impulse_alloc(K);
    gsl_rng *r = gsl_rng_alloc(gsl_rng_default);
    size_t noutlier;
//     statistics::std_vec_to_gsl_vec(&Y,&x);
    for (size_t i=0;i<Y.size();i++) gsl_vector_set (x, i, Y.at(i));
    /* apply impulse detection filter */
	/// THIS IS NOT WORKING IN WINDOWS 32BIT FOR SOME INPUT VALUES
//     gsl_filter_impulse(GSL_FILTER_END_TRUNCATE, GSL_FILTER_SCALE_QN, t, x, y,
//                      xmedian, xsigma, &noutlier, ioutlier, w);
//	GSL_FILTER_SCALE_QN very slow
// 	gsl_filter_impulse(GSL_FILTER_END_PADVALUE, GSL_FILTER_SCALE_MAD, t, x, y, xmedian, xsigma, &noutlier, ioutlier, w);
	gsl_filter_impulse(GSL_FILTER_END_TRUNCATE, GSL_FILTER_SCALE_MAD, t, x, y, xmedian, xsigma, &noutlier, ioutlier, w); 
    for (int i=0;i<y->size;i++) 
		Y_filtered.push_back(y->data[i]);
    gsl_vector_free(x);
    gsl_vector_free(y);
    gsl_vector_free(xmedian);
    gsl_vector_free(xsigma);
    gsl_vector_int_free(ioutlier);
    gsl_filter_impulse_free(w);
    gsl_rng_free(r);
    return Y_filtered;
}

std::vector<double> statistics::gaussian_filter(std::vector<double> Y, int window_size, double alpha) {
    std::vector<double> Y_filtered;
    
    const size_t N = Y.size();                        /* length of time series */
    const size_t K = window_size;                         /* window size */
    gsl_vector *x = gsl_vector_alloc(N);         /* input std::vector */
    gsl_vector *y = gsl_vector_alloc(N);        /* filtered output std::vector for alpha1 */
    gsl_vector *k1 = gsl_vector_alloc(K);        /* Gaussian kernel for alpha1 */
    gsl_rng *r = gsl_rng_alloc(gsl_rng_default);
    gsl_filter_gaussian_workspace *gauss_p = gsl_filter_gaussian_alloc(K);
    /* compute kernels without normalization */
    gsl_filter_gaussian_kernel(alpha, 0, 0, k1);

//     statistics::std_vec_to_gsl_vec(&Y,&x);
    for (size_t i=0;i<Y.size();i++)gsl_vector_set (x, i, Y.at(i));
    /* apply filters */
    gsl_filter_gaussian(GSL_FILTER_END_PADVALUE, alpha, 0, x, y, gauss_p);
//     Y_filtered = statistics::gsl_vec_to_std_vec(&y1);
    for (int i=0;i<y->size;i++) Y_filtered.push_back(y->data[i]);
    
    gsl_vector_free(x);
    gsl_vector_free(y);
    gsl_vector_free(k1);
    gsl_rng_free(r);
    gsl_filter_gaussian_free(gauss_p);

    return Y_filtered;
}

int statistics::get_next_min_index(std::vector<double> Y, int skip_points) {
    int index=0;
    double min=Y[index];
    for (int i=skip_points;i<Y.size();i=i+1+skip_points) {
        index = i;
        if (Y[i] > min) break;
        min = Y[i];
    }
    return index;
}

int statistics::get_next_max_index(std::vector<double> Y, int skip_points) {
    int index=0;
    double max=Y[index];
    for (int i=skip_points;i<Y.size();i=i+1+skip_points) {
        index = i;
        if (Y[i] < max) break;
        max = Y[i];
    }
    return index;
}

double statistics::get_next_min(std::vector<double> Y, int skip_points) {
    double min=Y[0];
    for (int i=1;i<Y.size();i=i+1+skip_points) {
        if (Y[i] < min) min=Y[i];
        else break;
    }
    return min;
}

double statistics::get_next_max(std::vector<double> Y,int skip_points) {
    double max=Y[0];
    for (int i=1;i<Y.size();i=i+1+skip_points) {
        if (Y[i] > max) max=Y[i];
        else break;
    }
    return max;
}

/*
std::vector<std::vector<double>> statistics::get_local_minima_XY(std::vector<double> X, std::vector<double> Y) {
    
    std::vector<std::vector<double>> minima;
	return minima;
}
*/

bool statistics::std_mat_to_gsl_vec(std::vector<std::vector<double>> *std_mat, gsl_vector ** vec, int col) {
    *vec = gsl_vector_alloc(std_mat->size());
    for (size_t i=0;i<std_mat->size();i++) {
        if (col>=std_mat->at(i).size()) {
            return false;
        }
        gsl_vector_set (*vec, i, std_mat->at(i)[col]);
    }
    return true;
}

bool statistics::std_vec_to_gsl_vec(std::vector<double> *std_vec, gsl_vector ** vec) {
    *vec = gsl_vector_alloc(std_vec->size());
    for (size_t i=0;i<std_vec->size();i++)gsl_vector_set (*vec, i, std_vec->at(i));
    return true;
}
bool statistics::std_vec_to_gsl_vec(const std::vector<double>& std_vec, gsl_vector ** vec) {
    *vec = gsl_vector_alloc(std_vec.size());
    for (size_t i=0;i<std_vec.size();i++)gsl_vector_set (*vec, i, std_vec.at(i));
    return true;
}


std::vector<double> statistics::gsl_vec_to_std_vec(gsl_vector ** vec) {
    std::vector<double> Y;
    for (int i=0;i<(*vec)->size;i++) Y.push_back((*vec)->data[i]);
//     gsl_vector_free(vec);
    return Y;
}


std::vector<double> statistics::interpolate_dataXY_to_X(std::vector<std::vector<double>>& data_XY, std::vector<double>& X) {
    std::vector<double> Y;
    int N = data_XY.size();
    gsl_interp_accel *acc = gsl_interp_accel_alloc ();
    const gsl_interp_type *t = gsl_interp_cspline_periodic;
    gsl_spline *spline = gsl_spline_alloc (t, N);
    gsl_vector *x, *y;
    statistics::std_mat_to_gsl_vec(&data_XY,&x,0);
    statistics::std_mat_to_gsl_vec(&data_XY,&y,1);
    gsl_spline_init (spline, x->data, y->data, N);
    double *Xi= &X[0];
    for (int i=0;i<X.size();i++) {
        Y.push_back(gsl_spline_eval (spline, Xi[i], acc));
    }
    gsl_spline_free (spline);
    gsl_interp_accel_free (acc);
    gsl_vector_free(x);
    gsl_vector_free(y);
    return Y;
}

/*
 * 1st derivative (numerical)
 */
std::vector<double> statistics::derive_vec(std::vector<double> X, std::vector<double> Y) {
    std::vector<double> d_Y(Y.size());
    for (int i=1;i<X.size();i++) {
        d_Y[i]=(Y[i]-Y[i-1])/(X[i]-X[i-1]);
    }
    return d_Y;
}

int statistics::get_index_from_upper_limit_in_vec(std::vector<double> vec, double upper_limit) {
    int erg=0;
    for (int i=0;i<vec.size();i++) {
        if (vec[i]>upper_limit) {
            erg=i;
            break;
        }
    }
    return erg;
}

std::vector<std::vector<double>> statistics::truncate_mat_by_limit_in_col(std::vector<std::vector<double>> *data_XY, double limit, int col) {
    std::vector<std::vector<double>> result_XY;
    //1st sort matrix by col?
    for (int i=0;i<data_XY->size();i++) {
        if (data_XY->at(i)[col] > limit) result_XY.push_back(data_XY->at(i));
    }
//     std::cout << "limit = " << limit << "\tresult_XY.size()" << result_XY.size() << std::endl;
    return result_XY;
}

std::vector<std::vector<double>> statistics::get_moving_window_statistics_on_dataX_YY(std::vector<std::vector<double>> *data_X_YY, int window_size, int column) {
    std::vector<std::vector<double>> mat_results;
    if (window_size<2) window_size=10;
    if (column>=data_X_YY->at(0).size()) return mat_results;
    
    int N=data_X_YY->size();
    gsl_vector *x = gsl_vector_alloc(N);
    statistics::std_mat_to_gsl_vec(data_X_YY,&x,column);
    gsl_vector *xmedian = gsl_vector_alloc(N);
    gsl_vector *xmad = gsl_vector_alloc(N);
    gsl_vector *xiqr = gsl_vector_alloc(N);
    gsl_vector *xSn = gsl_vector_alloc(N);
    gsl_vector *xQn = gsl_vector_alloc(N);
    gsl_vector *xsd = gsl_vector_alloc(N);
    gsl_vector *xsd2 = gsl_vector_alloc(N);
    gsl_vector *xsd3 = gsl_vector_alloc(N);
    gsl_vector *xmean = gsl_vector_alloc(N);
    gsl_vector *xmin = gsl_vector_alloc(N);
    gsl_vector *xmax = gsl_vector_alloc(N);
    
    gsl_movstat_workspace * w = gsl_movstat_alloc(window_size);
//     gsl_rng *r = gsl_rng_alloc(gsl_rng_default);
    gsl_movstat_mad(GSL_MOVSTAT_END_TRUNCATE, x, xmedian, xmad, w);
    gsl_movstat_qqr(GSL_MOVSTAT_END_TRUNCATE, x, 0.25, xiqr, w);
    gsl_movstat_Sn(GSL_MOVSTAT_END_TRUNCATE, x, xSn, w);
    gsl_movstat_Qn(GSL_MOVSTAT_END_TRUNCATE, x, xQn, w);  
    gsl_movstat_sd(GSL_MOVSTAT_END_TRUNCATE, x, xsd, w);
    gsl_movstat_sd(GSL_MOVSTAT_END_TRUNCATE, xsd, xsd2, w);
    gsl_movstat_sd(GSL_MOVSTAT_END_TRUNCATE, xsd2, xsd3, w);
    gsl_movstat_mean(GSL_MOVSTAT_END_PADVALUE, x, xmean, w);
    gsl_movstat_minmax(GSL_MOVSTAT_END_PADVALUE, x, xmin, xmax, w);
    for (int j=0;j<N;j++) {
//         data_XY->at(j)={xmedian->data[j],xmad->data[j],xiqr->data[j],xSn->data[j]};
        mat_results.at(j).push_back(xmedian->data[j]); //3
        mat_results.at(j).push_back(xmad->data[j]);    //4
        mat_results.at(j).push_back(xiqr->data[j]);    //5
        mat_results.at(j).push_back(xSn->data[j]);     //6 bk
        mat_results.at(j).push_back(xQn->data[j]);     //7 b
        mat_results.at(j).push_back(xsd->data[j]);     //8 c
        mat_results.at(j).push_back(xmean->data[j]);   //9
        mat_results.at(j).push_back(xmax->data[j]);    //10
        mat_results.at(j).push_back(xmin->data[j]);    //11
        mat_results.at(j).push_back(xsd2->data[j]);     //12 
        mat_results.at(j).push_back(xsd3->data[j]);     //13 
    }
    gsl_vector_free(x);
    gsl_vector_free(xmedian);
    gsl_vector_free(xmad);
    gsl_vector_free(xiqr);
    gsl_vector_free(xSn);
    gsl_vector_free(xQn);
    gsl_vector_free(xsd);
    gsl_vector_free(xsd3);
    gsl_vector_free(xsd2);
    gsl_vector_free(xmean);
    gsl_vector_free(xmin);
    gsl_vector_free(xmax);
//     gsl_rng_free(r);
    gsl_movstat_free(w);
    return mat_results;
}

std::vector<std::vector<double>> statistics::get_moving_window_statistics_from_Y(std::vector<double> *Y, int window_size) {
    
    std::vector<std::vector<double>> mat_results;
    if (window_size<2) window_size=10;
    mat_results=tools::mat::transpose_matrix_double({*Y});
    int N=Y->size();
    gsl_vector *x = gsl_vector_alloc(N);
//     statistics::std_vec_to_gsl_vec(Y,&x);

    for (size_t i=0;i<Y->size();i++) gsl_vector_set (x, i, Y->at(i));
    gsl_vector *xmedian = gsl_vector_alloc(N);
    gsl_vector *xmad = gsl_vector_alloc(N);
    gsl_vector *xiqr = gsl_vector_alloc(N);
    gsl_vector *xSn = gsl_vector_alloc(N);
    gsl_vector *xQn = gsl_vector_alloc(N);
    gsl_vector *xsd = gsl_vector_alloc(N);
    gsl_vector *xsd2 = gsl_vector_alloc(N);
    gsl_vector *xsd3 = gsl_vector_alloc(N);
    gsl_vector *xmean = gsl_vector_alloc(N);
    gsl_vector *xmin = gsl_vector_alloc(N);
    gsl_vector *xmax = gsl_vector_alloc(N);
    
    gsl_movstat_workspace * w = gsl_movstat_alloc(window_size);
//     gsl_rng *r = gsl_rng_alloc(gsl_rng_default);
    gsl_movstat_mad(GSL_MOVSTAT_END_TRUNCATE, x, xmedian, xmad, w);
    gsl_movstat_qqr(GSL_MOVSTAT_END_TRUNCATE, x, 0.25, xiqr, w);
    gsl_movstat_Sn(GSL_MOVSTAT_END_TRUNCATE, x, xSn, w);
    gsl_movstat_Qn(GSL_MOVSTAT_END_TRUNCATE, x, xQn, w);  
    gsl_movstat_sd(GSL_MOVSTAT_END_TRUNCATE, x, xsd, w);
    gsl_movstat_sd(GSL_MOVSTAT_END_TRUNCATE, xsd, xsd2, w);
    gsl_movstat_sd(GSL_MOVSTAT_END_TRUNCATE, xsd2, xsd3, w);
    gsl_movstat_mean(GSL_MOVSTAT_END_PADVALUE, x, xmean, w);
    gsl_movstat_minmax(GSL_MOVSTAT_END_PADVALUE, x, xmin, xmax, w);
    for (int j=0;j<N;j++) {
//         data_XY->at(j)={xmedian->data[j],xmad->data[j],xiqr->data[j],xSn->data[j]};
        mat_results.at(j).push_back(xmedian->data[j]); //1
        mat_results.at(j).push_back(xmad->data[j]);    //2
        mat_results.at(j).push_back(xiqr->data[j]);    //3
        mat_results.at(j).push_back(xSn->data[j]);     //4 bk
        mat_results.at(j).push_back(xQn->data[j]);     //5 b
        mat_results.at(j).push_back(xsd->data[j]);     //6 c
        mat_results.at(j).push_back(xmean->data[j]);   //7
        mat_results.at(j).push_back(xmax->data[j]);    //8
        mat_results.at(j).push_back(xmin->data[j]);    //9
        mat_results.at(j).push_back(xsd2->data[j]);     //10 
        mat_results.at(j).push_back(xsd3->data[j]);     //11 
    }
    gsl_vector_free(x);
    gsl_vector_free(xmedian);
    gsl_vector_free(xmad);
    gsl_vector_free(xiqr);
    gsl_vector_free(xSn);
    gsl_vector_free(xQn);
    gsl_vector_free(xsd);
    gsl_vector_free(xsd3);
    gsl_vector_free(xsd2);
    gsl_vector_free(xmean);
    gsl_vector_free(xmin);
    gsl_vector_free(xmax);
//     gsl_rng_free(r);
    gsl_movstat_free(w);
    return mat_results;
}

std::vector<double> statistics::get_moving_window_function(const std::vector<double>& Y, int window_size, statistics::functiontype function, const gsl_movstat_end_t gsl_movstat_end)
{
	if (window_size<2) window_size=10;
    
    int N=Y.size();
    gsl_vector *x = gsl_vector_alloc(N);
    for (size_t i=0;i<Y.size();i++) gsl_vector_set (x, i, Y.at(i));
    gsl_vector *results = gsl_vector_alloc(N);
    gsl_movstat_workspace * w = gsl_movstat_alloc(window_size);
    function(gsl_movstat_end, x, results, w);
	std::vector<double> results_vec(N);
    for (int j=0;j<N;j++) {
        results_vec.at(j)=(results->data[j]);   
    }
    gsl_vector_free(x);
    gsl_vector_free(results);
    gsl_movstat_free(w);
    return results_vec;
}


std::vector<double> statistics::get_moving_window_qqr_from_Y(const std::vector<double>& Y, int window_size, double q)
{
	if (window_size<2) window_size=10;
    
    int N=Y.size();
	gsl_vector *x = get_gsl_vec(Y);
//     gsl_vector *x = gsl_vector_alloc(N);
//     for (size_t i=0;i<Y.size();i++) gsl_vector_set (x, i, Y.at(i));
    gsl_vector *results = gsl_vector_alloc(N);
    gsl_movstat_workspace * w = gsl_movstat_alloc(window_size);
    gsl_movstat_qqr(GSL_MOVSTAT_END_TRUNCATE, x, q , results, w);
	std::vector<double> sum(N);
    for (int j=0;j<N;j++) {
        sum.at(j)=(results->data[j]);   
    }
    gsl_vector_free(x);
    gsl_vector_free(results);
    gsl_movstat_free(w);
    return sum;
}

std::vector<double> statistics::get_moving_window_min_from_Y(const std::vector<double>& Y, int window_size)
{
// 	if (window_size<2) window_size=10;
//     
//     int N=Y.size();
//     gsl_vector *x = gsl_vector_alloc(N);
//     for (size_t i=0;i<Y.size();i++) gsl_vector_set (x, i, Y.at(i));
//     gsl_vector *results = gsl_vector_alloc(N);
//     gsl_movstat_workspace * w = gsl_movstat_alloc(window_size);
//     gsl_movstat_min(GSL_MOVSTAT_END_TRUNCATE, x, results, w);
// 	std::vector<double> sum(N);
//     for (int j=0;j<N;j++) {
//         sum.at(j)=(results->data[j]);   
//     }
//     gsl_vector_free(x);
//     gsl_vector_free(results);
//     gsl_movstat_free(w);
//     return sum;
	return get_moving_window_function(Y,window_size,gsl_movstat_min);
}

std::vector<double> statistics::get_moving_window_max_from_Y(const std::vector<double>& Y, int window_size)
{
// 	if (window_size<2) window_size=10;
//     
//     int N=Y.size();
//     gsl_vector *x = gsl_vector_alloc(N);
//     for (size_t i=0;i<Y.size();i++) gsl_vector_set (x, i, Y.at(i));
//     gsl_vector *results = gsl_vector_alloc(N);
//     gsl_movstat_workspace * w = gsl_movstat_alloc(window_size);
//     gsl_movstat_max(GSL_MOVSTAT_END_TRUNCATE, x, results, w);
// 	std::vector<double> sum(N);
//     for (int j=0;j<N;j++) {
//         sum.at(j)=(results->data[j]);   
//     }
//     gsl_vector_free(x);
//     gsl_vector_free(results);
//     gsl_movstat_free(w);
//     return sum;
	return get_moving_window_function(Y,window_size,gsl_movstat_max);
}

std::vector<double> statistics::get_moving_window_sum_from_Y(const std::vector<double>& Y, int window_size)
{
// 	if (window_size<2) window_size=10;
//     
//     int N=Y.size();
//     gsl_vector *x = gsl_vector_alloc(N);
//     for (size_t i=0;i<Y.size();i++) gsl_vector_set (x, i, Y.at(i));
//     gsl_vector *results = gsl_vector_alloc(N);
//     gsl_movstat_workspace * w = gsl_movstat_alloc(window_size);
//     gsl_movstat_sum(GSL_MOVSTAT_END_TRUNCATE, x, results, w);
// 	std::vector<double> sum(N);
//     for (int j=0;j<N;j++) {
//         sum.at(j)=(results->data[j]);   
//     }
//     gsl_vector_free(x);
//     gsl_vector_free(results);
//     gsl_movstat_free(w);
//     return sum;
	return get_moving_window_function(Y,window_size,gsl_movstat_sum);
}



std::vector<double> statistics::get_moving_window_mean_from_Y(const std::vector<double>& Y, int window_size) {
//     if (window_size<2) window_size=10;
//     std::vector<double> sd;
//     int N=Y.size();
//     gsl_vector *x = gsl_vector_alloc(N);
//     for (size_t i=0;i<Y.size();i++) gsl_vector_set (x, i, Y.at(i));
//     gsl_vector *xsd = gsl_vector_alloc(N);
//     gsl_movstat_workspace * w = gsl_movstat_alloc(window_size);
//     gsl_movstat_mean(GSL_MOVSTAT_END_TRUNCATE, x, xsd, w);
//     for (int j=0;j<N;j++) {
//         sd.push_back(xsd->data[j]);     //8 c
//     }
//     gsl_vector_free(x);
//     gsl_vector_free(xsd);
//     gsl_movstat_free(w);
//     return sd;
	return get_moving_window_function(Y,window_size,gsl_movstat_mean);
}

std::vector<double> statistics::get_moving_window_median_from_Y(const std::vector<double>& Y, int window_size) {
//     if (window_size<2) window_size=10;
//     std::vector<double> sd;
//     int N=Y.size();
//     gsl_vector *x = gsl_vector_alloc(N);
//     for (size_t i=0;i<Y.size();i++) gsl_vector_set (x, i, Y.at(i));
//     gsl_vector *xsd = gsl_vector_alloc(N);
//     gsl_movstat_workspace * w = gsl_movstat_alloc(window_size);
//     gsl_movstat_median(GSL_MOVSTAT_END_TRUNCATE, x, xsd, w);
//     for (int j=0;j<N;j++) {
//         sd.push_back(xsd->data[j]);     //8 c
//     }
//     gsl_vector_free(x);
//     gsl_vector_free(xsd);
//     gsl_movstat_free(w);
//     return sd;
	return get_moving_window_function(Y,window_size,gsl_movstat_median);
}

std::vector<double> statistics::get_moving_window_MAD_from_Y(const std::vector<double>& Y, int window_size) {
    if (window_size<2) window_size=10;
    std::vector<double> sd;
    int N=Y.size();
    gsl_vector *x = gsl_vector_alloc(N);
    for (size_t i=0;i<Y.size();i++) gsl_vector_set (x, i, Y.at(i));
    gsl_vector *xmad = gsl_vector_alloc(N);
	gsl_vector *xmedian = gsl_vector_alloc(N);
    gsl_movstat_workspace * w = gsl_movstat_alloc(window_size);
    gsl_movstat_mad(GSL_MOVSTAT_END_TRUNCATE, x, xmedian, xmad, w);
    for (int j=0;j<N;j++) {
        sd.push_back(xmad->data[j]);     //8 c
    }
    gsl_vector_free(x);
    gsl_vector_free(xmad);
	gsl_vector_free(xmedian);
    gsl_movstat_free(w);
    return sd;
}

std::vector<double> statistics::get_moving_window_sd_from_Y(const std::vector<double>& Y, int window_size) {
//     if (window_size<2) window_size=10;
//     std::vector<double> sd;
//     int N=Y.size();
//     gsl_vector *x = gsl_vector_alloc(N);
// //     statistics::std_vec_to_gsl_vec(&Y,&x);
//     for (size_t i=0;i<Y.size();i++)gsl_vector_set (x, i, Y.at(i));
//     gsl_vector *xsd = gsl_vector_alloc(N);
//     gsl_movstat_workspace * w = gsl_movstat_alloc(window_size);
//     gsl_movstat_sd(GSL_MOVSTAT_END_TRUNCATE, x, xsd, w);
//     for (int j=0;j<N;j++) {
//         sd.push_back(xsd->data[j]);     //8 c
//     }
//     gsl_vector_free(x);
//     gsl_vector_free(xsd);
//     gsl_movstat_free(w);
//     return sd;
	return get_moving_window_function(Y,window_size,gsl_movstat_sd);
}

// int statistics::get_moving_window_max_index_from_Y(std::vector<double> Y, int window_size) {
//     int max_pos;
//     int N=Y.size();
//     gsl_vector *x = gsl_vector_alloc(N);
// //     statistics::std_vec_to_gsl_vec(&Y,&x);
//     for (size_t i=0;i<Y.size();i++)gsl_vector_set (x, i, Y.at(i));
//     gsl_movstat_workspace * w = gsl_movstat_alloc(window_size);
//     max_pos = gsl_stats_max_index(x->data,1,Y.size());
//     gsl_vector_free(x);
//     gsl_movstat_free(w);
//     return max_pos;
// }

double statistics::get_correlation_from_dataXY(std::vector<std::vector<double>> *data_XY1, std::vector<std::vector<double>> *data_XY2) {
    gsl_vector *vec1, *vec2;
    statistics::std_mat_to_gsl_vec(data_XY1,&vec1,1);
    statistics::std_mat_to_gsl_vec(data_XY2,&vec2,1);    
    double corr = gsl_stats_correlation(vec1->data,1,vec2->data,1,data_XY2->size());
    gsl_vector_free(vec1);
    gsl_vector_free(vec2);
    return corr;
}

double statistics::get_correlation_Y1_Y2(const std::vector<double>& Y1, const std::vector<double>& Y2) 
{
    gsl_vector *vec1, *vec2;
	statistics::std_vec_to_gsl_vec(Y1,&vec1);
	statistics::std_vec_to_gsl_vec(Y2,&vec2);
    double corr = gsl_stats_correlation(vec1->data,1,vec2->data,1,Y1.size());
    gsl_vector_free(vec1);
    gsl_vector_free(vec2);
    return corr;
}

std::vector<double> statistics::get_minmax_from_dataXY_tensor(std::vector<std::vector<std::vector<double>>> dataXY_tensor) {
    std::vector<double> max_values_of_all_colls;
    std::vector<std::vector<double>> mat, mat_transposed;
    mat=dataXY_tensor.at(0);
    for (int i=1;i<dataXY_tensor.size();i++) {
        mat.insert(mat.end(),dataXY_tensor.at(i).begin(),dataXY_tensor.at(i).end());
    }
//     mat_transposed=Tools::transpose_matrix_double(mat);
    gsl_vector * vec;
    for (int i=0;i<2;i++) { // X + Y column
        statistics::std_mat_to_gsl_vec(&mat,&vec,i);
        double max = gsl_stats_max(vec->data,1,mat.size());
        double min = gsl_stats_min(vec->data,1,mat.size());
        max_values_of_all_colls.push_back(min);
        max_values_of_all_colls.push_back(max);
    }
    gsl_vector_free(vec);
    return max_values_of_all_colls;
}

int statistics::get_max_index_from_Y(std::vector<double> Y) {
    gsl_vector * vec = gsl_vector_alloc(Y.size());
//     statistics::std_vec_to_gsl_vec(&Y,&vec);
    for (size_t i=0;i<Y.size();i++)gsl_vector_set (vec, i, Y.at(i));
    double max = gsl_stats_max_index(vec->data,1,Y.size());
    gsl_vector_free(vec);
    return max;
}

double statistics::get_gastwirth_estimator_from_Y ( std::vector<double> Y )
{
	gsl_vector * vec = gsl_vector_alloc(Y.size());
    for (size_t i=0;i<Y.size();i++)
		gsl_vector_set (vec, i, Y.at(i));
	gsl_sort(vec->data,1,Y.size());
	double gastwirth = gsl_stats_gastwirth_from_sorted_data(vec->data,1,Y.size());
    gsl_vector_free(vec);
    return gastwirth;
}


double statistics::get_trimmed_mean_from_Y(std::vector<double> Y, float alpha)
{
	gsl_vector * vec = gsl_vector_alloc(Y.size());
    for (size_t i=0;i<Y.size();i++)
		gsl_vector_set (vec, i, Y.at(i));
	gsl_sort(vec->data,1,Y.size());
	double tr_mean = gsl_stats_trmean_from_sorted_data(alpha,vec->data,1,Y.size());
    gsl_vector_free(vec);
    return tr_mean;
}


double statistics::get_mean_from_Y(std::vector<double> Y) {
    gsl_vector * vec = gsl_vector_alloc(Y.size());
    for (size_t i=0;i<Y.size();i++) gsl_vector_set (vec, i, Y.at(i));
    double max = gsl_stats_mean(vec->data,1,Y.size());
    gsl_vector_free(vec);
    return max;
}

double statistics::get_quantile_from_Y(std::vector<double> Y, double percentile) 
{
    gsl_vector * vec = gsl_vector_alloc(Y.size());
    for (size_t i=0;i<Y.size();i++)
		gsl_vector_set (vec, i, Y.at(i));
	gsl_sort(vec->data,1,Y.size());
	double quantile = gsl_stats_quantile_from_sorted_data(vec->data,1,Y.size(),percentile);
    gsl_vector_free(vec);
    return quantile;
}

double statistics::get_median_from_Y(std::vector<double> Y) {
    gsl_vector * vec = gsl_vector_alloc(Y.size());
    for (size_t i=0;i<Y.size();i++)gsl_vector_set (vec, i, Y.at(i));
    double median = gsl_stats_median(vec->data,1,Y.size());
    gsl_vector_free(vec);
    return median;
}

double statistics::get_mad_from_Y(std::vector<double> Y) {
    gsl_vector * vec = gsl_vector_alloc(Y.size());
    for (size_t i=0;i<Y.size();i++)gsl_vector_set (vec, i, Y.at(i));
    int c = Y.size();
    double w[c];
    double mad = gsl_stats_mad(vec->data,1,c,w);
    gsl_vector_free(vec);
    return mad;
}

double statistics::get_sd_from_Y(std::vector<double> Y) {
    gsl_vector * vec = gsl_vector_alloc(Y.size());
    for (size_t i=0;i<Y.size();i++)gsl_vector_set (vec, i, Y.at(i));
    int c = Y.size();
    double w[c];
    double sd = gsl_stats_sd(vec->data,1,c);
    gsl_vector_free(vec);
    return sd;
}

int statistics::get_min_index_from_Y(std::vector<double> Y) {
    gsl_vector * vec = gsl_vector_alloc(Y.size());
//     statistics::std_vec_to_gsl_vec(&Y,&vec);
    for (size_t i=0;i<Y.size();i++)gsl_vector_set (vec, i, Y.at(i));
    double min = gsl_stats_min_index(vec->data,1,Y.size());
    gsl_vector_free(vec);
    return min;
}

double statistics::get_min_from_Y(std::vector<double> Y) 
{
    return Y[get_min_index_from_Y(Y)];
}
double statistics::get_max_from_Y(std::vector<double> Y) 
{
    return Y[get_max_index_from_Y(Y)];
}


// statis statistics::get_statistics_from_dataXY_transposed(std::vector<std::vector<double>> *data_XY_transposed) {
//     int c=data_XY_transposed->at(0).size(); // anzahl der zeilen
//     statis stats;
//     if (c<3) return stats;
//     double w[c];
//     
//     gsl_vector * vec = gsl_vector_alloc(c);
// //     statistics::std_vec_to_gsl_vec(&data_XY_transposed->at(1),&vec);
//     for (size_t i=0;i<c;i++) gsl_vector_set (vec, i, data_XY_transposed->at(1)[i]);
//     
//     stats.mean = gsl_stats_mean(vec->data,1,c);
//     stats.sd = gsl_stats_sd(vec->data,1,c);
//     
//     stats.max_index = gsl_stats_max_index(vec->data,1,c);
// //     stats.max = Y[stats.max_index];
//     stats.min_index = gsl_stats_min_index(vec->data,1,c);
// //     stats.min = Y[stats.min_index];
//     stats.abs_dev = gsl_stats_absdev(vec->data,1,c);
//     
//     stats.skew = gsl_stats_skew(vec->data,1,c);
//     stats.kurtosis = gsl_stats_kurtosis(vec->data,1,c);
//     stats.auto_corr = gsl_stats_lag1_autocorrelation(vec->data,1,c);
//     stats.median = gsl_stats_median(vec->data,1,c);
//     gsl_sort(vec->data,1,c);
//     stats.quantile = gsl_stats_quantile_from_sorted_data(vec->data,1,c,0.75);
//     stats.mean_trimmed = gsl_stats_trmean_from_sorted_data(0.25,vec->data,1,c);
//     stats.mad = gsl_stats_mad(vec->data,1,c,w);
//     stats.sn = gsl_stats_Sn_from_sorted_data(vec->data,1,c,w);
// //     stats.qn = gsl_stats_Qn_from_sorted_data(vec->data,1,c,w,wi);
//     
//     stats.area=statistics::get_area_under_dataXY_transposed(*data_XY_transposed,(*data_XY_transposed)[0][0],(*data_XY_transposed)[0].back());
//     stats.max=data_XY_transposed->at(1)[stats.max_index];
//     stats.min=data_XY_transposed->at(1)[stats.min_index];
//     stats.max_x=data_XY_transposed->at(0)[stats.max_index];
//     stats.max_x=data_XY_transposed->at(0)[stats.min_index];
// 
//     gsl_vector_free(vec);
//     return stats;
// }

// double statistics::get_mad(std::vector<double> Y) {
//     double mad;
//     int c=Y.size();
//     double w[c];
//     gsl_vector * vec;
//     statistics::std_vec_to_gsl_vec(&Y,&vec);
//     
// }

// statis statistics::get_statistics(std::vector<double> Y) {
//     statis stats;
//     int c=Y.size();
//     double w[c];
//     gsl_vector * vec = gsl_vector_alloc(Y.size());
// //     statistics::std_vec_to_gsl_vec(&Y,&vec);
//     for (size_t i=0;i<Y.size();i++) gsl_vector_set (vec, i, Y.at(i));
//     
//     stats.mean = gsl_stats_mean(vec->data,1,c);
//     stats.sd = gsl_stats_sd(vec->data,1,c);
//     
//     stats.min_index = gsl_stats_min_index(vec->data,1,c);
//     stats.max_index = gsl_stats_max_index(vec->data,1,c);
//     stats.max = Y[stats.max_index];
//     stats.min = Y[stats.min_index];
// //     stats.min = Y[stats.min_index];
//     stats.abs_dev = gsl_stats_absdev(vec->data,1,c);
//     
//     stats.skew = gsl_stats_skew(vec->data,1,c);
//     stats.kurtosis = gsl_stats_kurtosis(vec->data,1,c);
//     stats.auto_corr = gsl_stats_lag1_autocorrelation(vec->data,1,c);
//     stats.median = gsl_stats_median(vec->data,1,c);
//     gsl_sort(vec->data,1,c);
//     stats.quantile = gsl_stats_quantile_from_sorted_data(vec->data,1,c,0.75);
//     stats.mean_trimmed = gsl_stats_trmean_from_sorted_data(0.25,vec->data,1,c);
//     stats.mad = gsl_stats_mad(vec->data,1,c,w);
//     stats.sn = gsl_stats_Sn_from_sorted_data(vec->data,1,c,w);
// //     stats.qn = gsl_stats_Qn_from_sorted_data(vec->data,1,c,w,wi);
//     gsl_vector_free(vec);
//     return stats;
// }

std::vector<double> statistics::common_vector(std::vector<std::vector<double>> data_vectors)
{
	std::vector<double> D_mins, D_maxs;
	double D_res=0;
	for (auto& vec:data_vectors)
	{
		D_mins.push_back(get_min_from_Y(vec));
		D_maxs.push_back(get_max_from_Y(vec));
		D_res = abs(*vec.end()-*vec.rbegin())/(vec.size()-1);
	}
	D_res = D_res / data_vectors.size();
	const double D_min = get_max_from_Y(D_mins);
	const double D_max = get_min_from_Y(D_maxs);
	
	if (D_res <= 0) return {};
	const int cdv_size = floor(abs(D_max-D_min)/D_res) + 1;
	if (cdv_size < 1) return {};
	std::vector<double> common_data_vec(cdv_size);
	for (int i=0;i<cdv_size;i++)
		common_data_vec[i] = i * D_res + D_min;
	return common_data_vec;
}

double statistics::polynom_integrator (double x, void * params) {
    double sum=0;
    std::vector<double> *poly_params=(std::vector<double>*) params;
    for (int i=0;i<poly_params->size();i++) {
        sum=sum+poly_params->at(i)*pow(x,i);
    }
    return sum;
}


/* numerical integration*/
double statistics::get_area_under_dataXY_transposed(std::vector<std::vector<double>> data_XY_transposed,double lower_X, double upper_X) {
    std::vector<std::vector<double>> data_XY = tools::mat::transpose_matrix_double(data_XY_transposed);
    return statistics::get_area_under_dataXY(data_XY,lower_X,upper_X);
}



/* numerical integration*/
double statistics::get_area_under_dataXY(std::vector<std::vector<double>> data_XY,double lower_X, double upper_X) {
    if (upper_X<0 || lower_X <0) return -1;
    if (upper_X==lower_X) return 0;
    if (upper_X<lower_X) {
        double temp;
        temp=upper_X;
        upper_X=lower_X;
        lower_X=temp;
    }
    double sum=0;
    for (int i=1;i<data_XY.size();i++) {
        double X=data_XY.at(i)[0];
        double Y=abs(data_XY.at(i)[1]);
        double dX=abs(data_XY.at(i-1)[0]-X);
        if (X>=lower_X && X<=upper_X) sum=sum+dX*Y;
    }
    return sum;
}

/*analytical integration using polynoms*/
double statistics::get_area_under_polynom(std::vector<double> polynom_parameters, double lower_X, double upper_X) {
    if (upper_X<0 || lower_X <0) return -1;
    if (upper_X==lower_X) return 0;
    if (upper_X<lower_X) {
        double temp;
        temp=upper_X;
        upper_X=lower_X;
        lower_X=temp;
    }
    double sum=0;
    for (int i=1;i<polynom_parameters.size()+1;i++) {
        sum=sum+polynom_parameters[i-1]*(pow(upper_X,i)-pow(lower_X,i))/i;
    }
    return sum;
}

// bool statistics::gsl_vec_to_std_vec(gsl_vector * vec, std::vector<double> *std_vec) {
//     std_vec->clear();
//     for (size_t i=0;i<vec->size;i++) {
//         std_vec->push_back(gsl_vector_get (vec, i));
//     }
//     return true;
// }

std::vector<int> statistics::get_positions_of_zero_values(std::vector<std::vector<double>> matrix, double treshold) {
    std::vector<int> positions;
    for (int i=0;i<matrix.size();i++) {
        if (matrix[i].size()<2) return positions;
        if (abs(matrix[i][1]<treshold)) positions.push_back(i);
    }
    return positions;
}

std::vector<double> statistics::fit_poly_XY_transposed(std::vector<std::vector<double>> XY, int poly_grad) {
    std::vector<double> Y_fit;
    int i, n;
  double chisq;
  gsl_matrix *X, *cov;
  gsl_vector *y, *w, *c;
  const int p = poly_grad;
  std::vector<double> fit_parameters(p);
//     if (p>9) return false;
    
  n = XY[0].size();
    
  X = gsl_matrix_alloc (n, p);
  y = gsl_vector_alloc (n);
  w = gsl_vector_alloc (n);

  c = gsl_vector_alloc (p);
  cov = gsl_matrix_alloc (p, p);
    
  chisq=-1;
  for (i = 0; i < n; i++)
    {
      double xi=XY[0][i];
      double yi=XY[1][i];
      gsl_vector_set (y, i, yi);
      for (int ii=0;ii<p;ii++) {
          gsl_matrix_set (X, i, ii, pow(xi,ii));
      }
    }
  {
    gsl_multifit_linear_workspace * work = gsl_multifit_linear_alloc (n, p);
//     gsl_multifit_wlinear (X, w, y, c, cov, &chisq, work);
    gsl_multifit_linear (X, y, c, cov, &chisq, work);
    gsl_multifit_linear_free (work);
  }

#define C(i) (gsl_vector_get(c,(i)))
#define COV(i,j) (gsl_matrix_get(cov,(i),(j)))
    for (int ii=0;ii<p;ii++) {
        fit_parameters.at(ii)=C(ii);
    }
    
    gsl_matrix_free (X);
    gsl_vector_free (y);
    gsl_vector_free (w);
    gsl_vector_free (c);
    gsl_matrix_free (cov);
    return statistics::get_poly_Y(fit_parameters,XY[0]);

}

std::vector<double> statistics::get_poly_Y(std::vector<double> parameters, std::vector<double> X) {
    int p = parameters.size();
    std::vector<double> data_Y(X.size());
    double yi,xi;
    for (int x=0;x<X.size();x++) {
        yi=0;
        for (int i=0;i<p;i++) {
            xi=X.at(x);
            yi=pow(xi,i)*parameters.at(i)+yi;
        }
        data_Y[x]=(yi);
    }
    return data_Y;
}

// std::vector<double> statistics::get_polynom_Y(std::vector<double> parameters, std::vector<double> *data_X) {
//     int p = parameters.size();
//     std::vector<double> data_Y(data_X->size());
//     double yi,xi;
//     for (int x=0;x<data_X->size();x++) {
//         yi=0;
//         for (int i=0;i<p;i++) {
//             xi=data_X->at(x);
//             yi=pow(xi,i)*parameters.at(i)+yi;
//         }
//         data_Y[x]=(yi);
//     }
//     return data_Y;
// }

std::vector<double> statistics::derive_poly(std::vector<double> parameters) {
    std::vector<double> derived_parameters;
    int p = parameters.size();
    for (int i=1;i<p;i++) {
        derived_parameters.push_back(i*parameters[i]);
    }
    return derived_parameters;
}

std::vector<double> statistics::integrate_poly(std::vector<double> parameters) {
    std::vector<double> integrated_parameters;
    int p = parameters.size();
    integrated_parameters.push_back(0); // assuming integration with boarders 
    for (int i=1;i<p;i++) {
        integrated_parameters.push_back(parameters[i-1]/i);
    }
    return integrated_parameters;
}

bool statistics::first_derivative_poly(std::vector<double> *parameters, std::vector<std::vector<double>> *data_XY_dY) {
    int p = parameters->size();
    double xi;
    double yi;
    for (int x=0;x<data_XY_dY->size();x++) {
        yi=0;
        for (int i=1;i<p;i++) {
            xi=data_XY_dY->at(x)[0];
            yi=pow(xi,i-1)*i*parameters->at(i)+yi;
        }
        data_XY_dY->at(x).push_back(yi);
    }
    return false;
}

bool statistics::second_derivative_poly(std::vector<double> *parameters, std::vector<std::vector<double>> *data_XY_ddY) {
    int p = parameters->size();
    double xi;
    for (int x=0;x<data_XY_ddY->size();x++) {
        for (int i=2;i<p;i++) {
            xi=data_XY_ddY->at(x)[0];
            (data_XY_ddY->at(x)).push_back(pow(xi,i-2)*(i-1)*parameters->at(i));
        }
    }
    return false;
}



std::vector<double> statistics::get_polynom_Y(std::vector<double> parameters, std::vector<double> *data_X) {
    int p = parameters.size();
    std::vector<double> data_Y(data_X->size());
    double yi,xi;
    for (int x=0;x<data_X->size();x++) {
        yi=0;
        for (int i=0;i<p;i++) {
            xi=data_X->at(x);
            yi=pow(xi,i)*parameters.at(i)+yi;
        }
        data_Y[x]=(yi);
    }
    return data_Y;
}

double statistics::fit_poly (std::vector<std::vector<double>> *data_matrix, std::vector<double> *fit_parameters)  {
  int i, n;
  double chisq;
  gsl_matrix *X, *cov;
  gsl_vector *y, *w, *c;
  const int p = fit_parameters->size();
//     if (p>9) return false;
    
  n = data_matrix->size();
    
  X = gsl_matrix_alloc (n, p);
  y = gsl_vector_alloc (n);
  w = gsl_vector_alloc (n);

  c = gsl_vector_alloc (p);
  cov = gsl_matrix_alloc (p, p);
    
  chisq=-1;
  for (i = 0; i < n; i++)
    {
      double xi=data_matrix->at(i)[0];
      double yi=data_matrix->at(i)[1];
      gsl_vector_set (y, i, yi);
      for (int ii=0;ii<p;ii++) {
          gsl_matrix_set (X, i, ii, pow(xi,ii));
      }
    }
  {
    gsl_multifit_linear_workspace * work = gsl_multifit_linear_alloc (n, p);
//     gsl_multifit_wlinear (X, w, y, c, cov, &chisq, work);
    gsl_multifit_linear (X, y, c, cov, &chisq, work);
    gsl_multifit_linear_free (work);
  }

#define C(i) (gsl_vector_get(c,(i)))
#define COV(i,j) (gsl_matrix_get(cov,(i),(j)))
    for (int ii=0;ii<p;ii++) {
        fit_parameters->at(ii)=C(ii);
    }
    
    gsl_matrix_free (X);
    gsl_vector_free (y);
    gsl_vector_free (w);
    gsl_vector_free (c);
    gsl_matrix_free (cov);
  return chisq;
}

/*
bool statistics::polynomfit_XY(std::vector<std::vector<double>> data_XY,data *fitted_data, int polynom_grad, double interp_res) {
    if (polynom_grad<1) return false;
    // data_XY_XY contains 2 columns from ONE implanted element Y(X)
    double chisq_best,chisq;
    std::vector<double> fit_parameters(polynom_grad);
    std::vector<double> fit_parameters_best(polynom_grad);
    chisq_best=statistics::fit_poly(&data_XY,&fit_parameters);
//     if (chisq_best==-1) std::cout << "error\n";
    fit_parameters_best=fit_parameters;
    if (polynom_grad==1) {
        for (int i=1;i<15;i++) {
            fit_parameters.resize(i);
            chisq=statistics::fit_poly(&data_XY,&fit_parameters);
//             if (chisq==-1) std::cout << "error\n";
            if (chisq<chisq_best)  { 
                fit_parameters_best=fit_parameters;
                chisq_best=chisq;
            }
        }
    }
    fit_parameters=fit_parameters_best;
    fitted_data->fit_info.chisq_abs = round(chisq_best);
    fitted_data->fit_info.fit_parameters=fit_parameters;
    std::vector<double> X, Y;
    if (interp_res>0) {
        int points = data_XY.back()[0]/interp_res;
        X.resize(points);
        for (int i=0;i<points;i++) {
            X[i]=i*interp_res;
        }
        Y=statistics::get_polynom_Y(fit_parameters,&X);
    } else {
        std::vector<std::vector<double>> temp = Tools::transpose_matrix_double(data_XY);
        X=temp[0];
        Y=statistics::get_polynom_Y(fit_parameters,&X);
    }
    fitted_data->data_XY_transposed.push_back(X);
    fitted_data->data_XY_transposed.push_back(Y);
    std::vector<double> xx = Tools::transpose_matrix_double(data_XY).at(0);
    Y=statistics::get_polynom_Y(fit_parameters,&xx);
    double mean = statistics::get_mean_from_Y(Y);
    fitted_data->fit_info.chisq_rel = chisq_best/mean;
    
    return true;
}
*/


std::vector<double> statistics::interpolate_data_XY_transposed(std::vector<std::vector<double>> data_XY_transposed, std::vector<double> X) {
    std::vector<double> Y,xv,yv;
    double yi;
    double *x,*y;
//     std::vector<std::vector<double>> data_XY_XY_transposed=data_XY_transposed;
    
    const size_t c=data_XY_transposed[0].size();
    gsl_interp_accel *acc = gsl_interp_accel_alloc ();
//     gsl_spline *spline = gsl_spline_alloc (gsl_interp_cspline, c);
    gsl_spline *spline_akima = gsl_spline_alloc(gsl_interp_akima, c);
    xv=data_XY_transposed[0];
    yv=data_XY_transposed[1];
// 	print(xv);
    x=&xv[0];
    y=&yv[0];
    gsl_spline_init (spline_akima, x, y, c);
// 	std::cout << "init done\n";
//     for (size_t i = 0; i < c; ++i)
//     printf("%g %g\n", x[i], y[i]);
        
    for (int i=0; i < X.size(); i++) {
//         std::cout << X->at(i) << std::endl;
        yi = gsl_spline_eval (spline_akima, X.at(i), acc);
// 		std::cout << "A";
        
        Y.push_back(yi);
      }
//       std::cout << std::endl;
    gsl_spline_free (spline_akima);
    gsl_interp_accel_free (acc);
    return Y;
}







/********* NEW *************/

//from file:///tmp/mozilla_florian0/Example-programs-for-B_002dsplines.html  ???
std::vector<double> statistics::interpolate_bspline(std::map<double, double>& data_XY,std::vector<double> X_new, int bspline_degree)
{
	const size_t k = bspline_degree +1;
	const size_t n = data_XY.size();
// 	const size_t nbreak = data_XY.size();
	/* nbreak = ncoeffs + 2 - k */
// 	const size_t ncoeffs = nbreak - 2 + k;
	const size_t ncoeffs = 20;
	const size_t nbreak = ncoeffs + 2 - k;
	size_t i, j;
	gsl_bspline_workspace *bw;
	gsl_vector *B;
	double dy;
		gsl_rng *r;
	gsl_vector *c, *w;
	gsl_vector *x, *y;
	gsl_matrix *X, *cov;
	gsl_multifit_linear_workspace *mw;
	double chisq, Rsq, dof, tss;

	gsl_rng_env_setup();
	r = gsl_rng_alloc(gsl_rng_default);

	/* allocate a cubic bspline workspace (k = 4) */
	bw = gsl_bspline_alloc(k, nbreak);
	B = gsl_vector_alloc(ncoeffs);

	x = gsl_vector_alloc(n);
	y = gsl_vector_alloc(n);
	X = gsl_matrix_alloc(n, ncoeffs);
	c = gsl_vector_alloc(ncoeffs);
	w = gsl_vector_alloc(n);
	cov = gsl_matrix_alloc(ncoeffs, ncoeffs);
	mw = gsl_multifit_linear_alloc(n, ncoeffs);

	/* this is the data to be fitted */  
	std::vector<double> Xdata,Ydata;
	tools::vec::split_map_to_vecs(data_XY,&Xdata,&Ydata);
	for (i = 0; i < n; ++i)
    {
		gsl_vector_set (y, i, Ydata[i]);
		gsl_vector_set (x, i, Xdata[i]);
    }

  /* use uniform breakpoints on the intervall */
  gsl_bspline_knots_uniform(Xdata.front(), Xdata.back(), bw);
// 	gsl_bspline_knots(x,bw);
  /* construct the fit matrix X */
  for (i = 0; i < n; ++i)
    {
      double xi = gsl_vector_get(x, i);

      /* compute B_j(xi) for all j */
      gsl_bspline_eval(xi, B, bw);

      /* fill in row i of X */
      for (j = 0; j < ncoeffs; ++j)
        {
          double Bj = gsl_vector_get(B, j);
          gsl_matrix_set(X, i, j, Bj);
        }
    }
	
  /* do the fit */
  ///ATTENTION hier ist irgend ne matrix nicht quadratisch
  gsl_multifit_wlinear(X, w, y, c, cov, &chisq, mw);

  dof = n - ncoeffs;
  tss = gsl_stats_wtss(w->data, 1, y->data, 1, y->size);
  Rsq = 1.0 - chisq / tss;

  fprintf(stderr, "chisq/dof = %e, Rsq = %f\n", chisq / dof, Rsq);

  printf("\n\n");

	
  /* output the smoothed curve */
	if (X_new.size()==0) X_new = Xdata;
	std::vector<double> Y_new(X_new.size());
    std::vector<double> Y_err(X_new.size());
	
	for (int i = 0; i<X_new.size(); i++)
	{
		gsl_bspline_eval(X_new[i], B, bw);
		gsl_multifit_linear_est(B, c, cov, &Y_new[i], &Y_err[i]);
	}


	gsl_rng_free(r);
	gsl_bspline_free(bw);
	gsl_vector_free(B);
	gsl_vector_free(x);
	gsl_vector_free(y);
	gsl_matrix_free(X);
	gsl_vector_free(c);
	gsl_vector_free(w);
	gsl_matrix_free(cov);
	gsl_multifit_linear_free(mw);

	return Y_new;
}

std::vector<double> statistics::get_gsl_vec(gsl_vector* Y)
{
	std::vector<double> Yout(Y->size);
	
	for (int i = 0; i < Y->size; ++i)
		Yout.at(i) = gsl_vector_get(Y, i);
	return Yout;
}

gsl_vector * statistics::get_gsl_vec(const std::vector<double>& Y)
{
	gsl_vector *y;
	int n = Y.size();
	y = gsl_vector_alloc(n);
	for (int i = 0; i < n; ++i)
	{
		gsl_vector_set (y, i, Y[i]);
// 		std::cout << "y=" << *(y->data) << std::endl;
	}
	return y;
}

std::vector<double> statistics::bspline_smooth(const std::vector<double>& Ydata, std::vector<double> Xdata, unsigned int breakpoints, const size_t spline_order)
{
// 	breakpoints=100;
	if (breakpoints==0)
		breakpoints=50;
	//nbreak = ncoeffs + 2 - spline_order
	//ncoeffs = nbreak - 2 + spline_order 
	if (breakpoints - 2 + spline_order < 1)
		return {};
	if (Xdata.size()==0)
	{
		Xdata.resize(Ydata.size());
		for (int i=0;i<Ydata.size();i++)
			Xdata.at(i)=i;
	}
	if (Xdata.size() != Ydata.size())
		return {};
// 	std::cout << "Xdata.size()=" << Xdata.size() << "\tYdata.size()=" << Ydata.size() << std::endl;
// 	print(Xdata);
	const size_t n = Ydata.size();
	const size_t nbreak = breakpoints;
	const size_t ncoeffs = breakpoints - 2 + spline_order;
	size_t i, j;
	gsl_bspline_workspace *bw;
	gsl_vector *B;
// 	double dy;
	gsl_rng *r;
	gsl_vector *c, *w;
	gsl_vector *x ;
	gsl_vector *y ;
	
	gsl_matrix *X, *cov;
	gsl_multifit_linear_workspace *mw;
	double chisq, Rsq, dof, tss;

	gsl_rng_env_setup();
	r = gsl_rng_alloc(gsl_rng_default);

	/* allocate a cubic bspline workspace (k = 4) */
	bw = gsl_bspline_alloc(spline_order, nbreak);
	B = gsl_vector_alloc(ncoeffs);

	x = gsl_vector_alloc(n);
	y = gsl_vector_alloc(n);
	X = gsl_matrix_alloc(n, ncoeffs);
	c = gsl_vector_alloc(ncoeffs);
	w = gsl_vector_alloc(n);
	cov = gsl_matrix_alloc(ncoeffs, ncoeffs);
	mw = gsl_multifit_linear_alloc(n, ncoeffs);
	
	x = get_gsl_vec(Xdata);
	y = get_gsl_vec(Ydata);
	
	/* this is the data to be fitted */
	for (i = 0; i < n; ++i)
// 	{
// 		double sigma;
// 		double xi = (15.0 / (n - 1)) * i;
// 		double yi = cos(xi) * exp(-0.1 * xi);
// 
// 		sigma = 0.1 * yi;
// 		dy = gsl_ran_gaussian(r, sigma);
// 		yi += dy;
// 
// 		gsl_vector_set(x, i, xi);
// 		gsl_vector_set(y, i, yi);
		gsl_vector_set(w, i, 1.0) ;
// 
// 		printf("%f %f\n", xi, yi);
// 	}

	/* use uniform breakpoints on [0, 15] */
	gsl_bspline_knots_uniform(Xdata.front(), Xdata.back(), bw);

	/* construct the fit matrix X */
	for (i = 0; i < n; ++i)
	{
		double xi = Xdata.at(i);

		/* compute B_j(xi) for all j */
		gsl_bspline_eval(xi, B, bw);

		/* fill in row i of X */
		for (j = 0; j < ncoeffs; ++j)
		{
			double Bj = gsl_vector_get(B, j);
			gsl_matrix_set(X, i, j, Bj);
		}
	}
	
	/* do the fit */
	gsl_multifit_wlinear(X, w, y, c, cov, &chisq, mw);
// 	gsl_multifit_linear(X, y, c, cov, &chisq, mw);

	dof = n - ncoeffs;
	tss = gsl_stats_wtss(w->data, 1, y->data, 1, y->size);
// 	tss = gsl_stats_tss(y->data,1,y->size);
	Rsq = 1.0 - chisq / tss;

// 	fprintf(stderr, "chisq/dof = %e, Rsq = %f\n",
// 					chisq / dof, Rsq);

// 	printf("\n\n");

	/* output the smoothed curve */
	std::vector<double> Y_fitted(n);
	std::vector<double> Y_fitted_err(n);
	
	for (int i=0;i<Xdata.size();i++)
	{
		gsl_bspline_eval(Xdata.at(i), B, bw);
		gsl_multifit_linear_est(B, c, cov, &Y_fitted.at(i), &Y_fitted_err.at(i));
	}
	
// 	Y_fitted.at(Y_fitted.size()-1) = Y_fitted.at(Y_fitted.size()-2);

// 	plot_t plot(false,false);
// 	plot.Y1.range(0,1E5,false);
// 	plot.Y2.range(0,1E5,false);
// 	plot.Y1.add_curve(Xdata,Ydata);
// 	plot.Y2.add_curve(Xdata,Y_fitted);
// 	plot.to_screen("",0);
	
	
	
	gsl_rng_free(r);
	gsl_bspline_free(bw);
	gsl_vector_free(B);
	gsl_vector_free(x);
	gsl_vector_free(y);
	gsl_matrix_free(X);
	gsl_vector_free(c);
// 	gsl_vector_free(w);
	gsl_matrix_free(cov);
	gsl_multifit_linear_free(mw);

	return Y_fitted;
}


std::vector<double> statistics::interpolate_data_XY(const std::map<double,double>& data_XY, const std::vector<double>& X) 
{

    std::vector<double> Y,xv,yv;
    double yi;
    double *x,*y;
    const size_t c=data_XY.size();
	
// 	for (auto& d : data_XY)
// 		std::cout << d.first << "\t" << d.second << std::endl;
	
// 	for (auto& x : X)
// 		std::cout << x << std::endl;
// 	std::cout << "data_XY.size()=" << data_XY.size() << std::endl;
// 	std::cout << "X.size()=" << X.size() << std::endl;
	
	if (c<10) return {};
    gsl_interp_accel *acc = gsl_interp_accel_alloc ();
	
	//TODO:maybe use gsl_interp_steffen instead of gsl_interp_akima
    gsl_spline *spline_akima = gsl_spline_alloc(gsl_interp_akima, c);
	tools::vec::split_map_to_vecs(data_XY,&xv,&yv);

    x=&xv[0];
    y=&yv[0];
    gsl_spline_init (spline_akima, x, y, c);
	
	
    for (int i=0; i < X.size(); i++) {

        yi = gsl_spline_eval (spline_akima, X.at(i), acc);
        Y.push_back(yi);
      }
// 	std::cout << "data_XY.size()=" << data_XY.size() << std::endl;
// 	std::cout << "X.size()=" << X.size() << std::endl;
    gsl_spline_free (spline_akima);
    gsl_interp_accel_free (acc);
    return Y;
}


double statistics::integrate(std::map<double,double>& data_XY, unsigned int start_idx, unsigned int stop_idx) 
{
    double sum=0;
	std::map<double,double>::iterator it,it_prev;
	double dY,dX;
	it = data_XY.begin();
	int i=0;
    for (++it;it!=data_XY.end();++it)
	{
		if (i > stop_idx)	break;
		i++;
		it_prev=it;
		--it_prev;
        dY=abs(it->second+it_prev->second)/2;
        dX=abs(it->first-it_prev->first);
        if (start_idx>=i) sum=sum+dX*dY;
    }
    return sum;
}


std::map<double, double> change_X_resolution(const std::map<double, double>& data_XY,const double new_resolution)
{
	if (new_resolution<=0) return {};
	const int size = floor((data_XY.rbegin()->first - data_XY.begin()->first)/new_resolution)+1;
	std::vector<double> new_X(size);
	for (int i=0;i<size;i++)
		new_X[i]=i*new_resolution + data_XY.begin()->first;
	std::vector<double> new_Y = statistics::interpolate_data_XY(data_XY,new_X);
	std::map<double,double> new_data_XY_map;
	tools::vec::combine_vecs_to_map(&new_X,&new_Y,&new_data_XY_map);
	return new_data_XY_map;
}

unsigned int statistics::factorial(const unsigned int& fac)
{
	if (fac==0) return 1;
	unsigned int result = 1;
	for (int i=1;i<=fac;i++)
		result *= i;
	return result;
}

std::vector<double> statistics::absolute(std::vector<double> data)
{
	for (auto& d:data)
		d = abs(d);
	return data;
}

std::vector<double> statistics::round(std::vector<double> data_s ,unsigned int decimals)
{
	const double multiplier = pow(10,decimals);
	for (auto& d : data_s)
        d = ::round(d*multiplier)/multiplier;
	return data_s;
}
double statistics::round(double data_s ,unsigned int decimals)
{
	const double multiplier = pow(10,decimals);
	return ::round(data_s*multiplier)/multiplier;
}


/*************************************************************/
/***   			    statistics::histogram_t 			  ****/
/*************************************************************/

statistics::histogram_t::histogram_t(const std::vector<double>& data, unsigned int number_of_bins) : 
			data(data), number_of_bins(number_of_bins), range(data,number_of_bins)
{
}

std::vector<unsigned int> statistics::histogram_t::linear_bins() const
{
	return fill_bins(range.equal_distributed_linear());
}
std::vector<unsigned int> statistics::histogram_t::log10_bins() const
{
	return fill_bins(range.equal_distributed_log10());
}

std::vector<unsigned int> statistics::histogram_t::fill_bins(const std::vector<double>& range) const
{
	std::vector<unsigned int> histogram(number_of_bins);
	gsl_histogram * h = gsl_histogram_alloc (number_of_bins);
	gsl_histogram_set_ranges(h,&range[0],range.size());
	for (const auto& d : data)
		gsl_histogram_increment(h,d);
	for (int i=0;i<number_of_bins;i++)
	{
		histogram[i] = gsl_histogram_get(h,i);
// 		double b=0;
// 		double t=0;
// 		gsl_histogram_get_range(h,i,&b,&t);
// 		std::cout << "range[" << i << "]=(" << b << "," << t << ")" << std::endl;
	}
	gsl_histogram_free(h);
	return histogram;
}

/*************************************************************/
/***   			  statistics::histogram_t::range_t	  	  ****/
/*************************************************************/

// statistics::histogram_t::range_t::range_t(const double& min, const double& max, const unsigned int& number_of_bins) : min(min), max(max), number_of_bins(number_of_bins)
// {
// }

statistics::histogram_t::range_t::range_t(const std::vector<double>& data, const unsigned int& number_of_bins) : data(data), number_of_bins(number_of_bins)
{
}

const std::vector<double> statistics::histogram_t::range_t::equal_distributed_linear() const
{
	std::vector<double> r(number_of_bins+1);
	auto min = get_min_from_Y(data);
	auto delta = (get_max_from_Y(data)-min)/number_of_bins;
	for (int i=0;i<number_of_bins+1;i++)
	{
		r[i]=min+delta*i;
	}
	return r;
}

const std::vector<double> statistics::histogram_t::range_t::equal_distributed_log10() const
{
	auto data_abs = absolute(data);
	std::vector<double> r(number_of_bins+1);
	double min=1, max=1;
	std::sort(data_abs.begin(),data_abs.end());
	for (auto& d : data_abs)
	{
		if (d>0)
		{
			min=d;
			break;
		}
	}
	for (auto i=data_abs.rbegin();i!=data_abs.rend();i++)
	{
		if (*i>0)
		{
			max=*i;
			break;
		}
	}
	const auto lmax = log10(max);
	const auto lmin = log10(min);
	auto delta = (lmax-lmin)/number_of_bins;
	for (int i=0;i<number_of_bins+1;i++)
	{
		r[i]=min*pow(10,delta*i);
	}
	return r;
}




