#include "quantity.hpp"

quantity::map_t::map_t(const quantity_t& X, const quantity_t& Y) : X(X), Y(Y)
{
	if (X.data().size()!=Y.data().size())
		logger::error("quantity::map_t::map_t()","X and Y have different sizes: X=" + std::to_string(X.data().size())+"; Y="+ std::to_string(Y.data().size()));
}

const quantity::map_t quantity::map_t::map_interp_akima(const quantity_t new_X) const
{
	const auto new_Y = Y.interp(X,new_X);
	return map_t(new_X,new_Y);
}

const map<double,double> quantity::map_t::data_map() const
{
	return tools::vec::combine_vecs_to_map(X.data(),Y.data());
}

const quantity::map_t quantity::map_t::remove_inf() const
{
	vector<unsigned int> remove_pos;
	for (int i=0; i<X.data().size() && i<Y.data().size();i++)
	{
		if (isinf(X.data().at(i)) || isinf(Y.data().at(i)))
			remove_pos.push_back(i);
	}
	if (remove_pos.size()==0)
		return *this;
	return map_t(X.remove_data_by_index(remove_pos),Y.remove_data_by_index(remove_pos));
}

const quantity::map_t quantity::map_t::pop_back() const
{
	return map_t(X.pop_back(),Y.pop_back());
}

const quantity::map_t quantity::map_t::remove_by_X(quantity::quantity_t X_s) const
{
	X_s = X_s.change_unit(X.unit());
	if (!X_s.is_set())
	{
		logger::error("quantity::map_t::remove_X()","X units are not the same",to_string() + " " + X_s.to_string());
		return *this;
	}
	vector<unsigned int> rem_idx;
	for (int i=0;i<X.data().size();i++)
	{
		if (tools::vec::find_in_vec(X.data().at(i),X_s.data())!=nullptr)
			rem_idx.push_back(i);
	}
	return {X.remove_data_by_index(rem_idx),Y.remove_data_by_index(rem_idx)};
}


const quantity::map_t quantity::map_t::remove_nan() const
{
	vector<unsigned int> remove_pos;
	for (int i=0; i<X.data().size() && i<Y.data().size();i++)
	{
		if (isnan(X.data().at(i)) || isnan(Y.data().at(i)))
			remove_pos.push_back(i);
	}
	if (remove_pos.size()==0)
		return *this;
	return map_t(X.remove_data_by_index(remove_pos),Y.remove_data_by_index(remove_pos));
}

bool quantity::map_t::is_set() const
{
	if (!X.is_set()) return false;
	if (!Y.is_set()) return false;
	return true;
}

std::string quantity::map_t::to_string() const
{
	stringstream ss;
	ss << "X: " << X.to_string() << "; Y: " << Y.to_string() << endl;
	return ss.str();
}

std::string quantity::map_t::to_string_detailed() const
{
	stringstream ss;
	ss << "X: " << X.to_string_detailed() << "; Y: " << Y.to_string_detailed() << endl;
	return ss.str();
}

fit_functions::polynom_t quantity::map_t::polynom(unsigned int polynom_grade) const
{
	return fit_functions::polynom_t(polynom_grade,data_map());
}

fit_functions::polynom_t quantity::map_t::polynom(const vector<unsigned>& rank, const vector<double>& polynom_start_parameters) const
{
	const auto data = tools::vec::combine_vecs_to_map(X.data(), Y.data());
	fit_functions::polynom_t poly(rank,polynom_start_parameters,data);
	return poly;
}

quantity::quantity_t quantity::map_t::polyfit(const quantity_t& x_vals, fit_functions::polynom_t polynom_s) const
{
	return {Y,polynom_s.y_data(x_vals.data())};
	
}

quantity::quantity_t quantity::map_t::polyfit(const quantity_t& x_vals, unsigned int polynom_grade) const
{
	return Y.fit_polynom_by_x_data(X,x_vals,polynom_grade);
}

quantity::quantity_t quantity::map_t::polyfit(const quantity_t& x_vals, const vector<unsigned>& rank, const vector<double>& polynom_start_parameters) const
{
	auto y_data = polynom(rank,polynom_start_parameters).y_data(x_vals.data());
	return {Y,y_data};
}

