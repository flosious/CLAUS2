#include "cluster.hpp"

cluster_t::cluster_t(std::__cxx11::string clustername)
{
	vector<string> clustername_parts = tools::str::get_strings_between_delimiter(clustername, " ");
	*this = cluster_t(clustername_parts);
// 	for (auto& element_name : element_names)
// 		elements_p.push_back(element_name);
	
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

const std::__cxx11::string cluster_t::to_string() const
{
// 	for (map<const isotope_t,const int>::iterator it=isotopes_amount().begin();it!=isotopes_amount().end();++it)
	if (isotopes_amount().size()==0) return "";
	stringstream out;
	for (auto& iso : isotopes_amount())
	{
		out << "\t" << iso.first.to_string() << iso.second;
	}
	return out.str();
}


const bool cluster_t::is_set() const
{
	if (isotopes_amount().size()>0) return true;
	return false;
}

const map<isotope_t,int> & cluster_t::isotopes_amount() const
{
	return isotopes_amount_p;
}


cluster_t::cluster_t(vector<std::__cxx11::string> clustername_parts)
{
	smatch match;
	int nucleons;
	int atoms;
	string symbol;
	for (auto& iso_p : clustername_parts)
	{
		if (regex_search(iso_p,match,regex("^([0-9]{0,3})([a-zA-Z]{1,3})([0-9]*)"))) 
		{
			if (match[1]!="") nucleons = tools::str::str_to_int(match[1]);
			else 
			{
				logger::error("cluster_t::cluster_t() nucleons not parseable, skipping", iso_p);
				continue;
			}
			if (match[2]!="") symbol = match[2];
			else
			{
				logger::error("cluster_t::cluster_t() symbol not parseable, skipping", iso_p);
				continue;
			}
			if (match[3]!="") atoms = tools::str::str_to_double(match[3]);
			else atoms = 1;
			isotopes_amount_p.insert(pair<isotope_t,int> ({symbol,nucleons},atoms));
		}
	}
}

cluster_t::cluster_t(vector<isotope_t>& isotopes_s) 
{
	for (auto& iso : isotopes_s)
		isotopes_amount_p.insert(pair<isotope_t,int> (iso,1));
}

cluster_t::cluster_t(map<isotope_t,int>& isotopes_amount_s) : isotopes_amount_p(isotopes_amount_s)
{	
}


const concentration_t* cluster_t::concentration()
{
	return &concentration_p;
}

const intensity_t* cluster_t::intensity()
{
	return &intensity_p;
}

const sputter_depth_t* cluster_t::sputter_depth()
{
	return &sputter_depth_p;
}

const sputter_time_t* cluster_t::sputter_time()
{
	return &sputter_time_p;
}

