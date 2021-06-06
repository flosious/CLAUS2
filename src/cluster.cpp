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

// cluster_t::cluster_t(string clustername) : isotopes(parse_clustername(clustername))
// {
// }

// unsigned int cluster_t::equilibrium_index_pos(unsigned int start)
// {
// 	if (equilibrium_index_pos_s>=0)
// 		return equilibrium_index_pos_s;
// 	
// 	/*copied from claus1*/
// 	vector<double> Y;
// 	
// 	if (intensity.is_set())
// 		Y = intensity.filter_gaussian(5,4).data; 
// 	else if (concentration.is_set())
// 		Y = concentration.filter_gaussian(5,4).data; 
// 	else 
// 	{
// 		logger::error("cluster_t::equilibrium_index_pos()","neither intensity nor concentration in cluster set; this should never happen", to_string(),"returning 0");
// 		return 0;
// 	}
// 	
// 	/*remove first 5% of all data; this is dangerous and should be solved better*/
// // 	if (measurement->settings.sputter_element() == "Cs" && Y.size()>20) 
// // 		start = 20;
// // 	else
// // 		start = 1;
// // 	equilibrium_starting_pos = start;
// // 	cout << "Y.size(b4)=" << Y.size() << endl;
// // 	Y.erase(Y.begin(),Y.begin()+start);
// // 	cout << "Y.size(after)=" << Y.size() << endl;
// 	/*****************************/
// 			
// 	double treshold = statistics::get_mad_from_Y(Y)/2;
// 	double median = statistics::get_median_from_Y(Y);
// 	set<int> extrema_idx;
// 	vector<int> maxIdx, minIdx;
// 		
// // 	if (!statistics::get_extrema_indices(maxIdx,minIdx,Y,treshold))
// // 		if (is_reference()) equilibrium_starting_pos = statistics::get_index_for_next_value_within_treshold(Y,median-treshold/2,median+treshold/2,1);
// // // 		else equilibrium_starting_pos = measurement->equilibrium_starting_pos();
// // 		else equilibrium_starting_pos = 0;
// // 	}
// // 	//type C
// // 	else if (minIdx.size()==0 && maxIdx.size()==1) // just the global maximum
// // 	{
// // // 		cout << name() << " type C2" << endl;
// // 		if (is_reference()) equilibrium_starting_pos = statistics::get_index_for_next_value_within_treshold(Y,median-treshold/2,median+treshold/2,1);
// // // 		else equilibrium_starting_pos = measurement->equilibrium_starting_pos();
// // 		else equilibrium_starting_pos = 0;
// 	if (statistics::get_extrema_indices(maxIdx,minIdx,Y,treshold))
// 	{
// 		/*remove right sided*/
// 		for (auto& m:maxIdx)
// 			if (m>0.5*Y.size()) m=0;
// 		for (auto& m:minIdx)
// 			if (m>0.5*Y.size()) m=0;
// 			
// 		set<int> maxIdx_set (maxIdx.begin(),maxIdx.end());
// 		set<int> minIdx_set (minIdx.begin(),minIdx.end());
// 		
// 		/*remove trivials*/
// 		maxIdx_set.erase(0);
// 		maxIdx_set.erase(1);
// 		minIdx_set.erase(0);
// 		minIdx_set.erase(1);
// 		
// // 		print("maxIdx_set: " + name());
// // 		print(maxIdx_set);
// // 		print("minIdx_set: " + name());
// // 		print(minIdx_set);
// 		
// // 		//type E D
// // 		if (minIdx_set.size()==0 && maxIdx_set.size()==0)
// // 		{
// // // 			cout << name() << " type E or D" << endl;
// // 			if (is_reference()) equilibrium_starting_pos = statistics::get_index_for_next_value_within_treshold(Y,median-treshold/2,median+treshold/2,1);
// // // 			else equilibrium_starting_pos = measurement->equilibrium_starting_pos();
// // 			else equilibrium_starting_pos = 0;
// // 		}
// 		// type G H
// 		if (minIdx_set.size()==0 && maxIdx_set.size()!=0 )
// 		{
// // 			if (is_reference()) equilibrium_starting_pos = statistics::get_index_for_next_value_within_treshold(Y,median-treshold/2,median+treshold/2,*maxIdx_set.begin());
// // 			else equilibrium_starting_pos = 0;
// // 			equilibrium_starting_pos=*maxIdx_set.begin(); 
// // 			cout << name() << " type G or H" << endl;
// 		}
// 		//type A B F --> implant
// 		else 
// 		{
// 			start=*minIdx_set.begin(); 
// 			/* check total signal sum for miss interpretation */
// 			double sum=0;
// 			double sum_equilibrium=0;
// 			for (int i=0;i<Y.size();i++)
// 				sum+=Y[i];
// 			for (int i=start;i<Y.size();i++)
// 				sum_equilibrium+=Y[i];
// 			if ((sum_equilibrium-(Y.size()-start)*Y.back())/(sum-Y.size()*Y.back())<0.7)
// 				start=0;
// // 			cout << name() << " type A or B or F" << endl;
// 		}
// 	}
// // 	cout << measurement->lot() << "_" << measurement->wafer()  << "_" << name() <<" equilibrium_starting_pos = " << equilibrium_starting_pos << endl;
// // 	result.equilibrium_starting_pos=equilibrium_starting_pos+start;
// // 	reduce_quantities_by_equlibrium_starting_pos(result);
// 	equilibrium_index_pos_s = start;
// 	return equilibrium_index_pos_s;
// }


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

