/*
	Copyright (C) 2022 Florian Bärwolf
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

#ifndef CALC_T_HPP
#define CALC_T_HPP

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
#include "log.hpp"
#include "pse.hpp"

/*
 * contains all calculation methods
 */
class calc_t
{
protected:

public:
	///covering general methods for D- and ToF-SIMS
	class sims_t
	{
	private:
		/// stores a logically constant copy of Z and N
		class cluster_relations_copies_t
		{
		private:
			cluster_t zaehler_p;
			cluster_t nenner_p;
            class_logger_t logger;
		public:
			cluster_relations_copies_t(const cluster_t& Z, const cluster_t& N, const quantity::abundance_t& abundance_ratio={});
			const cluster_t& zaehler() const;
			const cluster_t& nenner() const;
			quantity::quantity_t abundance_ratio;
		};
		/// stores a pointer of Z and N into its measurement clusters
		class cluster_relations_t
		{
		private:
// 			cluster_t* zaehler_p;
// 			cluster_t* nenner_p;
            class_logger_t logger;
		public:

			cluster_relations_t(const measurements_::sims_t& measurement, const cluster_t& zaehler_s, const cluster_t& nenner_s);
// 			cluster_relations_t(cluster_t& zaehler, const cluster_t& nenner);
			const measurements_::sims_t& measurement;
			///ATTENTION can be nullptr
			const cluster_t* zaehler;
			///ATTENTION can be nullptr
			const cluster_t* nenner;
			///checks the existence of the clusters in the measurement
			bool clusters_exist() const;
		}; // cluster_relations_t
		class Crel_t : public cluster_relations_t
		{
		private:
// 			static const quantity::substance_amount_t rel_zero;
            class_logger_t logger;
		public:
			Crel_t(const measurements_::sims_t& measurement, const cluster_t& zaehler, const cluster_t& nenner);
			///from measurement->matrix->sample; always set
			const quantity::substance_amount_t substance_amount(const isotope_t& iso) const;
			///from measurement->matrix->sample; always std::set 
			const quantity::substance_amount_t substance_amount(const cluster_t& cluster) const;
			///ratios of the elemental concentrations
			const quantity::concentration_t from_sample_matrix_elements() const;
			///ratios of the isotopical concentrations
			const quantity::concentration_t from_sample_matrix_isotopes() const;
		}; // Crel_t
		class Irel_t : public cluster_relations_t
		{
		private:
			bool intensities_are_set_in_clusters() const;
            class_logger_t logger;
		public:
			Irel_t(const measurements_::sims_t& measurement, const cluster_t& zaehler, const cluster_t& nenner);
			///point by point
			const quantity::intensity_t from_clusters_pbp() const;
			///ratios of the median of clusters intensities
			const quantity::intensity_t from_clusters_median_ratios() const;
			///ratios of the mean of clusters intensities
			const quantity::intensity_t from_clusters_mean_ratios() const;
			///ratios of the last factor*data.size() mean of clusters intensities; skip first 1-factor data
			const quantity::intensity_t from_clusters_cut_mean_ratios(float cut=0.25) const;
			///ratios of the last factor*data.size() median of clusters intensities; skip first 1-factor data
			const quantity::intensity_t from_clusters_cut_median_ratios(float cut=0.25) const;
			///truncated median: that means cut of statistically sorted data
			const quantity::intensity_t from_clusters_trimmed_mean_ratios(float trim = 0.25) const;
		}; //Irel_t
		///all measurements, which should be used for these calculations
		std::vector<measurements_::sims_t> measurements;
		///the template for the different fit routines; all virtual functions must be overridden
		class Crel_to_Irel_data_fit_routine_template : public cluster_relations_copies_t
		{
		private:
            class_logger_t logger;
			///just the isotopes are saved without data
// 			cluster_t zaehler_p;
			///just the isotopes are saved without data
// 			cluster_t nenner_p;
			
