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

mgroups_::sims_t::calc_t::matrix_c::polynom_fit_Crels_to_Irels_c::polynom_fit_Crels_to_Irels_c(vector<unsigned int> rank, vector<double> fit_start_parameters, calc_t& calc) : 
										calc(calc),rank(rank),matrix_isotopes_concentrations(matrix_isotopes_concentrations_f(fit_start_parameters))
{
}

void mgroups_::sims_t::calc_t::matrix_c::polynom_fit_Crels_to_Irels_c::plot_to_screen(double sleep_sec) const
{ 
	for (auto& MIC : matrix_isotopes_concentrations)
		MIC.plot_to_screen(sleep_sec);
}

const vector<mgroups_::sims_t::calc_t::matrix_c::polynom_fit_Crels_to_Irels_c::matrix_isotope_concentration_c> mgroups_::sims_t::calc_t::matrix_c::polynom_fit_Crels_to_Irels_c::matrix_isotopes_concentrations_f(vector<double> fit_start_parameters)
{
	vector<matrix_isotope_concentration_c> results;
	for (auto& MC : calc.MG.matrix_clusters())
	{
		results.push_back({MC,calc,rank,fit_start_parameters});
	}
	return results;
}

bool mgroups_::sims_t::calc_t::matrix_c::polynom_fit_Crels_to_Irels_c::concentration(measurements_::sims_t& M)
{
	const auto matrix_clusters = calc.MG.matrix_clusters();
// 	for (auto& MC : matrix_clusters)
// 	{
		for (auto& matrix_isotope_concentration : matrix_isotopes_concentrations)
		{
			const auto Conc = matrix_isotope_concentration.concentration(M);
			if (Conc.is_set())
				M.cluster(matrix_isotope_concentration.matrix_cluster)->concentration = Conc;
			else
			{
				M.cluster(matrix_isotope_concentration.matrix_cluster)->concentration.clear();
				logger::warning(3,"mgroups_::sims_t::calc_t::matrix_c::polynom_fit_Crels_to_Irels_c::concentration()","concentration not set",matrix_isotope_concentration.matrix_cluster.to_string(),M.to_string_short());
			}
// 			matrix_isotope_concentration.
		}
// 	}
	
	return true;
}



/****************************************************************/
/******         matrix_isotope_concentration_c            *******/
/****************************************************************/
mgroups_::sims_t::calc_t::matrix_c::polynom_fit_Crels_to_Irels_c::matrix_isotope_concentration_c::matrix_isotope_concentration_c(const cluster_t& matrix_cluster, calc_t& calc, vector<unsigned int> rank, vector<double> fit_start_parameters) : 
																calc(calc), matrix_cluster(matrix_cluster), polynom_fits_Crels_to_Irels(polynom_fits_Crels_to_Irels_f(rank, fit_start_parameters))
{
}

const vector<mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c> mgroups_::sims_t::calc_t::matrix_c::polynom_fit_Crels_to_Irels_c::matrix_isotope_concentration_c::polynom_fits_Crels_to_Irels_f(vector<unsigned int> rank, vector<double> fit_start_parameters) const
{
	vector<polynom_fit_Crel_to_Irel_c> polynom_fit_Crel_to_Irel_results;
	for (auto& mc : calc.MG.matrix_clusters())
	{
		if (mc == matrix_cluster) continue;
		polynom_fit_Crel_to_Irel_c Crel_to_Irel(mc, matrix_cluster,calc,rank,fit_start_parameters);
		if (!Crel_to_Irel.execute_fit_polynom())
		{
			logger::info(3,"mgroups_::sims_t::calc_t::matrix_c::polynom_fit_Crels_to_Irels_c::matrix_isotope_concentration_c::polynom_fits_Crels_to_Irels_f()","polyfit NOT seccessful: ("+mc.to_string()+")/("+matrix_cluster.to_string()+")");
		}
		else
			logger::info(3,"mgroups_::sims_t::calc_t::matrix_c::polynom_fit_Crels_to_Irels_c::matrix_isotope_concentration_c::polynom_fits_Crels_to_Irels_f()","polyfit successful: ("+mc.to_string()+")/("+matrix_cluster.to_string()+")");
		polynom_fit_Crel_to_Irel_results.push_back(Crel_to_Irel);
	}
	return polynom_fit_Crel_to_Irel_results;
}

void mgroups_::sims_t::calc_t::matrix_c::polynom_fit_Crels_to_Irels_c::matrix_isotope_concentration_c::plot_to_screen(double sleep_sec) const
{
	for (auto& poly_fit_Crel_2_Irel : polynom_fits_Crels_to_Irels)
		poly_fit_Crel_2_Irel.plot_to_screen(sleep_sec);
}

quantity::concentration_t mgroups_::sims_t::calc_t::matrix_c::polynom_fit_Crels_to_Irels_c::matrix_isotope_concentration_c::concentration(measurements_::sims_t& M) const
{
// 	[matrix_cluster](Irel) =  ( 1 + SUM_(i!=j) ([E_i]/[matrix_cluster]))^-1 using polynom_fits_Crels_to_Irels
	quantity::concentration_t results;
// 	SUM_(i!=j) { ([E_i]/[matrix_cluster]) }
	for (auto& P : polynom_fits_Crels_to_Irels)
	{
		if (!P.polynom.fitted())
		{
			logger::warning(3,"mgroups_::sims_t::calc_t::matrix_c::polynom_fit_Crels_to_Irels_c::matrix_isotope_concentration_c::concentration()",M.to_string_short(),P.zaehler.to_string()+"/"+P.nenner.to_string(),"Polynom not fitted");
			return {};
		}
		//test
// 		auto Test_Crel = P.polynom.fitted_y_data({0.2,0.5});
// 		logger::debug(11,"mgroups_::sims_t::calc_t::matrix_c::polynom_fit_Crels_to_Irels_c::matrix_isotope_concentration_c::concentration()","Irel=0.2,0.5","Crel="+tools::to_string(Test_Crel.front())+","+tools::to_string(Test_Crel.back()));
		
		const auto Crel_ = P.Crel(M);
		logger::debug(11,"mgroups_::sims_t::calc_t::matrix_c::polynom_fit_Crels_to_Irels_c::matrix_isotope_concentration_c::concentration()",M.to_string_short(),P.zaehler.to_string()+"/"+P.nenner.to_string(),"Crel="+Crel_.to_string_detailed());
		if (!Crel_.is_set())
		{
			logger::warning(3,"mgroups_::sims_t::calc_t::matrix_c::polynom_fit_Crels_to_Irels_c::matrix_isotope_concentration_c::concentration()","fit was not successful; Crel not set","zaehler="+P.zaehler.to_string()+"; nenner="+P.nenner.to_string(),M.to_string_short());
			return {};
		}
	
		if (!results.is_set())
			results = Crel_;
		else
			results += Crel_;
	}
// 	[matrix_cluster] =  ( 1 + SUM )^-1
	return ((results+1).invert()).change_unit("at%");
// 	results = ((results + 1).invert()) * 100;
// 	return {results.data,units::derived::atom_percent};
}
