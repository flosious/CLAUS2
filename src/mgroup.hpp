/*
	Copyright (C) 2020-2022 Florian Bärwolf
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

#pragma once

#ifndef MEASUREMENT_GROUP_T_HPP
#define MEASUREMENT_GROUP_T_HPP

#include <map>
#include <unordered_map>
#include <algorithm>
#include <set>
#include <vector>
#include <string>
#include <list>
#include <sstream>
#include "quantity.hpp"
#include "msettings.hpp"
// #include "sample.hpp"
#include "definitions.hpp"
#include "measurement.hpp"
#include "fit_functions.hpp"

using namespace std;


/* FORWARD DECLARATIONS*/
/************************/


class mgroups_
{
public:
	/*
	* there can not be different measurement methods in the same measurement_group.
	* that means: there can only be list<dsims_measurement_t> in dsims_measurement_group_t
	* this is a parent class
	*/
	class mgroup_t 
	{
	// 	friend class config_t;
	protected:
		static bool use_olcdb;
		static bool use_group;
		static bool use_settings;
	protected:
		///measurements belonging to this group
// 		mgroup_t(filenames::filename_t& fn, files_::file_t& f, list<sample_t>& samples_list);
		
	public:
		mgroup_t(measurements_::measurement_t& measurement);
		string to_string(const string del=", ");
		/*const defitions*/
		///"51087" from filename: 51087_SJZ307#A#B_w17_X1Y5_mQ_13kVCs-_g5q.dp_rpc_asc
		const int olcdb;
		///measurement group identifier: "5" from filename: 51087_SJZ307#A#B_w17_X1Y5_mQ_13kVCs-_g5q.dp_rpc_asc
		const string group;
		///"d-sims", "tof-sims", "xps", "profiler", ...
		/*ctors*/

		/*functions*/

		/*static functions*/
		/*operators*/
		bool operator==( const mgroup_t& obj) const;
		bool operator!=( const mgroup_t& obj) const;
	};

	class sims_t: public mgroup_t
	{
	private:
	protected:
		class calc_t
		{
		protected:
			///calculates the ratios of intensities of 2 (matrix)clusters and their corresponding elemental substance_amount
			class Crel_to_Irel_c
			{
			protected:
				Crel_to_Irel_c(const cluster_t& zaehler,const cluster_t& nenner,calc_t& calc);
				calc_t& calc;
				/// concentration with "1" as unit
				static const quantity::concentration_t Crel_template;
			public:
				const cluster_t zaehler;
				const cluster_t nenner;
				///DO NOT USE! supporting points of all known intensities and concentrations ratios for the clusters zaehler and nenner
				const pair<quantity::quantity_t,quantity::quantity_t> known_Crels_from_Clusters_to_Irels() const;
				///supporting points of all known intensities and concentrations ratios for the clusters zaehler and nenner
				const pair<quantity::quantity_t,quantity::quantity_t> known_Crels_from_sample_matrix_to_Irels_truncated_median() const;
				const quantity::intensity_t Irel(const measurements_::sims_t* M) const;
				const quantity::intensity_t Irel_from_truncated_medians(const measurements_::sims_t* M) const;
				const quantity::concentration_t Crel_from_sample(const measurements_::sims_t* M) const;
			}; // Crel_to_Irel_c
			///interpolates [E_i]/[E_j] = POLYNOM( (E_i)/(E_j) ) for 2 clusters (zaehler and nenner)
			class polynom_fit_Crel_to_Irel_c : public Crel_to_Irel_c
			{
			private:
				static vector<double> fit_start_parameters(const vector<unsigned int> rank);
				///the polynomial fitting routine
				const fit_functions::polynom_t polynom_f(const vector<unsigned int> rank) const;
			public:
				polynom_fit_Crel_to_Irel_c(const cluster_t& zaehler,const cluster_t& nenner, calc_t& calc, vector<unsigned int> rank, vector<double> fit_start_parameters);
				const fit_functions::polynom_t polynom;
				///returns Crel using Crel_template from Irel polynomfit; Crel===[E_i]/[E_j];; Irel===(E_i)/(E_j)
				quantity::concentration_t Crel(const measurements_::sims_t* M) const;
				void plot_to_screen(double sleep_sec=0) const;
			};
			///interpolates [E_i]/[E_j] = POLYNOM( (E_i)/(E_j) ) for all given clusters and all combinations
			const vector<polynom_fit_Crel_to_Irel_c> polynom_fit_Crels_to_Irels(vector<cluster_t> cluster_names, vector<unsigned int> rank, vector<double> fit_start_parameters);
			
