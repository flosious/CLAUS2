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

/*******************************************/
/**** mgroups_::sims_t::calc_t::matrix_c ***/
/*******************************************/

mgroups_::sims_t::calc_t::matrix_c::matrix_c(calc_t& calc) : MG(calc.MG), calc(calc), measurements(calc.measurements)
{
}

// concentration_t mgroups_::sims_t::calc_t::matrix_c::concentration(measurements_::sims_t& M, const cluster_t& matrix_cluster)
// {
// 	vector<polynom_fit_Crel_to_Irel_c> polynoms;
// 	auto mat_Cs_in_M = M.matrix_clusters(MG.matrix_isotopes()).clusters;
// 	cluster_t* mc_nenner = M.cluster(matrix_cluster);
// 	if (mc_nenner == nullptr)
// 	{
// 		logger::error("mgroups_::sims_t::calc_t::matrix_c::concentration()","matrix_cluster is nullptr","returning empty");
// 		return {};
// 	}
// 	for (auto& MC : mat_Cs_in_M)
// 	{
// 		if (MC == mc_nenner ) continue;
// 		
// 	}
// }
// 


mgroups_::sims_t::calc_t& mgroups_::sims_t::calc_t::matrix_c::median_const_from_db(bool overwrite)
{
	for (auto& mat_C_to_I : MG.matrix_cluster_to_matrix_iso_abundance())
	{
		cluster_t* C = mat_C_to_I.first;
		
		if (!C->concentration.is_set() || overwrite)
		{
			C->concentration = concentration_t( (C->intensity / C->intensity.median() ) * mat_C_to_I.second);
			logger::debug(7,"mgroups_::sims_t::calc_t::matrix_c::median_const_from_db()",C->concentration.to_string());
			C->concentration.change_unit(units::derived::atom_percent);
		}
	}	
	return calc;
}

mgroups_::sims_t::calc_t& mgroups_::sims_t::calc_t::matrix_c::const_from_db(quantity_t (*operation) (quantity_t), bool overwrite)
{
	for (auto& mat_C_to_I : MG.matrix_cluster_to_matrix_iso_abundance())
	{
		cluster_t* C = mat_C_to_I.first;
// 		isotope_t* mat_iso = mat_C_to_I.second;
		if (!C->concentration.is_set() || overwrite)
		{
			C->concentration = concentration_t( (C->intensity / operation(C->intensity) ) * mat_C_to_I.second).change_unit(units::derived::atom_percent);
// 			logger::debug(7,"mgroups_::sims_t::calc_t::matrix_c::const_from_db()",C->concentration.to_string());
		}
	}	
	return calc;
}

bool mgroups_::sims_t::calc_t::matrix_c::intensities_are_set() const
{
	bool result = true;
	auto MCs = MG.matrix_clusters();
	for (auto& M : measurements)
	{
		for (auto& C : MCs)
		{
			auto CC = tools::find_in_V(C,M->clusters);
			if (CC==nullptr)
			{
				logger::error("mgroups_::sims_t::calc_t::matrix_c::intensities_are_set()","could NOT find cluster " + C.to_string() + " in measurement " + M->to_string(),"returning false");
				result = false;
				continue;
			}
			if (!CC->intensity.is_set())
			{
				logger::error("mgroups_::sims_t::calc_t::matrix_c::intensities_are_set()","intensity is not set in cluster " + C.to_string() + " in measurement " + M->to_string(),"returning false");
				result = false;
				continue;
			}
		}
	}
	return result;
}

// vector<mgroups_::sims_t::calc_t::matrix_c::polynom_fit_Crel_to_Irel_c> mgroups_::sims_t::calc_t::matrix_c::polynom_fits_Crels_to_Irels(const cluster_t& matrix_cluster, vector<unsigned int> rank, vector<double> fit_start_parameters)
// {
// 	vector<mgroups_::sims_t::calc_t::matrix_c::polynom_fit_Crel_to_Irel_c> results;
// 	const auto matrix_clusters = MG.matrix_clusters();
// // 	isotope_t E_j = matrix_cluster.corresponding_isotope(MG.matrix_isotopes());
// 	for (auto& Cz : matrix_clusters)
// 	{
// 		if (Cz == matrix_cluster) continue;
// 		results.push_back({Cz,matrix_cluster,calc,rank,fit_start_parameters});
// 	}
// 	return results;
// }

