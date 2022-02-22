#include "quantity.hpp"



/*******************************/

quantity::quantity_t::quantity_t(const quantity_t& quant_s, double data) : quantity_t(quant_s,vector<double>{data})
{
}

quantity::quantity_t::quantity_t(const quantity_t& quant_s, vector<double> data) : name_p(quant_s.name()),unit_p(quant_s.unit()),data(data)
{
}

quantity::quantity_t::quantity_t(string name_s, vector<double> data_s, unit_t unit_s)
{
	name_p = name_s;
	unit_p = unit_s;
	data = data_s;
}

quantity::quantity_t::quantity_t(string name_s,unit_t unit_s)
{
	name_p = name_s;
	unit_p = unit_s;
}

quantity::quantity_t::quantity_t(vector<double> data_s,unit_t unit_s)
{
	data = data_s;
	unit_p = unit_s;
}

quantity::quantity_t::quantity_t()
{

}

quantity::quantity_t::quantity_t(const quantity_t& quant_s, const unit_t& unit_s) : unit_p(unit_s), data(quant_s.data), name_p(quant_s.name())
{
}

void quantity::quantity_t::clear()
{
	data.clear();
}

quantity::quantity_t::quantity_t(double data_s, unit_t unit_s) : unit_p(unit_s), data(data_s)
{
}

const vector<double> quantity::quantity_t::data_X_1D() const
{
	vector<double> X(data.size());
	for (int x=0;x<X.size();x++)
		X[x]=x;
	return X;
}


const std::map<double,double> quantity::quantity_t::data_XY_1D()
{
	std::map<double,double> data_XY; 
	tools::vec::combine_vecs_to_map(data_X_1D(),&data,&data_XY);
	return data_XY;
}

fit_functions::polynom_t quantity::quantity_t::polynom(unsigned int polynom_grade) const
{
	fit_functions::polynom_t poly(polynom_grade,data);
// 	if (!is_set()) 
// 		return poly;
	
// 	std::map<double,double> data_XY;
// 	vector<double> X(data.size());
// 	for (int x=0;x<X.size();x++)
// 		X[x]=x;
// 	tools::vec::combine_vecs_to_map(&X,&data,&data_XY);

	return poly;
}

quantity::quantity_t quantity::quantity_t::bspline_smoothing( unsigned int breakpoints, const size_t spline_order) const
{
	if (!is_set())
		return {};
	
	stringstream ss;
	ss << "BSpline" << spline_order<< "_smooth(" << name() << ")";
	return {ss.str(),statistics::bspline_smooth(data,{}, breakpoints,spline_order),unit()};
}

quantity::quantity_t quantity::quantity_t::bspline_smoothing(const quantity_t& Xdata, unsigned int breakpoints, const size_t spline_order) const
{
	if (!is_set())
		return {};
	
	stringstream ss;
	ss << "BSpline" << spline_order<< "_smooth(" << name() << ")";
	return {ss.str(),statistics::bspline_smooth(data,Xdata.data, breakpoints,spline_order),unit()};
}

quantity::quantity_t quantity::quantity_t::interp(const quantity_t& old_X, const quantity_t& new_X) const
{
	if (!is_set())
		return {};
	if (data.size()==1)
		return *this;
	if (!old_X.is_set() || !new_X.is_set() || !is_set()) 
	{
		logger::error("quantity::quantity_t::interp","!old_X.is_set() || !new_X.is_set() || !is_set()","","returning empty");
		return {};
	}
	if (old_X.data.size()!=data.size())
	{
		logger::error("quantity::quantity_t::interp","old_X.data.size()!=data.size()","current_X=" +to_string() +" old_X="+old_X.to_string(),"returning empty");
		return {};
	}
	if (old_X.unit().base_units_exponents != new_X.unit().base_units_exponents)
	{
		logger::error("quantity::quantity_t::interp","old_X.unit().base_units_exponents != new_X.unit().base_units_exponents","","returning empty");
		return {};
	}
	quantity_t old_X_same_unit;
	if (old_X.unit() != new_X.unit())
		old_X_same_unit = old_X.change_unit(new_X.unit());
	else
		old_X_same_unit = old_X;
	
	std::map<double,double> XY_data;
	tools::vec::combine_vecs_to_map(&old_X_same_unit.data,&data,&XY_data);
	
	if (XY_data.begin()->first > new_X.data.front() )
	{
		logger::error("quantity::quantity_t::interp","old_X start="+ tools::to_string(XY_data.begin()->first) +" new_X start=" + tools::to_string(new_X.data.front()),"extrapolation not supported","returning empty");
		return {};
	}
	if (XY_data.rbegin()->first < new_X.data.back() )
	{
		logger::error("quantity::quantity_t::interp","old_X stop=" + tools::to_string(XY_data.rbegin()->first) + " new_X stop="+tools::to_string(new_X.data.back()),"extrapolation not supported","returning empty");
		return {};
	}
	return {name(),statistics::interpolate_data_XY(XY_data,new_X.data),unit()};
}


quantity::quantity_t quantity::quantity_t::fit_polynom_by_x_data(quantity_t& x_data, quantity_t new_x_data, int polynom_grade) const
{
	if (!is_set())
		return {};
	if (!x_data.is_set()) return quantity_t();
	if (!is_set()) return quantity_t();
	if (polynom_grade==-1) polynom_grade=17; // seems to be good for implant profiles
	
	
	std::map<double,double> data_XY;
	tools::vec::combine_vecs_to_map(&x_data.data,&data,&data_XY);
	fit_functions::polynom_t polynom(polynom_grade,data_XY);
	stringstream ss;
	ss << "poly" << polynom_grade << "_fit(" << name() << ")";
	quantity_t fit(ss.str(),{},{unit()});
	if (new_x_data.is_set()) fit.data=polynom.y_data(new_x_data.data);
	else fit.data=polynom.y_data(x_data.data);
	return fit;
}