			sims_t& MG;
		public:
			///interpolates [E_i]/[E_j] = k * (E_i)/(E_j)  for all given clusters and all combinations
			const vector<polynom_fit_Crel_to_Irel_c> proportional_fit_Crels_to_Irels(vector<cluster_t> cluster_names);
			///sputter_rate
			class SR_c
			{
			private:
				sims_t& MG;
				calc_t& calc;
				const vector<measurements_::sims_t*>& measurements;
			public:
				///overwrite with calculation results (even if they are empty); false -> just use value if no is already set
				
				SR_c(calc_t& calc);
				calc_t& from_crater_depths(bool overwrite = false);
			 	calc_t& from_implant_max(bool overwrite = false);
				calc_t& from_ref_fit(bool overwrite = false);
				///sets SR for known matrices
				calc_t& copy_to_same_matrices(bool overwrite = false);
				///uses SR from known matrices like Si and SiGe30 to interpolate to unknown matrices like SiGe24
				calc_t& interpolate_from_known_matrices(vector<unsigned int> rank={1,1,1}, bool overwrite = false);
			};
			///sputter_depth
			class SD_c
			{
			private:
				sims_t& MG;
				calc_t& calc;
				const vector<measurements_::sims_t*>& measurements;
			public:
				///overwrite with calculation results (even if they are empty); false -> just use value if no is already set
				bool overwrite = false;
				SD_c(calc_t& calc);
				calc_t& from_SR();
			};
			///sensitivity_factor
			class SF_c
			{
			private:
				sims_t& MG;
				calc_t& calc;
				const vector<measurements_::sims_t*>& measurements;
				const map<cluster_t*,quantity::intensity_t> RSFs_to_ref_intensities();
			public:
				///overwrite with calculation results (even if they are empty); false -> just use value if no is already set
				SF_c(calc_t& calc);
				calc_t& from_implant_dose(bool overwrite = false);
				calc_t& from_implant_max(bool overwrite = false);
				calc_t& from_ref_fit_(bool overwrite = false);
				calc_t& from_RSF_mean_ref(bool overwrite = false);
				calc_t& from_SF_trimmed_mean_ref(bool overwrite = false);
				calc_t& from_RSF_pbp_ref(bool overwrite = false);
				calc_t& from_RSF_median_ref(bool overwrite = false);
			};
			///relative_sensitivity_factor
			class RSF_c
			{
			private:
				sims_t& MG;
				calc_t& calc;
				const vector<measurements_::sims_t*>& measurements;
				const map<cluster_t*,quantity::intensity_t> SFs_to_ref_intensities();
			public:
				///overwrite with calculation results (even if they are empty); false -> just use value if no is already set
				RSF_c(calc_t& calc);
				calc_t& from_SF_mean_ref(bool overwrite = false);
				calc_t& from_SF_trimmed_mean_ref(bool overwrite = false);
				calc_t& from_SF_pbp_ref(bool overwrite = false);
				calc_t& from_SF_median_ref(bool overwrite = false);
// 				calc_t& polynom_interpolation_from_matrix(bool overwrite = false, const int max_rank=2);
				///sets RSF for known matrices
				calc_t& copy_to_same_matrices(bool overwrite=false);
				///uses SR from known matrices like Si and SiGe30 to interpolate to unknown matrices like SiGe24
				calc_t& interpolate_from_known_matrices(vector<unsigned int> rank={1,1,1}, bool overwrite = false);
			};
			class concentration_c
			{
			private:
				sims_t& MG;
				calc_t& calc;
				const vector<measurements_::sims_t*>& measurements;
			public:
				///overwrite with calculation results (even if they are empty); false -> just use value if no is already set
				concentration_c(calc_t& calc);
// 				calc_t& from_matrix_pbp();
// 				calc_t& from_matrix_mean();
// 				calc_t& from_matrix_trimmed_mean(float trimm=0.25);
				calc_t& from_SF(bool overwrite = false);
			};
			///matrix calibration
			class matrix_c
			{
			private:
				sims_t& MG;
				calc_t& calc;
				const vector<measurements_::sims_t*>& measurements;
				///checks whether all intensities of all matrix_clusters are set
				bool intensities_are_set() const;
			public:
				///calculation of matrix concentrations
				matrix_c(calc_t& calc);
				///rank{0,1,0} -> 0*a0+1*a1*x+0*a2*x*x == 0*a0+1*a1*x =rank{0,1}
				calc_t& interpolate(const vector<unsigned int> polynom_rank = {0,1}, bool overwrite = false);
				calc_t& linear_interpolated_elemental_concentration(bool overwrite = false);
				calc_t& pbp_const_from_db(bool overwrite = false);
				calc_t& mean_const_from_db(bool overwrite = false);
				calc_t& median_const_from_db(bool overwrite = false);
				calc_t& trimmed_mean_const_from_db(bool overwrite = false);
				calc_t& const_from_db(quantity::quantity_t (*operation) (quantity::quantity_t), bool overwrite=false);
			}; // matrix_c 
			~calc_t();
			calc_t(sims_t& MG);
			sims_t& implant_references(bool overwrite=false);
			///calculates SRs and RSFs for references, using the most precise models available and going to less precise, if not sufficient information is available
			sims_t& references(bool overwrite=false);
			
