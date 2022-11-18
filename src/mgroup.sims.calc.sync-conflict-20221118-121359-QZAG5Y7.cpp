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

mgroups_::sims_t::calc_t::~calc_t()
{
}

mgroups_::sims_t::calc_t::calc_t(sims_t& MG) :
    MG(MG), SRs(*this), SDs(*this), SFs(*this), RSFs(*this), matrices(*this), concentrations(*this), measurements(MG.measurements())
{
}

mgroups_::sims_t& mgroups_::sims_t::calc_t::references(bool overwrite)
{
	for (auto& M : measurements)
	{
		measurements_::sims_t::calc_t c = M->calc(overwrite);
		//SR
		c.SR.from_crater_depths().SR.from_implant_max();
		//SD
		c.SD.from_SR();
		//SF
		c.SF.from_db_dose().SF.from_db_max();
		//RSF 
		c.RSF.from_SF_mean_ref();
	}
	return MG;
}

mgroups_::sims_t::calc_t& mgroups_::sims_t::calc_t::SR_SF_from_implants_maxima(bool overwrite)
{
	for (auto& M : measurements)
	{
		M->calc().SR.from_implant_max(overwrite).SF.from_db_max(overwrite);
	}
	return *this;
}

mgroups_::sims_t::calc_t& mgroups_::sims_t::calc_t::normalize_to_ref_intensity(bool overwrite)
{
	for (auto& M : measurements)
	{
		for (auto& C : M->clusters)
		{
			quantity::intensity_t Iref = M->matrix_clusters().intensity_sum();
// 			std::cout << std::endl << Iref.to_string_detailed() << std::endl;
			
			if (C.intensity.is_set() && Iref.is_set() && !M->matrix_clusters().is_cluster_in_matrix(C))
			{
                //logger::debug(11,"mgroups_::sims_t::calc_t::normalize_to_ref_intensity()","Iref="+Iref.to_string_detailed());
				C.intensity = ((C.intensity / Iref ) * Iref.median());
// 				std::cout << std::endl << C.intensity.to_string_detailed() << std::endl;
			}
		}
// 		M->plot_now(0);
	}
	return *this;
}

// const std::vector<mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c> mgroups_::sims_t::calc_t::polynom_fit_Crels_to_Irels(
// 											std::vector<cluster_t> 		cluster_names,
// 											std::vector<unsigned int> 	rank, 
// 											std::vector<double> 			fit_start_parameters)
// {
// 	std::vector<mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c> polynom_fit_Crel_to_Irelvec;
// 	for (auto cluster_name_zaehler:cluster_names)
// 	{
// 		for (auto cluster_name_nenner:cluster_names)
// 		{
// 			if (cluster_name_nenner == cluster_name_zaehler)
// 				continue;
// 			polynom_fit_Crel_to_Irelvec.push_back({cluster_name_zaehler,cluster_name_nenner,*this,rank,fit_start_parameters});
// 		}
// 	}
// 	return polynom_fit_Crel_to_Irelvec;
// }
// 
// const std::vector<mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c> mgroups_::sims_t::calc_t::proportional_fit_Crels_to_Irels(std::vector<cluster_t> 		cluster_names)
// {
// 	return polynom_fit_Crels_to_Irels(cluster_names,{0,1},{0,1});
// }


/*************************/
/*****   SR_c  **********/
/*************************/

mgroups_::sims_t::calc_t::SR_c::SR_c(calc_t& calc)
    : MG(calc.MG), calc(calc), measurements(calc.measurements),
      logger(__FILE__,"mgroups_::sims_t::calc_t::SR_c")
{
}

mgroups_::sims_t::calc_t& mgroups_::sims_t::calc_t::SR_c::from_crater_depths(bool overwrite)
{
	for (auto& M : measurements)
	{
		if (!overwrite && M->crater.SR.is_set()) continue;
// 		if (save_calc_esults)
// 			MG.saved_calc_results.at(M).SR.from_crater_depths();
// 		else
			M->calc().SR.from_crater_depths();
// 		M->calc().SR.from_crater_depths();
	}
	return calc;
}