quantity::quantity_t quantity::quantity_t::polyfit_derivative(unsigned int polynom_grade, unsigned int derivative) const
{
	fit_functions::polynom_t polynom_s = polynom(polynom_grade);
	
	if (!polynom_s.successfully_fitted()) 
	{
		logger::error("quantity::quantity_t::polyfit_derivative","!polynom_s.successfully_fitted()","returning empty");
		return {};
	}
	stringstream ss;
	ss << "poly" << polynom_grade << "_derivative" << derivative<< "_fit(" << name() << ")";
	quantity_t fit(ss.str(),{},{unit()});
	fit.data=polynom_s.derivative(derivative).y_data(data_X_1D());
	return fit;
}


quantity::quantity_t quantity::quantity_t::polyfit(unsigned int polynom_grade) const
{
// 	if (!is_set()) 
// 		return quantity_t();
// 	
// 	if (idx_stop<0)
// 		idx_stop = data.size()-1;
// 	if (data.size()<idx_stop)
// 	{
// // 		cout << "return {}" << endl;
// 		return {};
// 	}
// 	
// 	if (idx_start!=0 || idx_stop!=data.size()-1)
// 	{
// // 		cout << "return {}" << endl;
// 		return remove_data_by_index(idx_start,idx_stop).polyfit(polynom_grade);
// 	}
// 	
// // 	vector<double> data;
// 	fit_functions::polynom_t polynom;
// 	map<double,double> data_XY;
// 	vector<double> X(data.size());
	
// 	tools::vec::combine_vecs_to_map(&X,&data,&data_XY);
	
// 	cout << "data.size()=" << data.size() << endl;
	fit_functions::polynom_t polynom_s = polynom(polynom_grade);
	if (!polynom_s.successfully_fitted()) 
	{
		return {};
	}
	stringstream ss;
	ss << "poly" << polynom_grade << "_fit(" << name() << ")";
	quantity_t fit(ss.str(),{},{unit()});
	fit.data=polynom_s.y_data(data_X_1D());
	return fit;
}

quantity::quantity_t quantity::quantity_t::round_() const
{
	if (!is_set()) return {};
	quantity_t copy = *this;
	for (int i=0;i<copy.data.size();i++)
        copy.data.at(i) = round(copy.data.at(i));
	return copy;
}

quantity::quantity_t quantity::quantity_t::diff(bool preserve_size) const
{
	if (!is_set() || data.size()==1) return {};
	stringstream ss;
	ss << "diff("<<name()<<")";
	quantity_t diff(ss.str(),unit());
	diff.data.resize(data.size()-1);
	for (int i=0;i<data.size()-1;i++)
		diff.data.at(i)=data.at(i+1)-data.at(i);
	if (preserve_size)
		diff.data.push_back(diff.data.back()); // copy last element
	return diff;
}



quantity::quantity_t quantity::quantity_t::integrate_pbp(const quantity_t& x_data) const
{
	if (data.size()!=x_data.data.size()) return {};
	if (data.size()==1) // fall back to simple multiplication
		return *this * x_data;
// 	quantity_t area;

// 	area.dimension = x_data.dimension + "*" + dimension;
// 	area.unit = x_data.unit + "*" + unit;
// 	area.name = "integral_pbp("+name+")d("+x_data.name+")";
	
	stringstream n,u;
	n << "integral_pbp("<<name()<<")d(" << x_data.name() << ")";
	quantity_t area(n.str(),{},unit()*x_data.unit());
	area.data.resize(data.size());

	double sum=0;
	double dY,dX;
	area.data[0]=abs(data[0]+data[1])/2 * abs(x_data.data[0]-x_data.data[1]);
    for (int i=1;i<data.size();i++)
	{
        dY=abs(data[i]+data[i-1])/2;
        dX=abs(x_data.data[i]-x_data.data[i-1]);
		sum=sum+dX*dY;
        area.data[i] = sum; 
    }
	return area;
}

// quantity_t quantity::quantity_t::integrate(quantity_t& x_data, double lower_X_limit, double upper_X_limit)
// {
// 	return integrate(x_data,lower_X_limit,upper_X_limit);
// }

quantity::quantity_t quantity::quantity_t::integrate(const quantity_t& x_data, unsigned int lower_X_index, unsigned int upper_X_index) const
{
	if (lower_X_index<0)
		      lower_X_index=0;
	if (upper_X_index>=x_data.data.size() || upper_X_index<=0)
		      upper_X_index = x_data.data.size() -1 ;
	if (x_data.data.size() != data.size())
	{
		logger::error("quantity::quantity_t::integrate()","x_data.data.size() != data.size()","return empty");
		return {};
	}
	
	auto int_pbp = integrate_pbp(x_data);
	double sum=int_pbp.data.at(upper_X_index);
	if (lower_X_index>0)
		sum -= int_pbp.data.at(lower_X_index) ;
	stringstream n,u;
	n << "integral("<<name()<<")d(" << x_data.name() << ")";
	quantity_t area(n.str(),{sum},unit()*x_data.unit());
	return area;
}

int quantity::quantity_t::get_value_index_position_in_strictly_monotonic_increasing_vector(const double value, const vector<double>& monotonic_vec)
{
	for (int i=0;i<monotonic_vec.size();i++)
	{
		if (monotonic_vec.at(i)>=value)
			return i;
	}
	return -1;
}