// map<isotope_t, vector<mgroups_::sims_t::calc_t::matrix_c::Crel_to_Irel_c>> mgroups_::sims_t::calc_t::matrix_c::isotopes_to_Crels_from_Irels(const vector<unsigned int> polynom_rank) const
// {
// 	vector<double> polynom_fit_start_parameters(polynom_rank.size(),0);
// 	for (int i=0;i<polynom_rank.size();i++)
// 		polynom_fit_start_parameters.at(i) = polynom_rank.at(i);
// // 	logger::debug(11,"mgroups_::sims_t::calc_t::matrix_c::isotopes_to_Crels_from_Irels()","polynom_rank=",tools::vec::to_string(polynom_rank));
// 	logger::debug(11,"mgroups_::sims_t::calc_t::matrix_c::isotopes_to_Crels_from_Irels()","polynom_fit_start_parameters=",tools::vec::to_string(polynom_fit_start_parameters));
// 	
// 	const auto matrix_isotopes = MG.matrix_isotopes();
// 	
// 	map<isotope_t, vector<mgroups_::sims_t::calc_t::matrix_c::Crel_to_Irel_c>> Ej_2_Crels_from_Irels;
// // 	for (auto& Cn : MG.matrix_clusters())
// // 	{
// // 		isotope_t E_j = Cn.corresponding_isotope(matrix_isotopes);
// // 		Ej_2_Crels_from_Irels.insert(pair<isotope_t, vector<mgroups_::sims_t::calc_t::matrix_c::Crel_to_Irel_c>> (E_j,{}) );
// // 	}
// 	
// 	for (auto& Cz : MG.matrix_clusters())
// 	{
// 		for (auto& Cn : MG.matrix_clusters())
// 		{
// 			if (Cn == Cz) continue; // this will lead to a +1 within the formular
// 			isotope_t E_j = Cn.corresponding_isotope(matrix_isotopes);
// 			if (Ej_2_Crels_from_Irels.find(E_j)==Ej_2_Crels_from_Irels.end())
// 				Ej_2_Crels_from_Irels.insert(pair<isotope_t,vector<mgroups_::sims_t::calc_t::matrix_c::Crel_to_Irel_c> > (E_j,{Crel_to_Irel_c(Cz,Cn,calc,polynom_rank,polynom_fit_start_parameters)}));
// 			else
// 				Ej_2_Crels_from_Irels.at(E_j).push_back(Crel_to_Irel_c(Cz,Cn,calc,polynom_rank,polynom_fit_start_parameters));
// 		}
// 	}
// 	return Ej_2_Crels_from_Irels;
// }

/*
 * this function interpolates each matrix cluster from gives matrix isotopes from reference samples (database)
 * rank = {0,1,0} means linear RSFs without offset (0*c0*x^0 + 1*c1*x^1 + 0*c2*x^2)
 */ 
mgroups_::sims_t::calc_t& mgroups_::sims_t::calc_t::matrix_c::interpolate(const vector<unsigned int> polynom_rank, bool overwrite)
{
	/*
	 * MG.matrix_clusters() contains all clusters for all matrices of all measurements
	 * that means there could be measurements which have not the particular cluster, e.g.:
	 * measurement of reference Si sample not necessary need Ge cluster, except for background level determination
	 * this problem will be worked around by introducing a check_cluster_consistency() function for the whole MG
	 * this function will only work for same matrix_clusters for all measurements within this group
	*/
	if (!MG.check_matrix_cluster_consistency())
	{
		logger::error("mgroups_::sims_t::calc_t::matrix_c::interpolate()","MG.check_matrix_cluster_consistency returned false","returning");
		return calc;
	}
	if (!intensities_are_set())
	{
		logger::error("mgroups_::sims_t::calc_t::matrix_c::interpolate()","not all intensities are set","returning");
		return calc;
	}
	vector<double> fit_start_parameters(polynom_rank.size(),0);
	polynom_c polyfit(polynom_rank,fit_start_parameters,calc);
	const auto matrix_isotopes = MG.matrix_isotopes();
	for (auto& M : measurements)
	{
		///use results no matter what
		if (overwrite)
			polyfit.concentration(*M);
		else
		{
			auto M_copy = *M;
			if (!polyfit.concentration(M_copy)) continue;
			for (auto& MC : M->matrix_clusters(matrix_isotopes).clusters)
			{
				if (!MC->concentration.is_set())
					MC->concentration = M_copy.cluster(*MC)->concentration;
			}
		}
	}
	
	//show me 
	polyfit.plot_to_screen(0);
	
	return calc;
}

