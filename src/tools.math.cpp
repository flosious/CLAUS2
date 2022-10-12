#include "tools.hpp"

std::vector<double> tools::math::multiply(std::vector<double>& values, double factor)
{
	std::vector<double> results(values.size());
	for (int i=0;i<values.size();i++)
	{
		results[i]=values.at(i)*factor;
	}
	return results;
}

std::vector<double> tools::math::add(std::vector<double>& values, double summand)
{
	std::vector<double> results(values.size());
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
