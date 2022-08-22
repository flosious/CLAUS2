#include "cluster.hpp"

cluster_t::cluster_t() :  logger(global_logger,__FILE__,"cluster_t")
{
}

cluster_t::cluster_t(set<isotope_t> isotopes,
					 quantity::sputter_time_t sputter_time, 
					 quantity::intensity_t intensity, 
					 quantity::depth_t sputter_depth, 
					 quantity::concentration_t concentration) :
					 isotopes(isotopes.begin(),isotopes.end()),
					 sputter_time(sputter_time), 
					 intensity(intensity), 
					 sputter_depth(sputter_depth), 
                     concentration(concentration),
                     logger(global_logger,__FILE__,"cluster_t")
{
}

cluster_t::cluster_t(string clustername, 
			  quantity::sputter_time_t sputter_time,
			  quantity::intensity_t intensity,
			  quantity::depth_t sputter_depth,
			  quantity::concentration_t concentration) : 
			  sputter_time(sputter_time), 
			  intensity(intensity), 
			  sputter_depth(sputter_depth), 
			  concentration(concentration),
              isotopes(parse_clustername(clustername)),
              logger(global_logger,__FILE__,"cluster_t")
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
            //logger::error("cluster_t::parse_clustername()","regex_search(iso_p,match,regex('^([0-9]{0,3})([a-zA-Z]{1,3})([0-9]*)')",clustername,"return {}");
			return {};
		}
	}
    //logger::debug(20,"cluster_t::parse_clustername()","clustername",clustername);
	return isotopes;
}


cluster_t::cluster_t(const vector<isotope_t>& isotopes_s)  : isotopes(isotopes_s), logger(global_logger,__FILE__,"cluster_t")
{
}

cluster_t::cluster_t(const isotope_t isotope_s) : isotopes({isotope_s}), logger(global_logger,__FILE__,"cluster_t")
{
}

string cluster_t::to_string(const string del) const
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

quantity::intensity_t cluster_t::intensity_background(float treshold) const
{
	if (!intensity.is_set())
		return {};
	unsigned int bins = 10;
// 	statistics::histogram_t histogram(intensity.data(),10);
// 	cout << endl << "HISTOGRAM: " << endl;
// 	print(histogram.linear_bins());
// 	print(histogram.log10_bins());
	
	auto histograms = histogram_builder_t(intensity);
	auto histo = histograms.equally_linearly_distanced_bins(bins);
	auto histo_detail = histo.rebuild(histo.quantity_data_size_to_its_bin().rbegin()->second).equally_log10_distanced_bins(bins);
// 	cout << "linear: " << endl << histo.to_string() << endl;
// 	cout << "log10: " << endl << histo_detail.to_string() << endl;
// 	cout << histo.quantity_data_size_to_its_bin().rbegin()->second.quantities_in_bin().median().to_string() << endl;
// 	cout << histo.quantity_data_size_to_its_bin().rbegin()->second.quantities_in_bin().get_data_by_index_rel(0,0.25).median().to_string() << endl;
// 	for (const auto& m : histograms.equally_linearly_distanced_bins(bins).quantity_data_size_to_its_bin())
// 		cout << m.first << endl;
	return histo.quantity_data_size_to_its_bin().rbegin()->second.quantities_in_bin().remove_data_equal_to(0).median();
// 	return histo.quantity_data_size_to_its_bin().rbegin()->second.quantities_in_bin().remove_data_equal_to(0).get_data_by_index_rel(0,0.25).median();
}

quantity::concentration_t cluster_t::concentration_background(float treshold) const
{
	if (!concentration.is_set())
		return {};
	unsigned int bins = 10;
// 	statistics::histogram_t histogram(intensity.data(),10);
// 	cout << endl << "HISTOGRAM: " << endl;
// 	print(histogram.linear_bins());
// 	print(histogram.log10_bins());
	
	auto histograms = histogram_builder_t(concentration);
	auto histo = histograms.equally_linearly_distanced_bins(bins);
// 	auto histo_detail = histo.rebuild(histo.quantity_data_size_to_its_bin().rbegin()->second).equally_log10_distanced_bins(bins);
// 	cout << "linear: " << endl << histo.to_string() << endl;
// 	cout << "log10: " << endl << histo_detail.to_string() << endl;
// 	cout << histo.quantity_data_size_to_its_bin().rbegin()->second.quantities_in_bin().median().to_string() << endl;
// 	cout << histo.quantity_data_size_to_its_bin().rbegin()->second.quantities_in_bin().get_data_by_index_rel(0,0.25).median().to_string() << endl;
	const auto sum = histo.quantities_in_bins();
// 	for (const auto& m : histo.quantity_data_size_to_its_bin())
// 		cout << m.first << "\t[" << m.second.start().data().front() << "," << m.second.stop().data().front() << ")\trel:" << (double)m.first/sum*100 << endl ;
	double relative_size = (double)histo.quantity_data_size_to_its_bin().rbegin()->first/sum;
	if (relative_size < treshold)
	{
        //logger::info(3,"cluster_t::concentration_background()","number of points for background detection not sufficient: relative_size=" + std::to_string(relative_size));
		return {};
	}
// 	return histo.quantity_data_size_to_its_bin().rbegin()->second.quantities_in_bin().remove_data_equal_to(0).get_data_by_index_rel(0,0.25).median();
	return histo.quantity_data_size_to_its_bin().rbegin()->second.quantities_in_bin().remove_data_equal_to(0).median();
}

