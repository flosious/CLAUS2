#include "mgroup.hpp"

mgroups::dsims_t::dsims_t(measurements::dsims_t* measurement) : sims_t(measurement)
{
	measurements.insert(measurement);
}

void mgroups::dsims_t::insert_measurement(measurements::dsims_t* M_p)
{
	measurements.insert(M_p);
	sims_t::insert_measurement(M_p);
}

std::__cxx11::string mgroups::dsims_t::to_string()
{
	stringstream ss;
	const string del = ",";
	ss << mgroup_t::to_string() << del;
	ss << settings().sputter_ion().to_string() << del;
	ss << settings().secondary_voltage().to_string() << del;
	ss << "secondary_polarity: " << settings().secondary_polarity();
	return ss.str();
}

msettings::dsims_t mgroups::dsims_t::settings()
{
	for (auto* M : measurements)
		if (M->settings().is_set())
			return M->settings();
	return {};
}

bool msettings::dsims_t::operator==(msettings::dsims_t& obj)
{
	if (secondary_polarity()!=obj.secondary_polarity()) return false;
	if (secondary_voltage() != obj.secondary_voltage()) return false;
	if (sputter_ion() != obj.sputter_ion()) return false;
	return true;
}

bool msettings::dsims_t::operator!=(msettings::dsims_t& obj)
{
	return !operator==(obj);
}
