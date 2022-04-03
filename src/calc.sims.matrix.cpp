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
								   const vector<measurements_::sims_t>& measurements, 
								   const vector<RSF_t>& RSFs) : 
								   matrix_isotopes_p(matrix_isotopes),
								   measurements_p(measurements),
								   matrix_clusters_p(matrix_clusters_from_matrix_isotopes()),
								   RSFs_p(matrix_clusters(),elemental_Crel_to_median_Irel_linear_fitted(),RSFs)
{
// 	for (auto& M:measurements)
// 		for (auto& i : M.sample->matrix().isotopes())
// 			cout << i.substance_amount.to_string_detailed() << endl;
	
}

const calc_t::sims_t::matrix_t::RSFs_t& calc_t::sims_t::matrix_t::RSFs() const
{
	return RSFs_p;
}

const vector<isotope_t>& calc_t::sims_t::matrix_t::matrix_isotopes() const
{
	return matrix_isotopes_p;
}
const set<cluster_t> calc_t::sims_t::matrix_t::matrix_clusters() const
{
	return matrix_clusters_p;
}

const vector<measurements_::sims_t>& calc_t::sims_t::matrix_t::measurements() const
{
	return measurements_p;
}

const set<cluster_t> calc_t::sims_t::matrix_t::matrix_clusters_from_matrix_isotopes() const
{
	
	set<cluster_t> matrix_clusters;
	set<cluster_t> clusters;
	
// 	cout << "measurements().size()=" << measurements().size() << endl;
	for (const auto& M : measurements())
	{
// 		cout << "M.clusters.size()=" << M.clusters.size() << endl;
		for (const auto& C : M.clusters)
		{
			clusters.insert(C);
		}
	}

	for (const auto& C : clusters)
	{
		if (C.isotopes.size()!=1) 
			continue;
		if (std::find(matrix_isotopes().begin(),matrix_isotopes().end(),C.isotopes.front())==matrix_isotopes().end())
			continue;
		matrix_clusters.insert(C);
	}

	return matrix_clusters;
}

const vector<calc_t::sims_t::Crel_to_Irel_data_polynomial_fit_t> calc_t::sims_t::matrix_t::elemental_Crel_to_median_Irel_linear_fitted() const
{
	logger::debug(13,"calc_t::sims_t::matrix_t::elemental_Crel_to_median_Irel_linear_fitted()","entering");
	vector<calc_t::sims_t::Crel_to_Irel_data_polynomial_fit_t> Crels_to_Irels;
	for (const auto& Z : matrix_clusters())
	{
		for (const auto& N : matrix_clusters())
		{
			if (Z==N)
				continue;
			calc_t::sims_t::Crel_to_Irel_data_collector_t Crel_to_Irel_data_collector(Z,N,measurements());
// 			cout << Crel_to_Irel_data_collector.elemental_Crel().Irel_from_median().Crel_to_Irel_data.first.to_string_detailed() << endl;
			auto const RSF = Crel_to_Irel_data_collector.elemental_Crel().Irel_from_median().polynom({0,1},{0,1});
			if (!RSF.polynom().successfully_fitted())
			{
				logger::info(3,"calc_t::sims_t::matrix_t::elemental_Crel_to_median_Irel_linear_fitted()","RSF not successfully fitted", "Z:" + RSF.zaehler().to_string() + ", N:" + RSF.nenner().to_string());
				continue;
			}
			Crels_to_Irels.push_back(RSF);
		}
	}
	logger::debug(13,"calc_t::sims_t::matrix_t::elemental_Crel_to_median_Irel_linear_fitted()","leaving");
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
	return relative_elemental_concentrations(vector<cluster_t>(matrix_clusters().begin(),matrix_clusters().end()),measurement);
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
									   double data_s) : RSF_t(zaehler,nenner,vector<double>{data_s})
{
}

calc_t::sims_t::matrix_t::RSF_t::RSF_t(const cluster_t zaehler, 
									   const cluster_t nenner, 
									   vector<double> data_s, 
									   unit_t unit_s, 
									   quantity::quantity_t::dimension_t dim_s) :
									   cluster_relations_copies_t(zaehler,nenner),
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

// const cluster_t& calc_t::sims_t::matrix_t::RSF_t::zaehler() const
// {
// 	return zaehler_p;
// }
// const cluster_t& calc_t::sims_t::matrix_t::RSF_t::nenner() const
// {
// 	return nenner_p;
// }