quantity::quantity_t quantity::quantity_t::integrate(const quantity_t& x_data, const quantity_t& x_data_start, const quantity_t& x_data_stop) const
{
	unsigned int lower_X_limit=0;
	unsigned int upper_X_limit=0;
	if (!x_data_start.is_scalar())
		lower_X_limit=0;
	else
	{
		if (x_data.unit().base_units_exponents!=x_data_start.unit().base_units_exponents)
		{
			logger::error("quantity::quantity_t::integrate()","x_data.unit()="+x_data.unit().to_string(),"x_data_start.unit()="+x_data_start.unit().to_string());
			return {};
		}
		if (x_data.unit()!=x_data_start.unit())
			return integrate(x_data,x_data_start.change_unit(x_data.unit()),x_data_stop.change_unit(x_data.unit()));
		int x = get_value_index_position_in_strictly_monotonic_increasing_vector(x_data_start.data.front(),x_data.data);
		if (x>=0)
			lower_X_limit=x;
	}
	if (!x_data_stop.is_scalar())
		upper_X_limit = x_data.data.size() -1;
	else
	{
		if (x_data.unit().base_units_exponents!=x_data_stop.unit().base_units_exponents)
		{
			logger::error("quantity::quantity_t::integrate()","x_data.unit()="+x_data.unit().to_string(),"x_data_stop.unit()="+x_data_stop.unit().to_string());
			return {};
		}
		if (x_data.unit()!=x_data_stop.unit())
			return integrate(x_data,x_data_start.change_unit(x_data.unit()),x_data_stop.change_unit(x_data.unit()));
		int x = get_value_index_position_in_strictly_monotonic_increasing_vector(x_data_stop.data.front(),x_data.data);
		if (x>=0)
			upper_X_limit=x;
	}
	if (x_data.data.size() != data.size())
	{
		logger::error("quantity::quantity_t::integrate()","x_data.data.size() != data.size()","return empty");
		return {};
	}
	return integrate(x_data,lower_X_limit,upper_X_limit);
}

quantity::quantity_t quantity::quantity_t::max_at_x(const quantity_t& X) const
{
	if (data.size()!=X.data.size())
		return {};
	if (data.size()==0)
		return {};
// 	stringstream n;
// 	n << name()  << "(" << X.name() << ")at_max(" << name() << ")";
// 	map<double,double> XY_data;
// 	tools::vec::combine_vecs_to_map(&X.data,&data,&XY_data);
// 	vector<double> X_data,Y_data;
// 	tools::vec::split_map_to_vecs(XY_data,&X_data,&Y_data);
	
// 	if (X_data.size()==0) return {};
	int vec_pos=statistics::get_max_index_from_Y(data);
	quantity_t x_at_max_p = X;
	x_at_max_p.data = {X.data.at(vec_pos)};
	return x_at_max_p;
}

/// TESTEN!!!
// quantity_t quantity::quantity_t::max_at_x(const quantity_t& X, double lower_X_limit, double upper_X_limit) const
// {
// 	for(int x=0;x<X.data.size();x++)
// 	{
// 		if (x<lower_X_limit || x>upper_X_limit) 
// 		{
// 			X.data.erase(X.data.begin()+x);
// 			x--;
// 		}
// 	}
// 	return max_at_x(X);
// }

quantity::quantity_t quantity::quantity_t::min_at_x(quantity_t& X, double lower_X_limit, double upper_X_limit) const
{
	stringstream n;
	n << name()  << "("<<X.name()<<")at_min("<<name()<<")";
	std::map<double,double> XY_data;
	tools::vec::combine_vecs_to_map(&X.data,&data,&XY_data);
	for(std::map<double,double>::iterator it=XY_data.begin();it!=XY_data.end();++it)
	{
		if (it->first<lower_X_limit || it->first>upper_X_limit) 
		{
			XY_data.erase(it);
			--it;
		}
	}
	vector<double> X_data,Y_data;
	tools::vec::split_map_to_vecs(XY_data,&X_data,&Y_data);
	int vec_pos=statistics::get_min_index_from_Y(Y_data);
	quantity_t x_at_min_p(n.str(),{X_data[vec_pos]},unit());
	return x_at_min_p;
}


quantity::quantity_t quantity::quantity_t::median() const
{
	stringstream n;
	n << "median(" << name() << ")";
	quantity_t median(n.str(),{statistics::get_median_from_Y(data)},unit());
	return median;
}

quantity::quantity_t quantity::quantity_t::quantile(double percentile) const
{
	if (!is_set())
		return {};
	if (percentile>1 || percentile<0) percentile=0.75;
	stringstream n;
	n << "percentile" << percentile << "(" << name() << ")";
	quantity_t quantile(n.str(),{statistics::get_quantile_from_Y(data,percentile)},unit());
	return quantile;
}

quantity::quantity_t quantity::quantity_t::geo_mean() const
{
	if (!is_set())
		return {};
	stringstream n;
	n << "geo_mean(" << name() << ")";
	quantity_t mean(n.str(),unit());
	mean.data.resize(1);
	mean.data.front()=1;
	for (auto& d:data)
		mean.data.front() *= d;
	mean.data.front() = pow(mean.data.front(),1/data.size());
	return mean;
}

quantity::quantity_t quantity::quantity_t::mean() const
{
	if (!is_set())
		return {};
	stringstream n;
	n << "mean(" << name() << ")";
	quantity_t mean(n.str(),{statistics::get_mean_from_Y(data)},unit());
	return mean;
}

quantity::quantity_t quantity::quantity_t::cut_mean(float alpha) const
{
	if (!is_set())
		return {};
	stringstream n;
	n << "cut_"<< alpha << "mean(" << name() << ")";
	int start = alpha * data.size();
	int stop = (1-alpha) * data.size();
	auto reduced = at(start,stop);
	quantity_t mean(n.str(),{statistics::get_mean_from_Y(reduced.data)},unit());
	return mean;
}

quantity::quantity_t quantity::quantity_t::cut_median(float alpha) const
{
	if (!is_set())
		return {};
	stringstream n;
	n << "cut_"<< alpha << "median(" << name() << ")";
	int start = alpha * data.size();
	int stop = (1-alpha) * data.size();
	auto reduced = at(start,stop);
	quantity_t mean(n.str(),{statistics::get_median_from_Y(reduced.data)},unit());
	return mean;
}

quantity::quantity_t quantity::quantity_t::trimmed_mean(float alpha) const
{
	if (!is_set())
		return {};
	stringstream n;
	n << "tr" << alpha << "_mean(" << name() << ")";
	quantity_t tr_mean(n.str(),{statistics::get_trimmed_mean_from_Y(data,alpha)},unit());
	return tr_mean;
}

quantity::quantity_t quantity::quantity_t::gastwirth() const
{
	if (!is_set())
		return {};
	stringstream n;
	n << "gastwirth(" << name() << ")";
	quantity_t gastw(n.str(),{statistics::statistics::get_gastwirth_estimator_from_Y(data)},unit());
	return gastw;
}