// concentration_t& cluster_t::concentration()
// {
// 	if (concentration_p.is_set())
// 		return concentration_p;
// 	if (SF.is_set() && intensity().is_set())
// 		concentration_p = SF*intensity();
// 	return concentration_p;
// }
// 
// intensity_t& cluster_t::intensity()
// {
// 	if (intensity.is_set())
// 		return intensity;
// 	if (concentration_p.is_set() && SF.is_set())
// 		intensity = concentration_p / SF;
// 	return intensity;
// }
// 
// sputter_depth_t& cluster_t::sputter_depth()
// {
// 	if (sputter_depth_p.is_set())
// 		return sputter_depth_p;
// // 	if (sputter_time.is_set() && SR.is_set())
// // 		sputter_depth_p = SR*sputter_time;
// 	return sputter_depth_p;
// }
// 
// sputter_time_t& cluster_t::sputter_time
// {
// 	if (sputter_time_p.is_set())
// 		return sputter_time_p;
// // 	if (sputter_depth_p.is_set() && SR.is_set())
// // 		sputter_time_p = sputter_depth_p / SR ;
// 	return sputter_time_p;
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

// cluster_t cluster_t::change_sputter_time_resolution(sputter_time_t sputter_time_res)
// {
// 	return change_sputter_time(sputter_time.resolution(sputter_time_res));
// }
// 
// cluster_t cluster_t::change_sputter_depth_resolution(sputter_depth_t sputter_depth_res)
// {
// 	return change_sputter_depth(sputter_depth().resolution(sputter_depth_res));
// }

// cluster_t cluster_t::interpolate(sputter_time_t& new_sputter_time, sputter_time_t& old_sputter_time)
// {
// 	if (!sputter_time.is_set() && !old_sputter_time.is_set())
// 	{
// 		logger::error("cluster_t::change_sputter_time","!sputter_time.is_set() && !old_sputter_time.is_set()","","returning empty");
// 		return {};
// 	}
// 	if (!new_sputter_time.is_set()) 
// 	{
// 		logger::error("cluster_t::change_sputter_time","!new_sputter_time.is_set()","","returning empty");
// 		return {};
// 	}
// 	if (!old_sputter_time.is_set())
// 		old_sputter_time = sputter_time.change_unit(new_sputter_time.unit()); 
// 	if (old_sputter_time.unit() != new_sputter_time.unit())// same units?
// 	{
// 		logger::error("cluster_t::change_sputter_time","old_sputter_time.unit() != new_sputter_time.unit()","","returning empty");
// 		return {};
// 	}
// 	//new data is not allowed to be larger compared to old data
// 	if (new_sputter_time.max() > old_sputter_time.max())
// 	{
// 		logger::error("cluster_t::change_sputter_time","new_sputter_time.max() > old_sputter_time->max()","","returning empty");
// 		return {};
// 	}
// 	if (new_sputter_time.min() < old_sputter_time.min())
// 	{
// 		logger::error("cluster_t::change_sputter_time","new_sputter_time.min() < old_sputter_time->min()","","returning empty");
// 		return {};
// 	}
// 	
// 	cluster_t new_cluster(isotopes);
// 	map<double,double> XY_data;
// 	new_cluster.sputter_time_p = new_sputter_time;
// 	if (sputter_depth().is_set())
// 	{
// 		new_cluster.sputter_depth() = sputter_depth().interp(old_sputter_time,new_sputter_time);
// 	}
// 	if (concentration().is_set())
// 	{
// 		new_cluster.concentration() = concentration().interp(old_sputter_time,new_sputter_time);
// 	}
// 	if (intensity().is_set())
// 	{
// 		new_cluster.intensity() = intensity().interp(old_sputter_time,new_sputter_time);
// 	}
// // 	if (SR.is_set() && SR.data.size()>1)
// // 	{
// // 		new_cluster.SR = SR.interp(old_sputter_time,new_sputter_time);
// // 	}
// 	if (SF.is_set() && SF.data.size()>1)
// 	{
// 		new_cluster.SF = SF.interp(old_sputter_time,new_sputter_time);
// 	}
// 	if (RSF.is_set() && RSF.data.size()>1)
// 	{
// 		new_cluster.RSF = RSF.interp(old_sputter_time,new_sputter_time);
// 	}
// 	return new_cluster;
// }

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
