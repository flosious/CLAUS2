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

void mgroups_::sims_t::calc_t::matrix_c::polynom_c::plot_to_screen(double sleep_sec) const
{
	for (auto& MIC : matrix_isotopes_concentrations)
		MIC.plot_to_screen(sleep_sec);
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

bool mgroups_::sims_t::calc_t::matrix_c::polynom_c::concentration(measurements_::sims_t& M)
{
	const auto matrix_clusters = calc.MG.matrix_clusters();
	for (auto& MC : matrix_clusters)
	{
		for (auto& matrix_isotope_concentration : matrix_isotopes_concentrations)
		{
			const auto C = matrix_isotope_concentration.concentration(M,MC);
			if (C.is_set())
				M.cluster(MC)->concentration = C;
			else
				logger::warning(3,"mgroups_::sims_t::calc_t::matrix_c::polynom_c::concentration()","concentration not set",MC.to_string(),M.to_string_short());
		}
	}
	
// 	///sum of all isotopes(matrix_clusters) == 100at%; look for unknown/unset concentration matrix_cluster
// 	cluster_t* unset_matrix_cluster = nullptr;
// 	for (auto& MC : matrix_clusters)
// 	{
// 		if (!M.cluster(MC)->concentration.is_set())
// 		{
// 			if (unset_matrix_cluster!= nullptr)
// 			{
// 				logger::warning(3,"","more than 1 unknown matrix element","returning false");
// 				return false;
// 			}
// 			else
// 				unset_matrix_cluster = M.cluster(MC);
// 		}
// 	}
// 	if (unset_matrix_cluster!=nullptr)
// 	{
// 		logger::error("mgroups_::sims_t::calc_t::matrix_c::polynom_c::concentration()","unset_matrix_cluster " + unset_matrix_cluster->to_string(),"false");
// 		return false;
// 	}
// 	///there is 1 unknown matrix_cluster and we know the isotopes in the matrix --> we can calculate its concentration
// 	if (unset_matrix_cluster!=nullptr && M.sample->matrix().is_set())
// 	{
// 		isotope_t* IM = M.sample->matrix().isotope(unset_matrix_cluster->corresponding_isotope(calc.MG.matrix_isotopes()));
// 		concentration_t sum;
// 		if (IM == nullptr)
// 		{
// 			logger::error("mgroups_::sims_t::calc_t::matrix_c::polynom_c::concentration()","this should never happen, tell florian","false");
// 			return false;
// 		}
// 		if (!IM->abundance.is_set())
// 		{
// 			logger::warning(3,"mgroups_::sims_t::calc_t::matrix_c::polynom_c::concentration()",unset_matrix_cluster->to_string() +" ; " + IM->to_string() + "abundance unknown","false");
// 		}
// 		
// 		const auto mat_isos = calc.MG.matrix_isotopes();
// 		for (auto& MC : M.matrix_clusters(mat_isos).clusters)
// 		{
// 			if (MC == unset_matrix_cluster) continue;
// 			if (!sum.is_set())
// 				sum = MC->concentration / M.sample->matrix().isotope(MC->corresponding_isotope(mat_isos))->abundance;
// 		}
// 	}
	
	return true;
}

/************************************************************/
/******           polynom_fit_Crel_to_Irel_c          *******/
/************************************************************/
mgroups_::sims_t::calc_t::matrix_c::polynom_c::polynom_fit_Crel_to_Irel_c::polynom_fit_Crel_to_Irel_c(const cluster_t& zaehler,const cluster_t& nenner,calc_t& calc, vector<unsigned int> rank, vector<double> fit_start_parameters) :
																				Crel_to_Irel_c(zaehler,nenner,calc),
																				polynom(rank,fit_start_parameters)
{
}

void mgroups_::sims_t::calc_t::matrix_c::polynom_c::polynom_fit_Crel_to_Irel_c::plot_to_screen(double sleep_sec) const
{
	const unsigned int data_points_for_fit_resolution=100;
	if (!polynom.fitted())	return;
	const auto Crel_to_Irel = known_Crels_from_sample_matrix_to_Irels_truncated_median();
	if (!Crel_to_Irel.first.is_set() || !Crel_to_Irel.second.is_set()) return;
	
	plot_t plot(false);
	
	stringstream title_X, title_Y, title_window;
	title_X << "Irel = (" << zaehler.to_string() << ") / (" << nenner.to_string() << ")";
	title_Y << "Crel = [" << zaehler.corresponding_isotope(calc.MG.matrix_isotopes()).to_string() << "] / [";
	title_Y << nenner.corresponding_isotope(calc.MG.matrix_isotopes()).to_string() << "]";
	title_window << title_Y.rdbuf() << " over " << title_X.rdbuf() << " chisq=" << polynom.chisq();
	
	//generate data to show the fit in high resolution
	const quantity_t x_res = Crel_to_Irel.second.resolution()/data_points_for_fit_resolution;
	const quantity_t X(title_X.str(),(Crel_to_Irel.second.change_resolution(x_res/data_points_for_fit_resolution)).data,units::SI::one); 
	logger::debug(15,"mgroups_::sims_t::calc_t::matrix_c::polynom_c::polynom_fit_Crel_to_Irel_c::plot_to_screen()",Crel_to_Irel.second.to_string_detailed());
	logger::debug(15,"mgroups_::sims_t::calc_t::matrix_c::polynom_c::polynom_fit_Crel_to_Irel_c::plot_to_screen()",x_res.to_string_detailed());
	logger::debug(15,"mgroups_::sims_t::calc_t::matrix_c::polynom_c::polynom_fit_Crel_to_Irel_c::plot_to_screen()",X.to_string_detailed());
	const quantity_t Y(title_Y.str(),polynom.fitted_y_data(X.data),units::SI::one);
	plot.Y1.add_curve(X,Y,"fit rank: " + tools::vec::to_string(polynom.rank));
	plot.Y1.add_points({X,Crel_to_Irel.second.data},{Y,Crel_to_Irel.first.data},"data");
	plot.to_screen(title_window.str(),sleep_sec);
}

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
	if (X_to_Y_map.size()<1 || X_to_Y_map.size() < polynom.rank.size())
	{
		logger::warning(3,"mgroups_::sims_t::calc_t::matrix_c::polynom_c::polynom_fit_Crel_to_Irel_c::execute_fit_polynom()","to less data points: " + tools::to_string(X_to_Y_map.size()),"returning false");
		return false;
	}
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

void mgroups_::sims_t::calc_t::matrix_c::polynom_c::matrix_isotope_concentration_c::plot_to_screen(double sleep_sec) const
{
	for (auto& poly_fit_Crel_2_Irel : polynom_fits_Crels_to_Irels)
		poly_fit_Crel_2_Irel.plot_to_screen(sleep_sec);
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
			logger::warning(3,"mgroups_::sims_t::calc_t::matrix_c::polynom_c::matrix_isotope_concentration_c::concentration()","fit was not successful; Crel not set","zaehler="+P.zaehler.to_string()+"; nenner="+P.nenner.to_string(),M.to_string_short());
			return {};
		}
	
		if (!results.is_set())
			results = Crel_;
		else
			results += Crel_;
	}
// 	[matrix_cluster] =  ( 1 + SUM )^-1
	results = ((results + 1).invert()) * 100;
	return {results.data,units::derived::atom_percent};
}
