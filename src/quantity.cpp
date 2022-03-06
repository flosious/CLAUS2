#include "quantity.hpp"

/***********STATICS*********/
quantity::quantity_t::dimension_t quantity::quantity_t::get_dimension_from_unit(const unit_t& unit)
{
	return dimension_t(unit.base_units_exponents.meters,
					   unit.base_units_exponents.kilograms, 
					   unit.base_units_exponents.seconds, 
					   unit.base_units_exponents.amperes, 
					   unit.base_units_exponents.moles, 
					   unit.base_units_exponents.kelvins, 
					   unit.base_units_exponents.candelas);
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

/*******************************/

quantity::quantity_t::quantity_t(const quantity_t& quant,const unit_t& unit, string add_to_history) : 
			quantity_t(quant.name(),quant.data(),unit,quant.dimension(),quant.operations_history(),add_to_history)
{
}

// quantity::quantity_t::quantity_t(const quantity_t& Q) : name_s()

quantity::quantity_t::quantity_t() : name_s(""), data_s({}),dimension_s(dimensions::SI::relative)
{
}

quantity::quantity_t::quantity_t(const string& name,const vector<double>& data,const unit_t& unit) : 
									name_s(name), data_s(data), unit_s(unit), dimension_s(quantity_t::get_dimension_from_unit(unit))
{
}

quantity::quantity_t::quantity_t(const quantity::quantity_t& quant, const double& data, std::string add_to_history) : 
	quantity_t(quant.name(),{data},quant.unit(),quant.dimension(),quant.operations_history(),add_to_history)
{
}

quantity::quantity_t::quantity_t(const quantity::quantity_t& quant, const vector<double>& data, std::string add_to_history) : 
	quantity_t(quant.name(),data,quant.unit(),quant.dimension(),quant.operations_history(),add_to_history)
{
}

quantity::quantity_t::quantity_t(const quantity::quantity_t& quant, const unit_t& unit, const quantity::quantity_t::dimension_t& dimension, std::string add_to_history) :
	quantity_t(quant.name(),quant.data(),unit,dimension,quant.operations_history(),add_to_history)
{
}

quantity::quantity_t::quantity_t(const string& name,const unit_t& unit, string add_to_history) :
	name_s(name), data_s({}), unit_s(unit), dimension_s(quantity_t::get_dimension_from_unit(unit)), operations_history_s({add_to_history})
{
}

quantity::quantity_t::quantity_t(const string& name,const vector<double>& data,const unit_t& unit, const dimension_t& dimension, const vector<string>& operations_history,string add_to_history) : 
	name_s(name), data_s(data), unit_s(unit), dimension_s(dimension), operations_history_s(tools::vec::add(operations_history,add_to_history))
{
}

quantity::quantity_t::quantity_t(const string& name,const vector<double>& data,const unit_t& unit, const dimension_t& dimension) : 
	name_s(name), data_s(data), unit_s(unit), dimension_s(dimension)
{
}

bool quantity::quantity_t::is_relative() const
{
	return dimension().is_relative();
}

const vector<double>& quantity::quantity_t::data() const
{
	return data_s;
}

const vector<string>& quantity::quantity_t::operations_history() const
{
	return operations_history_s;
}

const string& quantity::quantity_t::name() const
{
	return name_s;
}

const quantity::quantity_t::dimension_t& quantity::quantity_t::dimension() const
{
	return dimension_s;
}
const unit_t& quantity::quantity_t::unit() const
{
	return unit_s;
}


const vector<double> quantity::quantity_t::data_X_1D() const
{
	vector<double> X(data().size());
	for (int x=0;x<X.size();x++)
		X[x]=x;
	return X;
}

quantity::quantity_t& quantity::quantity_t::clear()
{
	data_s.clear();
	return* this;
}

const std::map<double,double> quantity::quantity_t::data_XY_1D()
{
	std::map<double,double> data_XY; 
	tools::vec::combine_vecs_to_map(data_X_1D(),&data(),&data_XY);
	return data_XY;
}

fit_functions::polynom_t quantity::quantity_t::polynom(unsigned int polynom_grade) const
{
	fit_functions::polynom_t poly(polynom_grade,data());
	return poly;
}

quantity::quantity_t quantity::quantity_t::bspline_smoothing( unsigned int breakpoints, const size_t spline_order) const
{
	if (!has_data())
		return {};
	
	stringstream ss;
	ss << "BSpline" << spline_order<< "_smooth";
	
// 	operations_history_s.push_back(ss.str());
// 	data_s = statistics::bspline_smooth(data(),{}, breakpoints,spline_order);
	
	return {*this,statistics::bspline_smooth(data(),{}, breakpoints,spline_order),ss.str()};
}

quantity::quantity_t quantity::quantity_t::bspline_smoothing(const quantity_t& Xdata, unsigned int breakpoints, const size_t spline_order) const
{	
	if (!has_data() || !Xdata.has_data())
		return {};
	
	stringstream ss;
	ss << "BSpline" << spline_order<< "_smooth";
	
// 	operations_history_s.push_back(ss.str());
// // 	data_s = statistics::bspline_smooth(data(),Xdata.data(), breakpoints,spline_order);
	
	return {*this,statistics::bspline_smooth(data(),Xdata.data(), breakpoints,spline_order),ss.str()};
}

quantity::quantity_t quantity::quantity_t::interp(const quantity_t& old_X, const quantity_t& new_X) const
{
	if (!is_set())
		return {};
	if (data().size()==1)
		return {};
	if (!old_X.is_set() || !new_X.is_set() || !is_set()) 
	{
		logger::error("quantity::quantity_t::interp","!old_X.is_set() || !new_X.is_set() || !is_set()","","returning empty");
		return {};
	}
	if (old_X.data().size()!=data().size())
	{
		logger::error("quantity::quantity_t::interp","old_X.data.size()!=data.size()","current_X=" +to_string() +" old_X="+old_X.to_string(),"returning empty");
		return {};
	}
	if (old_X.dimension() != new_X.dimension())
	{
		logger::error("quantity::quantity_t::interp","old_X.dimension() != new_X.dimension()","","returning empty");
		return {};
	}
	auto old_X_same_units = old_X.change_unit(new_X.unit());
	
	std::map<double,double> XY_data;
	tools::vec::combine_vecs_to_map(&old_X_same_units.data_s,&data_s,&XY_data);
	
	if (XY_data.begin()->first > new_X.data().front() )
	{
		logger::error("quantity::quantity_t::interp","old_X start="+ tools::to_string(XY_data.begin()->first) +" new_X start=" + tools::to_string(new_X.data().front()),"extrapolation not supported","returning empty");
		return {};
	}
	if (XY_data.rbegin()->first < new_X.data().back() )
	{
		logger::error("quantity::quantity_t::interp","old_X stop=" + tools::to_string(XY_data.rbegin()->first) + " new_X stop="+tools::to_string(new_X.data().back()),"extrapolation not supported","returning empty");
		return {};
	}
// 	data_s = statistics::interpolate_data_XY(XY_data,new_X.data());
// 	operations_history_s.push_back("interp");
	return {*this,statistics::interpolate_data_XY(XY_data,new_X.data()),"interp"};
	
}


quantity::quantity_t quantity::quantity_t::fit_polynom_by_x_data(quantity_t& x_data, quantity_t new_x_data, int polynom_grade) const
{
	if (!is_set())
		return {};
	if (!x_data.is_set()) 
		return {};
	if (!new_x_data.is_set()) 
		return {};
	
	auto new_x_data_common_unit = x_data.change_unit(new_x_data.unit());
	
	if (!new_x_data_common_unit.is_set())
		return {};
	
	std::map<double,double> data_XY;
	tools::vec::combine_vecs_to_map(&x_data.data_s,&data_s,&data_XY);
	
	stringstream ss;
	ss << "poly" << polynom_grade << "_fit";
// 	operations_history_s.push_back(ss.str());
// 	data_s = polynom(polynom_grade).y_data(new_x_data_common_unit.data());
	
// 	return *this;
	return {*this,polynom(polynom_grade).y_data(new_x_data_common_unit.data()),ss.str()};
}

quantity::quantity_t quantity::quantity_t::polyfit_derivative(unsigned int polynom_grade, unsigned int derivative) const
{
	stringstream ss;
	ss << "poly" << polynom_grade << "_derivative" << derivative<< "_fit";
// 	operations_history_s.push_back(ss.str());
// 	data_s = polynom(polynom_grade).derivative(derivative).y_data(data_X_1D());
	return {*this,polynom(polynom_grade).derivative(derivative).y_data(data_X_1D()),ss.str()};
}


quantity::quantity_t quantity::quantity_t::polyfit(unsigned int polynom_grade) const
{
	stringstream ss;
	ss << "poly" << polynom_grade << "_fit";
// 	operations_history_s.push_back(ss.str());
// 	data_s = polynom(polynom_grade).y_data(data_X_1D());
	return {*this,polynom(polynom_grade).y_data(data_X_1D()),ss.str()};
}

quantity::quantity_t quantity::quantity_t::round_() const
{
	if (!is_set()) 
		return {};
	auto data_c = data_s;
	for (auto& d : data_c)
        d = round(d);
// 	operations_history_s.push_back("round");
	return {*this,data_c,"round"};
}

quantity::quantity_t quantity::quantity_t::diff() const
{
	if (!is_set() || data().size()==1) 
		return {};

	vector<double> diff_data(data().size());
	diff_data.at(0) = data_s.at(0) - 0;
	for (int i=1;i<diff_data.size();i++)
		diff_data.at(i)=data_s.at(i)-data_s.at(i-1);
// 	operations_history_s.push_back("diff");
// 	data_s = diff_data;
// 	diff_data.push_back(diff_data.back()); // copy last element
	return {*this,diff_data,"diff"};
}

quantity::quantity_t quantity::quantity_t::integrate_pbp(const quantity_t& x_data) const
{
	if (data().size()!=x_data.data().size()) 
		return {};
// 	if (data.size()==1) // fall back to simple multiplication, but this will ignore correctness of unit and dimension --> do not use!
// 		return *this * x_data;

	stringstream n;
	n << "integral_pbp_" << "d(" << x_data.name() << ")";
// 	operations_history_s.push_back(n.str());
	vector<double> area_data(data().size());

	double sum=0;
	double dY,dX;
// 	area_data[0]=abs(data[0]+data[1])/2 * abs(x_data.data[0]-x_data.data[1]);
	area_data[0]=abs(data()[0]+0)/2 * abs(x_data.data()[0]-0);
    for (int i=1;i<data().size();i++)
	{
        dY=abs(data()[i]+data()[i-1])/2;
        dX=abs(x_data.data()[i]-x_data.data()[i-1]);
		sum=sum+dX*dY;
        area_data[i] = sum; 
    }
    auto data_c = area_data;
	auto unit_c = unit_s*x_data.unit_s;
	auto dimension_c = dimension_s*x_data.dimension_s;
	return {name(),data_c,unit_c,dimension_c,operations_history(),n.str()};
}

quantity::quantity_t quantity::quantity_t::integrate(const quantity_t& x_data, unsigned int lower_X_index) const
{
	return integrate(x_data,lower_X_index,x_data.data().size()-1);
}

quantity::quantity_t quantity::quantity_t::integrate(const quantity_t& x_data, unsigned int lower_X_index, unsigned int upper_X_index) const
{
	if (lower_X_index<0)
		      lower_X_index=0;
	if (upper_X_index>=x_data.data().size() || upper_X_index<=0)
		      upper_X_index = x_data.data().size() -1 ;
	if (x_data.data().size() != data().size())
	{
		logger::error("quantity::quantity_t::integrate()","x_data.data.size() != data.size()","return empty");
		return {};
	}
	return get_data_by_index(lower_X_index,upper_X_index).integrate_pbp(x_data.get_data_by_index(lower_X_index,upper_X_index)).abs_sum();
// 	auto int_pbp = integrate_pbp(x_data);
// 	double sum=int_pbp.data().at(upper_X_index);
// 	if (lower_X_index>0)
// 		sum -= int_pbp.data().at(lower_X_index) ;
// 	stringstream n,u;
// 	n << "integral" << "d(" << x_data.name() << ")";
// 	quantity_t area(name,{sum},unit*x_data.unit,dimension*x_data.dimension,operations_history,n.str());
// 	return area;
}

quantity::quantity_t quantity::quantity_t::integrate(const quantity_t& x_data,const quantity_t& x_data_start) const
{
	
	return integrate(x_data,x_data_start,x_data.back());
}

quantity::quantity_t quantity::quantity_t::integrate(const quantity_t& x_data,const quantity_t& x_data_start,const quantity_t& x_data_stop) const
{
	unsigned int lower_X_idx=0;
	unsigned int upper_X_idx=x_data.data().size();
	if (!x_data_start.is_scalar() || !x_data_stop.is_scalar())
	{
		logger::error("quantity::quantity_t::integrate","start/stop quantities no scalars","returning empty");
		return {};
	}
	
	if (x_data.dimension() != x_data_start.dimension() || x_data.dimension() != x_data_stop.dimension())
	{
		logger::error("quantity::quantity_t::integrate()","x_data.dimension() != x_data_start.dimension() || x_data.dimension() != x_data_stop.dimension()","returning empty");
		return {};
	}
	
	auto x_data_start_same_unit = x_data_start.change_unit(x_data.unit());
	auto x_data_stop_same_unit =x_data_stop.change_unit(x_data.unit());
	
	lower_X_idx = get_value_index_position_in_strictly_monotonic_increasing_vector(x_data_start_same_unit.data().front(),x_data.data());
	if (lower_X_idx<0)
	{
		logger::error("quantity::quantity_t::integrate","lower_X_idx<0","returning empty");
		return {};
	}
	
	upper_X_idx = get_value_index_position_in_strictly_monotonic_increasing_vector(x_data_stop_same_unit.data().front(),x_data.data());
	if (upper_X_idx<0)
	{
		logger::error("quantity::quantity_t::integrate","upper_X_idx<0","returning empty");
		return {};
	}
	
	return integrate(x_data,lower_X_idx,upper_X_idx);
}

quantity::quantity_t quantity::quantity_t::x_at_max(const quantity_t& X) const
{
	if (data().size()!=X.data().size())
		return {};
	if (data().size()==0)
		return {};
	
	int idx=statistics::get_max_index_from_Y(data_s);
	if (idx>=X.data().size())
		return {};
	stringstream ss;
	ss << "x_at_max_idx(" << idx << ")";
	double x_at_max_p = X.data().at(idx);
// 	data_s = {x_at_max_p};
// 	operations_history_s.push_back(ss.str());
	return {*this,x_at_max_p,ss.str()};
}

quantity::quantity_t quantity::quantity_t::get_data_within_limits(double lower_limit, double upper_limit) const
{
	if (!is_set())
		return {};
	vector<unsigned int> erase_pos;
	for (int i=0;i<data().size();i++)
	{
		if (data_s.at(i)<lower_limit || data_s.at(i)>upper_limit)
			erase_pos.push_back(i);
	}
	return remove_data_by_index(erase_pos);
}

quantity::quantity_t quantity::quantity_t::x_at_min(quantity_t& X) const
{
	if (data().size()!=X.data().size())
		return {};
	if (data().size()==0)
		return {};
	
	int idx=statistics::get_max_index_from_Y(data_s);
	if (idx>=X.data().size())
		return {};
	stringstream ss;
	ss << "x_at_min_idx(" << idx << ")";
	double x_at_min_idx = X.data().at(idx);
// 	data_s = {x_at_min_idx};
// 	operations_history_s.push_back(ss.str());
	return {*this,x_at_min_idx,ss.str()};
}


quantity::quantity_t quantity::quantity_t::median() const
{
	if (!is_set())
		return {};
// 	data_s = {statistics::get_median_from_Y(data())};
// 	operations_history_s.push_back("median");
	return {*this,statistics::get_median_from_Y(data()),"median"};
}

quantity::quantity_t quantity::quantity_t::quantile(double percentile) const
{
	if (!is_set())
		return {};
	if (percentile>1 || percentile<0)
		return {};
	
	stringstream n;
	n << "percentile" << percentile;
// 	data_s = {statistics::get_quantile_from_Y(data(),percentile)};
// 	operations_history_s.push_back(n.str());
	return {*this,statistics::get_quantile_from_Y(data(),percentile),n.str()};
}

quantity::quantity_t quantity::quantity_t::geo_mean() const
{
	if (!is_set())
		return {};
	double gm = 1;
	for (auto& d : data_s)
	{
		if (d<0)
			return {};
		gm *= d;
	}
// 	data_s = {gm};
// 	operations_history_s.push_back("geo_mean");
	return {*this,gm,"geo_mean"};
}

quantity::quantity_t quantity::quantity_t::cut_mean(float alpha) const
{
	if (!is_set())
		return {};
	int start = alpha * data().size();
	int stop = (1-alpha) * data().size();
	return get_data_by_index(start,stop).mean();
}

quantity::quantity_t quantity::quantity_t::cut_median(float alpha) const
{
	if (!is_set())
		return {};
	int start = alpha * data().size();
	int stop = (1-alpha) * data().size();
	return get_data_by_index(start,stop).median();
}

quantity::quantity_t quantity::quantity_t::trimmed_mean(float alpha) const
{
	if (!is_set())
		return {};
	stringstream n;
	n << "trimmed" << alpha << "_mean"; 
// 	data_s = {statistics::get_trimmed_mean_from_Y(data(),alpha)};
// 	operations_history_s.push_back(n.str());
	return {*this,statistics::get_trimmed_mean_from_Y(data(),alpha),n.str()};
}

quantity::quantity_t quantity::quantity_t::mean() const
{
	if (!is_set())
		return {};
// 	data_s = {statistics::get_mean_from_Y(data())};
// 	operations_history_s.push_back("mean");
	return {*this,statistics::get_mean_from_Y(data()),"mean"};
}

quantity::quantity_t quantity::quantity_t::gastwirth() const
{
	if (!is_set())
		return {};
// 	data_s = {statistics::get_gastwirth_estimator_from_Y(data())};
// 	operations_history_s.push_back("gastwirth");
	return {*this,statistics::get_gastwirth_estimator_from_Y(data()),"gastwirth"};
}

quantity::quantity_t quantity::quantity_t::mad() const
{
	if (!is_set())
		return {};
// 	data_s = {statistics::get_mad_from_Y(data())};
// 	operations_history_s.push_back("mad");
	return {*this,statistics::get_mad_from_Y(data()),"mad"};
}

quantity::quantity_t quantity::quantity_t::back() const
{
	if (!is_set())
		return {};
// 	data_s = {data_s.back()};
	return {*this,{data_s.back()}};
}

quantity::quantity_t quantity::quantity_t::front() const
{
	if (!is_set())
		return {};
// 	data_s = {data_s.front()};
	return {*this,{data_s.front()}};
}

quantity::quantity_t quantity::quantity_t::moving_window_mad(int window_size) const
{
	if (!is_set())
		return {};
	if (window_size==0) 
		window_size = 0.05*data().size();
	if (window_size==0) 
		return {};

	stringstream n;
	n << "mw" << window_size <<"_mad";
// 	data_s = statistics::get_moving_window_MAD_from_Y(data(),window_size);
// 	operations_history_s.push_back(n.str());
	return {*this,statistics::get_moving_window_MAD_from_Y(data(),window_size),n.str()};
}

quantity::quantity_t quantity::quantity_t::reverse() const
{
	if (!is_set())
		return {};
	
	size_t size = data().size();
	vector<double> rev_d(size);
	for (int i=0;i<size;i++)
		rev_d.at(size-1-i) = data_s.at(i);
// 	data_s = rev_d;
// 	operations_history_s.push_back("reverse");
	return {*this,rev_d,"reverse"};
}


quantity::quantity_t quantity::quantity_t::moving_window_iqr(int window_size) const
{
	return moving_window_qqr(window_size,0.25);
}

quantity::quantity_t quantity::quantity_t::moving_window_qqr(int window_size, double q) const
{
	if (!is_set())
		return {};
	if (window_size==0) 
		window_size = 0.05*data().size();
	if (window_size==0) 
		return {};

	stringstream n;
	n << "mw" << window_size <<"_qqr" << q;
// 	data_s = statistics::get_moving_window_qqr_from_Y(data(),window_size,q);
// 	operations_history_s.push_back(n.str());
	return {*this,statistics::get_moving_window_qqr_from_Y(data(),window_size,q),n.str()};
}

quantity::quantity_t quantity::quantity_t::moving_window_max(int window_size) const
{
	if (!is_set())
		return {};
	if (window_size==0) 
		window_size = 0.05*data().size();
	if (window_size==0) 
		return {};

	stringstream n;
	n << "mw" << window_size <<"_max";
// 	data_s = statistics::get_moving_window_max_from_Y(data(),window_size);
// 	operations_history_s.push_back(n.str());
	return {*this,statistics::get_moving_window_max_from_Y(data(),window_size),n.str()};
}

quantity::quantity_t quantity::quantity_t::moving_window_min(int window_size) const
{
	if (!is_set())
		return {};
	if (window_size==0) 
		window_size = 0.05*data().size();
	if (window_size==0) 
		return {};

	stringstream n;
	n << "mw" << window_size <<"_min";
// 	data_s = statistics::get_moving_window_min_from_Y(data(),window_size);
// 	operations_history_s.push_back(n.str());
	return {*this,statistics::get_moving_window_min_from_Y(data(),window_size),n.str()};
}

quantity::quantity_t quantity::quantity_t::moving_window_sum(int window_size) const
{
	if (!is_set())
		return {};
	if (window_size==0) 
		window_size = 0.05*data().size();
	if (window_size==0) 
		return {};
	stringstream n;
	n << "mw" << window_size <<"_sum";
// 	data_s = statistics::get_moving_window_sum_from_Y(data(),window_size);
// 	operations_history_s.push_back(n.str());
	return {*this,statistics::get_moving_window_sum_from_Y(data(),window_size),n.str()};
}


quantity::quantity_t quantity::quantity_t::moving_window_mean(int window_size) const
{
	if (!is_set())
		return {};
	if (window_size==0) 
		window_size = 0.05*data().size();
	if (window_size==0) 
		return {};
	stringstream n;
	n << "mw" << window_size <<"_mean";
// 	data_s = statistics::get_moving_window_mean_from_Y(data(),window_size);
// 	operations_history_s.push_back(n.str());
	return {*this,statistics::get_moving_window_mean_from_Y(data(),window_size),n.str()};
}

quantity::quantity_t quantity::quantity_t::moving_window_median(int window_size) const
{
	if (!is_set())
		return {};
	if (window_size==0) 
		window_size = 0.05*data().size();
	if (window_size==0) 
		return {};
	stringstream n;
	n << "mw" << window_size <<"_median";
// 	data_s = statistics::get_moving_window_median_from_Y(data(),window_size);
// 	operations_history_s.push_back(n.str());
	return {*this,statistics::get_moving_window_median_from_Y(data(),window_size),n.str()};
}

quantity::quantity_t quantity::quantity_t::moving_window_sd(int window_size) const
{
	if (!is_set())
		return {};
	if (window_size==0) 
		window_size = 0.05*data().size();
	if (window_size==0) 
		return {};
	stringstream n;
	n << "mw" << window_size <<"_sd";
// 	data_s = statistics::get_moving_window_sd_from_Y(data(),window_size);
// 	operations_history_s.push_back(n.str());
	return {*this,statistics::get_moving_window_sd_from_Y(data(),window_size),n.str()};
}

quantity::quantity_t quantity::quantity_t::at(unsigned int pos) const
{
	return at(pos,pos+1);
}

quantity::quantity_t quantity::quantity_t::at(unsigned int start_pos, unsigned int end_pos) const
{
	return get_data_by_index(start_pos,end_pos);
}


quantity::quantity_t quantity::quantity_t::log10() const
{
	if (!is_set())
		return {};
	vector<double> data_log(data().size());
	for (int i=0;i<data().size();i++)
		data_log.at(i) = log(data_s.at(i));
// 	operations_history_s.push_back("log10");
// 	data_s = data_log;
	return {*this,data_log,"log10"};;
}

quantity::quantity_t quantity::quantity_t::sd() const
{
	if (!is_set())
		return {};
// 	operations_history_s.push_back("sd");
// 	data_s = {statistics::get_sd_from_Y(data())};
	return {*this,statistics::get_sd_from_Y(data()),"sd"};;
}

quantity::quantity_t quantity::quantity_t::max() const
{
	if (!is_set())
		return {};
// 	operations_history_s.push_back("max");
// 	data_s = {statistics::get_max_from_Y(data())};
	return {*this,statistics::get_max_from_Y(data()),"max"};;
}

quantity::quantity_t quantity::quantity_t::min() const
{
	if (!is_set())
		return {};
// 	operations_history_s.push_back("min");
// 	data_s = {statistics::get_min_from_Y(data())};
	return {*this,statistics::get_min_from_Y(data()),"min"};;
}

bool quantity::quantity_t::is_nan() const
{
	for (auto& d : data())
		if (isnan(d))
			return true;
	return false;
}

bool quantity::quantity_t::is_inf() const
{
	for (auto& d : data())
		if (isinf(d))
			return true;
	return false;
}

bool quantity::quantity_t::has_data() const
{
	if (data().size()==0) 
		return false;
	return true;
}

bool quantity::quantity_t::is_set() const
{
	if (data().size()==0) 
		return false;
	return true;
}

bool quantity::quantity_t::is_scalar() const
{
	if (!is_set()) return false;
	if (data().size()==1) return true;
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
	if (data().size()<window_size)
		return {};
	if (window_size==0) 
		window_size=0.05*data().size();
	if (window_size==0) 
		window_size=3;
	
	stringstream n;
	n << "impulse_filtered(" << window_size << "," << factor << ")";
// 	operations_history_s.push_back(n.str());
// 	data_s = {statistics::impulse_filter(data(),window_size,factor)};
	return {*this,statistics::impulse_filter(data(),window_size,factor),n.str()};;
}

quantity::quantity_t quantity::quantity_t::filter_gaussian(int window_size, double alpha) const
{
	if (!is_set())
		return {};
	if (data().size()<window_size)
		return {};
	if (window_size==0) 
		window_size=0.05*data().size();
	if (window_size==0) 
		window_size=3;
	
	stringstream n;
	n << "impulse_filtered(" << window_size << "," << alpha << ")";
// 	operations_history_s.push_back(n.str());
// 	data_s = statistics::gaussian_filter(data(),window_size,alpha);
	return {*this,statistics::gaussian_filter(data(),window_size,alpha),n.str()};;
}

quantity::quantity_t quantity::quantity_t::filter_recursive_median(int window_size) const
{
	if (!is_set())
		return {};
	if (data().size()<window_size)
		return {};
	if (window_size==0) 
		window_size=0.05*data().size();
	if (window_size==0) 
		window_size=3;
	
	stringstream n;
	n << "recursiveMedian_filtered(" << window_size<< ")";
// 	operations_history_s.push_back(n.str());
// 	data_s = statistics::recursive_median_filter(data(),window_size);
	return {*this,statistics::recursive_median_filter(data(),window_size),n.str()};;
}

quantity::quantity_t quantity::quantity_t::remove_inf() const
{
	vector<unsigned int> remove_idx;
	for (int i=0;i<data_s.size();i++)
		if (isinf(data_s.at(i)))
			remove_idx.push_back(i);
	return remove_data_by_index(remove_idx);
}

quantity::quantity_t quantity::quantity_t::remove_nan() const
{
	vector<unsigned int> remove_idx;
	for (int i=0;i<data_s.size();i++)
		if (isnan(data_s.at(i)))
			remove_idx.push_back(i);
	return remove_data_by_index(remove_idx);
}

quantity::quantity_t quantity::quantity_t::change_resolution(unsigned int new_data_size) const
{
	if (new_data_size<=1)
		return {};
	if (data().size()<=1)
		return {};
	quantity_t new_res = resolution() * (new_data_size-1)/(data().size()-1);
	return change_resolution(new_res);
}

quantity::quantity_t quantity::quantity_t::change_resolution(quantity_t new_res) const
{
	if (!is_set() || !new_res.is_set()) 
		return {};
// 	if (name != new_res.name)
// 	{
// 		logger::error("quantity::quantity_t::change_resolution()","name != new_res.name",name + "!=" + new_res.name,"returning empty");
// 		return {};
// 	}
	if (new_res.data().size()!=1)
		logger::debug(13,"quantity::quantity_t::change_resolution()","new_res.data.size()!=1",tools::to_string(new_res.data().size()));

	if (!new_res.is_set())
	{
		logger::debug(13,"quantity::quantity_t::change_resolution()","!new_res.is_set()","","returning empty");
		return {};
	}
	change_unit(new_res.unit());
	if (!is_set())
	{
		logger::debug(13,"quantity::quantity_t::resolution()","!old_Q.is_set()","","returning empty");
		return {};
	}
	logger::debug(13,"quantity::quantity_t::resolution()","new_res",new_res.to_string());
// 	return resolution(new_res.data.front());
	return resolution(new_res.data_s.front());
}

quantity::quantity_t quantity::quantity_t::resolution(double new_res) const
{
	if (!is_set())	
		return {};
	if (new_res==0) //the momentary resolutiion
	{
		return diff().absolute().mean();
	}
	if (new_res<0) 
		return {};
	if (data().size()<2) 
		return {};
	
	double max = statistics::get_max_from_Y(data_s);
	double min = statistics::get_min_from_Y(data_s);
	int new_size = floor((max-min)/new_res)+1;
	
	vector<double> new_data(new_size);
	for (int i=0;i<new_size;i++)
		new_data.at(i) = i*new_res + min;
// 	data_s = new_data;
// 	operations_history_s.push_back("resolutiion");
	return {*this,new_data,"resolutiion"};;
}

const string quantity::quantity_t::to_string() const
{
	ostringstream out;
	if (data().size()>1)
		out <<  name()  << operations_history_to_string() << " = " <<"<" << data().size() << ">" << " [" << unit().to_string() << "]";
	else if (data().size()==1)
		out <<  name() << operations_history_to_string() << " = " << data_s.front() << " [" << unit().to_string() << "]";
	else
		out <<  name() << operations_history_to_string() << " = " <<"<0>" << " []";
	return out.str();
}

string quantity::quantity_t::operations_history_to_string() const
{
	stringstream ss;
	for (auto& op : operations_history())
		ss << "->" << op;
	return ss.str();
}

const string quantity::quantity_t::to_string_detailed() const
{
	ostringstream out;
	
	if (data().size()>1)
	{
		out <<  name() << " = " <<"<" << data().size() << ">" << " [" << unit().to_string() << "]" << " {" << dimension().to_string() << "}  ";
		out << " " << min().to_string();
		out << " " << max().to_string();
		out << " " << median().to_string();
		out << " " << mean().to_string();
		out << " " << sd().to_string();
// 		out << " " << min().operations_history_to_string() << min().to_string();
// 		out << " " << max().operations_history_to_string() << max().to_string();
// 		out << " " << median().operations_history_to_string() << median().to_string();
// 		out << " " << mean().operations_history_to_string() << mean().to_string();
// 		out << " " << sd().operations_history_to_string() << sd().to_string();
	}
	else if (data().size()==1)
		out <<  name() << " = " << data_s.front() << " [" << unit().to_string() << "]" << " {" << dimension().to_string() << "}";
	else
	{
		out <<  name() << " = " <<"<0>" << " []" << " {}";
	}
	return out.str();
}

quantity::quantity_t quantity::quantity_t::change_unit(string target_unit_string) const
{
// 	if (!is_set())
// 	{
// 		logger::debug(15,"quantity::quantity_t::change_unit","q.unit_p.prefered_output_string=" + unit().prefered_output_string, "target_unit_string=" + target_unit_string );
// 	}
	return change_unit(unit_t(target_unit_string,target_unit_string));;
}

quantity::quantity_t quantity::quantity_t::change_unit(const unit_t& target_unit) const
{	
	if (!is_set())
		return {};
	if (unit()==target_unit) // nothing to change
		return *this;
	if (unit().base_units_exponents != target_unit.base_units_exponents) 
	{
		logger::error("quantity::quantity_t::change_unit","unit.base_units_exponents != target_unit.base_units_exponents",target_unit.to_string(),"returning this");
		return {};
	}
	auto data_c = data();
	double factor = unit().multiplier / target_unit.multiplier;
// 	*this = *this*factor;
	for (auto& d : data_c)
		d *= factor ;
	logger::debug(14,"quantity::quantity_t::change_unit","old: "+unit().to_string(),"new: " + target_unit.to_string(),"changed");
	stringstream ss;
	ss << "change_unit(" << target_unit.to_string() << ")";
	return {name(),data_c,target_unit,dimension(),operations_history(),ss.str()};
}

quantity::quantity_t quantity::quantity_t::invert() const
{
	if (!is_set()) 
		return {};
	vector<double> d_inv(data().size());
	for (int i=0;i<data().size();i++)
	{
		d_inv.at(i) = 1 / data_s.at(i);
	}
	return {name(),d_inv,unit().invert(),dimension().invert(),operations_history(),"invert"};;
}

quantity::quantity_t quantity::quantity_t::remove_data_from_begin(unsigned int stop) const
{
// 	cout << "stop=" << stop << endl;
	if (stop==0)
		return {};
	if (stop==data().size())
		return *this;
	
	return remove_data_by_index(0,stop);
}

quantity::quantity_t quantity::quantity_t::remove_data_from_begin(quantity_t& remove_stop) const
{
// 	if (!remove_stop.is_set())
// 		return {};
// 	cout << remove_stop.change_unit(unit).to_string() << endl;
	return remove_data_from_begin(remove_stop.change_unit(unit()).data().front());
}

quantity::quantity_t quantity::quantity_t::remove_data_by_index(vector<unsigned int> remove_pos) const
{
	if (remove_pos.size()==0) 
		return *this;
	if (!is_set())
		return {};
// 	data_s = tools::vec::erase(data_s,remove_pos);
// 	operations_history_s.push_back("remove_data_by_index");
	return {*this,tools::vec::erase(data_s,remove_pos),"remove_data_by_index"};
// 	vector<double> new_data(data().size()-remove_pos.size()); 
// 	int new_data_counter=0;
// 	sort (remove_pos.begin(),remove_pos.end());
// 	for (int i=0;i<data().size();i++)
// 	{
// 		for (int j=0;j<remove_pos.size();j++)
// 		{
// 			if (remove_pos[j]==i)
// 				continue;
// 			new_data.at(new_data_counter) = data_s.at(i);
// 			new_data_counter++;
// 		}
// 	}
}

quantity::quantity_t quantity::quantity_t::remove_data_by_index(unsigned int start, unsigned int stop) const
{
	if (!is_set())
		return *this;
	if (start<0)
		return {};
	if (start>=data().size())
	{
		logger::error("quantity::quantity_t::remove_data_by_index()","start>=data.size()","","return empty");
		return {};
	}
	if (stop>data().size())
	{
		stop=data().size();
	}

	vector<double> new_data(stop-start);
	int new_data_c = 0;
	for (int i=0;i<start;i++)
	{
		new_data.at(new_data_c);
		new_data_c++;
	}
	for (int i=stop;i<data().size();i++)
	{
		new_data.at(new_data_c);
		new_data_c++;
	}
	stringstream ss;
	ss << "remove_data_by_index(" << start << "," << stop << ")";
// 	operations_history_s.push_back(ss.str());
// 	data_s = new_data;
	return {*this,new_data,ss.str()};
}

quantity::quantity_t quantity::quantity_t::get_data_by_index(unsigned int start, unsigned int stop) const
{
	if (!is_set())
		return {};
	if (start<0)
		return {};
	if (start>data_s.size())
	{
		logger::error("quantity::quantity_t::get_data_by_index()","start>=data.size()","","returning");
		return {};
	}
	if (stop>data_s.size())
		stop=data_s.size();
	
	vector<double> new_data(data_s.begin()+start,data_s.begin()+stop);
// 	data_s = new_data;
	
	stringstream ss;
	ss << "get_data_by_index(" << start << "," << stop << ")";
// 	operations_history_s.push_back(ss.str());
	
	return {*this,new_data,ss.str()};
}

quantity::quantity_t quantity::quantity_t::absolute() const
{
	if (!is_set()) 
		return {};
	auto data_c = data_s;
	for (auto& d : data_c)
		d = abs(d);
// 	operations_history_s.push_back("absolute");
	return {*this,data_c,"absolute"};
}

quantity::quantity_t quantity::quantity_t::sum(int start, int stop) const
{
	if (!is_set()) 
		return {};
	if (start<0)
		return {};
	if (stop<=0)
		stop=data().size();

	double summe = 0;
	for (int i=start;i<stop;i++)
		summe += data_s.at(i);
// 	data_s = {summe};
// 	operations_history_s.push_back("sum");
	return {*this,{summe},"sum"};
}

quantity::quantity_t quantity::quantity_t::abs_sum() const
{
	return absolute().sum();
}

bool quantity::quantity_t::operator<(quantity_t obj) const
{
	if (*this > obj || *this == obj)
		return false;
	return true;
}

bool quantity::quantity_t::operator>(quantity_t obj) const
{
	if (!is_set() || !obj.is_set())
		return false;
	
	if (dimension()!=obj.dimension())
		return false;
	
	if (abs_sum().data().size()==0 ||  obj.abs_sum().data().size()==0)
		return false;
	if (abs_sum().data().front() <= obj.abs_sum().data().front())
		return false;
	return true;
}

bool quantity::quantity_t::operator==(quantity_t obj) const
{
	if (dimension()!=obj.dimension())
		return false;
	if (unit()!=obj.unit()) 
		return false;
	if (data().size()!=obj.data().size())	 
		return false;
	obj.change_unit(unit());
	for (int i=0;i<data().size();i++)
		if (data_s.at(i)!=obj.data_s.at(i)) 
			return false;
	return true;
}

bool quantity::quantity_t::operator!=(quantity_t obj) const
{
	return !(operator==(obj));
}

bool quantity::quantity_t::operator==(double number) const
{
	if (!dimension().is_relative()) 
		return false;
	if (!unit().base_units_exponents.is_relative())
	{
		logger::error("quantity::quantity_t::operator==","dimension is relative, but unit is not","tell florian");
		return false;
	}
	if (abs_sum().data().front()!=number)
		return false;
	return true;
}

bool quantity::quantity_t::operator!=(double number) const
{
	return !(operator==(number));
}

quantity::quantity_t& quantity::quantity_t::operator<<(const quantity_t& obj)
{
	if (!obj.is_set()) 
	{
		return *this;
	}
	if (!is_set())
	{
		*this = obj;
		return *this;
	}
	if (dimension()!=obj.dimension())
	{
		return *this;
	}
	
// 	if (unit.base_units_exponents!=obj.unit.base_units_exponents)
// 	{
// 		logger::error("quantity::quantity_t::operator<<()","base units are different: " + unit.base_units_exponents.to_string() + " != " + obj.unit.base_units_exponents.to_string());
// 		return;
// 	}
	auto C = obj;
	C = C.change_unit(unit());
	tools::vec::add(&data_s,C.data_s);
	return *this;
}

quantity::quantity_t quantity::quantity_t::operator+(quantity_t quantity) const
{
	if (name()!=quantity.name()) 
		return {}; // only allow addition within the same Größenart
		
	quantity = quantity.change_unit(unit());
	
	if (!is_set() || !quantity.is_set()) 
		return {};
	if (dimension() != quantity.dimension()) 
		return {};
// 	if (unit() != quantity.unit()) 
// 		return *this;
	
	if (data().size() == quantity.data().size())
	{
		for (int i=0; i< data().size();i++)
			quantity.data_s.at(i) += data_s.at(i);
		return quantity;
	} 
	else if (data().size()==1)
		return quantity + data_s.front();
	else if (quantity.data().size()==1)
		return *this + quantity.data_s.front();
	return {};
}

quantity::quantity_t quantity::quantity_t::operator+(const double summand) const
{
	if (!is_set()) 
		return {};
	
	auto data_c = data_s;
	for (auto& d : data_c)
	{
		d += summand;
	}
	return {*this,data_c};
}

// quantity::quantity_t& quantity::quantity_t::operator=(const quantity_t& quantity)
// {
// 	name_s = quantity.name();
// 	unit_s = quantity.unit();
// 	dimension_s = quantity.dimension();
// 	operations_history_s = quantity.operations_history();
// 	return *this;
// }

quantity::quantity_t quantity::quantity_t::operator-(quantity_t quantity) const
{
	return {*this + quantity*(-1)};
}
quantity::quantity_t quantity::quantity_t::operator-(const double summand) const
{
	return {*this + summand*(-1)};
}

quantity::quantity_t quantity::quantity_t::operator* (const double factor) const
{
	if (!is_set())
		return {};
	
	auto data_c = data_s;
	
	for (auto& d : data_c)
	{
		d *= factor;
	}
	return {*this,data_c};
}

quantity::quantity_t quantity::quantity_t::operator* (quantity_t quantity) const
{
	if (!is_set() || !quantity.is_set()) 
		return {};
	
	double multipier = 1;
	if (quantity.data().size()==1)
	{
		multipier = quantity.data_s.front();
		quantity::quantity_t Q(*this * multipier,unit_s * quantity.unit_s,dimension_s * quantity.dimension_s);
// 		cout << "operator* : " << Q.to_string_detailed() << endl;
		return Q;
	}
	else if (data().size()==1)
	{
		multipier = data_s.front();
// 		return {quantity * multipier,unit_s * quantity.unit_s,dimension_s * quantity.dimension_s};
		quantity::quantity_t Q(quantity * multipier,unit_s * quantity.unit_s,dimension_s * quantity.dimension_s);
		return Q;
	}
	
// 	if (multipier!=1)
// 	{
// 		return {*this * multipier,unit_s * quantity.unit_s,dimension_s * quantity.dimension_s};
// 	}
	else if (quantity.data().size()==data().size())
	{
// 		dimension_s = dimension_s * quantity.dimension_s;
// 		unit_s = unit_s * quantity.unit_s;
		for (int i=0;i<data_s.size();i++)
		{
// 			data_s.at(i) *= quantity.data_s.at(i);
			quantity.data_s.at(i) *= data_s.at(i);
		}
		stringstream ss;
		ss << name() << "*" << quantity.name();
		return {ss.str(),quantity.data_s,unit()*quantity.unit(),dimension()*quantity.dimension(),operations_history()};
	}
	return {};
}

quantity::quantity_t quantity::quantity_t::operator/ (const double divisor) const
{
	return *this*(1/divisor);
}

quantity::quantity_t quantity::quantity_t::operator/ (quantity_t quantity_p) const
{
// 	if (!is_set() || !quantity_p.is_set()) 
// 		return *this;
// 	
// 	if (quantity_p.data().size()==1)
// 	{
// 		return *this * quantity_p.invert();
// 	}
// 	else if (quantity_p.data().size()==data().size())
// 	{
// 		return *this * quantity_p.invert();
// 
// 	}
// 	else if (data().size()==1)
// 	{
// 		return quantity_p.invert() * *this;
// 	}
// 	else
// 	{
// 
// 	}
	quantity_t Q(*this * quantity_p.invert());
// 	cout << "operator/ : " << Q.to_string_detailed() << endl;
	return Q;
}



void quantity::quantity_t::operator+=(const double summand)
{
	*this = *this + summand;
}

void quantity::quantity_t::operator+=(quantity_t quantity_p)
{
	*this = *this + quantity_p;
}

/*********************/

quantity::mass_t::mass_t(vector<double> data_s,unit_t unit_s, dimension_t dim_s) : quantity_t("mass",data_s,unit_s,dim_s){}
quantity::mass_t::mass_t(const quantity_t& quantity_s) : quantity_t(quantity_s) {}

quantity::abundance_t::abundance_t(vector<double> data_s,unit_t unit_s, dimension_t dim_s) : quantity_t("abundance",data_s,unit_s,dim_s){}
quantity::abundance_t::abundance_t(const quantity_t& quantity_s) : quantity_t(quantity_s) {}

quantity::energy_t::energy_t(vector<double> data_s,unit_t unit_s, dimension_t dim_s) : quantity_t("energy",data_s,unit_s,dim_s){}
quantity::energy_t::energy_t(const quantity_t& quantity_s) : quantity_t(quantity_s) {}

quantity::rastersize_t::rastersize_t(vector<double> data_s,unit_t unit_s, dimension_t dim_s) : quantity_t( "rastersize",data_s,unit_s,dim_s){}
quantity::rastersize_t::rastersize_t(const quantity_t& quantity_s) : quantity_t(quantity_s) {}

quantity::depth_t::depth_t(vector<double> data_s,unit_t unit_s, dimension_t dim_s) : quantity_t("depth",data_s,unit_s,dim_s){}
quantity::depth_t::depth_t(const quantity_t& quantity_s) : quantity_t(quantity_s) {}

quantity::sputter_depth_t::sputter_depth_t(vector<double> data_s,unit_t unit_s, dimension_t dim_s) : quantity_t("sputter_depth",data_s,unit_s,dim_s){}
quantity::sputter_depth_t::sputter_depth_t(const quantity_t& quantity_s) : quantity_t(quantity_s) {}

quantity::total_sputter_depth_t::total_sputter_depth_t(vector<double> data_s,unit_t unit_s, dimension_t dim_s) : quantity_t("total_sputter_depth",data_s,unit_s,dim_s){}
quantity::total_sputter_depth_t::total_sputter_depth_t(const quantity_t& quantity_s) : quantity_t(quantity_s) {}

quantity::sputter_time_t::sputter_time_t(vector<double> data_s,unit_t unit_s, dimension_t dim_s) : quantity_t("sputter_time",data_s,unit_s,dim_s){}
quantity::sputter_time_t::sputter_time_t(const quantity_t& quantity_s) : quantity_t(quantity_s) {}

quantity::total_sputter_time_t::total_sputter_time_t(vector<double> data_s,unit_t unit_s, dimension_t dim_s) : quantity_t("total_sputter_time",data_s,unit_s,dim_s){}
quantity::total_sputter_time_t::total_sputter_time_t(const quantity_t& quantity_s) : quantity_t(quantity_s) {}

quantity::intensity_t::intensity_t(vector<double> data_s,unit_t unit_s, dimension_t dim_s) : quantity_t("intensity",data_s,unit_s,dim_s){}
quantity::intensity_t::intensity_t(const quantity_t& quantity_s) : quantity_t(quantity_s) {}

quantity::current_t::current_t(vector<double> data_s,unit_t unit_s, dimension_t dim_s) : quantity_t("current",data_s,unit_s,dim_s){}
quantity::current_t::current_t(const quantity_t& quantity_s) : quantity_t(quantity_s) {}

quantity::sputter_current_t::sputter_current_t(vector<double> data_s,unit_t unit_s, dimension_t dim_s) : quantity_t("sputter_current_t",data_s,unit_s,dim_s){}
quantity::sputter_current_t::sputter_current_t(const quantity_t& quantity_s) : quantity_t(quantity_s) {}

quantity::analysis_current_t::analysis_current_t(vector<double> data_s,unit_t unit_s, dimension_t dim_s) : quantity_t("primary_analysis_current",data_s,unit_s,dim_s){}
quantity::analysis_current_t::analysis_current_t(const quantity_t& quantity_s) : quantity_t(quantity_s) {}

quantity::concentration_t::concentration_t(vector<double> data_s,unit_t unit_s, dimension_t dim_s) : quantity_t("concentration",data_s,unit_s,dim_s){}
quantity::concentration_t::concentration_t(const quantity_t& quantity_s) : quantity_t(quantity_s) {}

quantity::dose_t::dose_t(vector<double> data_s,unit_t unit_s, dimension_t dim_s) : quantity_t("dose",data_s,unit_s,dim_s){}
quantity::dose_t::dose_t(const quantity_t& quantity_s) : quantity_t(quantity_s) {}

quantity::SF_t::SF_t(vector<double> data_s,unit_t unit_s, dimension_t dim_s) : quantity_t("sensitivity_factor",data_s,unit_s,dim_s){}
quantity::SF_t::SF_t(const quantity_t& quantity_s) : quantity_t(quantity_s) {}

///difficult to define: there are at least 2 very different definitions I know of ..
// quantity::RSF_t::RSF_t(vector<double> data_s,unit_t unit_s) : quantity_t("relative_sensitivity_factor",data_s,unit_s){}
// quantity::RSF_t::RSF_t(quantity_t quantity_s) : RSF_t(quantity_s.data(),quantity_s.unit()) {}

quantity::SR_t::SR_t(vector<double> data_s,unit_t unit_s, dimension_t dim_s) : quantity_t("sputter_rate",data_s,unit_s,dim_s){}
quantity::SR_t::SR_t(const quantity_t& quantity_s) : quantity_t(quantity_s) {}

///dim ML2T-3I-1
quantity::secondary_voltage_t::secondary_voltage_t(vector<double> data_s, unit_t unit_s, dimension_t dim_s) : quantity_t("secondary_voltage",data_s,unit_s,dim_s) {}
quantity::secondary_voltage_t::secondary_voltage_t(const quantity_t& quantity_s) : quantity_t(quantity_s) {}

quantity::substance_amount_t::substance_amount_t(vector<double> data_s, unit_t unit_s,dimension_t dim_s) : quantity_t("substance_amount",data_s,unit_s,dim_s) {}
quantity::substance_amount_t::substance_amount_t(const quantity_t& quantity_s) : quantity_t(quantity_s) {}

quantity::electrical_charge_t::electrical_charge_t(vector<double> data_s, unit_t unit_s, dimension_t dim_s) : quantity_t("electrical_charge",data_s,unit_s,dim_s) {}
quantity::electrical_charge_t::electrical_charge_t(const quantity_t& quantity_s) : quantity_t(quantity_s) {}