quantity::quantity_t quantity::quantity_t::mad() const
{
	if (!is_set())
		return {};
	stringstream n;
	n << "mad(" << name() << ")";
	quantity_t mad(n.str(),{statistics::statistics::get_mad_from_Y(data)},unit());
	return mad;
}

quantity::quantity_t quantity::quantity_t::back() const
{
	return {name(),{data.back()},unit()};
}

quantity::quantity_t quantity::quantity_t::front() const
{
	return {name(),{data.front()},unit()};
}

quantity::quantity_t quantity::quantity_t::moving_window_mad(int window_size) const
{
	if (!is_set())
		return {};
	if (window_size==0) window_size = 0.05*data.size();
	if (window_size==0) return quantity_t();

	stringstream n;
	n << "mw" << window_size <<"_mad(" << name() << ")";
	quantity_t mad(n.str(),{statistics::get_moving_window_MAD_from_Y(data,window_size)},unit());
// 	quantity_t mad(name(),{statistics::get_moving_window_MAD_from_Y(data,window_size)},unit());
	return mad;
}

quantity::quantity_t quantity::quantity_t::reverse() const
{
	auto copy = *this;
	size_t size = data.size();
	for (int i=0;i<size;i++)
		copy.data.at(size-1-i) = data.at(i);
	return copy;
}


quantity::quantity_t quantity::quantity_t::moving_window_iqr(int window_size) const
{
	return moving_window_qqr(window_size,0.25);
}

quantity::quantity_t quantity::quantity_t::moving_window_qqr(int window_size, double q) const
{
	if (!is_set())
		return {};
	if (window_size==0) window_size = 0.05*data.size();
	if (window_size==0) return quantity_t();

	stringstream n;
	n << "mw" << window_size <<"_qqr" << q <<"(" << name() << ")";
	quantity_t _mean(n.str(),{statistics::get_moving_window_qqr_from_Y(data,window_size,q)},unit());
	return _mean;
}

quantity::quantity_t quantity::quantity_t::moving_window_max(int window_size) const
{
	if (!is_set())
		return {};
	if (window_size==0) window_size = 0.05*data.size();
	if (window_size==0) return quantity_t();

	stringstream n;
	n << "mw" << window_size <<"_max(" << name() << ")";
	quantity_t _mean(n.str(),{statistics::get_moving_window_max_from_Y(data,window_size)},unit());
	return _mean;
}

quantity::quantity_t quantity::quantity_t::moving_window_min(int window_size) const
{
	if (!is_set())
		return {};
	if (window_size==0) window_size = 0.05*data.size();
	if (window_size==0) return quantity_t();

	stringstream n;
	n << "mw" << window_size <<"_min(" << name() << ")";
	quantity_t _mean(n.str(),{statistics::get_moving_window_min_from_Y(data,window_size)},unit());
	return _mean;
}

quantity::quantity_t quantity::quantity_t::moving_window_sum(int window_size) const
{
	if (!is_set())
		return {};
	if (window_size==0) window_size = 0.05*data.size();
	if (window_size==0) return quantity_t();

	stringstream n;
	n << "mw" << window_size <<"_sum(" << name() << ")";
	quantity_t _mean(n.str(),{statistics::get_moving_window_sum_from_Y(data,window_size)},unit());
	return _mean;
}


quantity::quantity_t quantity::quantity_t::moving_window_mean(int window_size) const
{
	if (!is_set())
		return {};
	if (window_size==0) window_size = 0.05*data.size();
	if (window_size==0) return quantity_t();

	stringstream n;
	n << "mw" << window_size <<"_mean(" << name() << ")";
	quantity_t _mean(n.str(),{statistics::get_moving_window_mean_from_Y(data,window_size)},unit());
	return _mean;
}

quantity::quantity_t quantity::quantity_t::moving_window_median(int window_size) const
{
	if (!is_set())
		return {};
	if (window_size==0) window_size = 0.05*data.size();
	if (window_size==0) return quantity_t();

	stringstream n;
	n << "mw" << window_size <<"_median(" << name() << ")";
	quantity_t _median(n.str(),{statistics::get_moving_window_median_from_Y(data,window_size)},unit());
	return _median;
}

quantity::quantity_t quantity::quantity_t::moving_window_sd(int window_size) const
{
	if (!is_set())
		return {};
	if (window_size==0) window_size = 0.05*data.size();
	if (window_size==0) return quantity_t();

	stringstream n;
	n << "mw" << window_size <<"_sd(" << name() << ")";
	quantity_t _sd(n.str(),{statistics::get_moving_window_sd_from_Y(data,window_size)},unit());
	return _sd;
}

quantity::quantity_t quantity::quantity_t::at(unsigned int pos) const
{
	if (!is_set())
		return {};
	if (data.size()>=pos)
		return {};
	return {name(),{data.at(pos)},unit()};
}

quantity::quantity_t quantity::quantity_t::at(unsigned int start_pos, unsigned int end_pos) const
{
	if (!is_set())
		return {};
	if (data.size()>=end_pos)
		end_pos = data.size()-1;
	unsigned int new_size = end_pos - start_pos + 1;
	vector<double> new_vec(new_size);
	for (int i=start_pos;i<new_size;i++)
	{
		new_vec.at(i-start_pos) = data.at(i);
	}
	return {name(),new_vec,unit()};
}


quantity::quantity_t quantity::quantity_t::log10() const
{
	if (!is_set())
		return {};
	quantity_t out = *this;
	for (auto& o : out.data)
		o = std::log10(o);
	return out;
}

quantity::quantity_t quantity::quantity_t::sd() const
{
	if (!is_set())
		return {};
	stringstream n;
	n << "sd(" << name() << ")";
	quantity_t sd(n.str(),{statistics::statistics::get_sd_from_Y(data)},unit());
	return sd;
}

quantity::quantity_t quantity::quantity_t::max() const
{
	if (!is_set()) return {};
	double max = statistics::get_max_from_Y(data);
	stringstream n;
	n << "max(" << name() << ")";
	return {n.str(),{max},unit()};
}

