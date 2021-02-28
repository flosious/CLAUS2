#include "measurement.hpp"

measurements::tofsims_t::tofsims_t(files::tofsims_t* file, sample_t* sample) : sims_t(file,sample)
{
	files.insert(file);
}


void measurements::tofsims_t::insert_file(files::tofsims_t* file_p)
{
	files.insert(file_p);
	measurements::sims_t::insert_file(file_p);
}

