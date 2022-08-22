/*
	Copyright (C) 2021 Florian Bärwolf
	floribaer@gmx.de

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


#ifndef CLUSTER_T_HPP
#define CLUSTER_T_HPP

#include <string>

#include <vector>
#include "log.hpp"
#include "quantity.hpp"
#include "regex"
#include <map>
// #include "filecontents.hpp"
#include "element.hpp"
// #include "sample.hpp"  // geht nicht wegen sample_t->files_ -> cluster_t->sample_t

// #include "crater.hpp"
//#include <mgl2/fltk.h>
// #include "measurement.hpp"
#include "histogram.hpp"

using namespace std;



class cluster_t
{
private:
    class_logger_t logger;
public:
	class RSF_t : public quantity::quantity_t
	{
	private:
        class_logger_t logger;
		vector<cluster_t> reference_clusters_p;
	public:
		///when no reference_cluster is set, then the reference_intensity from measurement::sims_t will be used
		RSF_t(const vector<cluster_t>& reference_clusters_s={});
		RSF_t(const quantity::quantity_t& q, const vector<cluster_t>& reference_clusters_s={});
		const vector<cluster_t>& reference_clusters() const;
	};
	class implant_parameters_t
	{
	public:
		quantity::intensity_t max_intensity;
		quantity::concentration_t max_concentration;
		quantity::intensity_t min_intensity;
		quantity::concentration_t min_concentration;
		quantity::sputter_depth_t sputter_depth_at_min;
		quantity::sputter_depth_t sputter_depth_at_max;
		quantity::sputter_time_t sputter_time_at_min;
		quantity::sputter_time_t sputter_time_at_max;
		quantity::concentration_t background_concentration;
		quantity::intensity_t background_intensity;
		quantity::dose_t dose;
	};
	friend class crater_t;
public:
//	int Draw(mglGraph * gr) override;
	/// 28Si2 Ge2
	cluster_t(string clustername, 
			  quantity::sputter_time_t sputter_time={},
			  quantity::intensity_t intensity={},
			  quantity::depth_t sputter_depth={},
			  quantity::concentration_t concentration={});
	cluster_t(set<isotope_t> isotopes, 
			  quantity::sputter_time_t sputter_time={},
			  quantity::intensity_t intensity={},
			  quantity::depth_t sputter_depth={},
			  quantity::concentration_t concentration={});
	cluster_t();
	cluster_t(const vector<isotope_t>& isotopes_s);
	cluster_t(const isotope_t isotope_s);
	///calculated parameters
	implant_parameters_t implant_parameters;
	///e.g. 11B for cluster {28Si 11B}
	isotope_t corresponding_isotope(const vector<isotope_t> reference_isotopes) const;
	quantity::quantity_t mass();
	quantity::abundance_t abundance();
	string name() const;
	string to_string(const string del=" ") const;
	const bool is_set() const;
	///lower treshold means less sensitive to background detection
	quantity::intensity_t intensity_background(float treshold=0.75) const; // from histogram
	///lower treshold means less sensitive to background detection
	quantity::concentration_t concentration_background(float treshold=0.75) const; // from histogram
	
	quantity::concentration_t concentration;
	quantity::intensity_t intensity;
	quantity::sputter_time_t sputter_time;
	quantity::depth_t sputter_depth;
	quantity::SF_t SF;
	RSF_t RSF;
// 	quantity::SF_t RSF;
	///cahnges resolution of all
	cluster_t interpolate(const quantity::quantity_t& new_Q, const quantity::quantity_t& old_Q) const;
	cluster_t filter_impulse(int window_size=0, float factor=5);

	vector<isotope_t> isotopes;
	static vector<isotope_t> parse_clustername(const string clustername);
	///checks isotopes and their substance_amount, e.g. 11B 28Si != 11B 28Si2
	bool operator==(const cluster_t& obj) const;
	bool operator<(const cluster_t& obj) const;
	bool operator!=(const cluster_t& obj) const;
	
// 	quantity::map_t concentration_vs_sputter_depth() const;
// 	quantity::map_t intensity_vs_sputter_depth() const;
// 	quantity::map_t concentration_vs_sputter_time() const;
// 	quantity::map_t intensity_vs_sputter_time() const;
};

// class implanted_cluster_t : cluster_t
// {
// public:
// 	quantity::intensity_t minimum_intensity;
// 	quantity::concentration_t minimum_concentration;
// 	quantity::intensity_t maximum_intensity;
// 	quantity::concentration_t maximum_concentration;
// 	quantity::sputter_time_t ST_at_max;
// 	quantity::sputter_depth_t SD_at_max;
// };

class matrix_clusters_c
{
private:
    class_logger_t logger;
public:
	matrix_clusters_c(vector<cluster_t>& clusters, const vector<isotope_t> matrix_isotopes);
	///only clusters with exactly ONE element can be a matrix_cluster, e.g. 30Si2 28Si; but NOT 28Si70Ge
	matrix_clusters_c(vector<cluster_t>& clusters);
	matrix_clusters_c();
	///E.g. for 28Si 30Si 70Ge  matrix_clusters Crels={[28Si]/[30Si],[30Si]/[28Si],[28Si]/[70Ge],[70Ge]/[28Si],[30Si]/[70Ge],[70Ge]/[30Si]}
// 	vector<Crel_t> Crels();
	///E.g. for 28Si 30Si 70Ge  matrix_clusters Irels={(28Si)/(30Si),(30Si)/(28Si),(28Si)/(70Ge),(70Ge)/(28Si),(30Si)/(70Ge),(70Ge)/(30Si)}
// 	vector<Crel_t> Irels();
	///E.g. for 28Si 30Si 70Ge  matrix_clusters Irels={(28Si)/(30Si),(30Si)/(28Si),(28Si)/(70Ge),(70Ge)/(28Si),(30Si)/(70Ge),(70Ge)/(30Si)}
	///and their corresponding Crels
// 	vector<Crel_to_Irel_t> Crels_to_Irels();
	void set_natural_abundances();
	cluster_t* cluster(const isotope_t iso);
	vector<cluster_t*> clusters;
	const vector<cluster_t*> clusters_from_ele(element_t ele) const;
	const vector<cluster_t> cluster_names();
	const vector<isotope_t> isotopes() const;
	const vector<element_t> elements() const;
	quantity::intensity_t intensity_sum() const;
	quantity::concentration_t concentration_sum() const;
	string to_string(const string del = "") const;
	bool is_cluster_in_matrix(const cluster_t& cluster);
	
	
};

extern Logger global_logger;

#endif // CLUSTER_T_HPP