		public:
// 			Crel_to_Irel_data_fit_routine_template(const cluster_t& zaehler, const cluster_t& nenner, const std::pair<quantity::concentration_t,quantity::intensity_t> Crel_to_Irel_data);
			Crel_to_Irel_data_fit_routine_template(const cluster_t& zaehler, const cluster_t& nenner, const quantity::map_t& Crel_to_Irel_map);
			virtual ~Crel_to_Irel_data_fit_routine_template();
			quantity::map_t Crel_to_Irel_map;
			std::string to_string() const;
			///just the isotopes are saved without data
// 			const cluster_t& zaehler() const;
			///just the isotopes are saved without data
// 			const cluster_t& nenner() const;
// 			const std::pair<quantity::concentration_t,quantity::intensity_t> Crel_to_Irel_data;
// 			const quantity::map_t& Crel_to_Irel_map() const;
			virtual const quantity::concentration_t Crel(const quantity::intensity_t Irel) const;
			virtual const quantity::concentration_t Crel(const measurements_::sims_t& measurement) const;
//			virtual void plot_to_screen(double sleep_sec=1) const;
			bool operator<(const Crel_to_Irel_data_fit_routine_template& obj) const;
		}; // Crel_to_Irel_data_fit_routine_template
	public:
		///the polynomial fit routine
		class Crel_to_Irel_data_polynomial_fit_t : public Crel_to_Irel_data_fit_routine_template
		{
		private:
			fit_functions::polynom_t polynom_p;
            class_logger_t logger;
		public:
			/// polynom must be from Crel_to_Irel_map !
			Crel_to_Irel_data_polynomial_fit_t(const cluster_t& zaehler, const cluster_t& nenner, const quantity::map_t& Crel_to_Irel_map, fit_functions::polynom_t polynom);
			const fit_functions::polynom_t& polynom() const;
			const quantity::concentration_t Crel(const quantity::intensity_t Irel) const final;
			const quantity::concentration_t Crel(const measurements_::sims_t& measurement) const final;
//			void plot_to_screen(double sleep_sec=1) const final;
		}; // Crel_to_Irel_data_polynomial_fit_t
		
		///selection of the available fit routines
		class Crel_to_Irel_data_fits_t : public cluster_relations_copies_t
		{
		private:
			///just the isotopes are saved without data
// 			cluster_t zaehler_p;
			///just the isotopes are saved without data
// 			cluster_t nenner_p;
            class_logger_t logger;
		public:
			Crel_to_Irel_data_fits_t(const quantity::map_t& Crel_to_Irel_map,const cluster_t& zaehler, const cluster_t& nenner);
			quantity::map_t Crel_to_Irel_map;
// 			const quantity::map_t& Crel_to_Irel_map() const;
			///just the isotopes are saved without data
// 			const cluster_t& zaehler() const;
			///just the isotopes are saved without data
// 			const cluster_t& nenner() const;
			const Crel_to_Irel_data_polynomial_fit_t polynom(const std::vector<unsigned int> polynom_rank, const std::vector<double> polynom_start_parameters) const;
		}; // Crel_to_Irel_data_fits_t
		