string calc_t::sims_t::matrix_t::RSF_t::to_string() const
{
	stringstream ss;
	ss << "Z=" << zaehler().to_string();
	ss << ", N=" << nenner().to_string();
	ss << ", " << quantity::quantity_t::to_string();
	return ss.str();
}

bool calc_t::sims_t::matrix_t::RSF_t::operator==(const calc_t::sims_t::matrix_t::RSF_t& obj) const
{
	if (zaehler() != obj.zaehler())
		return false;
	if (nenner() != obj.nenner())
		return false;
	return true;
}

bool calc_t::sims_t::matrix_t::RSF_t::operator!=(const calc_t::sims_t::matrix_t::RSF_t& obj) const
{
	return !operator==(obj);
}

bool calc_t::sims_t::matrix_t::RSF_t::operator<(const calc_t::sims_t::matrix_t::RSF_t& obj) const
{
	if (zaehler()==nenner())
		return false;
	if (zaehler().to_string() > nenner().to_string())
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
										 matrix_clusters_p(matrix_clusters), Crel_to_Irel_lin_fits_p(Crel_to_Irel_lin_fits), RSFs_manually_set_p(RSFs.begin(),RSFs.end())
{
}

calc_t::sims_t::matrix_t::RSFs_t::RSFs_t(const set<cluster_t>& matrix_clusters, 
										 const vector<calc_t::sims_t::Crel_to_Irel_data_polynomial_fit_t>& Crel_to_Irel_lin_fits, 
										 const set<calc_t::sims_t::matrix_t::RSF_t>& RSFs) :
										 matrix_clusters_p(matrix_clusters), Crel_to_Irel_lin_fits_p(Crel_to_Irel_lin_fits), RSFs_manually_set_p(RSFs)
{
}
/*getter functions for logical consts*/
const set<cluster_t>& calc_t::sims_t::matrix_t::RSFs_t::matrix_clusters() const
{
	return matrix_clusters_p;
}
const set<calc_t::sims_t::matrix_t::RSF_t>& calc_t::sims_t::matrix_t::RSFs_t::RSFs_manually_set() const
{
	return RSFs_manually_set_p;
}
const vector<calc_t::sims_t::Crel_to_Irel_data_polynomial_fit_t>& calc_t::sims_t::matrix_t::RSFs_t::Crel_to_Irel_lin_fits() const
{
	return Crel_to_Irel_lin_fits_p;
}


calc_t::sims_t::matrix_t::RSF_t calc_t::sims_t::matrix_t::RSFs_t::RSF(const Crel_to_Irel_data_polynomial_fit_t& Crel_to_Irel_lin_fit)
{
	if (Crel_to_Irel_lin_fit.polynom().successfully_fitted())
	{
		RSF_t T(Crel_to_Irel_lin_fit.zaehler(),Crel_to_Irel_lin_fit.nenner(),Crel_to_Irel_lin_fit.polynom().fit_parameters().at(1));
		return T;
	}
	return RSF_t{Crel_to_Irel_lin_fit.zaehler(),Crel_to_Irel_lin_fit.nenner()};
}

unsigned int calc_t::sims_t::matrix_t::RSFs_t::RSFs_count_from_clusters() const
{
	return (matrix_clusters().size()*matrix_clusters().size()-matrix_clusters().size());
}

const set<calc_t::sims_t::matrix_t::RSF_t> calc_t::sims_t::matrix_t::RSFs_t::RSFs_from_fits() const
{
	set<RSF_t> RSFs_set;
	for (auto& F : Crel_to_Irel_lin_fits())
	{
		const auto RSF_F = RSF(F);
		if (!RSF_F.is_set())
		{
			continue;
		}
		
		RSFs_set.insert(RSF_F);
	}
	return RSFs_set;
}

calc_t::sims_t::matrix_t::RSFs_t calc_t::sims_t::matrix_t::RSFs_t::valid_RSFs_and_fits() const
{
	vector<RSF_t> valid_RSFs_s;
	for (auto& RSF : RSFs_manually_set())
	{
		if (!RSF.is_set() || RSF.is_nan() || RSF.is_inf())
			continue;
		valid_RSFs_s.push_back(RSF);
	}
	vector<Crel_to_Irel_data_polynomial_fit_t> valid_fits;
	for (auto& fit : valid_fits)
	{
		if (!fit.polynom().successfully_fitted())
			continue;
		valid_fits.push_back(fit);
	}
	return RSFs_t(matrix_clusters(),valid_fits,valid_RSFs_s);
}

calc_t::sims_t::matrix_t::RSFs_t calc_t::sims_t::matrix_t::RSFs_t::add_missing_RSFs_from_inverse() const
{
	if (all_available_RSFs().size() == RSFs_count_from_clusters())
		return *this;
	set<RSF_t> RSFs_copy = RSFs_manually_set();
	for (auto& Z : matrix_clusters())
	{
		for (auto& N : matrix_clusters())
		{
			if (Z==N) 
				continue;
			if (RSF(Z,N).is_set() && !RSF(N,Z).is_set())
			{
// 				RSF_t RSF_adder(N,Z,RSF(Z,N).invert());
// 				RSFs_copy.insert(RSF_adder);
				RSFs_copy.insert(RSF_from_inverse(Z,N));
			}
		}
	}
	return {matrix_clusters(), Crel_to_Irel_lin_fits(),RSFs_copy};
}

calc_t::sims_t::matrix_t::RSFs_t calc_t::sims_t::matrix_t::RSFs_t::remove_Crel_to_Irel_fits_by_RSFs() const
{
	vector<Crel_to_Irel_data_polynomial_fit_t> residual_fits;
	for (auto& F : Crel_to_Irel_lin_fits())
	if (RSFs_manually_set().find(RSF(F))==RSFs_manually_set().end())
		residual_fits.push_back(F);
	return {matrix_clusters(),residual_fits,RSFs_manually_set()};
}

calc_t::sims_t::matrix_t::RSFs_t calc_t::sims_t::matrix_t::RSFs_t::improve_fit_by_removing_biggest_Irels(float relative_chisqr) const
{
	//Irel is X coordinate
	vector<Crel_to_Irel_data_polynomial_fit_t> new_fits;
	new_fits.reserve(Crel_to_Irel_lin_fits().size());
	for (auto& F : Crel_to_Irel_lin_fits())
	{
		auto new_map = F.Crel_to_Irel_map().sort_X_from_low_to_high();
		auto new_polynom = F.polynom();
		while (new_map.size()>=new_polynom.rank().size() && new_polynom.chisq_relative()>relative_chisqr)
		{
			new_map = new_map.remove_by_X(new_map.X().max());
			new_polynom = new_map.polynom(new_polynom.rank(),new_polynom.fit_parameters());
		}
		new_fits.push_back({F.zaehler(),F.nenner(),new_map,new_polynom});
	}
	return RSFs_t(matrix_clusters(),new_fits,RSFs_manually_set());
}


calc_t::sims_t::matrix_t::RSFs_t calc_t::sims_t::matrix_t::RSFs_t::improve_fit_by_removing_outliners(float relative_chisqr) const
{
	vector<Crel_to_Irel_data_polynomial_fit_t> new_fits;
	new_fits.reserve(Crel_to_Irel_lin_fits().size());
	for (auto& F : Crel_to_Irel_lin_fits())
	{
		const auto new_map = F.Crel_to_Irel_map().remove_outliners(F.polynom());
		const fit_functions::polynom_t new_polynom = new_map.polynom(F.polynom().rank(),F.polynom().fit_parameters());
		new_fits.push_back(Crel_to_Irel_data_polynomial_fit_t(F.zaehler(),F.nenner(),new_map,new_polynom));
	}
	return RSFs_t(matrix_clusters(),new_fits,RSFs_manually_set());
}

calc_t::sims_t::matrix_t::RSFs_t calc_t::sims_t::matrix_t::RSFs_t::add_missing_RSFs() const
{
	return add_missing_RSFs_from_inverse().add_missing_RSFs_from_virtual_cluster();
}

calc_t::sims_t::matrix_t::RSF_t calc_t::sims_t::matrix_t::RSFs_t::RSF(const cluster_t& zaehler, const cluster_t& nenner) const
{
	for (auto& rsf : RSFs_manually_set())
	{
		if (rsf.zaehler() != zaehler)
			continue;
		if (rsf.nenner() != nenner)
			continue;
		return rsf;
	}
	for (auto& Crel_to_Irel : Crel_to_Irel_lin_fits())
	{
		if (!Crel_to_Irel.polynom().successfully_fitted())
			continue;
		if (Crel_to_Irel.zaehler() != zaehler)
			continue;
		if (Crel_to_Irel.nenner()!= nenner)
			continue;
		RSF_t rsf(zaehler,nenner,Crel_to_Irel.polynom().fit_parameters().at(1));
		return rsf;
	}
	return {zaehler,nenner};
}

calc_t::sims_t::matrix_t::RSF_t calc_t::sims_t::matrix_t::RSFs_t::RSF_from_inverse(const cluster_t& zaehler, const cluster_t& nenner) const
{
	return RSF_t(zaehler,nenner,{RSF(nenner,zaehler).invert()});
}

calc_t::sims_t::matrix_t::RSF_t calc_t::sims_t::matrix_t::RSFs_t::RSF_with_worst_fit() const
{
	cluster_t Z,N;
	double chisqr_max=0;
	for (auto& fits : Crel_to_Irel_lin_fits())
	{
		if (fits.polynom().chisq_relative() > chisqr_max)
		{
			Z = fits.zaehler();
			N = fits.nenner();
			chisqr_max = fits.polynom().chisq_relative();
		}
	}
	return RSF(Z,N);
}

calc_t::sims_t::matrix_t::RSFs_t calc_t::sims_t::matrix_t::RSFs_t::remove_RSFs(const set<RSF_t>& rsfs_s) const
{
	auto new_RSFs = RSFs_manually_set();
	for (auto& rsf : rsfs_s)
	{
		new_RSFs.erase(rsf);
	}
	vector<Crel_to_Irel_data_polynomial_fit_t> new_Crel_to_Irels_lin_fits;
	new_Crel_to_Irels_lin_fits.reserve(Crel_to_Irel_lin_fits().size());
	for (auto& Crel_to_Irel : Crel_to_Irel_lin_fits())
	{
		bool insert = true;
		for (auto& rsf: rsfs_s)
		{
			if (rsf == RSF(Crel_to_Irel))
			{
				insert = false;
				break;
			}
		}
		if (insert)
			new_Crel_to_Irels_lin_fits.push_back(Crel_to_Irel);
	}
	return RSFs_t(matrix_clusters(),new_Crel_to_Irels_lin_fits,new_RSFs);
}

bool calc_t::sims_t::matrix_t::RSFs_t::sort_rel_chisqr_to_fits_by_chisqr(const pair<double,Crel_to_Irel_data_polynomial_fit_t>& P1, const pair<double,Crel_to_Irel_data_polynomial_fit_t>& P2)
{
	return (P1.first > P2.first);
}

set<calc_t::sims_t::matrix_t::RSF_t> calc_t::sims_t::matrix_t::RSFs_t::RSFs_with_worst_fit(int n) const
{
	if (n>=Crel_to_Irel_lin_fits().size())
		return {};
	vector<pair<double,Crel_to_Irel_data_polynomial_fit_t>> rel_chisqr_to_fits;
	rel_chisqr_to_fits.reserve(Crel_to_Irel_lin_fits().size());
	for (Crel_to_Irel_data_polynomial_fit_t F : Crel_to_Irel_lin_fits())
	{
		rel_chisqr_to_fits.push_back(pair<double,Crel_to_Irel_data_polynomial_fit_t>(F.polynom().chisq_relative(),F));
	}
	sort(rel_chisqr_to_fits.begin(),rel_chisqr_to_fits.end(),sort_rel_chisqr_to_fits_by_chisqr);
	set<calc_t::sims_t::matrix_t::RSF_t> RSFs;
	for (int i=0;i<n;i++)
	{
		RSFs.insert(RSF(rel_chisqr_to_fits.at(i).second));
	}
	return RSFs;
}

calc_t::sims_t::matrix_t::RSFs_t calc_t::sims_t::matrix_t::RSFs_t::remove_RSFs_with_worst_fit(int n) const
{
	return remove_RSFs(RSFs_with_worst_fit(n));
}

calc_t::sims_t::matrix_t::RSFs_t calc_t::sims_t::matrix_t::RSFs_t::remove_RSFs_above_relative_chisqr_treshold(float relativ_chisq) const
{
	vector<Crel_to_Irel_data_polynomial_fit_t> new_Crel_to_Irels_lin_fits;
	new_Crel_to_Irels_lin_fits.reserve(Crel_to_Irel_lin_fits().size());
	for (const auto& Crel_to_Irel : Crel_to_Irel_lin_fits())
	{
		if (Crel_to_Irel.polynom().chisq_relative()<=relativ_chisq)
			new_Crel_to_Irels_lin_fits.push_back(Crel_to_Irel);
	}
	return RSFs_t(matrix_clusters(),new_Crel_to_Irels_lin_fits,RSFs_manually_set());
}

calc_t::sims_t::matrix_t::RSFs_t calc_t::sims_t::matrix_t::RSFs_t::add_or_replace_RSF(const RSF_t& RSF) const
{
	set<RSF_t> new_RSFs = RSFs_manually_set();
	new_RSFs.erase(RSF);
	new_RSFs.insert(RSF);
	return RSFs_t(matrix_clusters(),Crel_to_Irel_lin_fits(),new_RSFs);
}

calc_t::sims_t::matrix_t::RSFs_t calc_t::sims_t::matrix_t::RSFs_t::add_or_replace_RSFs(const vector<RSF_t>& RSFs_s) const
{
	auto new_RSFs = RSFs_manually_set();
	for (auto& RSF : RSFs_s)
	{
		new_RSFs.erase(RSF);
		new_RSFs.insert(RSF);
	}
	return RSFs_t(matrix_clusters(),Crel_to_Irel_lin_fits(),new_RSFs);
}

calc_t::sims_t::matrix_t::RSFs_t calc_t::sims_t::matrix_t::RSFs_t::symmetrical_RSFs() const
{
	set<RSF_t> new_RSFs;
	for (auto rsf : RSFs_manually_set())
	{
		if (new_RSFs.find(rsf)!= new_RSFs.end())
			continue;
		if (!rsf.is_set())
			continue;
		if (!RSF(rsf.nenner(),rsf.zaehler()).is_set())
			continue;
		if (RSF(rsf.nenner(),rsf.zaehler()).is_nan())
			continue;
		if (RSF(rsf.nenner(),rsf.zaehler()).is_inf())
			continue;
		// wenn sich die Einheiten / dimensionen der RSFs unterscheiden, dann wird die linke seite genommen
		rsf << RSF(rsf.nenner(),rsf.zaehler()); 
		new_RSFs.insert(RSF_t(rsf.zaehler(), rsf.nenner(), rsf.mean()));
		new_RSFs.insert(RSF_t(rsf.nenner(), rsf.zaehler(), rsf.mean()));
	}
	return {matrix_clusters(),Crel_to_Irel_lin_fits(),new_RSFs};
}

const set<calc_t::sims_t::matrix_t::RSF_t> calc_t::sims_t::matrix_t::RSFs_t::all_available_RSFs() const
{
	auto fitted_RSFs = RSFs_from_fits();
	auto manual_RSFs = RSFs_manually_set();
	set<calc_t::sims_t::matrix_t::RSF_t> all_RSFs; 
	
	if (fitted_RSFs.size()>0)
		all_RSFs.insert(fitted_RSFs.begin(),fitted_RSFs.end());
	if (manual_RSFs.size()>0)
		all_RSFs.insert(manual_RSFs.begin(),manual_RSFs.end());
	return all_RSFs;
} 

const set<calc_t::sims_t::matrix_t::RSF_t> calc_t::sims_t::matrix_t::RSFs_t::unknown_RSFs() const
{
	set<RSF_t> all_possible_RFS;
	for (auto& Z : matrix_clusters())
	{
		for (auto& N : matrix_clusters())
		{
			if (Z==N)
				continue;
			all_possible_RFS.insert({Z,N,{}});
		}
	}
	
	//now substract the known
	for (auto& rsf : RSFs_manually_set())
		all_possible_RFS.erase(rsf);
	for (auto& Crel_to_Irel : Crel_to_Irel_lin_fits())
		all_possible_RFS.erase(RSF(Crel_to_Irel));
	
	return all_possible_RFS;
}

/* this is a rather complicated method
 * it calculates the concentration of all matrix clusters, where the RSFs are given
 */
calc_t::sims_t::matrix_t::RSFs_t calc_t::sims_t::matrix_t::RSFs_t::add_missing_RSFs_from_virtual_cluster() const
{
	if (all_available_RSFs().size() == RSFs_count_from_clusters())
		return *this;
	if (unknown_RSFs().size()>2)
	{
		logger::error("calc_t::sims_t::matrix_t::RSFs_t::add_missing_RSFs_from_virtual_cluster()","unknown_RSFs().size()>2","returning this");
		return *this;
	}
	
}


/****************************************/
/*****     concentration_c    ***********/
/****************************************/

calc_t::sims_t::matrix_t::concentration_c::concentration_c(const set<RSF_t>& RSFs, const set<cluster_t>& matrix_clusters) : RSFs_p(RSFs), matrix_clusters_p(matrix_clusters)
{
}

calc_t::sims_t::matrix_t::concentration_c::concentration_c(const RSFs_t& RSFs) : RSFs_p(RSFs.all_available_RSFs()),matrix_clusters_p(RSFs.matrix_clusters()) 
{
}

vector<quantity::concentration_t> calc_t::sims_t::matrix_t::concentration_c::calc_able_concentrations()
{
}

quantity::concentration_t calc_t::sims_t::matrix_t::concentration_c::concentration(const cluster_t& cluster)
{
	
}


