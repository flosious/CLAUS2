#include "cluster.hpp"

cluster_t::cluster_t()
{
}

cluster_t::cluster_t(set<isotope_t> isotopes,
					 sputter_time_t sputter_time, 
					 intensity_t intensity, 
					 sputter_depth_t sputter_depth, 
					 concentration_t concentration) :
					 isotopes(isotopes.begin(),isotopes.end()),
					 sputter_time(sputter_time), 
					 intensity(intensity), 
					 sputter_depth(sputter_depth), 
					 concentration(concentration)
{
}

cluster_t::cluster_t(string clustername, 
			  sputter_time_t sputter_time,
			  intensity_t intensity,
			  sputter_depth_t sputter_depth,
			  concentration_t concentration) : 
			  sputter_time(sputter_time), 
			  intensity(intensity), 
			  sputter_depth(sputter_depth), 
			  concentration(concentration),
			  isotopes(parse_clustername(clustername))
{
}

vector<isotope_t> cluster_t::parse_clustername(const string clustername)
{
	smatch match;
	int nucleons;
	double amount;
	string symbol;
	vector<isotope_t> isotopes;
	vector<string> clustername_parts = tools::str::get_strings_between_delimiter(clustername, " ");
	for (auto& iso_p : clustername_parts)
	{
		isotope_t iso(iso_p);
		if (iso.symbol!="" && iso.nucleons>0)
			isotopes.push_back(iso);
		else
		{
			logger::error("cluster_t::parse_clustername()","regex_search(iso_p,match,regex('^([0-9]{0,3})([a-zA-Z]{1,3})([0-9]*)')",clustername,"return {}");
			return {};
		}
	}
	return isotopes;
}


cluster_t::cluster_t(const vector<isotope_t>& isotopes_s)  : isotopes(isotopes_s)
{
}

std::__cxx11::string cluster_t::to_string(const string del) const
{
	const int max = isotopes.size();
	if (max==0) return "";
	stringstream out;
	for (int i=0;i<max;i++)
	{
		out << isotopes.at(i).to_string();
		if (i<max-1) out << del;
	}
	return out.str();
}

const bool cluster_t::is_set() const
{
	if (isotopes.size()>0) return true;
	return false;
}

bool cluster_t::operator!=(const cluster_t& obj) const
{
	return !operator==(obj);
}

bool cluster_t::operator==(const cluster_t& obj) const
{
	if (isotopes.size()!=obj.isotopes.size()) return false;
	for (auto& iso : isotopes)
		if (find(obj.isotopes.begin(),obj.isotopes.end(),iso)==obj.isotopes.end())
			return false;
	return true;
}

bool cluster_t::operator<(const cluster_t& obj) const
{
	if (isotopes.size()<obj.isotopes.size()) return true;
	if (isotopes.size()>obj.isotopes.size()) return false;
	
	for (int i=0;i<isotopes.size();i++)
	{
		if (isotopes[i] < obj.isotopes[i])
			return true;
		if (isotopes[i] == obj.isotopes[i])
			continue;
		return false;
	}
	
	return false;
}



int cluster_t::Draw(mglGraph* gr)
{
	int data_points=0;
	if (sputter_depth.is_set())
		data_points = sputter_depth.data.size();
	else
		data_points = sputter_time.data.size();
	
	mglData y(data_points);
	mglData x(data_points);
// 	mglData y2(data_points);
	
	if (sputter_time.is_set())
	{
		gr->SetRange('x',0,statistics::get_max_from_Y(sputter_time.data));
// 		gr->Axis("X");
		x.Set(sputter_time.data);
		gr->Label('x',"sputter time",0);
// 		gr->Plot(x);
	}
		
	if (intensity.is_set())
	{
		y.Set(intensity.data);
		
		gr->SetRange('y',1,1E6);
		gr->SetFunc("","lg(y)");
		
// 		gr->SetOrigin(0,0,0);
		gr->Axis("UE");
// 		gr->Label(y,"intensity T","E"); -> labes each data point
		gr->Label('y',"intensity",0);
// 		gr->Plot(x,y);
		gr->Plot(y,"k","legend 'intensity'");
// 		concentration_p = concentration_t((intensity()*1E17).data);
	}
	
	gr->SetRange('x',0,statistics::get_max_from_Y(sputter_time.data));
	gr->SetOrigin(0,1E6);
	gr->Axis("x_");
	
// 	x.Set(sputter_time.data);
// 	gr->Label('x',"");
		
	if (concentration.is_set())
	{
		y.Set(concentration.data);
		
		gr->SetRange('y',1E15,1E21);
		
		gr->SetOrigin(statistics::get_max_from_Y(sputter_time.data),0);
		
		gr->Axis("yUE","B");
// 		gr->SetFunc("","lg(y)");
		gr->Label('y',"#B{concentration}",0);
// 		gr->SetTickShift(mglPoint(100,10,0,0)); // does not work?
		gr->Plot(y,"B","legend 'concentration'");
	}
	gr->Grid();	
	return 0;
}

