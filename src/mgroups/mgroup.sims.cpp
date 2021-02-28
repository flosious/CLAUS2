#include "mgroup.hpp"

mgroups::sims_t::sims_t(measurements::sims_t* measurement) : mgroup_t(measurement)
{
	measurements.insert(measurement);
}

void mgroups::sims_t::insert_measurement(measurements::sims_t* M_p)
{
	measurements.insert(M_p);
	mgroup_t::insert_measurement(M_p);
}

bool mgroups::sims_t::operator==(mgroups::sims_t& obj)
{
	if (!mgroup_t::operator==(obj)) return false;
	if (use_settings)
	{
		if (settings() != obj.settings()) 
			return false;
	}
	
	return true;
}

std::__cxx11::string mgroups::sims_t::to_string()
{
	stringstream ss;
	const string del = ",";
	ss << mgroup_t::to_string() << del;
	ss << settings().to_string();
	return ss.str();
}

bool mgroups::sims_t::operator!=(mgroups::sims_t& obj)
{
	return !operator==(obj);
}

msettings::sims_t mgroups::sims_t::settings()
{
	for (auto* M : measurements)
		if (M->settings().is_set())
			return M->settings();
	return {};
}
