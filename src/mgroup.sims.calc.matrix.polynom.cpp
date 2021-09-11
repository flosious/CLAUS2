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

#include "mgroup.hpp"

mgroups_::sims_t::calc_t::matrix_c::polynom_c::polynom_c(vector<unsigned int> rank, vector<double> fit_start_parameters, calc_t& calc) : 
										calc(calc),rank(rank),matrix_isotopes_concentrations(matrix_isotopes_concentrations_f(fit_start_parameters))
{
}

const vector<mgroups_::sims_t::calc_t::matrix_c::polynom_c::matrix_isotope_concentration_c> mgroups_::sims_t::calc_t::matrix_c::polynom_c::matrix_isotopes_concentrations_f(vector<double> fit_start_parameters)
{
	vector<matrix_isotope_concentration_c> results;
	for (auto& MC : calc.MG.matrix_clusters())
	{
		results.push_back({MC,calc,rank,fit_start_parameters});
	}
	return results;
}

/************************************************************/
/******     polynom_fit_Crel_to_Irel_c     *******/
/************************************************************/
mgroups_::sims_t::calc_t::matrix_c::polynom_c::polynom_fit_Crel_to_Irel_c::polynom_fit_Crel_to_Irel_c(const cluster_t& zaehler,const cluster_t& nenner,calc_t& calc, vector<unsigned int> rank, vector<double> fit_start_parameters) :
																				Crel_to_Irel_c(zaehler,nenner,calc),
																				polynom(rank,fit_start_parameters)
{
}

const concentration_t mgroups_::sims_t::calc_t::matrix_c::polynom_c::polynom_fit_Crel_to_Irel_c::Crel_template = concentration_t({},units::SI::one);

concentration_t mgroups_::sims_t::calc_t::matrix_c::polynom_c::polynom_fit_Crel_to_Irel_c::Crel(measurements_::sims_t& M) const
{
	return {polynom.fitted_y_data(Irel(M).data),Crel_template.unit()};
}

bool mgroups_::sims_t::calc_t::matrix_c::polynom_c::polynom_fit_Crel_to_Irel_c::execute_fit_polynom()
{
	const auto Crel_to_Irel = known_Crels_from_sample_matrix_to_Irels_truncated_median();
	
	if (!Crel_to_Irel.first.is_set() || !Crel_to_Irel.second.is_set())
	{
		logger::warning(4,"mgroups_::sims_t::calc_t::matrix_c::polynom_c::polynom_fit_Crel_to_Irel_c::execute_fit_polynom()","known_Crels_from_sample_matrix_to_Irels_truncated_median() is not set","returning false");
		return false;
	}
	map<double,double> X_to_Y_map;
	tools::vec::combine_vecs_to_map(&Crel_to_Irel.second.data,&Crel_to_Irel.first.data, &X_to_Y_map); // Crel(Irel)
	if (!polynom.fit(X_to_Y_map))
	{
		//this should essentially never happen; if there is a GSL error, GSL handler will catch the exception
		logger::error("mgroups_::sims_t::calc_t::matrix_c::polynom_c::polynom_fit_Crel_to_Irel_c::execute_fit_polynom()","fit returned error","returning false");
		logger::debug(11,"mgroups_::sims_t::calc_t::matrix_c::polynom_c::polynom_fit_Crel_to_Irel_c::execute_fit_polynom()","X_to_Y_map.size()=" + tools::to_string(X_to_Y_map.size()));
		logger::debug(12,"mgroups_::sims_t::calc_t::matrix_c::polynom_c::polynom_fit_Crel_to_Irel_c::execute_fit_polynom()","X_to_Y_map",tools::to_string(X_to_Y_map));
		logger::debug(12,"mgroups_::sims_t::calc_t::matrix_c::polynom_c::polynom_fit_Crel_to_Irel_c::execute_fit_polynom()","rank",tools::vec::to_string(polynom.rank));
		return false;
	}
	else
	{
		logger::info(4,"mgroups_::sims_t::calc_t::matrix_c::polynom_c::polynom_fit_Crel_to_Irel_c::execute_fit_polynom()","fit successful","zaehler=" + zaehler.to_string()+" nenner=" + nenner.to_string());
	}
	return true;
}

/****************************************************************/
/******         matrix_isotope_concentration_c            *******/
/****************************************************************/
mgroups_::sims_t::calc_t::matrix_c::polynom_c::matrix_isotope_concentration_c::matrix_isotope_concentration_c(const cluster_t& matrix_cluster, calc_t& calc, vector<unsigned int> rank, vector<double> fit_start_parameters) : 
																calc(calc), matrix_cluster(matrix_cluster), polynom_fits_Crels_to_Irels(polynom_fits_Crels_to_Irels_f(rank, fit_start_parameters))
{
}

const vector<mgroups_::sims_t::calc_t::matrix_c::polynom_c::polynom_fit_Crel_to_Irel_c> mgroups_::sims_t::calc_t::matrix_c::polynom_c::matrix_isotope_concentration_c::polynom_fits_Crels_to_Irels_f(vector<unsigned int> rank, vector<double> fit_start_parameters) const
{
	vector<polynom_fit_Crel_to_Irel_c> polynom_fit_Crel_to_Irel_results;
	for (auto& mc : calc.MG.matrix_clusters())
	{
		if (mc == matrix_cluster) continue;
		polynom_fit_Crel_to_Irel_c Crel_to_Irel(mc, matrix_cluster,calc,rank,fit_start_parameters);
		if (!Crel_to_Irel.execute_fit_polynom())
		{
			logger::info(3,"mgroups_::sims_t::calc_t::matrix_c::polynom_c::matrix_isotope_concentration_c::polynom_fits_Crels_to_Irels_f()","polyfit NOT seccessful: ("+mc.to_string()+")/("+matrix_cluster.to_string()+")");
		}
		else
			logger::info(3,"mgroups_::sims_t::calc_t::matrix_c::polynom_c::matrix_isotope_concentration_c::polynom_fits_Crels_to_Irels_f()","polyfit successful: ("+mc.to_string()+")/("+matrix_cluster.to_string()+")");
		polynom_fit_Crel_to_Irel_results.push_back(Crel_to_Irel);
	}
	return polynom_fit_Crel_to_Irel_results;
}

concentration_t mgroups_::sims_t::calc_t::matrix_c::polynom_c::matrix_isotope_concentration_c::concentration(measurements_::sims_t& M, const cluster_t& matrix_cluster) const
{
// 	[matrix_cluster](Irel) =  ( 1 + SUM_(i!=j) ([E_i]/[matrix_cluster]))^-1 using polynom_fits_Crels_to_Irels
	concentration_t results;
// 	SUM_(i!=j) { ([E_i]/[matrix_cluster]) }
	for (auto& P : polynom_fits_Crels_to_Irels)
	{
		if (P.nenner != matrix_cluster) continue;
		const auto Crel_ = P.Crel(M);
		if (!Crel_.is_set())
		{
			logger::error("mgroups_::sims_t::calc_t::matrix_c::polynom_c::matrix_isotope_concentration_c::concentration()","Crel is not set; this is should never happen","zaehler="+P.zaehler.to_string()+"; nenner="+P.nenner.to_string(),M.to_string_short());
			return {};
		}
	
		if (!results.is_set())
			results = Crel_;
		else
			results += Crel_;
	}
// 	[matrix_cluster] =  ( 1 + SUM )^-1
	results = (results + 1).invert();
	return {results.data,units::derived::atom_percent};
}
