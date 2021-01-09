#include "measurement_group.hpp"



bool measurement_groups::measurement_group_t::definition_t::id=true;




/*****************************/




int measurement_groups::measurement_group_t::id() const
{
	return id_p;
}

std::__cxx11::string measurement_groups::measurement_group_t::measurement_tool_name() const
{
	return measurement_tool_name_p;
}






/****************************/






bool measurement_groups::measurement_group_t::operator==(const measurement_group_t& obj) const
{
	if (id() != obj.id())	return false;	
	return true;
}
bool measurement_groups::measurement_group_t::operator!=(const measurement_group_t& obj) const
{
	return !(operator==(obj));
}
bool measurement_groups::measurement_group_t::operator<(const measurement_group_t& obj) const
{
	if (definition.id) if (id()<obj.id()) return true;
	
	return false;
}