mgroups_::sims_t::calc_t & mgroups_::sims_t::calc_t::SR_c::from_implant_max(bool overwrite)
{
	for (auto& M : measurements)
	{
		if (!overwrite && M->crater.SR.is_set()) continue;
		auto C = M->calc();
		C.SR.from_implant_max();
	}
	return calc;
}

mgroups_::sims_t::calc_t& mgroups_::sims_t::calc_t::SR_c::copy_to_same_matrices(bool overwrite)
{
	auto& mat_to_SRs = MG.matrix_to_SRs();
	if (mat_to_SRs.size()==0)
		return calc;
	
	for (auto& M : measurements)
	{
        if (!M->sample.matrix().is_set()) continue;
		for (auto& mat_to_SR : mat_to_SRs)
		{
            if (M->sample.matrix() == mat_to_SR.first)
			{
				if (mat_to_SR.second.is_vector())
				{
					/*	this is unhandled at the moment, but will be important when reference samples with layer of different matrices are used.
					 *	one could use all the mean(SR point at matrix concentration) ... yeah maybe later...;-)
					 */
                    //logger::error("mgroups_::sims_t::calc_t::SR_c::copy_to_same_matrices()","SR is a vector, dont know how to handle, tell florian",M->to_string(),"ignoring");
				}
				if (overwrite || !M->crater.SR.is_set())
					M->crater.SR = mat_to_SR.second;
			}
		}
	}
	
	return calc;
}

mgroups_::sims_t::calc_t& mgroups_::sims_t::calc_t::SR_c::interpolate_from_known_sample_matrices(std::vector<unsigned int> rank, bool overwrite)
{
	if (MG.matrix_clusters().size()!=2)
	{
        //logger::error("mgroups_::sims_t::calc_t::SR_c::interpolate_from_known_sample_matrix()","MG.matrix_isotopes().size()!=2","returning calc");
		return calc;
	}
	const isotope_t relevant_isotope = MG.matrix_isotopes().front();
	data_collectors_t::sims_t dc(MG.measurements());
	quantity::table_t SR_vs_C_table;
	SR_vs_C_table.add(dc.get_column_concentrations_from_sample_matrix(relevant_isotope));
	SR_vs_C_table.add(dc.get_column_sputter_rates());
	auto SR_vs_C = SR_vs_C_table.remove_empty_lines().get_map();
	const auto P =SR_vs_C.polynom(rank,{rank.begin(),rank.end()});
	
	//plot
	if (!P.successfully_fitted())
	{
        //logger::warning(2,"mgroups_::sims_t::calc_t::SR_c::interpolate_from_known_sample_matrices","polynom not succesfully fitted: " + SR_vs_C.to_string());
		return calc;
	}
//	plot_t plot(false,false);
//	plot.Y1.add_points(SR_vs_C,"SR vs " + relevant_isotope.to_string()," Ro");
//	plot.Y1.add_polynom(SR_vs_C,P,P.to_string());
//	plot.to_screen("SR_vs_C",0);
	
	//copy to other measurements
	for (auto& M : MG.measurements())
	{
		if (!overwrite && M->crater.SR.is_set())
			continue;
		cluster_t* relevant_C = M->matrix_clusters().cluster(relevant_isotope);
		if (relevant_C==nullptr)
			continue;
		if (!relevant_C->concentration.is_set())
			continue;
		
		M->crater.SR = SR_vs_C.polyfit(relevant_C->concentration,P); //SR point by point
	}
	return calc;
}


/*************************/
/******   SD_c  **********/
/*************************/

mgroups_::sims_t::calc_t::SD_c::SD_c(calc_t& calc)
    : MG(calc.MG), calc(calc), measurements(calc.measurements),
      logger(__FILE__,"mgroups_::sims_t::calc_t::SD_c")
{
}

mgroups_::sims_t::calc_t & mgroups_::sims_t::calc_t::SD_c::from_SR(bool overwrite)
{
	for (auto& M : measurements)
	{
		if (!overwrite && M->crater.sputter_depth.is_set()) 
			continue;
// 		if (save_calc_esults)
// 			MG.saved_calc_results.at(M).SD.from_SR();
// 		else

		M->calc().SD.from_SR(overwrite);
	}
	return calc;
}

