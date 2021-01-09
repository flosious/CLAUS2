#include "matrix.hpp"

bool matrix_t::operator==(const matrix_t& obj) const
{
	if (elements.size()!=obj.elements.size()) return false;
	if (isotopes.size()!=obj.isotopes.size()) return false;
	
	for (auto& element:elements)
		if (obj.elements.find(element)!=obj.elements.end()) 
			return false;
	for (auto& isotope:isotopes)
		if (obj.isotopes.find(isotope)!=obj.isotopes.end()) 
			return false;
		
	return true;
}
bool matrix_t::operator!=(const matrix_t& obj) const
{
	return !(operator==(obj));
}
string matrix_t::to_string(std::__cxx11::string prefix)
{
	stringstream out;
	out << prefix << "elements:" << LINE_DELIMITER;
	out << prefix;
	for (auto& element:elements)
		out << element.to_string(prefix) << "\t";
	out << endl;
	out << prefix << "isotopes:" << LINE_DELIMITER;
	out << prefix;
	for (auto& isotope:isotopes)
		out << isotope.to_string(prefix) << "\t";
	return out.str();
}

