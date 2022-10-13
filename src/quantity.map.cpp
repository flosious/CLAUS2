#include "quantity.hpp"

quantity::map_t::map_t() : X_p(), Y_p(), logger(__FILE__,"quantity::map_t")
{
}

quantity::map_t::map_t(const quantity_t& X, const quantity_t& Y) : X_p(X), Y_p(Y), logger(__FILE__,"quantity::map_t")
{
	if (X.data().size()!=Y.data().size())
	{
        //logger::error("quantity::map_t::map_t()","X and Y have different sizes: X=" + X.to_string()+"; Y="+ Y.to_string(),"setting to empty");
		*this = {};
	}
	
}

quantity::map_t::map_t(const quantity_t& X, const quantity_t& Y, const std::vector<std::pair<double,double>>& XYdata_pairs) : X_p(X), Y_p(Y), logger(__FILE__,"quantity::map_t")
{
	std::vector<double> Xdata(XYdata_pairs.size());
	std::vector<double> Ydata(XYdata_pairs.size());
	int i=0;
	for (auto& XY : XYdata_pairs)
	{
		Xdata.at(i) = XY.first;
		Ydata.at(i) = XY.second;
		i++;
	}
	
	*this = map_t(quantity_t{X_p,Xdata},quantity_t{Y_p,Ydata});
}

const quantity::quantity_t& quantity::map_t::X() const
{
	return X_p;
}
const quantity::quantity_t& quantity::map_t::Y() const
{
	return Y_p;
}

const quantity::map_t quantity::map_t::map_interp_akima(const quantity_t new_X) const
{
	const auto new_Y = Y().interp(X(),new_X);
	return map_t(new_X,new_Y);
}

const std::map<double,double> quantity::map_t::data_map() const
{
	return tools::vec::combine_vecs_to_map(X().data(),Y().data());
}

const quantity::map_t quantity::map_t::remove_inf() const
{
	std::vector<unsigned int> remove_pos;
	for (int i=0; i<X().data().size() && i<Y().data().size();i++)
	{
		if (isinf(X().data().at(i)) || isinf(Y().data().at(i)))
			remove_pos.push_back(i);
	}
	if (remove_pos.size()==0)
		return *this;
	return map_t(X().remove_data_by_index(remove_pos),Y().remove_data_by_index(remove_pos));
}

const quantity::map_t quantity::map_t::pop_back() const
{
	return map_t(X().pop_back(),Y().pop_back());
}

const quantity::map_t quantity::map_t::back() const
{
	return map_t(X().back(),Y().back());
}

const quantity::map_t quantity::map_t::remove_by_X(quantity::quantity_t X_s) const
{
	X_s = X_s.change_unit(X().unit());
	if (!X_s.is_set())
	{
        //logger::error("quantity::map_t::remove_X()()","X() units are not the same",to_string() + " " + X_s.to_string());
		return *this;
	}
	std::vector<unsigned int> rem_idx;
	for (int i=0;i<X().data().size();i++)
	{
		if (tools::vec::find_in_vec(X().data().at(i),X_s.data())!=nullptr)
			rem_idx.push_back(i);
	}
	return {X().remove_data_by_index(rem_idx),Y().remove_data_by_index(rem_idx)};
}


const quantity::map_t quantity::map_t::remove_nan() const
{
	std::vector<unsigned int> remove_pos;
	for (int i=0; i<X().data().size() && i<Y().data().size();i++)
	{
		if (isnan(X().data().at(i)) || isnan(Y().data().at(i)))
			remove_pos.push_back(i);
	}
	if (remove_pos.size()==0)
		return *this;
	return map_t(X().remove_data_by_index(remove_pos),Y().remove_data_by_index(remove_pos));
}

bool quantity::map_t::is_set() const
{
	if (!X().is_set()) return false;
	if (!Y().is_set()) return false;
	return true;
}

bool quantity::map_t::is_single_point() const
{
	if (!X().is_scalar()) return false;
	if (!Y().is_scalar()) return false;
	return true;
}

std::string quantity::map_t::to_string() const
{
	std::stringstream ss;
	ss << "X: " << X().to_string() << "; Y: " << Y().to_string();
	return ss.str();
}

std::string quantity::map_t::to_string_detailed() const
{
	std::stringstream ss;
	ss << "X: " << X().to_string_detailed() << "; Y: " << Y().to_string_detailed() ;
	return ss.str();
}

std::string quantity::map_t::to_string_list() const
{
	std::stringstream ss;
    ss << X().name() << "\t" << Y().name() << std::endl;
    ss << X().unit().to_string() << "\t" << Y().unit().to_string() << std::endl;
    ss << X().dimension().to_string() << "\t" << Y().dimension().to_string() << std::endl;
	if (X().data().size()!=Y().data().size())
	{
		ss << "<"<< X().data().size() << ">\t<" << Y().data().size() << ">";
		return ss.str();
	}
	for (int i=0;i<X().data().size();i++)
        ss << X().data().at(i) << "\t" << Y().data().at(i) << std::endl;
	return ss.str();
}