// bool mgroups_::sims_t::calc_t::matrix_c::fit_isotopes_to_Crels_from_Irels()
// {
// 	bool result = true;
// 	//fit all supporting points for each (E_i)/(E_j) -> [E_i]/[E_j]
// 	for (auto& Ej_2_Crels_from_Irels : isotopes_2_Crels_from_Irels)
// 	{
// 		for (auto& Crel_2_Irel : Ej_2_Crels_from_Irels.second)
// 		{
// 			result = Crel_2_Irel.execute_fit_polynom();
// 		}
// 	}
// // 	return result;
// }

// const vector<quantity_t> mgroups_::sims_t::calc_t::matrix_c::Crels(const element_t& element)
// {
// 	for (auto& C1 : MG.matrix_clusters())
// 	{
// 		for (auto& C2 : MG.matrix_clusters())
// 		{
// 			if (C1==C2) continue;
// 			Crel_to_Irel_c Crel_to_Irel(C1,C2,calc);
// 		}
// 	}
// 	return {};
// }


/****************************************************/
/**********   Crel_to_Irel_c      *******************/
/****************************************************/

// mgroups_::sims_t::calc_t::matrix_c::Crel_to_Irel_c::Crel_to_Irel_c(const cluster_t& zaehler,const cluster_t& nenner,calc_t& calc, matrix_c& matrix_c_ref) :
// 																				zaehler(zaehler), nenner(nenner), calc(calc), matrix_c_ref(matrix_c_ref)
// 																				,polynom(matrix_c_ref.rank,matrix_c_ref.fit_starting_parameters)
// {
// }

mgroups_::sims_t::calc_t::matrix_c::Crel_to_Irel_c::Crel_to_Irel_c(const cluster_t& zaehler,const cluster_t& nenner,calc_t& calc) :
																				zaehler(zaehler), nenner(nenner), calc(calc)
{
}

const concentration_t mgroups_::sims_t::calc_t::matrix_c::Crel_to_Irel_c::Crel_template = concentration_t({},units::SI::one);

const intensity_t mgroups_::sims_t::calc_t::matrix_c::Crel_to_Irel_c::Irel(const measurements_::sims_t& M) const
{
	const cluster_t* Z = M.cluster(zaehler);
	const cluster_t* N = M.cluster(nenner);
	if (Z == nullptr || N == nullptr)
	{
		logger::error("mgroups_::sims_t::calc_t::matrix_c::Crel_to_Irel_c::Irel()","nullptr in M= " + M.to_string_short(),zaehler.to_string(),nenner.to_string());
		return {};
	}
	if (!Z->intensity.is_set() || !N->intensity.is_set())
		return {};
	return {(Z->intensity) / (N->intensity)};
}