string cluster_t::name() const
{
	return to_string("");
}


isotope_t cluster_t::corresponding_isotope(const vector<isotope_t > reference_isotopes) const
{
	if (isotopes.size()==1)
		return isotopes.at(0);
	set<isotope_t> isos;
	for (auto& i:isotopes)
	{
		if (find(reference_isotopes.begin(),reference_isotopes.end(),i)==reference_isotopes.end())
			isos.insert(i);
	}
	if (isos.size()==0)
	{
		logger::error("cluster_t::corresponding_isotope()","complex reference clusters are not supported","","returning empty");
		return {};
	}
	if (isos.size()>1)
	{
		logger::error("cluster_t::corresponding_isotope()","detected more than 1 possible isotope in cluster; unknown matrix isotopes? check database",to_string(),"returning empty");
		return {};
	}
	return *isos.begin();
}

cluster_t cluster_t::interpolate(quantity_t& new_Q, quantity_t& old_Q) const
{
	if (!new_Q.is_set() || !old_Q.is_set())
	{
		logger::error("cluster_t::interpolate()","!new_Q.is_set() || !old_Q.is_set()","","returning empty");
		return {};
	}
	if (new_Q.unit().base_units_exponents != old_Q.unit().base_units_exponents)// same base units?
	{
		logger::error("cluster_t::interpolate()","new_Q.unit().base_units_exponents != old_Q.unit().base_units_exponents","","returning empty");
		return {};
	}
	//new data is not allowed to be larger compared to old data
	if (new_Q.max() > old_Q.max())
	{
		logger::error("cluster_t::interpolate()","new_Q.max() > old_Q.max()","","returning empty");
		return {};
	}
	if (new_Q.min() < old_Q.min())
	{
		logger::error("cluster_t::interpolate()","new_Q.min() < old_Q.min()","","returning empty");
		return {};
	}
	cluster_t new_cluster(isotopes);
	map<double,double> XY_data;
// 	new_cluster.sputter_depth_p = new_sputter_depth;
	if (sputter_depth.is_set())
		new_cluster.sputter_depth = sputter_depth.interp(old_Q,new_Q);
	
	if (sputter_time.is_set())
		new_cluster.sputter_time = sputter_time.interp(old_Q,new_Q);
	
	if (concentration.is_set())
		new_cluster.concentration = concentration.interp(old_Q,new_Q);
	
	if (intensity.is_set())
		new_cluster.intensity = intensity.interp(old_Q,new_Q);
	
// 	if (SR.is_set() && SR.data.size()>1)
// 		new_cluster.SR = SR.interp(old_Q,new_Q);
	
	if (SF.is_set() && SF.data.size()>1)
		new_cluster.SF = SF.interp(old_Q,new_Q);
	
// 	if (RSF.is_set() && RSF.data.size()>1)
// 		new_cluster.RSF = RSF.interp(old_Q,new_Q);
	
	return new_cluster;
}

cluster_t cluster_t::filter_impulse(int window_size, float factor)
{
	cluster_t c_out = *this;
	if (intensity.is_set())
			c_out.intensity = intensity.filter_impulse(5);
	if (concentration.is_set())
		c_out.concentration = concentration.filter_impulse(5);
	if (SF.is_set())
		c_out.SF = SF.filter_impulse(5);
	return c_out;
}

// unsigned int cluster_t::sputter_equilibrium_starting_index() const
// {
// 	
// }
