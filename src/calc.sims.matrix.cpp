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

#include "calc.hpp"

calc_t::sims_t::matrix_t::matrix_t(const vector<isotope_t>& matrix_isotopes, 
								   const vector<measurements_::sims_t>& measurements) : 
								   matrix_isotopes(matrix_isotopes),measurements(measurements),matrix_clusters(matrix_clusters_p()), RSFs(elemental_Crel_to_median_Irel_linear_fitted())
{
// 	for (auto& M:measurements)
// 		for (auto& i : M.sample->matrix().isotopes())
// 			cout << i.substance_amount.to_string_detailed() << endl;
}

const set<cluster_t> calc_t::sims_t::matrix_t::matrix_clusters_p() const
{
	set<cluster_t> matrix_clusters;
	set<cluster_t> clusters;

	for (const auto& M : measurements)
	{
		for (const auto& C : M.clusters)
		{
			clusters.insert(C);
		}
	}

	for (const auto& C : clusters)
	{
		if (C.isotopes.size()!=1) 
			continue;
		if (std::find(matrix_isotopes.begin(),matrix_isotopes.end(),C.isotopes.front())==matrix_isotopes.end())
			continue;
		matrix_clusters.insert(C);
	}

	return matrix_clusters;
}

const vector<calc_t::sims_t::Crel_to_Irel_data_polynomial_fit_t> calc_t::sims_t::matrix_t::elemental_Crel_to_median_Irel_linear_fitted() const
{
	logger::debug(13,"calc_t::sims_t::matrix_t::RSFs_elemental_median()","entering");
	vector<calc_t::sims_t::Crel_to_Irel_data_polynomial_fit_t> Crels_to_Irels;
	for (const auto& Z : matrix_clusters)
	{
		for (const auto& N : matrix_clusters)
		{
			if (Z==N)
				continue;
			calc_t::sims_t::Crel_to_Irel_data_collector_t Crel_to_Irel_data_collector(Z,N,measurements);
// 			cout << Crel_to_Irel_data_collector.elemental_Crel().Irel_from_median().Crel_to_Irel_data.first.to_string_detailed() << endl;
			auto const RSF = Crel_to_Irel_data_collector.elemental_Crel().Irel_from_median().polynom({0,1},{0,1});
			if (!RSF.polynom.successfully_fitted())
			{
				logger::info(3,"calc_t::sims_t::matrix_t::RSFs_elemental_median()","RSF not successfully fitted", "Z:" + RSF.zaehler.to_string() + ", N:" + RSF.nenner.to_string());
				continue;
			}
			Crels_to_Irels.push_back(RSF);
		}
	}
	logger::debug(13,"calc_t::sims_t::matrix_t::RSFs_elemental_median()","leaving");
	return Crels_to_Irels;
}

quantity::concentration_t calc_t::sims_t::matrix_t::relative_elemental_concentration(const cluster_t& cluster, 
																		   const measurements_::sims_t& measurement)
{	
	quantity::quantity_t nenner;
	
// 	for (const auto& mc : matrix_clusters)
// 	{
// 		RSFs.corrected_RSF(mc,cluster);
// 	}
	
// 	for (const RSF_t& RSF : RSFs.raw_RSF())
// 	{
// 		if (cluster!=Crel_to_Irel.nenner)
// 			continue;
// 		if (!Crel_to_Irel.Crel(measurement).is_set())
// 			return {};
// 		if (!nenner.is_set())
// 			nenner = Crel_to_Irel.Crel(measurement);
// 		else
// 			nenner += Crel_to_Irel.Crel(measurement);
// 	}
	
	nenner += 1;
	
	return quantity::concentration_t(nenner.invert().change_unit(units::derived::atom_percent));
}

measurements_::sims_t calc_t::sims_t::matrix_t::relative_elemental_concentrations(const vector<cluster_t>& clusters, measurements_::sims_t measurement)
{
	for (const auto& c : clusters)
	{
		if (measurement.cluster(c)==nullptr)
			continue;
		if (!relative_elemental_concentration(c,measurement).is_set())
			continue;
		measurement.cluster(c)->concentration = relative_elemental_concentration(c,measurement);
	}
	return measurement;
}

