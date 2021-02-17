#include "measurement_group.hpp"

bool measurement_group_t::use_olcdb = true;
bool measurement_group_t::use_group = true;
bool measurement_group_t::use_settings = true;


/******************************************/


// const int measurement_group_t::olcdb() const
// {
// 	if (!use_olcdb) return -1;
// 	if (measurements().size()==0)
// 		return -1;
// 	if (measurements().at(0)->files()->size()==0)
// 		return -1;
// 	return measurements().at(0)->files()->front()->name->olcdb();
// }
// 
// const string measurement_group_t::group() const
// {
// 	if (!use_group) return "";
// 	if (measurements().size()==0)
// 		return "";
// 	if (measurements().at(0)->files()->size()==0)
// 		return "";
// 	return measurements().at(0)->files()->front()->name->group();
// }

const vector<measurement_t *> & measurement_group_t::measurements() const
{
	return measurements_p;
}


// bool measurement_groups::measurement_group_t::definition_t::id=true;
// 
// 
// 
// 
// /*****************************/
// 
// 
// 
// 
// int measurement_groups::measurement_group_t::id() const
// {
// 	return id_p;
// }
// 
// std::__cxx11::string measurement_groups::measurement_group_t::measurement_tool_name() const
// {
// 	return measurement_tool_name_p;
// }
// 
// 
// 
// 
// 
// 
// /****************************/
// 
// 
// 
// 
// 
// 
// bool measurement_groups::measurement_group_t::operator==(const measurement_group_t& obj) const
// {
// 	if (id() != obj.id())	return false;	
// 	return true;
// }
// bool measurement_groups::measurement_group_t::operator!=(const measurement_group_t& obj) const
// {
// 	return !(operator==(obj));
// }
// bool measurement_groups::measurement_group_t::operator<(const measurement_group_t& obj) const
// {
// 	if (definition.id) if (id()<obj.id()) return true;
// 	
// 	return false;
// }
