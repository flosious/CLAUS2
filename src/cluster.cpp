#include "cluster.hpp"

cluster_t::cluster_t(std::__cxx11::string clustername)
{
	vector<string> element_names = tools::str::get_strings_between_delimiter(clustername, " ");
	for (auto& element_name : element_names)
		elements_p.push_back(element_name);
// 	smatch match;
// 	if (regex_search(clustername,match,regex("^([0-9]{0,3})([a-zA-Z]{1,3})([0-9]*)"))) 
// 	{
// 		element_t element()
// 		if (match[1]!="") nucleons = tools::str::str_to_int(match[1]);
// 		if (match[2]!="") symbol = match[2];
// 		if (match[3]!="") atoms = tools::str::str_to_int(match[3]);
// 		else atoms = 1;
// 	}
// 	else if (regex_search(clustername,match,regex("^([a-zA-Z]{1,3})([0-9]*)"))) 
// 	{
// 		if (match[1]!="") nucleons = -1;
// 		if (match[2]!="") symbol = match[1];
// 		if (match[3]!="") atoms = tools::str::str_to_int(match[2]);
// 		else atoms = 1;
// 	}
// 	else if (regex_search(clustername,match,regex("^([a-zA-Z]{1,3})"))) 
// 	{
// 		if (match[1]!="") nucleons = -1;
// 		if (match[2]!="") symbol = match[1];
// 		if (match[3]!="") atoms = 1;
// 	}
}

cluster_t::cluster_t(std::__cxx11::string clustername, sputter_time_t sputter_time_s, intensity_t intensity_s, sputter_depth_t sputter_depth_s, concentration_t concentration_s)
{
	vector<string> element_names = tools::str::get_strings_between_delimiter(clustername, " ");
	for (auto& element_name : element_names)
		elements_p.push_back(element_name);
	
	sputter_depth_p = sputter_depth_s;
	sputter_time_p = sputter_time_s;
	concentration_p = concentration_s;
	intensity_p = intensity_s;
}

bool cluster_t::is_set() const
{
	if (elements_p.size()>0) return true;
	
	/*at least one data type should be set to be valid*/
	if (sputter_depth_p.is_set()) return true;
	if (sputter_time_p.is_set()) return true;
	if (intensity_p.is_set()) return true;
	if (concentration_p.is_set()) return true;
	return false;
}