quantity::quantity_t quantity::quantity_t::min() const
{
	if (!is_set()) return {};
	int min_index = statistics::get_min_index_from_Y(data);
// 	if (min_index==0) return {};
// 	if (min_index>data.size()-2) return {}; 
// 	if (min_index<2) return {}; 

	stringstream n;
	n << "min(" << name() << ")";
	quantity_t min(n.str(),{data[min_index]},unit());
	return min;
}

bool quantity::quantity_t::is_nan() const
{
	for (auto& d : data)
		if (isnan(d))
			return true;
	return false;
}

bool quantity::quantity_t::is_inf() const
{
	for (auto& d : data)
		if (isinf(d))
			return true;
	return false;
}

bool quantity::quantity_t::is_set() const
{
	if (data.size()==0) return false;
// 	if (dimension.length()==0) return false;
// 	if (!unit().is_set()) return false;
	return true;
}

bool quantity::quantity_t::is_scalar() const
{
	if (!is_set()) return false;
	if (data.size()==1) return true;
	return false;
}

bool quantity::quantity_t::is_vector() const
{
	if (!is_set()) return false;
	if (is_scalar()) return false;
	return true;
}

quantity::quantity_t quantity::quantity_t::filter_impulse(int window_size, float factor) const
{
	if (!is_set())
		return {};
	if (data.size()<window_size)  return *this;
	if (window_size==0) window_size=0.05*data.size();
	if (window_size==0) window_size=3;
	
	stringstream n;
	n << "impulse_filtered(" << name() << ")";
	quantity_t filtered(n.str(),{statistics::impulse_filter(data,window_size,factor)},unit());
	return filtered;
}

quantity::quantity_t quantity::quantity_t::filter_gaussian(int window_size, double alpha) const
{
	if (!is_set())
		return {};
	if (data.size()<window_size)  return *this;
	if (window_size==0) window_size=0.05*data.size();
	stringstream n;
	n << "gaussian_filtered(" << name() << ")";
	quantity_t gaussian_filtered(n.str(),{statistics::impulse_filter(data,window_size,alpha)},unit());
	return gaussian_filtered;
}

const quantity::quantity_t quantity::quantity_t::remove_inf() const
{
	vector<unsigned int> remove_idx;
	for (int i=0;i<data.size();i++)
		if (isinf(data.at(i)))
			remove_idx.push_back(i);
	return remove_data_by_index(remove_idx);
}

const quantity::quantity_t quantity::quantity_t::remove_nan() const
{
	vector<unsigned int> remove_idx;
	for (int i=0;i<data.size();i++)
		if (isnan(data.at(i)))
			remove_idx.push_back(i);
	return remove_data_by_index(remove_idx);
}


quantity::quantity_t quantity::quantity_t::filter_recursive_median(int window_size) const
{
	if (!is_set())
		return {};
	if (data.size()<window_size)  return *this;
	if (window_size==0) window_size=0.05*data.size();
	stringstream n;
	n << "recursiveMedian_filtered(" << name() << ")";
	quantity_t recursiveMedian_filtered(n.str(),{statistics::recursive_median_filter(data,window_size)},unit());
	return recursiveMedian_filtered;
}

quantity::quantity_t quantity::quantity_t::change_resolution(unsigned int new_data_size) const
{
	if (new_data_size<=1)
		return {};
	if (data.size()<=1)
		return {};
	quantity_t new_res = resolution() * (new_data_size-1)/(data.size()-1);
	return change_resolution({name(),new_res.data,new_res.unit()});
}

quantity::quantity_t quantity::quantity_t::change_resolution(quantity_t new_res) const
{
	if (!is_set() || !new_res.is_set()) return{};
// 	if (name() != new_res.name())
// 	{
// 		logger::error("quantity::quantity_t::change_resolution()","name() != new_res.name()",name() + "!=" + new_res.name(),"returning empty");
// 		return {};
// 	}
	if (new_res.data.size()!=1)
		logger::debug(13,"quantity::quantity_t::change_resolution()","new_res.data.size()!=1",tools::to_string(new_res.data.size()));
// 	new_res = new_res.change_unit(unit());
	if (!new_res.is_set())
	{
		logger::debug(13,"quantity::quantity_t::change_resolution()","!new_res.is_set()","","returning empty");
		return {};
	}
	quantity_t old_Q = change_unit(new_res.unit());
	if (!old_Q.is_set())
	{
		logger::debug(13,"quantity::quantity_t::resolution()","!old_Q.is_set()","","returning empty");
		return {};
	}
	logger::debug(13,"quantity::quantity_t::resolution()","new_res",new_res.to_string());
// 	return resolution(new_res.data.front());
	return old_Q.resolution(new_res.data.front());
}

quantity::quantity_t quantity::quantity_t::resolution(double new_res) const
{
	if (!is_set())	return {};
	if (new_res==0) //the momentary resolutiion
	{
		stringstream ss;
		ss << "resolution(" << name()<<")";
		return {ss.str(),{diff().absolute().mean().data},unit()};
	}
	if (new_res<0) return {};
	if (data.size()<2) return {};
	
	quantity_t new_quantity(name(),unit());
	double max = statistics::get_max_from_Y(data);
	double min = statistics::get_min_from_Y(data);
	int new_size = floor((max-min)/new_res)+1;
	new_quantity.data.resize(new_size);
	for (int i=0;i<new_size;i++)
		new_quantity.data.at(i) = i*new_res + min;
	return new_quantity;
}

const string quantity::quantity_t::to_string() const
{
	ostringstream out;
	if (data.size()>1)
		out <<  name() << " = " <<"<" << data.size() << ">" << " [" << unit().to_string() << "]";
	else if (data.size()==1)
		out <<  name() << " = " << data[0] << " [" << unit().to_string() << "]";
	else
		out <<  name() << " = " <<"<0>" << " []";
	return out.str();
}