bool cluster_t::operator!=(const cluster_t& obj) const
{
	return !operator==(obj);
}

bool cluster_t::operator==(const cluster_t& obj) const
{
	if (isotopes.size()!=obj.isotopes.size()) return false;
	for (auto& iso : isotopes)
	{
		auto found = find(obj.isotopes.begin(),obj.isotopes.end(),iso);
		if (found ==obj.isotopes.end())
			return false;
// 		if (found->substance_amount != iso.substance_amount)
// 			return false;
	}
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



//int cluster_t::Draw(mglGraph* gr)
//{
//	int data_points=0;
//	if (sputter_depth.is_set())
//		data_points = sputter_depth.data().size();
//	else
//		data_points = sputter_time.data().size();
	
//	mglData y(data_points);
//	mglData x(data_points);
//// 	mglData y2(data_points);
	
//	if (sputter_time.is_set())
//	{
//		gr->SetRange('x',0,statistics::get_max_from_Y(sputter_time.data()));
//// 		gr->Axis("X");
//		x.Set(sputter_time.data());
//		gr->Label('x',"sputter time",0);
//// 		gr->Plot(x);
//	}
		
//	if (intensity.is_set())
//	{
//		y.Set(intensity.data());
		
//		gr->SetRange('y',1,1E6);
//		gr->SetFunc("","lg(y)");
		
//// 		gr->SetOrigin(0,0,0);
//		gr->Axis("UE");
//// 		gr->Label(y,"intensity T","E"); -> labes each data point
//		gr->Label('y',"intensity",0);
//// 		gr->Plot(x,y);
//		gr->Plot(y,"k","legend 'intensity'");
//// 		concentration_p = quantity::concentration_t((intensity()*1E17).data());
//	}
	
//	gr->SetRange('x',0,statistics::get_max_from_Y(sputter_time.data()));
//	gr->SetOrigin(0,1E6);
//	gr->Axis("x_");
	
//// 	x.Set(sputter_time.data());
//// 	gr->Label('x',"");
		
//	if (concentration.is_set())
//	{
//		y.Set(concentration.data());
		
//		gr->SetRange('y',1E15,1E21);
		
//		gr->SetOrigin(statistics::get_max_from_Y(sputter_time.data()),0);
		
//		gr->Axis("yUE","B");
//// 		gr->SetFunc("","lg(y)");
//		gr->Label('y',"#B{concentration}",0);
//// 		gr->SetTickShift(mglPoint(100,10,0,0)); // does not work?
//		gr->Plot(y,"B","legend 'concentration'");
//	}
//	gr->Grid();
//	return 0;
//}

quantity::abundance_t cluster_t::abundance()
{
	quantity::abundance_t abu;
	for (auto& iso : isotopes)
	{
		if (!iso.abundance.is_set()) 
			continue;
		if (!abu.is_set())
			abu = iso.abundance;
		else
			abu = abu * iso.abundance;
	}
	return abu;
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
		{
            //logger::debug(14,"cluster_t::corresponding_isotope()","not found cluster isotope in reference_isotopes", i.to_string(),"this is a good message");
			isos.insert(i);
		}
	}
	
	if (isos.size()==0)
	{
        //logger::error("cluster_t::corresponding_isotope()","complex reference clusters are not supported","","returning empty");
		return {};
	}
	if (isos.size()>1)
	{
        //logger::error("cluster_t::corresponding_isotope()","detected more than 1 possible corresponding isotope in cluster; unknown matrix isotopes? check database",to_string(),"returning empty");
		return {};
	}
	return *isos.begin();
}