/*************************/
/*****   SF_c  **********/
/*************************/

mgroups_::sims_t::calc_t::SF_c::SF_c(calc_t& calc)
    : MG(calc.MG), calc(calc), measurements(calc.measurements),
      class_logger(__FILE__,"mgroups_::sims_t::calc_t::SF_c")
{
}

mgroups_::sims_t::calc_t& mgroups_::sims_t::calc_t::SF_c::from_RSF_median_ref(bool overwrite)
{
	for (auto& rsf_to_ref : RSFs_to_ref_intensities())
	{
		if (overwrite || !rsf_to_ref.first->SF.is_set())
		{
			rsf_to_ref.first->SF = quantity::SF_t (rsf_to_ref.first->RSF / rsf_to_ref.second.median()); // this is the median of a sum; not the sum of the medians!
		}
	}
	return calc;
}
mgroups_::sims_t::calc_t& mgroups_::sims_t::calc_t::SF_c::from_RSF_pbp_ref(bool overwrite)
{
    log_f;
	for (auto& rsf_to_ref : RSFs_to_ref_intensities())
	{
		if (overwrite || !rsf_to_ref.first->SF.is_set())
		{
			rsf_to_ref.first->SF = quantity::SF_t (rsf_to_ref.first->RSF / rsf_to_ref.second); 
		}
	}
	return calc;
}


const std::map<cluster_t*,quantity::intensity_t> mgroups_::sims_t::calc_t::SF_c::RSFs_to_ref_intensities()
{
    log_f;
	std::map<cluster_t*,quantity::intensity_t> rsf_to_ref_intensity;
	for (auto& M : measurements)
	{
		auto matrix_clusters = M->matrix_clusters();
		if (!matrix_clusters.intensity_sum().is_set())
		{
//            logger.error(M->to_string_short() + "intensity_sum: ").value("not set");
            //logger::error("mgroups_::sims_t::calc_t::RSF_c::RSFs_to_ref_intensities()","!M->reference_intensity().is_set()","continue");
			continue;
		}
		for (auto& C : M->clusters)
		{
			if (!C.RSF.is_set()) continue;
				rsf_to_ref_intensity.insert(std::pair<cluster_t*,quantity::intensity_t> (&C,matrix_clusters.intensity_sum()));
		}
	}
	return rsf_to_ref_intensity;
}

mgroups_::sims_t::calc_t & mgroups_::sims_t::calc_t::SF_c::from_implant_dose(bool overwrite)
{
    log_f;
	for (auto& M : measurements)
	{
		for (auto& C : M->clusters)
		{
			//skip cluster belonging to matrix
			if (M->matrix_clusters().is_cluster_in_matrix(C)) 
				continue;
			if (overwrite || !C.SF.is_set())
			{
				C.SF = M->calc().SF.from_db_dose(C).change_unit(units::derived::atoms_per_ccm/(units::derived::counts/units::SI::second));
                if (C.SF.is_set())
                    logger.info(M->to_string_short() + "->"+ C.to_string()).value(C.SF.to_string_short());
                else
                    logger.debug(M->to_string_short() + "->"+ C.to_string()).value("SF was not calculatable");
			}
		}
	}
	return calc;
}

mgroups_::sims_t::calc_t & mgroups_::sims_t::calc_t::SF_c::from_implant_max(bool overwrite)
{
	for (auto& M : measurements)
	{
		for (auto& C : M->clusters)
		{
			//skip cluster belonging to matrix
			if (M->matrix_clusters().is_cluster_in_matrix(C)) 
				continue;
			if (overwrite || !C.SF.is_set()) 
				C.SF = M->calc().SF.from_db_max(C);
		}
	}
	return calc;
}

/*************************/
/*****  RSF_c    *********/
/*************************/

mgroups_::sims_t::calc_t::RSF_c::RSF_c(calc_t& calc)
    : MG(calc.MG), calc(calc), measurements(calc.measurements),
      logger(__FILE__,"mgroups_::sims_t::calc_t::RSF_c")
{
}

