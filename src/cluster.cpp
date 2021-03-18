#include "cluster.hpp"

cluster_t::cluster_t(string clustername, 
			  sputter_time_t sputter_time,
			  intensity_t intensity,
			  sputter_depth_t sputter_depth,
			  concentration_t concentration) : 
			  sputter_time_p(sputter_time), 
			  intensity_p(intensity), 
			  sputter_depth_p(sputter_depth), 
			  concentration_p(concentration),
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
			if (match[3]!="") amount = tools::str::str_to_double (match[3]);
			else amount = 1;
			isotopes.push_back({symbol,nucleons,-1,amount});
		}
		else
		{
			logger::error("cluster_t::cluster_t() cluster not parseable, skipping", iso_p);
			return {};
		}
	}
	return isotopes;
}

cluster_t::cluster_t(string clustername) : isotopes(parse_clustername(clustername))
{
}


cluster_t::cluster_t(vector<isotope_t>& isotopes_s)  : isotopes(isotopes_s)
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

// vector<isotope_t>& cluster_t::isotopes
// {
// 	return isotopes;
// }

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
	
// 	set<isotope_t> isos(isotopes.begin(),isotopes.end());
// 	set<isotope_t> objisos(obj.isotopes.begin(),obj.isotopes.end());
	
// 	vector<isotope_t> i_vec(isos.begin(),isos)
	
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
	if (sputter_depth().is_set())
		data_points = sputter_depth().data.size();
	else
		data_points = sputter_time().data.size();
	
	mglData y(data_points);
	mglData x(data_points);
// 	mglData y2(data_points);
	
	if (sputter_time().is_set())
	{
		gr->SetRange('x',0,statistics::get_max_from_Y(sputter_time().data));
// 		gr->Axis("X");
		x.Set(sputter_time().data);
		gr->Label('x',"sputter time",0);
// 		gr->Plot(x);
	}
		
	if (intensity().is_set())
	{
		y.Set(intensity().data);
		
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
	
	gr->SetRange('x',0,statistics::get_max_from_Y(sputter_time().data));
	gr->SetOrigin(0,1E6);
	gr->Axis("x_");
	
// 	x.Set(sputter_time().data);
// 	gr->Label('x',"");
		
	if (concentration().is_set())
	{
		y.Set(concentration().data);
		
		gr->SetRange('y',1E15,1E21);
		
		gr->SetOrigin(statistics::get_max_from_Y(sputter_time().data),0);
		
		gr->Axis("yUE","B");
// 		gr->SetFunc("","lg(y)");
		gr->Label('y',"#B{concentration}",0);
// 		gr->SetTickShift(mglPoint(100,10,0,0)); // does not work?
		gr->Plot(y,"B","legend 'concentration'");
	}
	gr->Grid();	
	return 0;
}


