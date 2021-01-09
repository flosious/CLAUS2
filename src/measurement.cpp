#include "measurement.hpp"


bool measurements::measurement_t::definition_t::olcdb=true;
bool measurements::measurement_t::definition_t::repetition=true;
bool measurements::measurement_t::definition_t::sample=true;

/*******************/



int measurements::measurement_t::olcdb() const
{
// 	if (files.size()==0) return -1;
// 	return (*files.begin())->olcdb();
	return 0;
}

std::__cxx11::string measurements::measurement_t::repetition() const
{
// 	if (files.size()==0) return "";
// 	return (*files.begin())->repetition();
	return "";
}

bool measurements::measurement_t::operator==(const measurements::measurement_t& obj) const
{
	if (olcdb() != obj.olcdb())		return false;
	return true;
}
bool measurements::measurement_t::operator!=(const measurements::measurement_t& obj) const
{
	return !(operator==(obj));
}
bool measurements::measurement_t::operator<(const measurements::measurement_t& obj) const
{
	if (olcdb()<obj.olcdb()) return true;
	if (olcdb()>obj.olcdb()) return false;
	
	if (repetition()<obj.repetition()) return true;
	if (repetition()>obj.repetition()) return false;
	
	/*remember: this is a pointer comparison!*/
	if (sample<obj.sample) return true;
	if (sample>obj.sample) return false;
	
	return false;
}


