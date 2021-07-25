/*
	Copyright (C) 2020 Florian Bärwolf
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
// 		mgroup_t();
		
// 		bool is_set() const;
// 		mgroup_t();
		string to_string(const string del=", ");
// 		void insert_measurement(measurements::measurement_t* M_p);
		/*const defitions*/
		///"51087" from filename: 51087_SJZ307#A#B_w17_X1Y5_mQ_13kVCs-_g5q.dp_rpc_asc
		const int olcdb;
		/// "17" from filename: 51087_SJZ307#A#B_w17_X1Y5_mQ_13kVCs-_g5q.dp_rpc_asc
	// 	const int wafer;
		///"SJZ307" from filename: 51087_SJZ307#A#B_w17_X1Y5_mQ_13kVCs-_g5q.dp_rpc_asc
	// 	const string lot;
		///"#A#B" from filename: 51087_SJZ307#A#B_w17_X1Y5_mQ_13kVCs-_g5q.dp_rpc_asc
	// 	const string lot_split;
		/// X="1", Y="5" from filename: 51087_SJZ307#A#B_w17_X1Y5_mQ_13kVCs-_g5q.dp_rpc_asc
	// 	const sample_t::chip_t chip;
		///"Q" from filename: 51087_SJZ307#A#B_w17_X1Y5_mQ_13kVCs-_g5q.dp_rpc_asc
	// 	const string monitor;
		///multiple measurements of same sample use this: "q" from filename: 51087_SJZ307#A#B_w17_X1Y5_mQ_13kVCs-_g5q.dp_rpc_asc
	// 	const string repetition;
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
// 		class clusters_populated_with_t
// 		{
// 		public:
// 			//returns clusters where RSF is populated
// 			vector<cluster_t*> RSF();
// 			//returns clusters where RSF is populated
// 			vector<cluster_t*> SF();
// 		};
		class calc_t
		{
		protected:
			sims_t& MG;
			///if true, will save all calculated results (more RAM usage, but faster)
			static const bool save_calc_esults=true;
			
		public:
			///changes overwrite properties for all calc methods
			class jiang_c
			{
				sims_t& MG;
				calc_t& calc;
			public:
				jiang_c(calc_t& calc);
			};
			///sputter_rate
			class SR_c
			{
			private:
				sims_t& MG;
				calc_t& calc;
			public:
				///overwrite with calculation results (even if they are empty); false -> just use value if no is already set
				
				SR_c(calc_t& calc);
				calc_t& from_crater_depths(bool overwrite = false);
			 	calc_t& from_implant_max(bool overwrite = false);
				calc_t& from_ref_fit(bool overwrite = false);
				///sets SR for known matrices
				calc_t& copy_to_same_matrices(bool overwrite = false);
			};
			///sputter_depth
			class SD_c
			{
			private:
				sims_t& MG;
				calc_t& calc;
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
				const map<cluster_t*,intensity_t> RSFs_to_ref_intensities();
			public:
				///overwrite with calculation results (even if they are empty); false -> just use value if no is already set
				SF_c(calc_t& calc);
				calc_t& from_implant_dose(bool overwrite = false);
				calc_t& from_implant_max(bool overwrite = false);
				calc_t& from_ref_fit_(bool overwrite = false);
				calc_t& from_RSF_mean_ref(bool overwrite = false);
				calc_t& from_RSF_trimmed_mean_ref(bool overwrite = false);
				calc_t& from_RSF_pbp_ref(bool overwrite = false);
				calc_t& from_RSF_median_ref(bool overwrite = false);
				
			};
			///relative_sensitivity_factor
			class RSF_c
			{
			private:
				sims_t& MG;
				calc_t& calc;
				const map<cluster_t*,intensity_t> SFs_to_ref_intensities();
			public:
				///overwrite with calculation results (even if they are empty); false -> just use value if no is already set
				RSF_c(calc_t& calc);
				calc_t& from_SF_mean_ref(bool overwrite = false);
				calc_t& from_SF_trimmed_mean_ref(bool overwrite = false);
				calc_t& from_SF_pbp_ref(bool overwrite = false);
				calc_t& from_SF_median_ref(bool overwrite = false);
				calc_t& polynom_interpolation_from_matrix(bool overwrite = false, const int max_rank=2);
				///sets RSF for known matrices
				mgroups_::sims_t::calc_t& copy_to_same_matrices(bool overwrite=false);
			};
			class concentration_c
			{
			private:
				sims_t& MG;
				calc_t& calc;
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
				const std::map< cluster_t*, isotope_t* > matrix_cluster_to_iso();
			public:
				///calculation of matrix concentrations
				matrix_c(calc_t& calc);
				///rank{0,1,0} -> 0*a0+1*a1*x+0*a2*x*x == 0*a0+1*a1*x =rank{0,1}
				calc_t& from_jiangs_protocol(vector<int> rank = {0,1,0});
				///if all references have same matrix, then this function sets all other measurements/samples the same matrix
// 				calc_t& set_single_substrat_matrices();
				calc_t& pbp_const_from_db(bool overwrite = false);
				calc_t& mean_const_from_db(bool overwrite = false);
				calc_t& median_const_from_db(bool overwrite = false);
				calc_t& trimmed_mean_const_from_db(bool overwrite = false);
				calc_t& const_from_db(quantity_t (*operation) (quantity_t), bool overwrite=false);
			};
			calc_t(sims_t& MG);
			sims_t& implant_references(bool overwrite=false);
			sims_t& references(bool overwrite=false);
			
			///calculates SR and SF from implants in one step, this save CPU time for high resolution polynom interpolation to get maxima pos
			calc_t& SR_SF_from_implants_maxima(bool overwrite=false);
			
// 			map<sample_t::matrix_t,SR_t> matrices_to_SR();
// 			map<sample_t::matrix_t,RSF_t> matrices_to_RSF(const cluster_t& cluster);
			
			jiang_c jiang;
			SR_c SRs;
			SD_c SDs;
			SF_c SFs;
			RSF_c RSFs;
			matrix_c matrices;
			concentration_c concentrations;
		};
		///check if all measurements belong in this group
		void check();