			///calculates SR and SF from implants in one step, this save CPU time for high resolution polynom interpolation to get maxima pos
			calc_t& SR_SF_from_implants_maxima(bool overwrite=false);
			calc_t& normalize_to_ref_intensity(bool overwrite=false);

			const vector<measurements_::sims_t*> measurements;
			
			SR_c SRs;
			SD_c SDs;
			SF_c SFs;
			RSF_c RSFs;
			matrix_c matrices;
			concentration_c concentrations;
		};
		///check if all measurements belong in this group
		void check();
		///sets mat_isos as matrix_isotopes in all samples within all measurements within this group
		bool set_matrix_isotopes_in_unknown_samples();
	public:
		void export_origin_ascii(string path="/tmp/exports/", const string delimiter="\t");
		string to_string_short() const;
		calc_t calc();
		///all different matrices from all samples within this group
		set<sample_t::matrix_t> matrices();
		///all different clusters from all measurements within this group
		set<cluster_t> clusters();
		const map<sample_t::matrix_t,quantity::SF_t> matrix_to_RSF(const cluster_t& cluster);
		
		const map<sample_t::matrix_t,quantity::SR_t> matrix_to_SRs();
// 		const map<sample_t::matrix_t,quantity::intensity_t> matrix_to_intensity_sum();
		///known(set) matrices and their corresponding matrix_clusters; depricated, because there can be multiple corresponding clusters to one matrix isotope
// 		const vector< pair<const sample_t::matrix_t&, const measurements_::sims_t::matrix_clusters_c>> sample_mats_to_mat_clusters();
		sims_t(measurements_::sims_t& measurement);
		virtual vector<measurements_::sims_t*> measurements();
		virtual vector<measurements_::sims_t> measurements_copy();
		virtual const msettings::sims_t* settings() const;
		///listed RSF to coressponding cluster and matrix
		quantity::SF_t RSF(cluster_t cluster, sample_t::matrix_t matrix);
		///all isotopes contained in all matrices in all samples
		const vector<isotope_t> matrix_isotopes();
		///all clusters contained in all matrices in all measurements
		const vector<cluster_t> matrix_clusters();
		///all isotopes corresponding to matrix_clusters
		const vector<isotope_t> isotopes_corresponding_to_matrix_clusters();
		///matrix_cluster of each measurement pointing to its sample matrix isotope
// 		const std::map< cluster_t*, isotope_t > matrix_cluster_to_matrix_iso();
		const std::map< cluster_t*, quantity::substance_amount_t > matrix_cluster_to_matrix_iso_substance_amount();
		///returs pointer to the matching measurement within this group
		measurements_::sims_t* measurement(const measurements_::sims_t& M);
		bool check_cluster_consistency();
		bool check_matrix_cluster_consistency();
	};

	class dsims_t: public sims_t
	{
		friend class processor;
	private:
		vector<measurements_::dsims_t> measurements_p;
		const msettings::dsims_t settings_p;
	public:
		const msettings::sims_t* settings() const override;
		dsims_t(vector<measurements_::dsims_t>& dsims_measurements);
		dsims_t(measurements_::dsims_t& dsims_measurements);
		vector<measurements_::sims_t*> measurements() override;
		vector<measurements_::sims_t> measurements_copy() override;
		string to_string(const string del=", ");
		
		/*normalize to primary current*/
		dsims_t normalize_to_Iprimary();
		
		
		bool operator==(const dsims_t& obj) const;
		bool operator!=(const dsims_t& obj) const;
	};
	
// 	class tofsims_t: public sims_t
// 	{
// 	private:
// 		
// 	public:
// 		string to_string();
// 		set<measurements::tofsims_t*> measurements;
// 		void insert_measurement(measurements::tofsims_t* M_p);
// 		tofsims_t(measurements::tofsims_t* measurement);
// 	};
	
};


#endif // MEASUREMENT_GROUP_T_HPP