fit_functions::polynom_t quantity::map_t::polynom(unsigned int polynom_grade) const
{
	return fit_functions::polynom_t(polynom_grade,data_map());
}

fit_functions::polynom_t quantity::map_t::polynom(const std::vector<unsigned>& rank, const std::vector<double>& polynom_start_parameters) const
{
	const auto data = tools::vec::combine_vecs_to_map(X().data(), Y().data());
	fit_functions::polynom_t poly(rank,polynom_start_parameters,data);
	return poly;
}

quantity::map_t quantity::map_t::polyfit(fit_functions::polynom_t polynom_s,const unsigned int number_of_points) const
{
	return polyfit(polynom_s,X().change_resolution(number_of_points));
}

quantity::map_t quantity::map_t::polyfit(fit_functions::polynom_t polynom_s) const
{
	if (!polynom_s.successfully_fitted())
		return {};
	quantity_t fitted_Y(Y(),polynom_s.y_data(X().data()));
	return {X(),fitted_Y};
}

quantity::map_t quantity::map_t::polyfit(fit_functions::polynom_t polynom_s, quantity_t new_X) const
{
	if (!new_X.is_set())
		return {};
	quantity_t new_Y(Y(),polynom_s.y_data(new_X.data()));
	return {new_X,new_Y};
}

quantity::quantity_t quantity::map_t::polyfit(const quantity_t& x_vals, fit_functions::polynom_t polynom_s) const
{
	return {Y(),polynom_s.y_data(x_vals.data())};
	
}

quantity::quantity_t quantity::map_t::polyfit(const quantity_t& x_vals, unsigned int polynom_grade) const
{
	return Y().fit_polynom_by_x_data(X(),x_vals,polynom_grade);
}

quantity::quantity_t quantity::map_t::polyfit(const quantity_t& x_vals, const std::vector<unsigned>& rank, const std::vector<double>& polynom_start_parameters) const
{
	auto y_data = polynom(rank,polynom_start_parameters).y_data(x_vals.data());
	return {Y(),y_data};
}

quantity::quantity_t quantity::map_t::distance() const
{
	std::vector<double> distances(X().data().size());
	for (int i=0;i<X().data().size();i++)
		distances.at(i)=sqrt(pow(X().data().at(i),2)+pow(Y().data().at(i),2));
	quantity_t XY_radius("distance",distances,units::SI::one);
	if (X().unit() == Y().change_unit(X().unit()).unit())
	{
		const auto out = quantity_t(X(),distances);
		return out;
	}
	return XY_radius;
}

std::vector<int> quantity::map_t::bin_data(int bins_count) const
{
	if (bins_count==0 || X().data().size() == 0 || Y().data().size()==0 || X().data().size()!=Y().data().size())
		return {};
	
	return distance().bin_data(bins_count);
}

int quantity::map_t::filled_data_bins(int bins_count) const
{
	const auto BD = bin_data(bins_count);
	int c = 0;
	for (auto& b : BD)
		if (b>0)
			c++;
	return c;
}

int quantity::map_t::size() const
{
	return X().data().size();
}

quantity::map_t quantity::map_t::get_data_by_index(unsigned int start, unsigned int stop) const
{
	return {X().get_data_by_index(start,stop),Y().get_data_by_index(start,stop)};
}

const quantity::map_t quantity::map_t::change_resolution(const quantity_t new_X_resolution) const
{
	if (!new_X_resolution.is_set())
	{
        //logger::error("quantity::map_t::change_resolution()","!new_X_resolution.is_set()","returning empty");
		return {};
	}
	if (new_X_resolution.data().front()<=0)
	{
        //logger::error("quantity::map_t::change_resolution()","new_X_resolution.data().front()<=0","returning empty");
		return {};
	}
	const auto new_X = X().change_resolution(new_X_resolution);
	const auto new_Y = Y().interp(X(),new_X);
	return {new_X,new_Y};
}

std::vector<int> quantity::map_t::bin_data(const std::vector<double>& bins) const
{
	if (bins.size()==0 || X().data().size() == 0 || Y().data().size()==0 || X().data().size()!=Y().data().size())
		return {};
	
	return distance().bin_data(bins);
}

quantity::map_t quantity::map_t::remove_outliners( fit_functions::polynom_t polynom_s,float gof_treshold) const
{
	map_t result_map = *this;
	while (polynom_s.gof() < gof_treshold && polynom_s.rank().size()>=result_map.filled_data_bins(polynom_s.rank().size()))
	{
// 		result_map = result_map.delta_y(polynom_s).sort_Y_from_low_to_high().pop_back();
		auto X = result_map.delta_y(polynom_s).sort_Y_from_low_to_high().back().X();
// 		std::cout << std::endl << X.to_string() << std::endl;
		result_map = result_map.remove_by_X(X);
		polynom_s = result_map.polynom(polynom_s.rank(),polynom_s.fit_parameters());
        //logger::info(3,"quantity::map_t::remove_outliners()","removing from map: " + X.to_string());
        //logger::info(3,"quantity::map_t::remove_outliners()","new polynom: " + polynom_s.to_string());
	}
	return result_map;
}

