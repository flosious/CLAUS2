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
string matrix_t::to_string()
{
	stringstream out;
	out << "elements:" << LINE_DELIMITER;
	for (auto& element:elements)
		out << element.name() << "\t";
	out << endl;
	out << "isotopes:" << LINE_DELIMITER;
	for (auto& isotope:isotopes)
		out << isotope.name() << "\t";
	return out.str();
}