const string quantity::quantity_t::to_string_detailed() const
{
	ostringstream out;
	if (data.size()>1)
	{
		out <<  name() << " = " <<"<" << data.size() << ">" << " [" << unit().to_string() << "]";
		out << " " << min().to_string();
		out << " " << max().to_string();
		out << " " << median().to_string();
		out << " " << mean().to_string();
		out << " " << sd().to_string();
	}
	else if (data.size()==1)
		out <<  name() << " = " << data[0] << " [" << unit().to_string() << "]";
	else
	{
		out <<  name() << " = " <<"<0>" << " []";
	}
	return out.str();
}



// quantity_t quantity::quantity_t::mean() const
// {
// 	quantity_t mean_p{"mean("+name()+")",{statistics::get_mean_from_Y(data)},unit()};
// 	return mean_p;
// }
// 
// quantity_t quantity::quantity_t::median() const
// {
// 	quantity_t median_p{"median("+name()+")",{statistics::get_median_from_Y(data)},unit()};
// 	return median_p;
// }

const string quantity::quantity_t::name() const
{
	return name_p;
}

const unit_t& quantity::quantity_t::unit() const
{
	return unit_p;
}

const unit_t quantity::quantity_t::unit_copy() const
{
	return unit_p;
}

quantity::quantity_t quantity::quantity_t::change_unit(string target_unit_string) const
{
	quantity_t q = change_unit(unit_t(target_unit_string));
	if (q.is_set())
	{
		q.unit_p.prefered_output_string = target_unit_string;
		logger::debug(15,"quantity::quantity_t::change_unit()","q.unit_p.prefered_output_string=" + q.unit_p.prefered_output_string, "target_unit_string=" + target_unit_string );
		return q;
	}
	return {};
}

quantity::quantity_t quantity::quantity_t::change_unit(unit_t target_unit) const
{	
	if (!is_set())
		return {};
	if (unit()==target_unit) // nothing to change
		return *this;
	if (unit().base_units_exponents != target_unit.base_units_exponents) 
	{
		logger::warning(1,"quantity::quantity_t::change_unit()","unit().base_units_exponents != target_unit.base_units_exponents",target_unit.to_string(),"returning this");
		return {};
	}
// 	quantity_t copy = *this;
	double factor = unit().multiplier / target_unit.multiplier;
// 	for (int i=0;i<data.size();i++)
// 		copy.data[i] *= factor ;
	logger::debug(14,"quantity::quantity_t::change_unit()","old: "+unit().to_string(),"new: " + target_unit.to_string(),"changed");
	return quantity_t{*this * factor,target_unit};
}

quantity::quantity_t quantity::quantity_t::invert() const
{
	if (!is_set()) return {};
	quantity_t inv = *this;
	for (auto& d : inv.data)
		d = 1/d;
	stringstream ss;
	ss << "inv(" << name() << ")";
	inv.name_p = ss.str();
// 	ss.str("");
// 	ss << "1/" << unit().to_string();
// 	unit_t u(ss.str());
	return {inv.name(),inv.data, unit().invert()};
}

quantity::quantity_t quantity::quantity_t::remove_data_from_begin(unsigned int stop) const
{
// 	cout << "stop=" << stop << endl;
	if (stop==0)
		return *this;
	return remove_data_by_index(0,stop);
}

quantity::quantity_t quantity::quantity_t::remove_data_from_begin(const quantity_t& remove_stop) const
{
// 	if (!remove_stop.is_set())
// 		return {};
// 	cout << remove_stop.change_unit(unit()).to_string() << endl;
	return remove_data_from_begin(remove_stop.change_unit(unit()).data.front());
}

quantity::quantity_t quantity::quantity_t::remove_data_by_index(vector<unsigned int> remove_pos) const
{
	if (remove_pos.size()==0) 
		return *this;
	vector<double> new_data(data.size()-remove_pos.size()); 
	int new_data_counter=0;
	for (int i=0;i<data.size();i++)
	{
		for (const auto& skip : remove_pos)
		{
			if (skip==i)
				continue;
			new_data.at(new_data_counter) = data.at(i);
			new_data_counter++;
		}
	}
	const quantity_t new_q(name(),new_data,unit());
	return new_q;
}

quantity::quantity_t quantity::quantity_t::remove_data_by_index(unsigned int start, unsigned int stop) const
{
	if (!is_set())
		return {};
	if (start<0)
		start=0;
	if (start>=data.size())
	{
		logger::error("quantity::quantity_t::remove_data_by_index()","start>=data.size()","","return empty");
		return {};
	}
	if (stop>data.size())
	{
		stop=data.size();
	}
	quantity_t copy = *this;

	copy.data.erase(copy.data.begin()+start,copy.data.begin()+stop);
	return copy;
}

quantity::quantity_t quantity::quantity_t::get_data_by_index(unsigned int start, unsigned int stop) const
{
	if (!is_set())
		return {};
	if (start<0)
		start=0;
	if (start>data.size())
	{
		logger::error("quantity::quantity_t::get_data_by_index()","start>=data.size()","","return empty");
		return {};
	}
	if (stop>data.size())
	{
		stop=data.size();
	}
	quantity_t copy = *this;
	copy.data = vector<double> (data.begin()+start,data.begin()+stop);
	return copy;
}

quantity::quantity_t quantity::quantity_t::abs_sum() const
{
	if (!is_set()) return {};
	double sum=0;
	for (auto& d: data)
		sum += pow(d,2);
	sum = sqrt(sum);
	stringstream ss;
	ss << "abs_sum(" << name() << ")";
	return {ss.str(),{sum},unit()};
}

quantity::quantity_t quantity::quantity_t::absolute() const
{
	if (!is_set()) return {};
	quantity_t copy = *this;
	for (auto& d : copy.data)
		d=abs(d);
	stringstream ss;
	ss << "abs(" << name() << ")";
	return {ss.str(),copy.data,unit()};
}

quantity::quantity_t quantity::quantity_t::sum(int start, int stop) const
{
	if (!is_set()) return {};
	if (stop<0)
		stop = data.size();
	if (start<0)
		start = 0;
// 	quantity_t sum = *this;
// 	sum.data.resize(stop-start);
	double summe = 0;
	for (int i=start;i<stop;i++)
		summe += data.at(i);
	return {name(),{summe},unit()};
}


bool quantity::quantity_t::operator<(const quantity_t& obj) const
{
	if (*this > obj || *this == obj)
		return false;
	return true;
}