measurements_::sims_t calc_t::sims_t::matrix_t::relative_elemental_concentrations(measurements_::sims_t measurement)
{
	return relative_elemental_concentrations(vector<cluster_t>(matrix_clusters.begin(),matrix_clusters.end()),measurement);
}

/****************************************/
/***********     RSF_t     **************/
/****************************************/

calc_t::sims_t::matrix_t::RSF_t::RSF_t(const cluster_t zaehler, 
									   const cluster_t nenner,
									   const quantity::quantity_t& quantity_s) :
									   RSF_t(zaehler,nenner,quantity_s.data(),quantity_s.unit(),quantity_s.dimension())
{
}

calc_t::sims_t::matrix_t::RSF_t::RSF_t(const cluster_t zaehler, 
									   const cluster_t nenner, 
									   vector<double> data_s, 
									   unit_t unit_s, 
									   quantity::quantity_t::dimension_t dim_s) :
									   zaehler(zaehler),nenner(nenner), 
									   quantity::quantity_t("mRSF",data_s,unit_s,dim_s)
{
	if (!is_relative())
	{
		logger::error("calc_t::sims_t::matrix_t::RSF_t::RSF_t()","quantity not relative, but matrix RSF should always be relative",to_string());
		clear();
	}
	if (zaehler==nenner)
	{
		logger::error("calc_t::sims_t::matrix_t::RSF_t::RSF_t()","zaehler==nenner",to_string());
		clear();
	}
}

string calc_t::sims_t::matrix_t::RSF_t::to_string() const
{
	stringstream ss;
	ss << "Z=" << zaehler.to_string();
	ss << ", N=" << nenner.to_string();
	ss << ", " << quantity::quantity_t::to_string();
	return ss.str();
}

bool calc_t::sims_t::matrix_t::RSF_t::operator==(const calc_t::sims_t::matrix_t::RSF_t& obj) const
{
	if (zaehler != obj.zaehler)
		return false;
	if (nenner != obj.nenner)
		return false;
	return true;
}

bool calc_t::sims_t::matrix_t::RSF_t::operator!=(const calc_t::sims_t::matrix_t::RSF_t& obj) const
{
	return !operator==(obj);
}


bool calc_t::sims_t::matrix_t::RSF_t::operator<(const calc_t::sims_t::matrix_t::RSF_t& obj) const
{
	if (zaehler==nenner)
		return false;
	if (zaehler.to_string() > nenner.to_string())
		return false;
	return true;
}

bool calc_t::sims_t::matrix_t::RSF_t::operator>(const calc_t::sims_t::matrix_t::RSF_t& obj) const
{
	if (*this == obj)
		return false;
	if (*this < obj)
		return false;
	return true;
}


/****************************************/
/***********     RSFs_t    **************/
/****************************************/

calc_t::sims_t::matrix_t::RSFs_t::RSFs_t(const set<cluster_t>& matrix_clusters, 
										 const vector<calc_t::sims_t::Crel_to_Irel_data_polynomial_fit_t>& Crel_to_Irel_lin_fits, 
										 const vector<calc_t::sims_t::matrix_t::RSF_t>& RSFs) :
										 matrix_clusters(matrix_clusters), Crel_to_Irel_lin_fits(Crel_to_Irel_lin_fits), RSFs(RSFs.begin(),RSFs.end())
{
}

calc_t::sims_t::matrix_t::RSFs_t::RSFs_t(const set<cluster_t>& matrix_clusters, 
										 const vector<calc_t::sims_t::Crel_to_Irel_data_polynomial_fit_t>& Crel_to_Irel_lin_fits, 
										 const set<calc_t::sims_t::matrix_t::RSF_t>& RSFs) :
										 matrix_clusters(matrix_clusters), Crel_to_Irel_lin_fits(Crel_to_Irel_lin_fits), RSFs(RSFs)
{
}

calc_t::sims_t::matrix_t::RSF_t calc_t::sims_t::matrix_t::RSFs_t::RSF(const Crel_to_Irel_data_polynomial_fit_t& Crel_to_Irel_lin_fit)
{
	if (Crel_to_Irel_lin_fit.polynom.successfully_fitted())
		return RSF_t{Crel_to_Irel_lin_fit.zaehler,Crel_to_Irel_lin_fit.nenner,{Crel_to_Irel_lin_fit.polynom.fit_parameters().at(1)}};
	return RSF_t{Crel_to_Irel_lin_fit.zaehler,Crel_to_Irel_lin_fit.nenner};
}

