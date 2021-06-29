#include "quantity.hpp"



/*******************************/

quantity_t::quantity_t(const quantity_t& quant_s, double data) : quantity_t(quant_s,vector<double>{data})
{
}

quantity_t::quantity_t(const quantity_t& quant_s, vector<double> data) : name_p(quant_s.name()),unit_p(quant_s.unit()),data(data)
{
}

quantity_t::quantity_t(std::__cxx11::string name_s, vector<double> data_s, unit_t unit_s)
{
	name_p = name_s;
	unit_p = unit_s;
	data = data_s;
}

quantity_t::quantity_t(string name_s,unit_t unit_s)
{
	name_p = name_s;
	unit_p = unit_s;
}

quantity_t::quantity_t(vector<double> data_s,unit_t unit_s)
{
	data = data_s;
	unit_p = unit_s;
}

quantity_t::quantity_t()
{

}

quantity_t::quantity_t(const quantity_t& quant_s, const unit_t& unit_s) : unit_p(unit_s), data(quant_s.data), name_p(quant_s.name())
{
}

void quantity_t::clear()
{
	data.clear();
}

quantity_t::quantity_t(double data_s, unit_t unit_s) : unit_p(unit_s), data(data_s)
{
}


quantity_t quantity_t::interp(const quantity_t& old_X, const quantity_t& new_X) const
{
	if (!is_set())
		return {};
	if (!old_X.is_set() || !new_X.is_set() || !is_set()) 
	{
		logger::error("quantity_t::interp","!old_X.is_set() || !new_X.is_set() || !is_set()","","returning empty");
		return {};
	}
	if (old_X.data.size()!=data.size())
	{
		logger::error("quantity_t::interp","old_X.data.size()!=data.size()","","returning empty");
		return {};
	}
	if (old_X.unit().base_units_exponents != new_X.unit().base_units_exponents)
	{
		logger::error("quantity_t::interp","old_X.unit().base_units_exponents != new_X.unit().base_units_exponents","","returning empty");
		return {};
	}
	quantity_t old_X_same_unit;
	if (old_X.unit() != new_X.unit())
		old_X_same_unit = old_X.change_unit(new_X.unit());
	else
		old_X_same_unit = old_X;
	
	map<double,double> XY_data;
	tools::vec::combine_vecs_to_map(&old_X_same_unit.data,&data,&XY_data);
	return {name(),statistics::interpolate_data_XY(XY_data,new_X.data),unit()};
}


quantity_t quantity_t::fit_polynom_by_x_data(quantity_t& x_data, quantity_t new_x_data, int polynom_grade) const
{
	if (!is_set())
		return {};
	if (!x_data.is_set()) return quantity_t();
	if (!is_set()) return quantity_t();
	if (polynom_grade==-1) polynom_grade=17; // seems to be good for implant profiles
	
	fit_functions::polynom_t polynom;
	map<double,double> data_XY;
	tools::vec::combine_vecs_to_map(&x_data.data,&data,&data_XY);
	stringstream ss;
	ss << "poly" << polynom_grade << "_fit(" << name() << ")";
	quantity_t fit(ss.str(),{},{unit()});
	polynom.fit(data_XY,polynom_grade);
	
	if (new_x_data.is_set()) fit.data=polynom.fitted_y_data(new_x_data.data);
	else fit.data=polynom.fitted_y_data(x_data.data);
	return fit;
}

quantity_t quantity_t::polyfit(unsigned int polynom_grade, int idx_start, int idx_stop) const
{
	if (!is_set()) 
	{
// 		cout << "return {}" << endl;
		return quantity_t();
	}
// 	if (polynom_grade==-1) polynom_grade=17; // seems to be good for implant profiles
	if (idx_stop<0)
		idx_stop = data.size()-1;
	if (data.size()<idx_stop)
	{
// 		cout << "return {}" << endl;
		return {};
	}
	
	if (idx_start!=0 || idx_stop!=data.size()-1)
	{
// 		cout << "return {}" << endl;
		return remove_data_by_index(idx_start,idx_stop).polyfit(polynom_grade);
	}
	
// 	vector<double> data;
	fit_functions::polynom_t polynom;
	map<double,double> data_XY;
	vector<double> X(data.size());
	for (int x=0;x<X.size();x++)
		X[x]=x;
	tools::vec::combine_vecs_to_map(&X,&data,&data_XY);
	stringstream ss;
	ss << "poly" << polynom_grade << "_fit(" << name() << ")";
	quantity_t fit(ss.str(),{},{unit()});
// 	cout << "data.size()=" << data.size() << endl;
	if (!polynom.fit(data_XY,polynom_grade)) 
	{
// 		cout << "fit error" << endl;
		return {};
	}
	fit.data=polynom.fitted_y_data(X);
	return fit;
}