const std::map<cluster_t*,quantity::intensity_t> mgroups_::sims_t::calc_t::RSF_c::clusters_to_ref_intensities()
{
	std::map<cluster_t*,quantity::intensity_t> clusters_to_ref_intensity;
	for (auto& M : measurements)
	{
		auto matrix_clusters = M->matrix_clusters();
		if (!matrix_clusters.intensity_sum().is_set())
		{
            //logger::error("mgroups_::sims_t::calc_t::RSF_c::clusters_to_ref_intensities()","!M->reference_intensity().is_set(), " + M->to_string_short(),"continue");
			continue;
		}
		for (auto& C : M->clusters)
		{
			if (!C.SF.is_set()) continue;
			clusters_to_ref_intensity.insert(std::pair<cluster_t*,quantity::intensity_t> (&C,matrix_clusters.intensity_sum()));
		}
	}
	return clusters_to_ref_intensity;
}

mgroups_::sims_t::calc_t& mgroups_::sims_t::calc_t::RSF_c::from_SF_mean_ref(bool overwrite)
{
	for (auto& sf_to_ref : clusters_to_ref_intensities())
	{
		if (overwrite || !sf_to_ref.first->RSF.is_set())
			sf_to_ref.first->RSF = cluster_t::RSF_t (sf_to_ref.first->SF * sf_to_ref.second.mean()).change_unit(units::derived::atoms_per_ccm); // this is the mean of a sum; not the sum of the means! But math says it should be the same
	}
// 	quantity::quantity_t q_ref;
// 	const quantity::table_t table_refs(MG.ref_intensities());
// 	if (!table_refs.is_set())
// 	{
// 		logger::error("mgroups_::sims_t::calc_t::RSF_c::from_SF_mean_ref","!table_refs.is_set()","returning");
// 		return calc;
// 	}
// 	
// 	for (auto C : MG.clusters())
// 	{
// 		quantity::table_t SF_table(table_refs);
// 		SF_table.add(MG.intensities(C));
// 	}
	return calc;
}

mgroups_::sims_t::calc_t& mgroups_::sims_t::calc_t::RSF_c::from_SF_median_ref(bool overwrite)
{
	for (auto& sf_to_ref : clusters_to_ref_intensities())
	{
		if (overwrite || !sf_to_ref.first->RSF.is_set())
			sf_to_ref.first->RSF = cluster_t::RSF_t (sf_to_ref.first->SF * sf_to_ref.second.median()).change_unit(units::derived::atoms_per_ccm); // this is the median of a sum; not the sum of the medians!
	}
	return calc;
}

mgroups_::sims_t::calc_t& mgroups_::sims_t::calc_t::RSF_c::from_SF_pbp_ref(bool overwrite)
{
	for (auto& sf_to_ref : clusters_to_ref_intensities())
	{
		if (overwrite || !sf_to_ref.first->RSF.is_set())
			sf_to_ref.first->RSF = cluster_t::RSF_t (sf_to_ref.first->SF * sf_to_ref.second).change_unit(units::derived::atoms_per_ccm); // this is the median of a sum; not the sum of the medians!
	}
	return calc;
}

mgroups_::sims_t::calc_t & mgroups_::sims_t::calc_t::RSF_c::copy_to_same_matrices(bool overwrite)
{
	const auto cs = MG.clusters();
	 
	for (auto& C : cs)
	{
		//for the specific cluster C
		auto mat_to_RSF = MG.matrix_to_RSF(C);
		if (mat_to_RSF.size()==0) 
			continue; // no RSFs at all for this cluster at all
		for (auto& M : measurements)
		{
            if (!M->sample.matrix().is_set())
				continue; // unknown matrix
            auto mat = mat_to_RSF.find(M->sample.matrix());
			if (mat==mat_to_RSF.end()) 
				continue; // no RSF for this matrix and cluster
			if (mat->second.is_vector())
			{
				/*	this is unhandled at the moment, but will be important when reference samples with layer of different matrices are used.
				*	one could use all the mean(RSF point at matrix concentration) ... yeah maybe later...;-)
				*/
                //logger::error("mgroups_::sims_t::calc_t::RSF_c::from_others()","RSF is a vector, dont know how to handle, tell florian",M->to_string(),"ignoring");
				continue;
			}
			if (overwrite || !M->cluster(C)->RSF.is_set())
				M->cluster(C)->RSF = mat->second;
		}
	}
	return calc;
}