cluster_t cluster_t::interpolate(const quantity::quantity_t& new_Q, const quantity::quantity_t& old_Q) const
{
	if (!new_Q.is_set() || !old_Q.is_set())
	{
        //logger::error("cluster_t::interpolate()","!new_Q.is_set() || !old_Q.is_set()","","returning empty");
		return {};
	}

	if (new_Q.unit().base_units_exponents != old_Q.unit().base_units_exponents)// same base units?
	{
        //logger::error("cluster_t::interpolate()","new_Q.unit().base_units_exponents != old_Q.unit().base_units_exponents","","returning empty");
		return {};
	}
	//new data is not allowed to be larger compared to old data
	if (new_Q.max() > old_Q.max())
	{
        //logger::error("cluster_t::interpolate()","new_Q.max()=" + new_Q.max().to_string() +" > old_Q.max()="+old_Q.max().to_string(),"","returning empty");
		return {};
	}
	if (new_Q.min() < old_Q.min())
	{
        //logger::error("cluster_t::interpolate()","new_Q.min()="+new_Q.min().to_string()+" < old_Q.min()="+old_Q.min().to_string(),"","returning empty");
		return {};
	}
	cluster_t new_cluster(isotopes);
	map<double,double> XY_data;
// 	new_cluster.sputter_depth_p = new_sputter_depth;
	if (sputter_depth.is_set())
		new_cluster.sputter_depth = sputter_depth.interp(old_Q,new_Q); ///Exception safe???
	
	if (sputter_time.is_set())
		new_cluster.sputter_time = sputter_time.interp(old_Q,new_Q);
	
	if (concentration.is_set())
		new_cluster.concentration = concentration.interp(old_Q,new_Q);
	
	if (intensity.is_set())
		new_cluster.intensity = intensity.interp(old_Q,new_Q);
	
// 	if (SR.is_set() && SR.data().size()>1)
// 		new_cluster.SR = SR.interp(old_Q,new_Q);
	
	if (SF.is_set() && SF.data().size()>1)
		new_cluster.SF = SF.interp(old_Q,new_Q);
	
// 	if (RSF.is_set() && RSF.data().size()>1)
// 		new_cluster.RSF = RSF.interp(old_Q,new_Q);
	
	return new_cluster;
}

cluster_t cluster_t::filter_impulse(int window_size, float factor)
{
	cluster_t c_out = *this;
	
	if (intensity.is_set())
			c_out.intensity = intensity.filter_impulse(window_size,factor);
	
	if (concentration.is_set())
		c_out.concentration = concentration.filter_impulse(window_size,factor);
	
	
	//ATTENTION i am not sure if this is right ...
	if (SF.is_set())
		c_out.SF = SF.filter_impulse(window_size,factor);
// 	if (RSF.is_set())
// 		c_out.RSF = RSF.filter_impulse(window_size,factor);
	
	return c_out;
}


// quantity::map_t cluster_t::concentration_vs_sputter_depth() const
// {
// 	return {concentration(),crater.sputter_depth};
// }
// quantity::map_t cluster_t::concentration_vs_sputter_time() const
// {
// 	return {concentration(cluster),crater.sputter_time};
// }
// quantity::map_t cluster_t::intensity_vs_sputter_depth() const
// {
// 	return {intensity(cluster),crater.sputter_depth};
// }
// quantity::map_t cluster_t::intensity_vs_sputter_time() const
// {
// 	return {intensity(cluster),crater.sputter_time};
// }

/********************************************/
/* matrix_clusters_c */
/********************************************/

string matrix_clusters_c::to_string(const string del) const
{
	stringstream out;
	unsigned int i=0;
	unsigned int s = clusters.size();
	for (auto& C : clusters)
	{
		out << C->to_string();
		if (i < s-1)
			out << "+";
		i++;
	}
	return out.str();
}

/*"30Si" can be a reference cluster for elemental Si or isotopical purified Si*/
/*"30Si 28Si" can be a reference cluster for elemental Si or isotopical purified Si*/
/*"74Ge 28Si" can be a reference cluster for elemental Si+Ge or isotopical purified Si+Ge*/
matrix_clusters_c::matrix_clusters_c(vector<cluster_t>& clusters_s, const vector<isotope_t> matrix_isotopes) : logger(global_logger,__FILE__,"matrix_clusters_c")
{
	if (matrix_isotopes.size()==0)
		return;
	if (clusters_s.size()==0)
		return;
	
// 	cout << "matrix_isotopes.size(): " << matrix_isotopes.size() << endl;
// 	cout << "clusters_s.size(): " << clusters_s.size() << endl;
	
	bool insert=false;
	for (auto& C : clusters_s) 
	{
// 		cout << "C: " << C.to_string() << endl;
		insert = true;
		for (auto& I : C.isotopes)
		{
			if (find(matrix_isotopes.begin(),matrix_isotopes.end(),I)==matrix_isotopes.end())
			{
				insert=false;
				break;
			}
		}
		// if all isotopes within the cluster are isotopes within the matrix, then this is a matrix-cluster
		// that means: if at least 1 isotope within the cluster is NOT an isotope within the matrix, then this is no matrix-cluster
		if (insert) clusters.push_back(&C);
	}
}

