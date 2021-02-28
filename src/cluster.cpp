#include "cluster.hpp"

cluster_t::cluster_t(string clustername, 
			  sputter_time_t sputter_time,
			  intensity_t intensity,
			  sputter_depth_t sputter_depth,
			  concentration_t concentration) : sputter_time_p(sputter_time), intensity_p(intensity), sputter_depth_p(sputter_depth), concentration_p(concentration)
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

std::__cxx11::string cluster_t::to_string()
{
	const int max = isotopes().size();
	if (max==0) return "";
	stringstream out;
	for (int i=0;i<max;i++)
	{
		out << isotopes_p.at(i).to_string();
		if (i<max-1) out << ",";
	}

	return out.str();
}


const bool cluster_t::is_set() const
{
	if (isotopes_p.size()>0) return true;
	return false;
}

cluster_t::cluster_t(vector<std::__cxx11::string> clustername_parts)
{
	smatch match;
	int nucleons;
	double amount;
	string symbol;
	for (auto& iso_p : clustername_parts)
	{
		if (regex_search(iso_p,match,regex("^([0-9]{0,3})([a-zA-Z]{1,3})([0-9]*)"))) 
		{
			if (match[1]!="") nucleons = tools::str::str_to_int(match[1]);
			else 
			{
				nucleons=PSE.element(symbol)->isotope_with_highest_abundance()->nucleons;
			}
			if (match[2]!="") symbol = match[2];
			else
			{
				logger::error("cluster_t::cluster_t() symbol not parseable, skipping", iso_p);
				continue;
			}
			if (match[3]!="") amount = tools::str::str_to_double(match[3]);
			else amount = 1;
			isotopes_p.push_back({symbol,nucleons,-1,amount});
		}
		else
		{
			*this = cluster_t("");
			logger::error("cluster_t::cluster_t() cluster not parseable, skipping", iso_p);
			return;
		}
	}
}

cluster_t::cluster_t(vector<isotope_t>& isotopes_s)  : isotopes_p(isotopes_s)
{
}

const concentration_t& cluster_t::concentration()
{
	return concentration_p;
}

const intensity_t& cluster_t::intensity()
{
	return intensity_p;
}

const sputter_depth_t& cluster_t::sputter_depth()
{
	return sputter_depth_p;
}

const sputter_time_t& cluster_t::sputter_time()
{
	return sputter_time_p;
}

vector<isotope_t>& cluster_t::isotopes()
{
	return isotopes_p;
}

bool cluster_t::operator!=(const cluster_t& obj) const
{
	return !operator==(obj);
}

bool cluster_t::operator==(const cluster_t& obj) const
{
	if (isotopes_p.size()!=obj.isotopes_p.size()) return false;
	for (auto& iso : isotopes_p)
		if (find(obj.isotopes_p.begin(),obj.isotopes_p.end(),iso)==obj.isotopes_p.end())
			return false;
	return true;
}

bool cluster_t::operator<(const cluster_t& obj) const
{
	if (isotopes_p.size()<obj.isotopes_p.size()) return true;
	if (isotopes_p.size()>obj.isotopes_p.size()) return false;
	
// 	set<isotope_t> isos(isotopes_p.begin(),isotopes_p.end());
// 	set<isotope_t> objisos(obj.isotopes_p.begin(),obj.isotopes_p.end());
	
// 	vector<isotope_t> i_vec(isos.begin(),isos)
	
	for (int i=0;i<isotopes_p.size();i++)
	{
		if (isotopes_p[i] < obj.isotopes_p[i])
			return true;
		if (isotopes_p[i] == obj.isotopes_p[i])
			continue;
		return false;
	}
	
	return false;
}