		///collection of the relative concentrations and intensities of 2 clusters (zeahler & nenner) for all measurements
        class Crel_to_Irel_data_collector_t
		{
		private:
			///this is a helper class to have a nice interface
            class_logger_t logger;
			class collect_Irels_t
			{
			private:
// 				cluster_t zaehler;
// 				cluster_t nenner;
				static bool add_Crel_Irel_to_pair(std::pair<quantity::concentration_t,quantity::intensity_t>& Crel_to_Irel_pair, 
												  const quantity::concentration_t& Crel, const quantity::intensity_t& Irel);
			public:
				collect_Irels_t(const std::map< const quantity::concentration_t, const calc_t::sims_t::Irel_t >& Crel_data_to_Irels, const cluster_t& zaehler, const cluster_t& nenner);
				const cluster_t& zaehler;
				const cluster_t& nenner;
				const std::map<const quantity::concentration_t, const calc_t::sims_t::Irel_t> Crel_data_to_Irels;
				const Crel_to_Irel_data_fits_t Irel_from_median() const;
				const Crel_to_Irel_data_fits_t Irel_from_mean() const;
				///trim=0.25 discards the first and last 25% of SORTED data; checking all measurements
				const Crel_to_Irel_data_fits_t Irel_from_trimmed_mean(float trim=0.25) const;
				///cut=0.25 discards the first and last 25% of data; checking all measurements
				const Crel_to_Irel_data_fits_t Irel_from_cut_mean(float cut=0.25) const;
				///cut=0.25 discards the first and last 25% of data; checking all measurements
				const Crel_to_Irel_data_fits_t Irel_from_cut_median(float cut=0.25) const;
			}; //collect_Irels_t
// 			cluster_t zaehler;
// 			cluster_t nenner;
		protected:
			const std::vector<measurements_::sims_t>& measurements;
		public:
			Crel_to_Irel_data_collector_t(const cluster_t& zaehler, const cluster_t& nenner, const std::vector<measurements_::sims_t>& measurements);
			const cluster_t& zaehler;
			const cluster_t& nenner;
			const collect_Irels_t elemental_Crel();
			const collect_Irels_t isotopical_Crel();
		}; //Crel_to_Irel_data_collector_t
		
		
	public:
		sims_t(const std::vector<measurements_::sims_t>& measurements);
		/// calculations following the principal: [E_1] / [E_2] = matrix_RSF * (E_1)/(E_2)
		class matrix_t
		{
        private:
            class_logger_t logger;
		public:
			///matrix_clusters similar to matrix_clusters_c from cluster.hpp, but more restrictive and error save
			class clusters_t
			{
			private:
                class_logger_t logger;
				std::set<cluster_t> clusters_p;
			public:
				///
				clusters_t(const std::set<cluster_t>& matrix_clusters);
				clusters_t(const std::vector<cluster_t>& matrix_clusters);
				///checks if there are clusters with different isotopes, like "30Si1 29Si2";
				///if true, calculation of concentration of unknown isotopes is not possible
				bool has_mixed_isotopes_in_clusters() const;
				///checks if there are clusters with different elements, like "30Si1 70Ge"; This is not supported
				bool has_mixed_elements_in_clusters() const;
				const std::set<cluster_t>& clusters() const;
				const std::vector<isotope_t> isotopes() const;
				///in general there can be multiple clusters per element, e.g. 30Si2 and 28Si2 for Si
// 				const cluster_t cluster(const element_t& ele) const;
				const cluster_t cluster(const isotope_t& iso) const;
				const std::vector<cluster_t> clusters(const element_t& ele) const;
				const std::vector<cluster_t> clusters(const isotope_t& iso) const;
				///checks if the cluster is a matrix cluster (is in clusters_p)
				bool contains(const cluster_t& cluster) const;
				///all elements in the matrix
				std::vector<element_t> elements() const;
			}; // clusters_t
			
			///physical quantity mRSF (matrix RSF)
			class RSF_t : public quantity::quantity_t, public cluster_relations_copies_t
			{
            private:
                class_logger_t logger;
			public:
				std::string to_string() const;
// 				RSF_t(const cluster_relations_copies_t& cluster_relation,const std::vector<double>& data);
				RSF_t(const cluster_t zaehler,const cluster_t nenner, double data_s, const quantity::abundance_t& abundance_ratio={});
				///matrix RSFs are always relative!
				RSF_t(const cluster_t zaehler,const cluster_t nenner, std::vector<double> data_s={}, const quantity::abundance_t& abundance_ratio={},unit_t unit_s=units::SI::one,dimension_t dim_s=quantity::dimensions::SI::relative);
				///matrix RSFs are always relative!
				RSF_t(const cluster_t zaehler,const cluster_t nenner, const quantity_t& quantity_s , const quantity::abundance_t& abundance_ratio={});
				bool operator==(const RSF_t& obj) const;
				bool operator!=(const RSF_t& obj) const;
// 				bool operator<(const RSF_t& obj) const;
// 				bool operator>(const RSF_t& obj) const;
				RSF_t invert() const;
			}; // RSF_t
			
