#include "mgroup.hpp"

mgroups::tofsims_t::tofsims_t(measurements::tofsims_t* measurement) : sims_t(measurement)
{
	measurements.insert(measurement);
}

void mgroups::tofsims_t::insert_measurement(measurements::tofsims_t* M_p)
{
	measurements.insert(M_p);
	sims_t::insert_measurement(M_p);
}