bool quantity::quantity_t::operator>(const quantity_t& obj) const
{
	if (!is_set() || !obj.is_set())
		return false;
	quantity_t quantity_with_same_unit;
	
  	if (unit()!=obj.unit())
		quantity_with_same_unit = change_unit(obj.unit());
	else
		quantity_with_same_unit = *this;
	
	if (obj.data.size() != quantity_with_same_unit.data.size()) 
		return false;
// 	for (int i=0;i<obj.data.size();i++)
// 		if (obj.data.at(i) != quantity_with_same_unit.data.at(i))
// 			return false;
	if (abs_sum().data.front() <= obj.abs_sum().data.front())
		return false;
	return true;
}

bool quantity::quantity_t::operator==(const quantity_t& obj) const
{
	if (unit()!=obj.unit()) return false;
	if (data.size()!=obj.data.size())	 return false;
	for (int i=0;i<data.size();i++)
		if (data[i]!=obj.data[i]) return false;
	return true;
}

bool quantity::quantity_t::operator!=(const quantity_t& obj) const
{
	return !(operator==(obj));
}

void quantity::quantity_t::operator<<(quantity_t obj)
{
	if (!obj.is_set()) return;
	if (!is_set())
	{
		*this = obj;
		return;
	}
	if (unit().base_units_exponents!=obj.unit().base_units_exponents)
	{
		logger::error("quantity::quantity_t::operator<<()","base units are different: " + unit().base_units_exponents.to_string() + " != " + obj.unit().base_units_exponents.to_string());
		return;
	}
	quantity_t adder = obj.change_unit(unit());
	tools::vec::add(&data,adder.data);
}

quantity::quantity_t quantity::quantity_t::operator+(const quantity_t& quantity) const
{
	if (name()!=quantity.name()) return {}; // only allow addition within the same Größenart
	if (!is_set() || !quantity.is_set()) return {};
	if (unit() != quantity.unit()) return {};
	quantity_t sum=*this;
	if (data.size() == quantity.data.size())
	{
		sum.data.resize(data.size());
		for (int i=0; i< data.size()&&i<quantity.data.size();i++)
		{
			sum.data[i] = data[i] + quantity.data[i];
		}
	} 
	else if (data.size()==1)
	{
		sum.data.resize(quantity.data.size());
		for (int i=0; i< sum.data.size();i++)
		{
			sum.data[i] = data[0] + quantity.data[i];
		}
	}
	else if (quantity.data.size()==1)
	{
		sum.data.resize(data.size());
		for (int i=0; i< sum.data.size();i++)
		{
			sum.data[i] = quantity.data[0] + data[i];
		}
	}
	return sum;
}

quantity::quantity_t quantity::quantity_t::operator+(const double summand) const
{
	if (!is_set()) return {};
	quantity_t sum=*this;

	for (int i=0; i< data.size()&&i<data.size();i++)
	{
		sum.data[i] += summand;
	}
	return sum;
}

quantity::quantity_t quantity::quantity_t::operator-(const quantity_t& quantity) const
{
	return {*this + quantity*(-1)};
}
quantity::quantity_t quantity::quantity_t::operator-(const double summand) const
{
	return {*this + summand*(-1)};
}

quantity::quantity_t quantity::quantity_t::operator * (const quantity_t& quantity_p) const
{
	if (!is_set() || !quantity_p.is_set()) return {};

	quantity_t produkt{name() + "*" + quantity_p.name(),{unit()*quantity_p.unit()}};
	
	if (data.size()==1)
	{
		produkt.data=quantity_p.data;
		for (auto& d:produkt.data)
			d*=data[0];
	}
	else if (quantity_p.data.size()==1)
	{
		produkt.data=data;
		for (auto& d:produkt.data)
			d*=quantity_p.data[0];
	}
	else if (quantity_p.data.size()==data.size())
	{
		produkt.data=data;
		for (int i=0;i<data.size();i++)
			produkt.data[i] *= quantity_p.data[i];
	}
	else return {};
	return produkt;
}

quantity::quantity_t quantity::quantity_t::operator / (const quantity_t& quantity_p) const
{
	if (!is_set() || !quantity_p.is_set()) return {};
	quantity_t quotient{name() + " / (" + quantity_p.name() +")",{unit() / quantity_p.unit()}};
	
	if (quantity_p.data.size()==1)
	{
		quotient.data.resize(data.size());
		for (int i=0;i<data.size();i++)
			quotient.data[i]=data[i]/quantity_p.data[0];
	}
	else if (quantity_p.data.size()==data.size())
	{
		quotient.data.resize(data.size());
		for (int i=0;i<data.size();i++)
			quotient.data[i]=data[i]/quantity_p.data[i];
	}
	else if (data.size()==1)
	{
		quotient.data.resize(quantity_p.data.size());
		for (int i=0;i<quotient.data.size();i++)
			quotient.data[i]=data[0]/quantity_p.data[i];
	}
	else
	{
// 		cout << ">>>ERROR<<<: " <<  quotient.name() << endl;
// 		cout << name() << ".data.size()= " << data.size() << " divide by " << quantity_p.name() << ".data.size()= " << quantity_p.data.size() << endl;
		return {};
	}
	return quotient;
}

quantity::quantity_t quantity::quantity_t::operator/ (const double divisor) const
{
	quantity_t quotient=*this;
	for (int i=0;i<data.size();i++)
	{
		quotient.data[i]=data[i]/divisor;
	}

	return quotient;
}

quantity::quantity_t quantity::quantity_t::operator * (const double factor) const
{
	quantity_t multiplier=*this;
	for (int i=0;i<data.size();i++)
	{
		multiplier.data[i] *= factor;
	}
	return multiplier;
}

void quantity::quantity_t::operator+=(const double summand)
{
	*this = *this + summand;
}

void quantity::quantity_t::operator+=(const quantity_t& quantity_p)
{
	*this = *this + quantity_p;
}

/*********************/