// 		void set_matrix_isotopes_in_measurements();
	public:
		calc_t calc();
		///all different matrices from all samples within this group
		set<sample_t::matrix_t> matrices();
		///all different clusters from all measurements within this group
		set<cluster_t> clusters();
		const map<sample_t::matrix_t,RSF_t> matrix_to_RSF(const cluster_t& cluster);
		const map<sample_t::matrix_t,SR_t> matrix_to_SRs();
		const map<sample_t::matrix_t,intensity_t> matrix_to_intensity_sum();
		sims_t(measurements_::sims_t& measurement);
		///intersection of all matrix_clusters of all measurements within this group
// 		measurements_::dsims_t::matrix_clusters_c& common_matrix_clusters();
		virtual vector<measurements_::sims_t*> measurements();
		///listed RSF to coressponding cluster and matrix
		RSF_t RSF(cluster_t cluster, sample_t::matrix_t matrix);
		///all clusters referencing to matrices
		vector<cluster_t> matrix_clusters();
		vector<cluster_t*> matrix_clusters(measurements_::sims_t& M);
		///all isotopes contained in all matrices in all samples
		vector<isotope_t> matrix_isotopes();
		///sets mat_isos as matrix_isotopes in all samples within all measurements within this group
		bool set_matrix_isotopes_in_unknown_samples();
		///returs pointer to the matching measurement within this group
		measurements_::sims_t* measurement(const measurements_::sims_t& M);
	};

	class dsims_t: public sims_t
	{
		friend class processor;
	private:
		vector<measurements_::dsims_t> measurements_p;
	public:
		dsims_t(vector<measurements_::dsims_t>& dsims_measurements);
		dsims_t(measurements_::dsims_t& dsims_measurements);
		vector<measurements_::sims_t*> measurements() override;
		string to_string(const string del=", ");
		
		/*normalize to primary current*/
		dsims_t normalize_to_Iprimary();
		
		const msettings::dsims_t settings;
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
	
// 	template<typename Mt>
// 	static void try_insert_sources_into_target_vec(vector<Mt>& Ms,vector<Mt>& M_target)
// 	{
// 		for (auto& M : Ms)
// 		{
// 			Mt* M_p = tools::vec::find_in_vec(M,M_target);
// 			if (M_p == nullptr) // M  not  found in vec
// 			{
// 				M_target.push_back(M);
// 			}
// 		}
// 	}

	
};


#endif // MEASUREMENT_GROUP_T_HPP