quantity::map_t quantity::map_t::remove_worst_outliner(const fit_functions::polynom_t& polynom_s) const
{
	auto X = delta_y(polynom_s).sort_Y_from_low_to_high().back().X();
	return remove_by_X(X);
}

quantity::map_t quantity::map_t::swap_X_Y() const
{
	return {Y(),X()};
}

quantity::map_t quantity::map_t::at(unsigned int idx_pos) const
{
	return {X().at(idx_pos),Y().at(idx_pos)};
}

quantity::map_t quantity::map_t::max() const
{
	if (!Y().is_set())
		return {};
	const int idx = statistics::get_max_index_from_Y(Y().data());
	return {X().at(idx),Y().at(idx)};
}

quantity::map_t quantity::map_t::min() const
{
	if (!Y().is_set())
		return {};
	const int idx = statistics::get_max_index_from_Y(Y().data());
	return {X().at(idx),Y().at(idx)};
}

quantity::map_t quantity::map_t::absolute() const
{
	return {X(),Y().absolute()};
}

quantity::quantity_t quantity::map_t::integral() const
{
	return Y().integrate(X());
}

quantity::map_t quantity::map_t::sort_X_from_low_to_high() const
{
	auto pairs = XYdata_pairs();
	sort(pairs.begin(),pairs.end(),quantity::map_t::sort_by_x);
	return map_t(X(),Y(),pairs);
}
quantity::map_t quantity::map_t::sort_Y_from_low_to_high() const
{
	auto pairs = XYdata_pairs();
	sort(pairs.begin(),pairs.end(),quantity::map_t::sort_by_y);
	return map_t(X(),Y(),pairs);
}
quantity::map_t quantity::map_t::sort_X_from_high_to_low() const
{
	return sort_X_from_low_to_high().reverse_order();
}
quantity::map_t quantity::map_t::sort_Y_from_high_to_low() const
{
	return sort_Y_from_low_to_high().reverse_order();
}

quantity::map_t quantity::map_t::delta_y(const fit_functions::polynom_t& polynom_s) const
{
	quantity_t qy (Y(),polynom_s.y_data(X().data()));
	return delta_y(qy);
}

quantity::map_t quantity::map_t::delta_y(const quantity_t& Y_ref) const
{
	auto dy = (Y()-Y_ref).absolute();
	return {X(),dy};
}

quantity::map_t quantity::map_t::delta_y(const map_t& Y_ref) const
{
	return (*this - Y_ref).absolute();
}

std::vector<std::pair<double,double>> quantity::map_t::XYdata_pairs() const
{
	std::vector<std::pair<double,double>> pairs(size());
	for (int i=0;i<size();i++)
	{
		pairs.at(i) = std::pair<double,double> (X().data().at(i),Y().data().at(i));
	}
	return pairs;
}

quantity::map_t quantity::map_t::reverse_order() const
{
	return {X().reverse(), Y().reverse()};
}

/********* STATICS *********/


bool quantity::map_t::sort_by_x(const std::pair<double,double>& P1, const std::pair<double,double>& P2)
{
	return (P1.first < P2.first);
}

bool quantity::map_t::sort_by_y(const std::pair<double,double>& P1, const std::pair<double,double>& P2)
{
	return (P1.second < P2.second);
}

/*********OPERATORS*********/

quantity::map_t& quantity::map_t::operator<<(map_t obj)
{
	if (!is_set())
	{
		*this = obj;
		return *this;
	}
	obj.X_p=obj.X().change_unit(X().unit());
	obj.Y_p=obj.Y().change_unit(Y().unit());
	if (obj.is_set())
	{
		X_p << obj.X_p;
		Y_p << obj.Y_p;
	}
	return *this;
}

quantity::map_t quantity::map_t::operator+(map_t obj) const
{
	if (X()!=obj.X())
		return {};
	return *this + obj.Y();
}
quantity::map_t quantity::map_t::operator+(quantity_t Y_o) const
{
	Y_o = Y_o.change_unit(Y().unit());
	
	if (!Y_o.is_set())
		return {};
	
	return {X(),Y()+Y_o};
}

quantity::map_t quantity::map_t::operator-(map_t obj) const
{
	if (X()!=obj.X())
		return {};
	return *this - obj.Y();
}
quantity::map_t quantity::map_t::operator-(quantity_t Y_o) const
{
	Y_o = Y_o.change_unit(Y().unit());
	
	if (!Y_o.is_set())
		return {};
	
	return {X(),Y()-Y_o};
}

quantity::map_t quantity::map_t::operator*(map_t obj) const
{
	if (X()!=obj.X())
		return {};
	return *this * obj.Y();
}
quantity::map_t quantity::map_t::operator*(quantity_t Y_o) const
{
	if (!Y_o.is_set())
		return {};
	
	return {X(),Y()*Y_o};
}

quantity::map_t quantity::map_t::operator/(map_t obj) const
{
	if (X()!=obj.X())
		return {};
	return *this / obj.Y();
}
quantity::map_t quantity::map_t::operator/(quantity_t Y_o) const
{
	if (!Y_o.is_set())
		return {};
	
	return {X(),Y()/Y_o};
}