bool matrix_clusters_c::is_cluster_in_matrix(const cluster_t& cluster)
{
	for (auto& C : clusters)
		if (*C == cluster)
			return true;
	return false;
}

quantity::intensity_t matrix_clusters_c::intensity_sum() const
{
	quantity::intensity_t intensity;
	for (auto& C : clusters)
	{
// 		if (!intensity.is_set())
// 			intensity = C->intensity;
// 		else
// 		cout << "matrix_clusters_c::intensity_sum(): " << C->to_string() << endl;
		intensity += C->intensity;
	}
	return intensity;
}

quantity::concentration_t matrix_clusters_c::concentration_sum() const
{
	quantity::intensity_t concentration;
	for (auto& C : clusters)
	{
		if (!C->concentration.is_set()) return {};
		if (!concentration.is_set())
			concentration = C->concentration;
		else
			concentration += C->concentration;
	}
	return concentration;
}

const vector<cluster_t*> matrix_clusters_c::clusters_from_ele(element_t ele) const
{
	vector<cluster_t*> clusters_res;
	for (const auto& c : clusters)
	{
		//asuming that there can be just 1 element per matrix_cluster
		if (c->isotopes.front().symbol == ele.symbol)
			clusters_res.push_back(c);
	}
	return clusters_res;
}

const vector<cluster_t> matrix_clusters_c::cluster_names()
{
	if (clusters.size()==0)
		return {};
	vector<cluster_t> c_names(clusters.size());
	int i = 0;
	for (auto& C : clusters)
	{
		c_names.at(i) = C->isotopes;
		i++;
	}
	return c_names;
}

const vector<element_t> matrix_clusters_c::elements() const
{
	set<element_t> eles;
	for (const auto& iso : isotopes())
		eles.insert(iso);
	return {eles.begin(),eles.end()};
}

const std::vector< isotope_t > matrix_clusters_c::isotopes() const
{
	set<isotope_t> isotopes;
	if (clusters.size()==0)
		return {};
	for (auto& C : clusters)
	{
		isotopes.insert(C->isotopes.begin(),C->isotopes.end());
	}
	return {isotopes.begin(),isotopes.end()};
}

cluster_t * matrix_clusters_c::cluster(const isotope_t iso)
{
	for (auto& C: clusters)
	{
        //logger::debug(12,"matrix_clusters_c::cluster("+iso.to_string()+")","cluster=" + C->to_string());
		for (auto& I : C->isotopes)
		{
            //logger::debug(12,"matrix_clusters_c::cluster("+iso.to_string()+")","cluster->isotope="+I.to_string());
			if (I==iso)
				return C;
		}
	}
	return nullptr;
}

void matrix_clusters_c::set_natural_abundances()
{
	for (auto& C : clusters)
	{
		for (auto& I : C->isotopes)
		{
			I.set_natural_abundance_from_PSE();
		}
	}
}

/**************************************************************/
/******************  cluster_t::RSF_t  ************************/
/**************************************************************/


cluster_t::RSF_t::RSF_t(const quantity::quantity_t& q, const vector<cluster_t>& reference_clusters_s) : 
            quantity_t("RSF",q.data(),q.unit(),q.dimension()), reference_clusters_p(RSF_t(reference_clusters_s).reference_clusters_p),
            logger(global_logger,__FILE__,"cluster_t::RSF_t")
{
}

cluster_t::RSF_t::RSF_t(const vector<cluster_t>& reference_clusters_s) : logger(global_logger,__FILE__,"cluster_t::RSF_t")
{
	reference_clusters_p.reserve(reference_clusters_s.size());
	for (const auto& C : reference_clusters_s)
		reference_clusters_p.push_back(cluster_t{C.isotopes});
}

const vector<cluster_t>& cluster_t::RSF_t::reference_clusters() const
{
	return reference_clusters_p;
}