const pair<quantity_t,quantity_t> mgroups_::sims_t::calc_t::matrix_c::Crel_to_Irel_c::known_Crels_from_sample_matrix_to_Irels_truncated_median() const
{
	quantity_t Crel, Irel;
	const auto matrix_isotopes = calc.MG.matrix_isotopes();
	for (auto& M : calc.measurements)
	{
		if (!M->sample->matrix().is_set()) continue;
		auto Z = M->cluster(zaehler);
		if (Z==nullptr) continue;
		if (!Z->intensity.is_set()) continue;
// 		if (!Z->concentration.is_set()) continue;
		auto N = M->cluster(nenner);
		if (N==nullptr) continue;
		if (!N->intensity.is_set()) continue;
// 		if (!N->concentration.is_set()) continue;
		
		auto Zmat_iso = M->sample->matrix().isotope(zaehler.corresponding_isotope(matrix_isotopes));
		auto Nmat_iso = M->sample->matrix().isotope(nenner.corresponding_isotope(matrix_isotopes));
		
		if (Nmat_iso == nullptr || !Nmat_iso->substance_amount.is_set()) 
			continue; //skip division by 0
		if (Zmat_iso == nullptr || !Zmat_iso->substance_amount.is_set())
			Crel << (substance_amount_t({0})/substance_amount_t({1})); // this adds always a 0
		else
			Crel << (Zmat_iso->substance_amount / Nmat_iso->substance_amount);
		
		Irel << (Z->intensity.remove_data_from_begin(Z->intensity.data.size()/2).median() / N->intensity.remove_data_from_begin(N->intensity.data.size()/2).median());
		
		if (!Irel.is_set())
			logger::warning(3,"mgroups_::sims_t::calc_t::matrix_c::Crel_to_Irel_c::known_Crels_from_sample_matrix_to_Irels_truncated_median()","Irel is not set for " + zaehler.to_string() +" / " + nenner.to_string(),"ignoring");
		if (!Crel.is_set())
			logger::warning(3,"mgroups_::sims_t::calc_t::matrix_c::Crel_to_Irel_c::known_Crels_from_sample_matrix_to_Irels_truncated_median()","Crel is not set for " + Zmat_iso->to_string() +" / " + Nmat_iso->to_string(),"ignoring");
	}
// 	Crel_template = quantity_t(Crel.name(),{},Crel.unit());
	logger::debug(7,"mgroups_::sims_t::calc_t::matrix_c::Crel_to_Irel_c::known_Crels_from_sample_matrix_to_Irels_truncated_median()","Z="+zaehler.to_string()+" N="+nenner.to_string(),"Irel=" + Irel.to_string(), "Crel=" + Crel.to_string());
	logger::debug(7,"mgroups_::sims_t::calc_t::matrix_c::Crel_to_Irel_c::known_Crels_from_sample_matrix_to_Irels_truncated_median()","Crel",Crel.to_string_detailed());
	logger::debug(7,"mgroups_::sims_t::calc_t::matrix_c::Crel_to_Irel_c::known_Crels_from_sample_matrix_to_Irels_truncated_median()","Irel",Irel.to_string_detailed());
	
// 	plot_t plot;
// 	plot.Y2.log10_scale=false;
// 	plot.Y2.add_points(Irel,Crel,"");
// 	plot.to_screen(zaehler.to_string() +" / " + nenner.to_string(),0);
	
	return {Crel,Irel};
}

const pair<quantity_t,quantity_t> mgroups_::sims_t::calc_t::matrix_c::Crel_to_Irel_c::known_Crels_from_Clusters_to_Irels() const
{
	quantity_t Crel, Irel;
	for (auto& M : calc.measurements)
	{
// 		if (!M->sample->matrix().is_set()) continue;
		auto Z = M->cluster(zaehler);
		if (Z==nullptr) continue;
		if (!Z->intensity.is_set()) continue;
		if (!Z->concentration.is_set()) continue;
		auto N = M->cluster(nenner);
		if (N==nullptr) continue;
		if (!N->intensity.is_set()) continue;
		if (!N->concentration.is_set()) continue;
		
		Irel << (Z->intensity / N->intensity);
		Crel << (Z->concentration / N->concentration);
	}
// 	Crel_template = quantity_t(Crel.name(),{},Crel.unit());
	logger::debug(7,"mgroups_::sims_t::calc_t::matrix_c::Crel_to_Irel_c::known_Crels_from_Clusters_to_Irels()","Z="+zaehler.to_string()+" N="+nenner.to_string(),"Irel=" + Irel.to_string(), "Crel=" + Crel.to_string());
	
// 	plot_t plot;
// 	plot.Y2.log10_scale=false;
// 	plot.Y1.add_curve(Irel,Crel,"");
// 	plot.Y2.add_curve(Irel,Crel,"");
// 	plot.to_screen(zaehler.to_string() +" / " + nenner.to_string(),0);
	
	return {Crel,Irel};
}