mgroups_::sims_t::calc_t & mgroups_::sims_t::calc_t::RSF_c::interpolate_from_known_sample_matrices(std::vector<unsigned int> rank, bool overwrite)
{
	const std::set<cluster_t> clusters = MG.clusters();
	for (const auto& c : clusters)
		interpolate_from_known_sample_matrices(c);
	return calc;
}

mgroups_::sims_t::calc_t & mgroups_::sims_t::calc_t::RSF_c::interpolate_from_known_sample_matrices(const cluster_t& cluster, std::vector<unsigned int> rank, bool overwrite)
{
	if (MG.matrix_clusters().size()!=2)
	{
        //logger::error("mgroups_::sims_t::calc_t::RSF_c::interpolate_from_known_sample_matrix()","MG.matrix_isotopes().size()!=2","returning calc");
		return calc;
	}
	const isotope_t relevant_isotope = MG.matrix_isotopes().front();
	data_collectors_t::sims_t dc(MG.measurements());
	quantity::table_t RSF_vs_C_table;
	RSF_vs_C_table.add(dc.get_column_concentrations_from_sample_matrix(relevant_isotope));
	RSF_vs_C_table.add(dc.get_column_RSFs_from_cluster(cluster));
	auto RSF_vs_C = RSF_vs_C_table.remove_empty_lines().get_map();
	const auto P =RSF_vs_C.polynom(rank,{rank.begin(),rank.end()});
	
	//plot
	if (!P.successfully_fitted())
		return calc;
//	plot_t plot(false,false);
//	plot.Y1.add_points(RSF_vs_C,"RSF("+ cluster.to_string() +") vs " + relevant_isotope.to_string()," Ro");
//// 	plot.Y1.add_curve(RSF_vs_C.polyfit(P,100),P.to_string());
//	plot.Y1.add_polynom(RSF_vs_C,P,P.to_string());
//	plot.to_screen("RSF("+ cluster.to_string() +")_vs_C",0);
	
	//copy to other measurements
	for (auto& M : MG.measurements())
	{
		const auto c_in_m = M->cluster(cluster);
		if (c_in_m == nullptr)
			continue;
		if (!overwrite && c_in_m->RSF.is_set())
			continue;
		cluster_t* relevant_C = M->matrix_clusters().cluster(relevant_isotope);
		if (relevant_C==nullptr)
			continue;
		if (!relevant_C->concentration.is_set())
			continue;
		c_in_m->RSF = RSF_vs_C.polyfit(relevant_C->concentration,P); //SR point by point
        //logger::debug(5,"mgroups_::sims_t::calc_t::RSF_c::interpolate_from_known_sample_matrices","RSF: "+c_in_m->RSF.to_string()+" of "+ c_in_m->to_string() +" copied");
	}
	return calc;
}


/*************************/
/**   concentration_c  **/
/*************************/

mgroups_::sims_t::calc_t::concentration_c::concentration_c(calc_t& calc)
    : MG(calc.MG), calc(calc), measurements(calc.measurements),
      logger(__FILE__,"mgroups_::sims_t::calc_t::concentration_c")
{
}

mgroups_::sims_t::calc_t& mgroups_::sims_t::calc_t::concentration_c::from_SF(bool overwrite)
{
	for (auto& M : measurements)
	{
		for (auto& C : M->clusters)
		{
			if (C.concentration.is_set() && !overwrite) continue;
			//skip cluster belonging to matrix
			if (M->matrix_clusters().is_cluster_in_matrix(C)) continue;
// 			if (save_calc_esults)
// 				MG.saved_calc_results.at(M).concentration.from_SF(C);
// 			else
			C.concentration = M->calc().concentration.from_SF(C);
		}
	}
	return calc;
}

/****************************************************/
/**********   Crel_to_Irel_c      *******************/
/****************************************************/