quantity_t quantity_t::round_() const
{
	if (!is_set()) return {};
	quantity_t copy = *this;
	for (int i=0;i<copy.data.size();i++)
        copy.data.at(i) = round(copy.data.at(i));
	return copy;
}

quantity_t quantity_t::diff(bool preserve_size) const
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



quantity_t quantity_t::integrate_pbp(const quantity_t& x_data) const
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

// quantity_t quantity_t::integrate(quantity_t& x_data, double lower_X_limit, double upper_X_limit)
// {
// 	return integrate(x_data,lower_X_limit,upper_X_limit);
// }

quantity_t quantity_t::integrate(const quantity_t& x_data, unsigned int lower_X_limit, unsigned int upper_X_limit) const
{
	map<double,double> data_XY;
	tools::vec::combine_vecs_to_map(&x_data.data,&data,&data_XY);
	stringstream n;
	n << "integral("<<name()<<")d(" << x_data.name() << ")";
	quantity_t area(n.str(),{statistics::integrate(data_XY,lower_X_limit,upper_X_limit)},unit()*x_data.unit());
	return area;
}

quantity_t quantity_t::max_at_x(const quantity_t& X) const
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
// quantity_t quantity_t::max_at_x(const quantity_t& X, double lower_X_limit, double upper_X_limit) const
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