unsigned int calc_t::sims_t::matrix_t::RSFs_t::RSFs_count_from_clusters() const
{
	return (matrix_clusters.size()*matrix_clusters.size()-matrix_clusters.size());
}

const set<calc_t::sims_t::matrix_t::RSF_t> calc_t::sims_t::matrix_t::RSFs_t::RSFs_from_fits() const
{
	set<RSF_t> RSFs_set;
	for (auto& F : Crel_to_Irel_lin_fits)
	{
		const auto RSF_F = RSF(F);
		if (!RSF_F.is_set())
			continue;
		RSFs_set.insert(RSF_F);
	}
	return RSFs_set;
}

calc_t::sims_t::matrix_t::RSFs_t calc_t::sims_t::matrix_t::RSFs_t::valid_RSFs_and_fits() const
{
	vector<RSF_t> valid_RSFs_s;
	for (auto& RSF : RSFs)
	{
		if (!RSF.is_set() || RSF.is_nan() || RSF.is_inf())
			continue;
		valid_RSFs_s.push_back(RSF);
	}
	vector<Crel_to_Irel_data_polynomial_fit_t> valid_fits;
	for (auto& fit : valid_fits)
	{
		if (!fit.polynom.successfully_fitted())
			continue;
		valid_fits.push_back(fit);
	}
	return RSFs_t(matrix_clusters,valid_fits,valid_RSFs_s);
}

calc_t::sims_t::matrix_t::RSFs_t calc_t::sims_t::matrix_t::RSFs_t::add_missing_RSFs_from_inverse() const
{
	set<RSF_t> RSFs_copy = RSFs;
	for (auto& Z : matrix_clusters)
	{
		for (auto& N : matrix_clusters)
		{
			if (Z==N) 
				continue;
			if (RSF(Z,N).is_set() && !RSF(N,Z).is_set())
			{
				RSF_t RSF_adder(N,Z,RSF(Z,N).invert());
				RSFs_copy.insert(RSF_adder);
			}
		}
	}
	return {matrix_clusters, Crel_to_Irel_lin_fits,RSFs_copy};
}

calc_t::sims_t::matrix_t::RSFs_t calc_t::sims_t::matrix_t::RSFs_t::remove_Crel_to_Irel_fits_by_RSFs() const
{
	vector<Crel_to_Irel_data_polynomial_fit_t> residual_fits;
	for (auto& F : Crel_to_Irel_lin_fits)
	if (RSFs.find(RSF(F))==RSFs.end())
		residual_fits.push_back(F);
	return {matrix_clusters,residual_fits,RSFs};
}

calc_t::sims_t::matrix_t::RSFs_t calc_t::sims_t::matrix_t::RSFs_t::improve_RSFs(float relative_chisqr) const
{
	///outliners are usually at the end of the intervall, that means greatest value, that means last value
	vector<Crel_to_Irel_data_polynomial_fit_t> new_fits;
	for (auto& F : Crel_to_Irel_lin_fits)
	{
		if (F.polynom.chisq_relative() > relative_chisqr && F.Crel_to_Irel_map.fitable_data_size() > )
		{
			const auto new_map = F.Crel_to_Irel_map.remove_by_X(F.Crel_to_Irel_map.X.max());
			const auto new_polyfit = new_map.polynom(F.polynom.rank,F.polynom.fit_parameters());
			new_polyfit.
// 			Crel_to_Irel_data_polynomial_fit_t(F.zaehler,F.nenner,new_map);
		}
	}
}

calc_t::sims_t::matrix_t::RSFs_t calc_t::sims_t::matrix_t::RSFs_t::add_missing_RSFs() const
{
// 	if (valid_RSFs().RSFs.size()==RSFs_count_from_clusters())
// 		return remove_RSF_with_worse_chisqr_fit();
// 	return add_missing_RSFs_from_inverse().add_missing_RSFs_from_virtual_cluster();
}
