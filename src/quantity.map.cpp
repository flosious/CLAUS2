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
