#include "mgroup.hpp"

bool mgroups::mgroup_t::use_olcdb = true;
bool mgroups::mgroup_t::use_group = true;
bool mgroups::mgroup_t::use_settings = true;


/******************************************/

mgroups::mgroup_t::mgroup_t(measurements::measurement_t* measurement)
{
	insert_measurement(measurement);
}

void mgroups::mgroup_t::insert_measurement(measurements::measurement_t* M_p)
{
	measurements.insert(M_p);
}

const int mgroups::mgroup_t::olcdb() const
{
	for (measurements::measurement_t* M:measurements)
	{
		if (M->olcdb()>0)
			return M->olcdb();
	}
	return 0;
}

const std::__cxx11::string mgroups::mgroup_t::group() const
{
	for (measurements::measurement_t* M:measurements)
	{
		if (M->group()!="")
			return M->group();
	}
	return "";
}

std::__cxx11::string mgroups::mgroup_t::to_string()
{
	stringstream ss;
	const string del = ",";
	if (use_olcdb) ss << "olcdb: " << olcdb() << del;
	if (use_group) ss << "group: " << group() << del;
	ss << "measurements: " << measurements.size() << del;
	return ss.str();
}

bool mgroups::mgroup_t::operator==(mgroups::mgroup_t& obj) 
{
	if (use_olcdb)
		if (olcdb()!=obj.olcdb()) return false;
	
	if (use_group)
		if (group()!=obj.group()) return false;
		
	return true;
}

bool mgroups::mgroup_t::operator!=(mgroups::mgroup_t& obj)
{
	return !operator==(obj);
}