quantity::mass_t::mass_t(vector<double> data_s,unit_t unit_s) : quantity_t("mass",data_s,unit_s){}
quantity::mass_t::mass_t(quantity_t quantity_s) : mass_t(quantity_s.data,quantity_s.unit()) {}

quantity::abundance_t::abundance_t(vector<double> data_s,unit_t unit_s) : quantity_t("abundance",data_s,unit_s){}
quantity::abundance_t::abundance_t(quantity_t quantity_s) : abundance_t(quantity_s.data,quantity_s.unit()) {}

quantity::energy_t::energy_t(vector<double> data_s,unit_t unit_s) : quantity_t("energy",data_s,unit_s){}
quantity::energy_t::energy_t(quantity_t quantity_s) : energy_t(quantity_s.data,quantity_s.unit()) {}

quantity::rastersize_t::rastersize_t(vector<double> data_s,unit_t unit_s) : quantity_t( "rastersize",data_s,unit_s){}
quantity::rastersize_t::rastersize_t(quantity_t quantity_s) : rastersize_t(quantity_s.data,quantity_s.unit()) {}

quantity::depth_t::depth_t(vector<double> data_s,unit_t unit_s) : quantity_t("depth",data_s,unit_s){}
quantity::depth_t::depth_t(quantity_t quantity_s) : depth_t(quantity_s.data,quantity_s.unit()) {}

quantity::sputter_depth_t::sputter_depth_t(vector<double> data_s,unit_t unit_s) : quantity_t("sputter_depth",data_s,unit_s){}
quantity::sputter_depth_t::sputter_depth_t(quantity_t quantity_s) : sputter_depth_t(quantity_s.data,quantity_s.unit()) {}

quantity::total_sputter_depth_t::total_sputter_depth_t(vector<double> data_s,unit_t unit_s) : quantity_t("total_sputter_depth",data_s,unit_s){}
quantity::total_sputter_depth_t::total_sputter_depth_t(quantity_t quantity_s) : total_sputter_depth_t(quantity_s.data,quantity_s.unit()) {}

quantity::sputter_time_t::sputter_time_t(vector<double> data_s,unit_t unit_s) : quantity_t("sputter_time",data_s,unit_s){}
quantity::sputter_time_t::sputter_time_t(quantity_t quantity_s) : sputter_time_t(quantity_s.data,quantity_s.unit()) {}

quantity::total_sputter_time_t::total_sputter_time_t(vector<double> data_s,unit_t unit_s) : quantity_t("total_sputter_time",data_s,unit_s){}
quantity::total_sputter_time_t::total_sputter_time_t(quantity_t quantity_s) : total_sputter_time_t(quantity_s.data,quantity_s.unit()) {}

quantity::intensity_t::intensity_t(vector<double> data_s,unit_t unit_s) : quantity_t("intensity",data_s,unit_s){}
quantity::intensity_t::intensity_t(quantity_t quantity_s) : intensity_t(quantity_s.data,quantity_s.unit()) {}

quantity::current_t::current_t(vector<double> data_s,unit_t unit_s) : quantity_t("current",data_s,unit_s){}
quantity::current_t::current_t(quantity_t quantity_s) : current_t(quantity_s.data,quantity_s.unit()) {}

quantity::sputter_current_t::sputter_current_t(vector<double> data_s,unit_t unit_s) : quantity_t("sputter_current_t",data_s,unit_s){}
quantity::sputter_current_t::sputter_current_t(quantity_t quantity_s) : sputter_current_t(quantity_s.data,quantity_s.unit()) {}

quantity::analysis_current_t::analysis_current_t(vector<double> data_s,unit_t unit_s) : quantity_t("primary_analysis_current",data_s,unit_s){}
quantity::analysis_current_t::analysis_current_t(quantity_t quantity_s) : analysis_current_t(quantity_s.data,quantity_s.unit()) {}

quantity::concentration_t::concentration_t(vector<double> data_s,unit_t unit_s) : quantity_t("concentration",data_s,unit_s){}
quantity::concentration_t::concentration_t(quantity_t quantity_s) : concentration_t(quantity_s.data,quantity_s.unit()) {}

quantity::dose_t::dose_t(vector<double> data_s,unit_t unit_s) : quantity_t("dose",data_s,unit_s){}
quantity::dose_t::dose_t(quantity_t quantity_s) : dose_t(quantity_s.data,quantity_s.unit()) {}

quantity::SF_t::SF_t(vector<double> data_s,unit_t unit_s) : quantity_t("sensitivity_factor",data_s,unit_s){}
quantity::SF_t::SF_t(quantity_t quantity_s) : SF_t(quantity_s.data,quantity_s.unit()) {}

quantity::RSF_t::RSF_t(vector<double> data_s,unit_t unit_s) : quantity_t("relative_sensitivity_factor",data_s,unit_s){}
quantity::RSF_t::RSF_t(quantity_t quantity_s) : RSF_t(quantity_s.data,quantity_s.unit()) {}

quantity::SR_t::SR_t(vector<double> data_s,unit_t unit_s) : quantity_t("sputter_rate",data_s,unit_s){}
quantity::SR_t::SR_t(quantity_t quantity_s) : SR_t(quantity_s.data,quantity_s.unit()) {}

quantity::secondary_voltage_t::secondary_voltage_t(vector<double> data_s, unit_t unit_s) : quantity_t("secondary_voltage",data_s,unit_s) {}
quantity::secondary_voltage_t::secondary_voltage_t(quantity_t quantity_s) :secondary_voltage_t(quantity_s.data,quantity_s.unit()) {}

quantity::substance_amount_t::substance_amount_t(vector<double> data_s, unit_t unit_s) : quantity_t("substance_amount",data_s,unit_s) {}
quantity::substance_amount_t::substance_amount_t(quantity_t quantity_s) :substance_amount_t(quantity_s.data,quantity_s.unit()) {}

quantity::electrical_charge_t::electrical_charge_t(vector<double> data_s, unit_t unit_s) : quantity_t("electrical_charge",data_s,unit_s) {}
quantity::electrical_charge_t::electrical_charge_t(quantity_t quantity_s) :electrical_charge_t(quantity_s.data,quantity_s.unit()) {}