		private:
			/// sometimes (most of the times) RSFs are not linear and singular
			/// this class checks if a solution exists from the given parameters and calculates missing RSFs
			/// it internally creates a logical table of all RSFs and compares it to all given matrix_clusters
			class RSFs_t
			{
            private:
                class_logger_t logger;
			protected:
				static bool sort_rel_chisqr_to_fits_by_chisqr(const std::pair<double,Crel_to_Irel_data_polynomial_fit_t>& P1, const std::pair<double,Crel_to_Irel_data_polynomial_fit_t>& P2);
				clusters_t matrix_clusters_p;
				std::vector<RSF_t> RSFs_manually_set_p;
				std::vector<Crel_to_Irel_data_polynomial_fit_t> Crel_to_Irel_lin_fits_p;
			public:
				///TODO adding 0 for n=0 lin fit
				RSFs_t(const clusters_t& matrix_clusters, const std::vector<Crel_to_Irel_data_polynomial_fit_t>& Crel_to_Irel_lin_fits={}, const std::vector<RSF_t>& RSFs={});
				RSFs_t(const clusters_t& matrix_clusters, const std::vector<Crel_to_Irel_data_polynomial_fit_t>& Crel_to_Irel_lin_fits, const std::set<RSF_t>& RSFs);
				static RSF_t RSF(const Crel_to_Irel_data_polynomial_fit_t& Crel_to_Irel_lin_fit); 
				///Prefers manually std::set RSF over fitted ones from Crel_to_Irel_lin_fits
				RSF_t RSF(const cluster_t& zaehler, const cluster_t& nenner) const;
				const clusters_t& matrix_clusters() const;
				///all RSFs where nenner is the nenner
				const std::vector<RSF_t> RSFs(const cluster_t& nenner) const;
// 				const clusters_t& matrix_clusters() const;
				const std::vector<Crel_to_Irel_data_polynomial_fit_t>& Crel_to_Irel_lin_fits() const;
				const std::vector<RSF_t>& RSFs_manually_set() const;
				const std::vector<RSF_t> RSFs_from_fits() const;
				///prefers manually std::set RSFs over RSFs_from_fits
				const std::vector<RSF_t> all_available_RSFs() const;
				const std::vector<RSF_t> all_possible_RSFs() const;	///TODO ELMENTAL + ISOTOPICAL
				const std::vector<RSF_t> unknown_RSFs() const;	///TODO ELMENTAL + ISOTOPICAL
				///calculates number of RSFs from number of clusters (#clusters^2 - #clusters)
				unsigned int RSFs_count_from_clusters() const; ///TODO ELMENTAL + ISOTOPICAL
				///discards RSFs which have no data, etc.
				RSFs_t valid_RSFs_and_fits() const;
				///removes Crel_to_Irel_fits, which are already std::set by (manually given) RSFs
				RSFs_t remove_Crel_to_Irel_fits_by_RSFs() const;
				///RSF(Z,N) = 1 / RSF(N,Z)
				RSF_t RSF_from_inverse(const cluster_t& zaehler, const cluster_t& nenner) const;
				///asumes sum of all elements / isotopes is 100at%; then tries to calculate a virtual cluster with an substance_amount of (100at% - known elements/isotopes)
				RSF_t RSF_from_virtual_cluster(const cluster_t& zaehler, const cluster_t& nenner) const;
				///returns empty if worst fit can not be found, that means all fits are perfect or none are there
				const Crel_to_Irel_data_polynomial_fit_t& RSF_with_worst_gof() const;
				///returns the n worst fits
				std::set<RSF_t> RSFs_with_worst_fit(int n=1) const;
				///removes particulary Crel_to_Irel_lin_fits with relative_chisqr > 0.1
				RSFs_t remove_RSFs_above_relative_chisqr_treshold(float relativ_chisq=0.1) const;
				///just returns the polynom fits with gof() >= gof_treshold
				RSFs_t remove_RSFs_below_gof_treshold(float gof_treshold=0.5) const;
				///removes the manual setted and fitted RSFs
				RSFs_t remove_RSFs(const std::set<RSF_t>& rsfs_s) const;
				RSFs_t remove_RSFs_with_worst_fit(int n=1) const;
				///TODO:removes all data belonging to a certain measurement
				RSFs_t remove_measurement(const measurements_::sims_t& measurement) const;
				///uses add_missing_RSFs_from_inverse() and then add_missing_RSFs_from_virtual_cluster()
				RSFs_t add_missing_RSFs() const; ///TODO ELMENTAL + ISOTOPICAL
				RSFs_t add_missing_RSFs_from_inverse() const;  ///TODO ELMENTAL + ISOTOPICAL ist hier glaube ich nicht notwendig
				///only applicable if there is exactly 1 unknown RSF(Z,N) = RSF(N,Z) left
				RSFs_t add_missing_RSFs_from_virtual_cluster() const; ///TODO ELMENTAL + ISOTOPICAL
				///adds or replaces one RSF in the RSFs table
				RSFs_t add_or_replace_RSF(const RSF_t& RSF) const;
				///adds or replaces multiple RSFs in the RSFs table
				RSFs_t add_or_replace_RSFs(const std::vector<RSF_t>& RSFs_s) const;
				///improves RSFs fits by increasing linearity of Crel_to_Irel_poly_fits, by removing outliners outside of a relative_chisqr;
				RSFs_t improve_fits_by_removing_outliners(float gof_reshold=0.1) const;
				///improves RSFs fits by increasing linearity of Crel_to_Irel_poly_fits, by removing outliners outside of a relative_chisqr;
				RSFs_t improve_fits_by_removing_biggest_Irels(float relative_chisqr=0.05) const;
				///makes RSF(zaehler,nenner) == RSF(nenner,zaehler) by using their mean
				RSFs_t symmetrical_RSFs() const;
				///sets abundance_relations of all RSFs -> cluster_relations_copies_t
				RSFs_t add_natural_abundances() const;
//				void plot_now(double sleep_sec=1) const;
			}; // RSFs_t
		public:
			///calculation of matrix concentrations
			class concentration_c
			{
			private:
                class_logger_t logger;
// 				clusters_t matrix_clusters;
				measurements_::sims_t measurement_p;
				matrix_clusters_c matrix_clusters;
				RSFs_t RSFs_p;
				void set_natural_abundances_in_clusters();
			public:
// 				concentration_c(const std::set<RSF_t>& RSFs);
				concentration_c(const RSFs_t& RSFs, const measurements_::sims_t& measurement);
				///gets clusters from zaehler and nenner from RSFs
// 				const clusters_t matrix_clusters() const;
				///returns all used RSFs
				const RSFs_t& RSFs() const;
				///returns the specific used RSF
				const RSF_t& RSF(const cluster_t& zaehler, const cluster_t& nenner);
				///decides whether to use elemental or isotopical calculation
				bool use_elemental() const;
				
