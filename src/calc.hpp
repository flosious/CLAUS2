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
		class cluster_relations_t
		{
		private:
		public:
			cluster_relations_t(const measurements_::sims_t& measurement, const cluster_t& zaehler, const cluster_t& nenner);
			const measurements_::sims_t& measurement;
			const cluster_t* zaehler;
			const cluster_t* nenner;
			///checks the existence of the clusters in the measurement
			bool clusters_exist() const;
		};
		class Crel_t : public cluster_relations_t
		{
		private:
// 			static const quantity::substance_amount_t rel_zero;
		public:
			Crel_t(const measurements_::sims_t& measurement, const cluster_t& zaehler, const cluster_t& nenner);
			///from measurement->matrix->sample; always set
			const quantity::substance_amount_t substance_amount(const isotope_t& iso) const;
			///from measurement->matrix->sample; always set 
			const quantity::substance_amount_t substance_amount(const cluster_t& cluster) const;
			///ratios of the elemental concentrations
			const quantity::concentration_t from_sample_matrix_elements() const;
			///ratios of the isotopical concentrations
			const quantity::concentration_t from_sample_matrix_isotopes() const;
		};
		class Irel_t : public cluster_relations_t
		{
		private:
			bool intensities_are_set_in_clusters() const;
		public:
			Irel_t(const measurements_::sims_t& measurement, const cluster_t& zaehler, const cluster_t& nenner);
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
		};
		///all measurements, which should be used for these calculations
		vector<measurements_::sims_t> measurements;
		///the template for the different fit routines; all virtual functions must be overridden
		class Crel_to_Irel_data_fit_routine_template
		{
		protected:
			///true if clusters exists and if they exist in the measurement is ok
		public:
// 			Crel_to_Irel_data_fit_routine_template(const cluster_t& zaehler, const cluster_t& nenner, const pair<quantity::concentration_t,quantity::intensity_t> Crel_to_Irel_data);
			Crel_to_Irel_data_fit_routine_template(const cluster_t& zaehler, const cluster_t& nenner, const quantity::map_t& Crel_to_Irel_map);
			virtual ~Crel_to_Irel_data_fit_routine_template();
			string to_string() const;
			///just the isotopes are saved without data
			const cluster_t zaehler;
			///just the isotopes are saved without data
			const cluster_t nenner;
// 			const pair<quantity::concentration_t,quantity::intensity_t> Crel_to_Irel_data;
			const quantity::map_t Crel_to_Irel_map;
			virtual const quantity::concentration_t Crel(const quantity::intensity_t Irel) const;
			virtual const quantity::concentration_t Crel(const measurements_::sims_t& measurement) const;
			virtual void plot_to_screen(double sleep_sec=1) const;
		}; // Crel_to_Irel_data_fit_routine_template
	public:
		///the polynomial fit routine
		class Crel_to_Irel_data_polynomial_fit_t : public Crel_to_Irel_data_fit_routine_template
		{
		public:
			Crel_to_Irel_data_polynomial_fit_t(const cluster_t& zaehler, const cluster_t& nenner, const quantity::map_t& Crel_to_Irel_map, fit_functions::polynom_t polynom);
			const fit_functions::polynom_t polynom;
			const quantity::concentration_t Crel(const quantity::intensity_t Irel) const final;
			const quantity::concentration_t Crel(const measurements_::sims_t& measurement) const final;
			void plot_to_screen(double sleep_sec=1) const final;
		}; // Crel_to_Irel_data_polynomial_fit_t
		///selection of the available fit routines
		class Crel_to_Irel_data_fits_t
		{
		public:
			Crel_to_Irel_data_fits_t(const quantity::map_t& Crel_to_Irel_map,const cluster_t& zaehler, const cluster_t& nenner);
			const quantity::map_t Crel_to_Irel_map;
			const cluster_t& zaehler;
			const cluster_t& nenner;
			const Crel_to_Irel_data_polynomial_fit_t polynom(const vector<unsigned int> polynom_rank, const vector<double> polynom_start_parameters) const;
		}; // Crel_to_Irel_data_fits_t
        ///collection of the relative concentrations and intensities of 2 clusters (zeahler & nenner) for all measurements
        class Crel_to_Irel_data_collector_t
		{
		private:
			///this is a helper class to have a nice interface
			class collect_Irels_t
			{
			private:
				static bool add_Crel_Irel_to_pair(pair<quantity::concentration_t,quantity::intensity_t>& Crel_to_Irel_pair, 
												  const quantity::concentration_t& Crel, const quantity::intensity_t& Irel);
			public:
				collect_Irels_t(const std::map< const quantity::concentration_t, const calc_t::sims_t::Irel_t >& Crel_data_to_Irels, const cluster_t& zaehler, const cluster_t& nenner);
				const cluster_t& zaehler;
				const cluster_t& nenner;
				const map<const quantity::concentration_t, const calc_t::sims_t::Irel_t> Crel_data_to_Irels;
				const Crel_to_Irel_data_fits_t Irel_from_median() const;
				const Crel_to_Irel_data_fits_t Irel_from_mean() const;
				///trim=0.25 discards the first and last 25% of SORTED data; checking all measurements
				const Crel_to_Irel_data_fits_t Irel_from_trimmed_mean(float trim=0.25) const;
				///cut=0.25 discards the first and last 25% of data; checking all measurements
				const Crel_to_Irel_data_fits_t Irel_from_cut_mean(float cut=0.25) const;
				///cut=0.25 discards the first and last 25% of data; checking all measurements
				const Crel_to_Irel_data_fits_t Irel_from_cut_median(float cut=0.25) const;
			}; //collect_Irels_t
		protected:
			const vector<measurements_::sims_t>& measurements;
		public:
			Crel_to_Irel_data_collector_t(const cluster_t& zaehler, const cluster_t& nenner, const vector<measurements_::sims_t>& measurements);
			const cluster_t& zaehler;
			const cluster_t& nenner;
			const collect_Irels_t elemental_Crel();
			const collect_Irels_t isotopical_Crel();
		}; //Crel_to_Irel_data_collector_t
	public:
		sims_t(const vector<measurements_::sims_t>& measurements);
		/// calculations following the principal: [E_1] / [E_2] = matrix_RSF * (E_1)/(E_2)
		class matrix_t
		{
		public:
			class RSF_t : public quantity::quantity_t
			{
			public:
				const cluster_t zaehler;
				const cluster_t nenner;
				string to_string() const;
				///matrix RSFs are always relative!
				RSF_t(const cluster_t zaehler,const cluster_t nenner, vector<double> data_s={},unit_t unit_s=units::SI::one,dimension_t dim_s=quantity::dimensions::SI::relative);
				///matrix RSFs are always relative!
				RSF_t(const cluster_t zaehler,const cluster_t nenner, const quantity_t& quantity_s);
				bool operator==(const RSF_t& obj) const;
				bool operator!=(const RSF_t& obj) const;
				bool operator<(const RSF_t& obj) const;
				bool operator>(const RSF_t& obj) const;
			}; // RSF_t
			const vector<Crel_to_Irel_data_polynomial_fit_t> elemental_Crel_to_median_Irel_linear_fitted() const;
			const set<cluster_t> matrix_clusters_p() const;
		private:
			/// sometimes (most of the times) RSFs are not linear and singular
			/// this class checks if a solution exists from the given parameters and calculates missing RSFs
			/// it internally creates a logical table of all RSFs and compares it to all given matrix_clusters
			class RSFs_t
			{
			public:
				RSFs_t(const set<cluster_t>& matrix_clusters, const vector<Crel_to_Irel_data_polynomial_fit_t>& Crel_to_Irel_lin_fits={}, const vector<RSF_t>& RSFs={});
				RSFs_t(const set<cluster_t>& matrix_clusters, const vector<Crel_to_Irel_data_polynomial_fit_t>& Crel_to_Irel_lin_fits, const set<RSF_t>& RSFs);
				static RSF_t RSF(const Crel_to_Irel_data_polynomial_fit_t& Crel_to_Irel_lin_fit); 
				///Prefers manually set RSF over fitted ones from Crel_to_Irel_lin_fits
				RSF_t RSF(const cluster_t zaehler, const cluster_t nenner) const;
				const set<cluster_t> matrix_clusters;
				const vector<Crel_to_Irel_data_polynomial_fit_t> Crel_to_Irel_lin_fits;
				const set<RSF_t> RSFs;
				const set<RSF_t> RSFs_from_fits() const;
				///calculates number of RSFs from number of clusters
				unsigned int RSFs_count_from_clusters() const;
				///discards RSFs which have no data, etc.
				RSFs_t valid_RSFs_and_fits() const;
				///removes Crel_to_Irel_fits, which are already set by (manually given) RSFs
				RSFs_t remove_Crel_to_Irel_fits_by_RSFs() const;
				
				///RSF(Z,N) = 1 / RSF(N,Z)
				RSF_t RSF_from_inverse(const cluster_t& zaehler, const cluster_t& nenner) const;
				///asumes sum of all elements / isotopes is 100at%; then tries to calculate a virtual cluster with an substance_amount of (100at% - known elements/isotopes)
				RSF_t RSF_from_virtual_cluster(const cluster_t& zaehler, const cluster_t& nenner) const;
				RSFs_t remove_RSF_with_worse_chisqr_fit(float relativ_chisq=0.5) const;
				///uses add_missing_RSFs_from_inverse() and then add_missing_RSFs_from_virtual_cluster()
				RSFs_t add_missing_RSFs() const;
				RSFs_t add_missing_RSFs_from_inverse() const;
				///only applicable if there is exactly 1 unknown RSF(Z,N) = RSF(N,Z) left
				RSFs_t add_missing_RSFs_from_virtual_cluster() const;
				///adds or replaces one RSF in the RSFs table
				RSFs_t add_or_replace_RSF(const RSF_t& RSF) const;
				///adds or replaces multiple RSFs in the RSFs table
				RSFs_t add_or_replace_RSFs(const vector<RSF_t>& RSFs) const;
				///improves RSFs by increasing linearity of Crel_to_Irel_poly_fits, by removing outliners outside of a relative_chisqr;
				RSFs_t improve_RSFs(float relative_chisqr=0.05) const;
				///makes RSF(zaehler,nenner) == RSF(nenner,zaehler) by using their mean
				RSFs_t symmetrical_RSFs() const;
			}; // RSFs_t
		public:
			matrix_t(const vector<isotope_t>& matrix_isotopes, const vector<measurements_::sims_t>& measurements);
			const vector<isotope_t> matrix_isotopes;
			const vector<measurements_::sims_t>& measurements;
			const set<cluster_t> matrix_clusters;
			///successfull fitted RSFs for all clusters and measurements; aka calibration curves
			const RSFs_t RSFs;
// 			const vector<Crel_to_Irel_data_polynomial_fit_t> Crels_to_Irels;
			///successfull fitted RSF for all clusters and measurements; aka calibration curves
// 			const Crel_to_Irel_data_polynomial_fit_t& Crel_to_Irel_poly_fits(const cluster_t& zaehler, const cluster_t& nenner);
			///returns the concentration (at%) of the cluster within that measurement
			quantity::concentration_t relative_elemental_concentration(const cluster_t& cluster, const measurements_::sims_t& measurement);
			///returns a copy of the measurement with the successfull calculated concentrations (at%) of all clusters within that measurement
			measurements_::sims_t relative_elemental_concentrations(const vector<cluster_t>& clusters, measurements_::sims_t measurement);
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
			
		}; // sputter_rate_t
		class sputter_depth_t
		{
			
		}; // sputter_depth_t
	}; //calc_t::sims_t
}; //calc_t

#endif // CALC_T_HPP