quantity_t quantity_t::min_at_x(quantity_t& X, double lower_X_limit, double upper_X_limit) const
{
	stringstream n;
	n << name()  << "("<<X.name()<<")at_min("<<name()<<")";
	map<double,double> XY_data;
	tools::vec::combine_vecs_to_map(&X.data,&data,&XY_data);
	for(map<double,double>::iterator it=XY_data.begin();it!=XY_data.end();++it)
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


quantity_t quantity_t::median() const
{
	stringstream n;
	n << "median(" << name() << ")";
	quantity_t median(n.str(),{statistics::get_median_from_Y(data)},unit());
	return median;
}

quantity_t quantity_t::quantile(double percentile) const
{
	if (!is_set())
		return {};
	if (percentile>1 || percentile<0) percentile=0.75;
	stringstream n;
	n << "percentile" << percentile << "(" << name() << ")";
	quantity_t quantile(n.str(),{statistics::get_quantile_from_Y(data,percentile)},unit());
	return quantile;
}

quantity_t quantity_t::geo_mean() const
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

quantity_t quantity_t::mean() const
{
	if (!is_set())
		return {};
	stringstream n;
	n << "mean(" << name() << ")";
	quantity_t mean(n.str(),{statistics::get_mean_from_Y(data)},unit());
	return mean;
}

quantity_t quantity_t::trimmed_mean(float alpha) const
{
	if (!is_set())
		return {};
	stringstream n;
	n << "tr" << alpha << "_mean(" << name() << ")";
	quantity_t tr_mean(n.str(),{statistics::get_trimmed_mean_from_Y(data,alpha)},unit());
	return tr_mean;
}

quantity_t quantity_t::gastwirth() const
{
	if (!is_set())
		return {};
	stringstream n;
	n << "gastwirth(" << name() << ")";
	quantity_t gastw(n.str(),{statistics::statistics::get_gastwirth_estimator_from_Y(data)},unit());
	return gastw;
}

quantity_t quantity_t::mad() const
{
	if (!is_set())
		return {};
	stringstream n;
	n << "mad(" << name() << ")";
	quantity_t mad(n.str(),{statistics::statistics::get_mad_from_Y(data)},unit());
	return mad;
}

quantity_t quantity_t::back() const
{
	return {name(),{data.back()},unit()};
}

quantity_t quantity_t::front() const
{
	return {name(),{data.front()},unit()};
}

quantity_t quantity_t::moving_window_mad(int window_size) const
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

quantity_t quantity_t::moving_window_mean(int window_size) const
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

quantity_t quantity_t::moving_window_median(int window_size) const
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

quantity_t quantity_t::moving_window_sd(int window_size) const
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

quantity_t quantity_t::log10() const
{
	if (!is_set())
		return {};
	quantity_t out = *this;
	for (auto& o : out.data)
		o = std::log10(o);
	return out;
}

quantity_t quantity_t::sd() const
{
	if (!is_set())
		return {};
	stringstream n;
	n << "sd(" << name() << ")";
	quantity_t sd(n.str(),{statistics::statistics::get_sd_from_Y(data)},unit());
	return sd;
}

quantity_t quantity_t::max() const
{
	if (!is_set()) return {};
	double max = statistics::get_max_from_Y(data);
	stringstream n;
	n << "max(" << name() << ")";
	return {n.str(),{max},unit()};
}

quantity_t quantity_t::min() const
{
	if (!is_set()) return {};
	int min_index = statistics::get_max_index_from_Y(data);
	if (min_index==0) return {};
	if (min_index>data.size()-2) return {}; 
	if (min_index<2) return {}; 

	stringstream n;
	n << "min(" << name() << ")";
	quantity_t min(n.str(),{data[min_index]},unit());
	return min;
}

bool quantity_t::is_set() const
{
	if (data.size()==0) return false;
// 	if (dimension.length()==0) return false;
// 	if (!unit().is_set()) return false;
	return true;
}

quantity_t quantity_t::filter_impulse(int window_size, float factor) const
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

quantity_t quantity_t::filter_gaussian(int window_size, double alpha) const
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


quantity_t quantity_t::filter_recursive_median(int window_size) const
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


quantity_t quantity_t::resolution(quantity_t new_res) const
{
	if (!is_set() || !new_res.is_set()) return{};
	if (name() != new_res.name())
	{
		logger::error("quantity_t::resolution()","name() != new_res.name()",name() + "!=" + new_res.name(),"returning empty");
		return {};
	}
	if (new_res.data.size()!=1)
		logger::debug(4,"quantity_t::resolution()","new_res.data.size()!=1",tools::to_string(new_res.data.size()));
// 	new_res = new_res.change_unit(unit());
	if (!new_res.is_set())
	{
		logger::debug(4,"quantity_t::resolution()","!new_res.is_set()","","returning empty");
		return {};
	}
	quantity_t old_Q = change_unit(new_res.unit());
	if (!old_Q.is_set())
	{
		logger::debug(4,"quantity_t::resolution()","!old_Q.is_set()","","returning empty");
		return {};
	}
	logger::debug(5,"quantity_t::resolution()","new_res",new_res.to_string());
// 	return resolution(new_res.data.front());
	return old_Q.resolution(new_res.data.front());
}

quantity_t quantity_t::resolution(double new_res) const
{
	if (!is_set())	return {};
	if (new_res==0) //the momentary resolutiion
	{
		stringstream ss;
		ss << "resolution(" << name()<<")";
		return {ss.str(),{diff().mean().data},unit()};
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

const std::__cxx11::string quantity_t::to_string() const
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

// quantity_t quantity_t::mean() const
// {
// 	quantity_t mean_p{"mean("+name()+")",{statistics::get_mean_from_Y(data)},unit()};
// 	return mean_p;
// }
// 
// quantity_t quantity_t::median() const
// {
// 	quantity_t median_p{"median("+name()+")",{statistics::get_median_from_Y(data)},unit()};
// 	return median_p;
// }

const std::__cxx11::string quantity_t::name() const
{
	return name_p;
}

unit_t quantity_t::unit() const
{
	return unit_p;
}


quantity_t quantity_t::change_unit(unit_t target_unit) const
{	
	if (!is_set())
		return {};
	if (unit().base_units_exponents != target_unit.base_units_exponents) 
	{
		logger::warning(1,"quantity_t::change_unit","unit().base_units_exponents != target_unit.base_units_exponents",target_unit.to_string(),"returning this");
		return *this;
	}
// 	quantity_t copy = *this;
	double factor = unit().multiplier / target_unit.multiplier;
// 	for (int i=0;i<data.size();i++)
// 		copy.data[i] *= factor ;
	logger::debug(11,"quantity_t::change_unit",unit().to_string(),target_unit.to_string(),"changed");
	return quantity_t{*this * factor,target_unit};
}

quantity_t quantity_t::invert() const
{
	if (!is_set()) return {};
	quantity_t inv = *this;
	for (auto& d : inv.data)
		d = 1/d;
	stringstream ss;
	ss << "inv(" << name() << ")";
	inv.name_p = ss.str();
	ss.str("");
	ss << "1/" << unit().to_string();
	unit_t u(ss.str());
	return {inv.name(),inv.data,u};
}

quantity_t quantity_t::remove_data_from_begin(unsigned int stop) const
{
	return remove_data_by_index(0,stop);
}


quantity_t quantity_t::remove_data_by_index(unsigned int start, unsigned int stop) const
{
	if (!is_set())
		return {};
	if (start<0)
		start=0;
	if (start>=data.size())
	{
		logger::error("quantity_t::remove_data_by_index()","start>=data.size()","","return empty");
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

quantity_t quantity_t::get_data_by_index(unsigned int start, unsigned int stop) const
{
	if (!is_set())
		return {};
	if (start<0)
		start=0;
	if (start>data.size())
	{
		logger::error("quantity_t::filter_data_by_index()","start>=data.size()","","return empty");
		return {};
	}
	if (stop>data.size())
	{
// 		logger::error("quantity_t::filter_data_by_index()","stop>=data.size()","","return empty");
// 		return {};
		stop=data.size();
	}
	
	quantity_t copy = *this;
	copy.data = vector<double> (data.begin()+start,data.begin()+stop);
// 	copy.data.erase(copy.data.begin()+stop,copy.data.begin()+data.size());
// 	copy.data.erase(copy.data.begin(),copy.data.begin()+start);
	return copy;
}

quantity_t quantity_t::absolute() const
{
	if (!is_set()) return {};
	double sum=0;
	for (auto& d: data)
		sum += pow(d,2);
	sum = sqrt(sum);
	stringstream ss;
	ss << "abs(" << name() << ")";
	return {ss.str(),{sum},unit()};
}

quantity_t quantity_t::sum(int start, int stop) const
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


bool quantity_t::operator<(const quantity_t& obj) const
{
	if (*this > obj || *this == obj)
		return false;
	return true;
}

bool quantity_t::operator>(const quantity_t& obj) const
{
	quantity_t quantity_with_same_unit;
	
	if (unit()!=obj.unit())
		quantity_with_same_unit = change_unit(obj.unit());
	else
		quantity_with_same_unit = *this;
	
	if (obj.data.size() != quantity_with_same_unit.data.size()) 
		return false;
	for (int i=0;i<obj.data.size();i++)
		if (obj.data.at(i) != quantity_with_same_unit.data.at(i))
			return false;
	return true;
}

bool quantity_t::operator==(const quantity_t& obj) const
{
	if (unit()!=obj.unit()) return false;
	if (data.size()!=obj.data.size())	 return false;
	for (int i=0;i<data.size();i++)
		if (data[i]!=obj.data[i]) return false;
	return true;
}

bool quantity_t::operator!=(const quantity_t& obj) const
{
	return !(operator==(obj));
}

void quantity_t::operator<<(quantity_t obj)
{
	if (!obj.is_set()) return;
	if (!is_set())
	{
		*this = obj;
		return;
	}
	if (unit().base_units_exponents!=obj.unit().base_units_exponents)
		return;
	quantity_t adder = obj.change_unit(unit());
	tools::vec::add(&data,adder.data);
}

quantity_t quantity_t::operator+(const quantity_t& quantity) const
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

quantity_t quantity_t::operator+(const double summand) const
{
	if (!is_set()) return {};
	quantity_t sum=*this;

	for (int i=0; i< data.size()&&i<data.size();i++)
	{
		sum.data[i] += summand;
	}
	return sum;
}

quantity_t quantity_t::operator-(const quantity_t& quantity) const
{
	return {*this + quantity*(-1)};
}
quantity_t quantity_t::operator-(const double summand) const
{
	return {*this + summand*(-1)};
}

quantity_t quantity_t::operator * (const quantity_t& quantity_p) const
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

quantity_t quantity_t::operator / (const quantity_t& quantity_p) const
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

quantity_t quantity_t::operator/ (const double divisor) const
{
	quantity_t quotient=*this;
	for (int i=0;i<data.size();i++)
	{
		quotient.data[i]=data[i]/divisor;
	}

	return quotient;
}

quantity_t quantity_t::operator * (const double factor) const
{
	quantity_t multiplier=*this;
	for (int i=0;i<data.size();i++)
	{
		multiplier.data[i] *= factor;
	}
	return multiplier;
}

void quantity_t::operator+=(const double summand)
{
	*this = *this + summand;
}

void quantity_t::operator+=(const quantity_t& quantity_p)
{
	*this = *this + quantity_p;
}


/******************************************/
/*STATICS*/
// quantity_t quantity_t::sputter_energy()
// {
// 	return quantity_t{"sputter_energy",{"eV"}};
// }
// 
// quantity_t quantity_t::analysis_energy()
// {
// 	return quantity_t{"analysis_energy",{"keV"}};
// }
// 
// quantity_t quantity_t::analysis_rastersize()
// {
// 	return quantity_t{"analysis_rastersize",{"um"}};
// }
// 
// quantity_t quantity_t::concentration()
// {
// 	return quantity_t{"concentration",{"at/cm^3"}};
// }
// 
// quantity_t quantity_t::dose()
// {
// 	return quantity_t{"dose",{"at/cm^2"}};
// }



/*********************/

mass_t::mass_t(vector<double> data_s,unit_t unit_s) : quantity_t("mass",data_s,unit_s){}
mass_t::mass_t(quantity_t quantity_s) : mass_t(quantity_s.data,quantity_s.unit()) {}

abundance_t::abundance_t(vector<double> data_s,unit_t unit_s) : quantity_t("abundance",data_s,unit_s){}
abundance_t::abundance_t(quantity_t quantity_s) : abundance_t(quantity_s.data,quantity_s.unit()) {}

energy_t::energy_t(vector<double> data_s,unit_t unit_s) : quantity_t("energy",data_s,unit_s){}
energy_t::energy_t(quantity_t quantity_s) : energy_t(quantity_s.data,quantity_s.unit()) {}

rastersize_t::rastersize_t(vector<double> data_s,unit_t unit_s) : quantity_t( "rastersize",data_s,unit_s){}
rastersize_t::rastersize_t(quantity_t quantity_s) : rastersize_t(quantity_s.data,quantity_s.unit()) {}

depth_t::depth_t(vector<double> data_s,unit_t unit_s) : quantity_t("depth",data_s,unit_s){}
depth_t::depth_t(quantity_t quantity_s) : depth_t(quantity_s.data,quantity_s.unit()) {}

sputter_depth_t::sputter_depth_t(vector<double> data_s,unit_t unit_s) : quantity_t("sputter_depth",data_s,unit_s){}
sputter_depth_t::sputter_depth_t(quantity_t quantity_s) : sputter_depth_t(quantity_s.data,quantity_s.unit()) {}

total_sputter_depth_t::total_sputter_depth_t(vector<double> data_s,unit_t unit_s) : quantity_t("total_sputter_depth",data_s,unit_s){}
total_sputter_depth_t::total_sputter_depth_t(quantity_t quantity_s) : total_sputter_depth_t(quantity_s.data,quantity_s.unit()) {}

sputter_time_t::sputter_time_t(vector<double> data_s,unit_t unit_s) : quantity_t("sputter_time",data_s,unit_s){}
sputter_time_t::sputter_time_t(quantity_t quantity_s) : sputter_time_t(quantity_s.data,quantity_s.unit()) {}

total_sputter_time_t::total_sputter_time_t(vector<double> data_s,unit_t unit_s) : quantity_t("total_sputter_time",data_s,unit_s){}
total_sputter_time_t::total_sputter_time_t(quantity_t quantity_s) : total_sputter_time_t(quantity_s.data,quantity_s.unit()) {}

// analysis_energy_t::analysis_energy_t(vector<double> data_s,unit_t unit_s) : quantity_t("analysis_energy",data_s,unit_s){}
// analysis_energy_t::analysis_energy_t(quantity_t quantity_s) : analysis_energy_t(quantity_s.data,quantity_s.unit()) {}
// 
// analysis_rastersize_t::analysis_rastersize_t(vector<double> data_s,unit_t unit_s) : quantity_t("analysis_rastersize",data_s,unit_s){}
// analysis_rastersize_t::analysis_rastersize_t(quantity_t quantity_s) : analysis_rastersize_t(quantity_s.data,quantity_s.unit()) {}

intensity_t::intensity_t(vector<double> data_s,unit_t unit_s) : quantity_t("intensity",data_s,unit_s){}
intensity_t::intensity_t(quantity_t quantity_s) : intensity_t(quantity_s.data,quantity_s.unit()) {}

current_t::current_t(vector<double> data_s,unit_t unit_s) : quantity_t("current",data_s,unit_s){}
current_t::current_t(quantity_t quantity_s) : current_t(quantity_s.data,quantity_s.unit()) {}

sputter_current_t::sputter_current_t(vector<double> data_s,unit_t unit_s) : quantity_t("primary_sputter_current",data_s,unit_s){}
sputter_current_t::sputter_current_t(quantity_t quantity_s) : sputter_current_t(quantity_s.data,quantity_s.unit()) {}

analysis_current_t::analysis_current_t(vector<double> data_s,unit_t unit_s) : quantity_t("primary_analysis_current",data_s,unit_s){}
analysis_current_t::analysis_current_t(quantity_t quantity_s) : analysis_current_t(quantity_s.data,quantity_s.unit()) {}

concentration_t::concentration_t(vector<double> data_s,unit_t unit_s) : quantity_t("concentration",data_s,unit_s){}
concentration_t::concentration_t(quantity_t quantity_s) : concentration_t(quantity_s.data,quantity_s.unit()) {}

dose_t::dose_t(vector<double> data_s,unit_t unit_s) : quantity_t("dose",data_s,unit_s){}
dose_t::dose_t(quantity_t quantity_s) : dose_t(quantity_s.data,quantity_s.unit()) {}

SF_t::SF_t(vector<double> data_s,unit_t unit_s) : quantity_t("sensitivity_factor",data_s,unit_s){}
SF_t::SF_t(quantity_t quantity_s) : SF_t(quantity_s.data,quantity_s.unit()) {}

RSF_t::RSF_t(vector<double> data_s,unit_t unit_s) : quantity_t("relative_sensitivity_factor",data_s,unit_s){}
RSF_t::RSF_t(quantity_t quantity_s) : RSF_t(quantity_s.data,quantity_s.unit()) {}

SR_t::SR_t(vector<double> data_s,unit_t unit_s) : quantity_t("sputter_rate",data_s,unit_s){}
SR_t::SR_t(quantity_t quantity_s) : SR_t(quantity_s.data,quantity_s.unit()) {}

secondary_voltage_t::secondary_voltage_t(vector<double> data_s, unit_t unit_s) : quantity_t("secondary_voltage",data_s,unit_s) {}
secondary_voltage_t::secondary_voltage_t(quantity_t quantity_s) :secondary_voltage_t(quantity_s.data,quantity_s.unit()) {}

substance_amount_t::substance_amount_t(vector<double> data_s, unit_t unit_s) : quantity_t("substance_amount",data_s,unit_s) {}
substance_amount_t::substance_amount_t(quantity_t quantity_s) :substance_amount_t(quantity_s.data,quantity_s.unit()) {}

electrical_charge_t::electrical_charge_t(vector<double> data_s, unit_t unit_s) : quantity_t("electrical_charge",data_s,unit_s) {}
electrical_charge_t::electrical_charge_t(quantity_t quantity_s) :electrical_charge_t(quantity_s.data,quantity_s.unit()) {}




