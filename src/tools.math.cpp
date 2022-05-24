#include "tools.hpp"

vector<double> tools::math::multiply(vector<double>& values, double factor)
{
	vector<double> results(values.size());
	for (int i=0;i<values.size();i++)
	{
		results[i]=values.at(i)*factor;
	}
	return results;
}

vector<double> tools::math::add(vector<double>& values, double summand)
{
	vector<double> results(values.size());
	for (int i=0;i<values.size();i++)
	{
		results[i]=summand+values.at(i);
	}
	return results;
}

long long unsigned int tools::math::hexTOint(const std::string& hex)
{
	long long unsigned int x;   
	std::stringstream ss;
	ss << std::hex << hex;
	ss >> x;
	return x;
}


double tools::math::hexTOdouble(const std::string& hex)
{
	double x;   
	std::stringstream ss;
	ss << std::hex << hex;
	ss >> x;
	return x;
}