mgroups_::sims_t::calc_t::Crel_to_Irel_c::Crel_to_Irel_c(const cluster_t& zaehler,const cluster_t& nenner,calc_t& calc) :
                                                                                zaehler(zaehler), nenner(nenner), calc(calc),
                                                                                logger(__FILE__,"mgroups_::sims_t::calc_t::Crel_to_Irel_c")
{
}

const quantity::concentration_t mgroups_::sims_t::calc_t::Crel_to_Irel_c::Crel_template = quantity::concentration_t({},units::SI::one);

const quantity::intensity_t mgroups_::sims_t::calc_t::Crel_to_Irel_c::Irel(const measurements_::sims_t* M) const
{
	const cluster_t* Z = M->cluster(zaehler);
	const cluster_t* N = M->cluster(nenner);
	if (Z == nullptr || N == nullptr)
	{
        //logger::error("mgroups_::sims_t::calc_t::Crel_to_Irel_c::Irel()","nullptr in M= " + M->to_string_short(),zaehler.to_string(),nenner.to_string());
		return {};
	}
	if (!Z->intensity.is_set() || !N->intensity.is_set())
		return {};
	return {(Z->intensity) / (N->intensity)};
}

const quantity::intensity_t mgroups_::sims_t::calc_t::Crel_to_Irel_c::Irel_from_truncated_medians(const measurements_::sims_t* M) const
{
	const cluster_t* Z = M->cluster(zaehler);
	const cluster_t* N = M->cluster(nenner);
	if (Z == nullptr || N == nullptr)
	{
        //logger::error("mgroups_::sims_t::calc_t::Crel_to_Irel_c::Irel()","nullptr in M= " + M->to_string_short(),zaehler.to_string(),nenner.to_string());
		return {};
	}
	if (!Z->intensity.is_set() || !N->intensity.is_set())
		return {};
	const auto s = Z->intensity.data().size()/2;
	return {(Z->intensity.remove_data_from_begin(s).median()) / (N->intensity.remove_data_from_begin(s).median())};
}

const quantity::concentration_t mgroups_::sims_t::calc_t::Crel_to_Irel_c::Crel_from_sample(const measurements_::sims_t* M) const
{
	quantity::concentration_t Crel;
	std::cout << "M*:\t" << M << std::endl;
    if (M->sample.matrix().isotope({"70Ge"})==nullptr)
		std::cout << "nullptr" << std::endl;
	else
        std::cout << M->sample.matrix().isotope({"70Ge"}) << std::endl;
	return {};
	const auto matrix_isotopes = calc.MG.matrix_isotopes();
	std::cout << "measurements:" << std::endl;
	for (auto MM : calc.MG.measurements())
		std::cout << MM->to_string_short() << std::endl;
	
	std::cout << "M.to_string_short():"  << std::endl;
	std::cout << M->to_string_short() << std::endl;
    std::cout << "M.sample.matrix():"  << std::endl;
    sample_t::matrix_t mat = M->sample.matrix();
    std::cout << "M.sample.matrix().to_string():"  << std::endl;
	std::cout << mat.to_string() << std::endl;
    std::cout << "M.sample.matrix().isotope({'70Ge'})" <<std::endl;
	
	std::cout << "zaehler.corresponding_isotope(matrix_isotopes)=" << zaehler.corresponding_isotope(matrix_isotopes).to_string() << std::endl;
	std::cout << "nenner.corresponding_isotope(matrix_isotopes)=" << nenner.corresponding_isotope(matrix_isotopes).to_string() << std::endl;
	
	
    std::cout << "Zmat_iso:" << std::endl;

    const isotope_t* Zmat_iso = M->sample.matrix().isotope(zaehler.corresponding_isotope(matrix_isotopes));
    const isotope_t* Nmat_iso = M->sample.matrix().isotope(nenner.corresponding_isotope(matrix_isotopes));
	if (Nmat_iso == nullptr || !Nmat_iso->substance_amount.is_set()) 
		return {}; //division by 0
	if (Zmat_iso == nullptr || !Zmat_iso->substance_amount.is_set())
		return quantity::concentration_t({0},Crel_template.unit()); // always add 0
	return quantity::concentration_t((Zmat_iso->substance_amount / Nmat_iso->substance_amount).data(),Crel_template.unit());
}

