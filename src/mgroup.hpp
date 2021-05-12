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
		///if true, will save all calculated results (more RAM usage, but faster)
		static bool save_results;
	protected:
// 		class pbp_t
// 		{
// 		private:
// 			vector<measurements_::sims_t> measurements_s;
// 		public:
// 			pbp_t(sims_t& MG);
// 			vector<measurements_::sims_t*> measurements();
// 			const map<matrix_t,SR_t> SRs();
// 			const SR_t SR(matrix_t& matrix);
// 			const map<matrix_t,RSF_t> RSFs(cluster_t& cluster);
// 			const RSF_t RSF(cluster_t& cluster, matrix_t& matrix);
// 		};
// 		pbp_t pbp_s;
		
		
		class calc_t
		{
		public:
			class SR_c
			{
			private:
				sims_t& MG_r;
			public:
				///sputter_rate calculation methods
				SR_c(sims_t& MG);
				sims_t from_crater_depth();
				sims_t from_implant_max();
				sims_t polynom_interpolation_from_matrix(const int max_rank=2);
				///tries to find the best calculation method under given boundary conditions
				sims_t best();
			};
			class RSF_c
			{
			private:
				sims_t& MG_r;
			public:
				RSF_c(sims_t& MG);
				sims_t from_dose(); // SF
				sims_t from_implant_max(); // SF
				sims_t polynom_interpolation_from_matrix(const int max_rank=2);
				sims_t from_mean();
				sims_t from_median();
				///tries to find the best calculation method under given boundary conditions
				sims_t best();
			};
			///matrix calibration
			class matrix_c
			{
				private:
				sims_t& MG_r;
			public:
				///calculation of matrix concentrations
				matrix_c(sims_t& MG);
				///rank{0,1,0} -> 0*a0+1*a1*x+0*a2*x*x == 0*a0+1*a1*x =rank{0,1}
				sims_t from_jiangs_protocol(vector<int> rank = {0,1,0});
				sims_t from_point_by_point();
				sims_t from_mean();
				sims_t from_median();
				sims_t best();
			};
		};
		
		///check if all measurements belong in this group
		void check();
	public:
		sims_t(measurements_::sims_t& measurement);
		///looks up for common cluster in all measurements corresponding to the matrices
		vector<cluster_t> reference_clusters();
		virtual vector<measurements_::sims_t*> measurements();
		///listed RSF to coressponding cluster and matrix
		RSF_t RSF(cluster_t cluster, matrix_t matrix);
		///listed RSFs to coressponding cluster and matrix
		map<cluster_t,RSF_t> RSFs(matrix_t matrix);
// 		calc::RSFs_t RSFs();
		
		///point by point calculation
		
		///point by point calculation
// 		pbp_t& pbp();
// 		calc::pbp_t pbp();
		
		// calc.jiang(measurements& belonging to one/this group) ctor
		// calc.jiang.SR.median
		// calc.jiang.concentration(isotope/cluster) --> isotope with set concentration, but for wich measurement?
		// calc.jiang.concentrations() --> vector<isotope> with set concentrations
		// calc.jiang.reference.intensity this is a function of intensities of matrix signals
		// 
		// calc_t calc(&MRs) as references OUTside of MG_t
		// calc_t calc(&Ms) as references in MG_t
		// calc.jiang.measurements[].isotopes[]
		// calc.jiang.measurements[].elements[]
		// calc.jiang.measurements[].clusters[]
		// calc.jiang.RSFs
		// calc.jiang.SRs
		// calc.jiang.measurements[].clusters[].RSF
		// calc.jiang.measurements[].clusters[].SF
		// calc.jiang.measurements[].clusters[].isotopes[].concentration
// 		SR_t SR();
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
