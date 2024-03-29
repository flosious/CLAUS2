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
#include "calc.hpp"
#include "data_collector.hpp"
#include "calc.hpp"


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
    private:
        class_logger_t class_logger;
	// 	friend class config_t;
        std::string name_p="";
        std::string tool_name_p="uknownTool";
	protected:
		static bool use_olcdb;
		static bool use_group;
		static bool use_settings;
        template<typename M_t>
        static bool remove_measurement(std::vector<M_t>& measurements, const M_t* measurement)
        {
            class_logger_t class_logger(__FILE__,"mgroups_::mgroup_t");
            log_f;
            int idx = measurement_index(measurements,measurement);
            logger.debug("idx").value(idx);
            if (idx>=0)
            {
                logger.debug("measurement").value(measurements.at(idx).to_string_short());
                tools::vec::erase(measurements,{static_cast<unsigned int>(idx)});
                return true;
            }
            return false;
        }
        template<typename M_t>
        static int measurement_index(std::vector<M_t>& measurements, const M_t* measurement)
        {
            return tools::vec::get_index_position_by_comparing_pointers(measurements,measurement);
        }
        mgroup_t(measurements_::measurement_t& measurement);
        std::string export_location_p="";
        virtual std::vector<measurements_::measurement_t*> basic_measurements();
	public:
        virtual std::string to_string_short() const;
        ///default (export_location="") will use path of first measurement
        void set_export_location(std::string export_location);
        std::string export_location() const;
		std::string to_string(const std::string del=", ");
		/*const defitions*/
		///"51087" from filename: 51087_SJZ307#A#B_w17_X1Y5_mQ_13kVCs-_g5q.dp_rpc_asc
        int olcdb;
		///measurement group identifier: "5" from filename: 51087_SJZ307#A#B_w17_X1Y5_mQ_13kVCs-_g5q.dp_rpc_asc
        std::string group_id;
        ///given by user, optional
        std::string name() const;
        void set_name(const std::string& new_name);
        std::string tool_name() const;
		/*functions*/