				///returns the measurement
				measurements_::sims_t measurement();
				///concentration of the cluster
				quantity::concentration_t cluster(const cluster_t& cluster);
				///concentration of the clusters corresponding isotope
				quantity::concentration_t isotope(const isotope_t& isotope);
				///concentration of the clusters corresponding element
				quantity::concentration_t element(const element_t& element);
				///calculates clusters concentrations by RSFs in measurement_p
				concentration_c& concentrations_by_RSFs();
				///calculates clusters concentrations by using the total sum of atoms is 100% in measurement_p
				concentration_c& concentrations_by_filling_up_all_concentrations_to_1();
				concentration_c& clear_concentrations_of_mcs();
			};
		private:
			static const clusters_t clusters_from_RSF(const RSF_t& RSF);
			static const clusters_t clusters_from_RSFs(const std::vector<RSF_t>& RSFs);
			static const clusters_t clusters_from_RSFs(const std::set<RSF_t>& RSFs);
			std::vector<isotope_t> matrix_isotopes_p;
			std::vector<measurements_::sims_t> measurements_p;
			clusters_t matrix_clusters_p;
			RSFs_t RSFs_p;
			///used to populate matrix_clusters_p in ctor list
			const std::set<cluster_t> matrix_clusters_from_matrix_isotopes() const;
		protected:
			///dont use
			const std::vector<Crel_to_Irel_data_polynomial_fit_t> elemental_Crel_to_median_Irel_linear_fitted() const;
			///use this, because RSFs are for clusters, and those consist out of isotopes, not elements. So isotopes are the general case
			const std::vector<Crel_to_Irel_data_polynomial_fit_t> isotopical_Crel_to_median_Irel_linear_fitted() const; //isotopical_Crel_to_median_Irel_linear_fitted
		public:
			///create/get matrix RSFs from measurements and given RSFs; uses elemental_Crel_to_median_Irel_linear_fitted 
			matrix_t(const std::vector<isotope_t>& matrix_isotopes, const std::vector<measurements_::sims_t>& measurements, const std::vector<RSF_t>& RSFs={});
			matrix_t(const std::vector<isotope_t>& matrix_isotopes, const std::vector<RSF_t>& RSFs);
			const std::vector<isotope_t>& matrix_isotopes() const;
			const clusters_t& matrix_clusters() const;
			const std::vector<measurements_::sims_t>& measurements() const;
			