const std::pair<quantity::quantity_t,quantity::quantity_t> mgroups_::sims_t::calc_t::Crel_to_Irel_c::known_Crels_from_sample_matrix_to_Irels_truncated_median() const
{
	quantity::quantity_t Crels, Irels;
// 	std::cout << "D1" << std::endl;
	for (auto& M : calc.measurements)
	{
		if (M==nullptr)
			std::cout << "known_Crels_from_sample_matrix_to_Irels_truncated_median:\t" << "nullptr" << std::endl;
		else
			std::cout << "known_Crels_from_sample_matrix_to_Irels_truncated_median:\t" << M << std::endl;
		if (!Crel_from_sample(M).is_set()) continue;
// // 		if (!Irel(*M).is_set()) continue;
// // 		Irels << Irel(*M).remove_data_from_begin(Irel(*M).data().size()/2).median(); // this is the truncated_median from Irel and NOT Irel from truncated_medians; This is different!
// 		if (!Irel_from_truncated_medians(M).is_set()) continue;
// 		Irels << Irel_from_truncated_medians(M);
// 		Crels << Crel_from_sample(M);
	}
	return {Crels,Irels};
}

const std::pair<quantity::quantity_t,quantity::quantity_t> mgroups_::sims_t::calc_t::Crel_to_Irel_c::known_Crels_from_Clusters_to_Irels() const
{
	quantity::quantity_t Crel, Irel;
	for (auto& M : calc.measurements)
	{
// 		if (!M->sample.matrix().is_set()) continue;
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
// 	Crel_template = quantity::quantity_t(Crel.name(),{},Crel.unit());
    //logger::debug(7,"mgroups_::sims_t::calc_t::Crel_to_Irel_c::known_Crels_from_Clusters_to_Irels()","Z="+zaehler.to_string()+" N="+nenner.to_string(),"Irel=" + Irel.to_string(), "Crel=" + Crel.to_string());
	
// 	plot_t plot;
// 	plot.Y2.log10_scale=false;
// 	plot.Y1.add_curve(Irel,Crel,"");
// 	plot.Y2.add_curve(Irel,Crel,"");
// 	plot.to_screen(zaehler.to_string() +" / " + nenner.to_string(),0);
	
	return {Crel,Irel};
}

/************************************************************/
/******           polynom_fit_Crel_to_Irel_c          *******/
/************************************************************/
// mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c::polynom_fit_Crel_to_Irel_c(const cluster_t& zaehler,const cluster_t& nenner,calc_t& calc, std::vector<unsigned int> rank, std::vector<double> fit_start_parameters) :
// 																				Crel_to_Irel_c(zaehler,nenner,calc),
// 																				polynom(polynom_f(rank))
// {
// }
// 
// void mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c::plot_to_screen(double sleep_sec) const
// {
// 	const unsigned int data_points_for_fit_resolution=100;
// 	if (!polynom.successfully_fitted())	return;
// 
// 	const auto Crel_to_Irel = known_Crels_from_sample_matrix_to_Irels_truncated_median();
// // 	std::cout << "B2" << std::endl;
// 	if (!Crel_to_Irel.first.is_set() || !Crel_to_Irel.second.is_set()) return;
// 	plot_t plot(false);
// 	
// 	std::stringstream title_X, title_Y, title_window;
// 	title_X << "Irel = (" << zaehler.to_string() << ") / (" << nenner.to_string() << ")";
// 	title_Y << "Crel = [" << zaehler.corresponding_isotope(calc.MG.matrix_isotopes()).to_string() << "] / [";
// 	title_Y << nenner.corresponding_isotope(calc.MG.matrix_isotopes()).to_string() << "]";
// 	std::stringstream fit_params;
// 	title_window << title_Y.rdbuf() << " over " << title_X.rdbuf() << " chisq=" << polynom.chisq();
// 	
// 	//generate data to show the fit in high resolution
// 	const quantity::quantity_t x_res = Crel_to_Irel.second.resolution()/data_points_for_fit_resolution;
// 	const quantity::quantity_t X(title_X.str(),(Crel_to_Irel.second.change_resolution(x_res/data_points_for_fit_resolution)).data(),units::SI::one); 
// 	logger::debug(15,"mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c::plot_to_screen()",Crel_to_Irel.second.to_string_detailed());
// 	logger::debug(15,"mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c::plot_to_screen()",x_res.to_string_detailed());
// 	logger::debug(15,"mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c::plot_to_screen()",X.to_string_detailed());
// 	const quantity::quantity_t Y(title_Y.str(),polynom.y_data(X.data()),units::SI::one);
// 	
// 	plot.Y1.add_curve(X,Y,"fit rank: " + tools::vec::to_string(polynom.rank()));
// 	plot.Y1.add_points((quantity::quantity_t{X,Crel_to_Irel.second.data()}),(quantity::quantity_t{Y,Crel_to_Irel.first.data()}),"data"," ro");
// 	plot.to_screen(title_window.str(),sleep_sec);
// }
// 
// quantity::concentration_t mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c::Crel(const measurements_::sims_t* M) const
// {
// // 	logger::debug(11,"mgroups_::sims_t::calc_t::matrix_c::polynom_c::polynom_fit_Crel_to_Irel_c::Crel()","Irel= " + Irel(M).to_string(),"Crel=");
// 	quantity::concentration_t result(polynom.y_data(Irel(M).data()),Crel_template.unit());
// 	logger::debug(11,"mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c::Crel()","Irel= " + Irel(M).to_string_detailed(),"Crel=" + result.to_string_detailed());
// // 	return {polynom.fitted_y_data(Irel(M).data()),Crel_template.unit()};
// 	return result;
// }
// 
// std::vector<double> mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c::fit_start_parameters(const std::vector<unsigned int> rank)
// {
// 	std::vector<double> out(rank.size());
// 	for (int i=0;i<rank.size();i++)
// 		out[i]=rank[i];
// 	return out;
// }
// 
// const fit_functions::polynom_t mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c::polynom_f(const std::vector<unsigned int> rank) const
// {
// // 	std::cout << "RANK" << std::endl;
// // 	print(rank);
// 	const auto Crel_to_Irel = known_Crels_from_sample_matrix_to_Irels_truncated_median();
// 	const fit_functions::polynom_t polynom_empty(0,{0});
// 	
// 	if (!Crel_to_Irel.first.is_set() || !Crel_to_Irel.second.is_set())
// 	{
// 		logger::warning(4,"mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c::polynom_f()","known_Crels_from_sample_matrix_to_Irels_truncated_median() is not set","returning false");
// 		return polynom_empty;
// 	}
// 	
// 	std::map<double,double> X_to_Y_map;
// 	tools::vec::combine_vecs_to_map(&Crel_to_Irel.second.data(),&Crel_to_Irel.first.data(), &X_to_Y_map); // Crel(Irel)
// 	if (X_to_Y_map.size()<1 || X_to_Y_map.size() < rank.size())
// 	{
// 		logger::warning(3,"mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c::polynom_f()","to less data points: " + tools::to_string(X_to_Y_map.size()),"returning false");
// 		return polynom_empty;
// 	}
// 	const fit_functions::polynom_t polynom_result(rank,fit_start_parameters(rank),X_to_Y_map);
// 	if (!polynom_result.successfully_fitted())
// 	{
// 		//this should essentially never happen; if there is a GSL error, GSL handler will catch the exception
// 		logger::error("mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c::polynom_f()","fit returned error","returning false");
// 		logger::debug(11,"mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c::polynom_f()","X_to_Y_map.size()=" + tools::to_string(X_to_Y_map.size()));
// 		logger::debug(12,"mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c::polynom_f()","X_to_Y_map",tools::to_string(X_to_Y_map));
// 		logger::debug(12,"mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c::polynom_f()","rank",tools::vec::to_string(rank));
// 		return polynom_empty;
// 	}
// 	else
// 	{
// 		logger::info(4,"mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c::polynom_f()","fit successful","zaehler=" + zaehler.to_string()+" nenner=" + nenner.to_string());
// 	}
// 	return polynom_result;
// }