//        virtual int remove_measurements();
		/*static functions*/
		/*operators*/
		bool operator==( const mgroup_t& obj) const;
		bool operator!=( const mgroup_t& obj) const;
	};

	class sims_t: public mgroup_t
	{
	private:
        class_logger_t class_logger;
	protected:
        class references_t
        {
        private:
            class_logger_t class_logger;
            sims_t& MG;
        public:
            references_t(sims_t& MG_s);
            std::vector<measurements_::sims_t*> all() const;
            std::vector<measurements_::sims_t*> implants() const;
        };
		class calc_t
		{
        private:
            class_logger_t class_logger;
		protected:
			///calculates the ratios of intensities of 2 (matrix)clusters and their corresponding elemental substance_amount
			class Crel_to_Irel_c
			{
            private:
                class_logger_t class_logger;
			protected:
				Crel_to_Irel_c(const cluster_t& zaehler,const cluster_t& nenner,calc_t& calc);
				calc_t& calc;
				/// concentration with "1" as unit
				static const quantity::concentration_t Crel_template;
			public:
				const cluster_t zaehler;
				const cluster_t nenner;
				///DO NOT USE! supporting points of all known intensities and concentrations ratios for the clusters zaehler and nenner
				const std::pair<quantity::quantity_t,quantity::quantity_t> known_Crels_from_Clusters_to_Irels() const;
				///supporting points of all known intensities and concentrations ratios for the clusters zaehler and nenner
				const std::pair<quantity::quantity_t,quantity::quantity_t> known_Crels_from_sample_matrix_to_Irels_truncated_median() const;
				const quantity::intensity_t Irel(const measurements_::sims_t* M) const;
				const quantity::intensity_t Irel_from_truncated_medians(const measurements_::sims_t* M) const;
				const quantity::concentration_t Crel_from_sample(const measurements_::sims_t* M) const;
			}; // Crel_to_Irel_c
			
			sims_t& MG;
		public:
			///sputter_rate
			class SR_c
			{
			private:
                class_logger_t class_logger;
				sims_t& MG;
				calc_t& calc;
				const std::vector<measurements_::sims_t*>& measurements;
			public:
				///overwrite with calculation results (even if they are empty); false -> just use value if no is already set
				SR_c(calc_t& calc);
				calc_t& from_crater_depths(bool overwrite = false);
			 	calc_t& from_implant_max(bool overwrite = false);
				calc_t& from_ref_fit(bool overwrite = false);
				///sets SR for known matrices
				calc_t& copy_to_same_matrices(bool overwrite = false);
				///uses SR from known matrices like Si and SiGe30 to interpolate to unknown matrices like SiGe24
				calc_t& interpolate_from_known_sample_matrices(std::vector<unsigned int> rank={1,1}, bool overwrite = false);
                ///NOT TESTED YET normalizes given SR to crater_depth
                calc_t& normalize_to_crater_depth();
            };
			///sputter_depth
			class SD_c
			{
			private:
                class_logger_t class_logger;
				sims_t& MG;
				calc_t& calc;
				const std::vector<measurements_::sims_t*>& measurements;
			public:
				///overwrite with calculation results (even if they are empty); false -> just use value if no is already set
				bool overwrite = false;
				SD_c(calc_t& calc);
				calc_t& from_SR(bool overwrite=false);
			};
			///sensitivity_factor
			class SF_c
			{
			private:
                class_logger_t class_logger;
				sims_t& MG;
				calc_t& calc;
				const std::vector<measurements_::sims_t*>& measurements;
				const std::map<cluster_t*,quantity::intensity_t> RSFs_to_ref_intensities();
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
                /// 0 < percentile < 1
                calc_t& from_RSF_percentile_ref(double percentile_s, bool overwrite = false);
			};
			///relative_sensitivity_factor
			class RSF_c
			{
			private:
                class_logger_t class_logger;
				sims_t& MG;
				calc_t& calc;
				const std::vector<measurements_::sims_t*>& measurements;
				const std::map<cluster_t*,quantity::intensity_t> clusters_to_ref_intensities();
			public:
				///overwrite with calculation results (even if they are empty); false -> just use value if no is already set
				RSF_c(calc_t& calc);
				calc_t& from_SF_mean_ref(bool overwrite = false);
				calc_t& from_SF_trimmed_mean_ref(bool overwrite = false);
                calc_t& from_SF_pbp_ref(bool overwrite = false);
				calc_t& from_SF_median_ref(bool overwrite = false);
                /// 0 < percentile < 1
                calc_t& from_SF_percentile_ref(double percentile_s, bool overwrite = false);
// 				calc_t& polynom_interpolation_from_matrix(bool overwrite = false, const int max_rank=2);
				///sets RSF for known matrices
				calc_t& copy_to_same_matrices(bool overwrite=false);
				///uses RSF from known matrices like Si and SiGe30 to interpolate to unknown matrices like SiGe24; for 1 specific cluster
				calc_t& interpolate_from_known_sample_matrices(const cluster_t& cluster, std::vector<unsigned int> rank={1,1}, bool overwrite = false);
				///uses RSF from known matrices like Si and SiGe30 to interpolate to unknown matrices like SiGe24; for all clusters
				calc_t& interpolate_from_known_sample_matrices(std::vector<unsigned int> rank={1,1}, bool overwrite = false);
			};
			class concentration_c
			{
			private:
                class_logger_t class_logger;
				sims_t& MG;
				calc_t& calc;
				const std::vector<measurements_::sims_t*>& measurements;
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
                class_logger_t class_logger;
				sims_t& MG;
				calc_t& calc;
				const std::vector<measurements_::sims_t*>& measurements;
				///checks whether all intensities of all matrix_clusters are set
				bool intensities_are_set() const;
			public:
				///calculation of matrix concentrations
				matrix_c(calc_t& calc);
				///rank{0,1,0} -> 0*a0+1*a1*x+0*a2*x*x == 0*a0+1*a1*x =rank{0,1}
				calc_t& interpolate(const std::vector<unsigned int> polynom_rank = {0,1}, bool overwrite = false);
				calc_t& linear_interpolated_elemental_concentration(bool overwrite = false);
				calc_t& pbp_const_from_db(bool overwrite = false);
				calc_t& mean_const_from_db(bool overwrite = false);
				calc_t& median_const_from_db(bool overwrite = false);
				///reference_isotopes will std::set mgroups_::sims_t::set_reference_isotopes_in_measurements()
				calc_t& median_const_from_reference_isotopes(bool overwrite = false);
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

            const std::vector<measurements_::sims_t*> measurements;
			
			SR_c SRs;
			SD_c SDs;
			SF_c SFs;
			RSF_c RSFs;
			matrix_c matrices;
			concentration_c concentrations;
            ///fully automatic calculation
            void full_auto();
		};
		///check if all measurements belong in this group
		void check();
		///sets mat_isos as matrix_isotopes in all samples within all measurements within this group
		bool set_matrix_isotopes_in_unknown_samples();
		std::vector<isotope_t> matrix_isotopes_p;

	public:
        std::vector<measurements_::measurement_t*> basic_measurements() override;
        sims_t(measurements_::sims_t& measurement);
        void export_origin_ascii(std::string path="", const std::string delimiter="\t");
        std::string to_string_short() const override;
		calc_t calc();
		///all different matrices from all samples within this group
		std::set<sample_t::matrix_t> matrices();
		///all different clusters from all measurements within this group
		std::set<cluster_t> clusters();
		const std::map<sample_t::matrix_t, cluster_t::RSF_t> matrix_to_RSF(const cluster_t& cluster);
		const std::map<sample_t::matrix_t,quantity::SR_t> matrix_to_SRs();
		const std::vector<const measurements_::sims_t*> measurements_const();
		virtual std::vector<measurements_::sims_t*> measurements();
		virtual std::vector<measurements_::sims_t> measurements_copy();
		virtual const msettings::sims_t* settings() const;
		///listed RSF to coressponding cluster and matrix
		quantity::SF_t RSF(cluster_t cluster, sample_t::matrix_t matrix);
		///all isotopes contained in all matrices in all samples
		const std::vector<isotope_t> matrix_isotopes();
		void set_reference_isotopes_in_measurements();
		///all clusters contained in all matrices in all measurements
		const std::vector<cluster_t> matrix_clusters();
		///all isotopes corresponding to matrix_clusters
		const std::vector<isotope_t> isotopes_corresponding_to_matrix_clusters();
		const std::map< cluster_t*, quantity::substance_amount_t > matrix_cluster_to_matrix_iso_substance_amount();
		///returs pointer to the matching measurement within this group
		measurements_::sims_t* measurement(const measurements_::sims_t& M);
		bool check_cluster_consistency();
		bool check_matrix_cluster_consistency();
		void set_natural_abundances_in_matrix_clusters();
		///from sample matrix
		quantity::table_t matrix_concentrations(const isotope_t& isotope);
		///from sample matrix
		quantity::table_t matrix_concentrations(const element_t& element);
		///from all isotopes of sample matrices in all measurements
		quantity::table_t matrix_concentrations();
		///of all measurements for the specified cluster
		quantity::table_t concentrations(const cluster_t& cluster);
		///of all measurements for the specified cluster
		quantity::table_t intensities(const cluster_t& cluster);
		///of all measurements for the specified cluster
		quantity::table_t SFs(const cluster_t& cluster);
		///of all measurements for the specified cluster
		quantity::table_t RSFs(const cluster_t& cluster);
		///of all clusters and measurements
		quantity::table_t concentrations();
		///of all clusters and measurements
		quantity::table_t intensities();
		///of all matrix clusters and measurements
		quantity::table_t ref_intensities();
		///of all clusters and measurements
		quantity::table_t SFs();
		///of all clusters and measurements
		quantity::table_t RSFs();
		quantity::table_t SRs();
		quantity::table_t measurement_names();
        references_t references();
    }; //sims_t

	class dsims_t: public sims_t
	{
// 		friend class processor;
	private:
        class_logger_t class_logger;
	public:
        std::vector<measurements_::dsims_t> measurements_p;
        msettings::dsims_t settings_p;
        dsims_t(std::vector<measurements_::dsims_t*> dsims_measurements);
        dsims_t(std::vector<measurements_::dsims_t>& dsims_measurements);
		dsims_t(measurements_::dsims_t& dsims_measurements);
        const msettings::sims_t* settings() const final;
		std::vector<measurements_::sims_t*> measurements() final;
		std::vector<measurements_::sims_t> measurements_copy() final;
		std::string to_string(const std::string del=", ");
		
		/*normalize to primary current*/
		dsims_t normalize_to_Iprimary();
        ///true: successfully remove
        bool remove_measurement(const measurements_::dsims_t* measurement);
        int measurement_index(const measurements_::dsims_t* measurement);

		bool operator==(const dsims_t& obj) const;
		bool operator!=(const dsims_t& obj) const;
	};
	
	class tofsims_t: public sims_t
	{
	private:
        class_logger_t class_logger;
	public:
        std::vector<measurements_::tofsims_t> measurements_p;
        msettings::tofsims_t settings_p;
        tofsims_t(std::vector<measurements_::tofsims_t*> tofsims_measurements);
        tofsims_t(std::vector<measurements_::tofsims_t>& tofsims_measurements);
		tofsims_t(measurements_::tofsims_t& tofsims_measurements);
		virtual ~tofsims_t();
		const msettings::sims_t* settings() const final;
		std::vector<measurements_::sims_t*> measurements() final;
		std::vector<measurements_::sims_t> measurements_copy() final;
		std::string to_string(const std::string del=", ");

        ///true: successfully remove
        bool remove_measurement(const measurements_::tofsims_t* measurement);
        int measurement_index(const measurements_::tofsims_t* measurement);

		bool operator==(const tofsims_t& obj) const;
		bool operator!=(const tofsims_t& obj) const;
	};
	
};


#endif // MEASUREMENT_GROUP_T_HPP