			///successfull fitted RSFs for all clusters and measurements; aka calibration curves
			const RSFs_t& RSFs() const;
// 			const std::vector<Crel_to_Irel_data_polynomial_fit_t> Crels_to_Irels;
			///successfull fitted RSF for all clusters and measurements; aka calibration curves
// 			const Crel_to_Irel_data_polynomial_fit_t& Crel_to_Irel_poly_fits(const cluster_t& zaehler, const cluster_t& nenner);
			///returns the concentration (at%) of the cluster within that measurement
			quantity::concentration_t relative_elemental_concentration(const cluster_t& cluster, const measurements_::sims_t& measurement);
			///returns a copy of the measurement with the successfull calculated concentrations (at%) of all clusters within that measurement
			measurements_::sims_t relative_elemental_concentrations(const std::vector<cluster_t>& clusters, measurements_::sims_t measurement);
			///returns a copy of the measurement with the successfull calculated concentrations (at%) of all clusters within that measurement
			measurements_::sims_t relative_elemental_concentrations(measurements_::sims_t measurement);
		}; //calc_t::sims_t::matrix_t
		/// calculations following the principal: [E_1] = matrix_RSF * (E_1)/(E_Reference)
		class dopant_RSF_t
		{
			
		}; // dopant_RSF_t
		class SF_t
		{
			
		}; // SF_t
		class sputter_rate_t
		{
		public:
			sputter_rate_t(const quantity::quantity_t& SR_s);
// 			sputter_rate_t();
			quantity::map_t SRs_vs_cluster_concentration_medians() const;
			quantity::map_t SRs_vs_cluster_concentration_means() const;
			quantity::map_t SRs_vs_sample_matrix_concentrations() const;
		}; // sputter_rate_t
		class sputter_depth_t
		{
			
		}; // sputter_depth_t
	}; //calc_t::sims_t
}; //calc_t



#endif // CALC_T_HPP
