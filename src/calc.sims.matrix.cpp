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

calc_t::sims_t::matrix_t::matrix_t(const std::vector<isotope_t>& matrix_isotopes, 
								   const std::vector<measurements_::sims_t>& measurements, 
								   const std::vector<RSF_t>& RSFs) : 
								   
								   matrix_isotopes_p(matrix_isotopes),
								   measurements_p(measurements),
								   matrix_clusters_p(matrix_clusters_from_matrix_isotopes()),
                                   RSFs_p(matrix_clusters(),isotopical_Crel_to_median_Irel_linear_fitted(),RSFs),
                                   logger(global_logger,__FILE__,"calc_t::sims_t::matrix_t")
// 								   RSFs_p(matrix_clusters(),elemental_Crel_to_median_Irel_linear_fitted(),RSFs)
{
// 	for (auto& M:measurements)
// 		for (auto& i : M.sample->matrix().isotopes())
// 			std::cout << i.substance_amount.to_string_detailed() << std::endl;
	
}

const calc_t::sims_t::matrix_t::RSFs_t& calc_t::sims_t::matrix_t::RSFs() const
{
	return RSFs_p;
}

const std::vector<isotope_t>& calc_t::sims_t::matrix_t::matrix_isotopes() const
{
	return matrix_isotopes_p;
}


const std::vector<measurements_::sims_t>& calc_t::sims_t::matrix_t::measurements() const
{
	return measurements_p;
}

const std::set<cluster_t> calc_t::sims_t::matrix_t::matrix_clusters_from_matrix_isotopes() const
{
	
	std::set<cluster_t> matrix_clusters;
	std::set<cluster_t> clusters;
	
// 	std::cout << "measurements().size()=" << measurements().size() << std::endl;
	for (const auto& M : measurements())
	{
// 		std::cout << "M.clusters.size()=" << M.clusters.size() << std::endl;
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
		matrix_clusters.insert(C.isotopes);
	}

	return matrix_clusters;
}

const std::vector<calc_t::sims_t::Crel_to_Irel_data_polynomial_fit_t> calc_t::sims_t::matrix_t::elemental_Crel_to_median_Irel_linear_fitted() const
{
    //logger::debug(13,"calc_t::sims_t::matrix_t::elemental_Crel_to_median_Irel_linear_fitted()","entering");
	std::vector<calc_t::sims_t::Crel_to_Irel_data_polynomial_fit_t> Crels_to_Irels;
	for (const auto& Z : matrix_clusters().clusters())
	{
		for (const auto& N : matrix_clusters().clusters())
		{
			if (Z==N)
				continue;
			calc_t::sims_t::Crel_to_Irel_data_collector_t Crel_to_Irel_data_collector(Z,N,measurements());
			auto const RSF = Crel_to_Irel_data_collector.elemental_Crel().Irel_from_median().polynom({0,1},{0,1});
			if (!RSF.polynom().successfully_fitted())
			{
                //logger::info(3,"calc_t::sims_t::matrix_t::elemental_Crel_to_median_Irel_linear_fitted()","RSF not successfully fitted", "Z:" + RSF.zaehler().to_string() + ", N:" + RSF.nenner().to_string());
				continue;
			}
			Crels_to_Irels.push_back(RSF);
		}
	}
    //logger::debug(13,"calc_t::sims_t::matrix_t::elemental_Crel_to_median_Irel_linear_fitted()","leaving");
	return Crels_to_Irels;
}

const std::vector<calc_t::sims_t::Crel_to_Irel_data_polynomial_fit_t> calc_t::sims_t::matrix_t::isotopical_Crel_to_median_Irel_linear_fitted() const
{
    //logger::debug(13,"calc_t::sims_t::matrix_t::isotopical_Crel_to_median_Irel_linear_fitted()","entering");
	std::vector<calc_t::sims_t::Crel_to_Irel_data_polynomial_fit_t> Crels_to_Irels;
	for (const auto& Z : matrix_clusters().clusters())
	{
		for (const auto& N : matrix_clusters().clusters())
		{
			if (Z==N)
				continue;
			calc_t::sims_t::Crel_to_Irel_data_collector_t Crel_to_Irel_data_collector(Z,N,measurements());
			auto const RSF = Crel_to_Irel_data_collector.isotopical_Crel().Irel_from_median().polynom({0,1},{0,1});
			if (!RSF.polynom().successfully_fitted())
			{
                //logger::info(3,"calc_t::sims_t::matrix_t::isotopical_Crel_to_median_Irel_linear_fitted()","RSF not successfully fitted", "Z:" + RSF.zaehler().to_string() + ", N:" + RSF.nenner().to_string());
				continue;
			}
// 			RSF.plot_to_screen(0);
			Crels_to_Irels.push_back(RSF);
		}
	}
    //logger::debug(13,"calc_t::sims_t::matrix_t::isotopical_Crel_to_median_Irel_linear_fitted()","leaving");
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

measurements_::sims_t calc_t::sims_t::matrix_t::relative_elemental_concentrations(const std::vector<cluster_t>& clusters, measurements_::sims_t measurement)
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
	const auto mcs = matrix_clusters().clusters();
	return relative_elemental_concentrations(std::vector<cluster_t>(mcs.begin(),mcs.end()),measurement);
}

const calc_t::sims_t::matrix_t::clusters_t& calc_t::sims_t::matrix_t::matrix_clusters() const
{
	return matrix_clusters_p;
}

/****************************************/
/**********     statics    **************/
/****************************************/


const calc_t::sims_t::matrix_t::clusters_t clusters_from_RSF(const calc_t::sims_t::matrix_t::RSF_t& RSF)
{
	std::set<cluster_t> clusters;
	clusters.insert(RSF.nenner());
	clusters.insert(RSF.zaehler());
	return calc_t::sims_t::matrix_t::clusters_t(clusters);
}
const calc_t::sims_t::matrix_t::clusters_t clusters_from_RSFs(const std::set<calc_t::sims_t::matrix_t::RSF_t>& RSFs)
{
	std::set<cluster_t> clusters;
	for (auto& RSF : RSFs)
	{
		clusters.insert(RSF.nenner());
		clusters.insert(RSF.zaehler());
	}
	return calc_t::sims_t::matrix_t::clusters_t(clusters);
}
const calc_t::sims_t::matrix_t::clusters_t clusters_from_RSFs(const std::vector<calc_t::sims_t::matrix_t::RSF_t>& RSFs)
{
	return clusters_from_RSFs(std::set<calc_t::sims_t::matrix_t::RSF_t>{RSFs.begin(),RSFs.end()});
}


/****************************************/
/***********     RSF_t     **************/
/****************************************/


calc_t::sims_t::matrix_t::RSF_t::RSF_t(const cluster_t zaehler, 
									   const cluster_t nenner,
									   const quantity::quantity_t& quantity_s, 
									   const quantity::abundance_t& abundance_ratio) :
									   RSF_t(zaehler,nenner,quantity_s.data(),abundance_ratio,quantity_s.unit(),quantity_s.dimension())
{
}

calc_t::sims_t::matrix_t::RSF_t::RSF_t(const cluster_t zaehler, 
									   const cluster_t nenner, 
									   double data_s, 
									   const quantity::abundance_t& abundance_ratio) : RSF_t(zaehler,nenner,std::vector<double>{data_s}, abundance_ratio)
{
}

calc_t::sims_t::matrix_t::RSF_t::RSF_t(const cluster_t zaehler, 
									   const cluster_t nenner, 
									   std::vector<double> data_s,
									   const quantity::abundance_t& abundance_ratio,
									   unit_t unit_s, 
									   quantity::quantity_t::dimension_t dim_s) :
									   cluster_relations_copies_t(zaehler,nenner,abundance_ratio),
                                       quantity::quantity_t("mRSF",data_s,unit_s,dim_s),
                                       logger(global_logger,__FILE__,"calc_t::sims_t::matrix_t::RSF_t")
{
	if (!is_relative())
	{
        //logger::error("calc_t::sims_t::matrix_t::RSF_t::RSF_t()","quantity not relative, but matrix RSF should always be relative",to_string());
		clear();
	}
	if (zaehler==nenner)
	{
        //logger::error("calc_t::sims_t::matrix_t::RSF_t::RSF_t()","zaehler==nenner",to_string());
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

std::string calc_t::sims_t::matrix_t::RSF_t::to_string() const
{
	std::stringstream ss;
	ss << "Z=" << zaehler().to_string();
	ss << ", N=" << nenner().to_string();
	ss << ", " << quantity::quantity_t::to_string();
	ss << ", " << abundance_ratio.to_string();
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

calc_t::sims_t::matrix_t::RSF_t calc_t::sims_t::matrix_t::RSF_t::invert() const
{
	return RSF_t(nenner(),zaehler(),quantity::quantity_t::invert(),abundance_ratio.invert());
}


// bool calc_t::sims_t::matrix_t::RSF_t::operator<(const calc_t::sims_t::matrix_t::RSF_t& obj) const
// {
// 	if (zaehler()!=obj.zaehler())
// 		return true;
// 	if (nenner()!=obj.nenner())
// 		return true;
// // 	if (zaehler().to_string() < obj.zaehler().to_string())
// // 		return true;
// // 	if (nenner().to_string() > obj.nenner().to_string())
// // 		return false;
// 	return false;
// }
// 
// bool calc_t::sims_t::matrix_t::RSF_t::operator>(const calc_t::sims_t::matrix_t::RSF_t& obj) const
// {
// 	if (*this == obj)
// 		return false;
// 	if (*this < obj)
// 		return false;
// 	return true;
// }

/****************************************/
/*********     clusters_t    ************/
/****************************************/

calc_t::sims_t::matrix_t::clusters_t::clusters_t(const std::set<cluster_t>& matrix_clusters) : logger(global_logger,__FILE__,"calc_t::sims_t::matrix_t::clusters_t")
{
	for (auto& C : matrix_clusters)
	{
		clusters_p.insert(C.isotopes);
	}
}

calc_t::sims_t::matrix_t::clusters_t::clusters_t(const std::vector<cluster_t>& matrix_clusters) : 
								clusters_t(std::set<cluster_t>{matrix_clusters.begin(),matrix_clusters.end()})
{
}

const std::set<cluster_t>& calc_t::sims_t::matrix_t::clusters_t::clusters() const
{
	return clusters_p;
}

const std::vector<isotope_t> calc_t::sims_t::matrix_t::clusters_t::isotopes() const
{
	std::vector<isotope_t> isos;
	isos.reserve(clusters().size());
	for (const auto& c : clusters())
	{
		isos.push_back(c.isotopes.front());
	}
	return isos;
}

const cluster_t calc_t::sims_t::matrix_t::clusters_t::cluster(const isotope_t& iso) const
{
	const auto cs = clusters(iso);
	if (cs.size()==1)
		return cs.front();
    //logger::error("calc_t::sims_t::matrix_t::clusters_t::cluster()","found multiple clusters for 1 corresponding isotope","returning empty");
	return {};
}

// const cluster_t calc_t::sims_t::matrix_t::clusters_t::cluster(const element_t& ele) const
// {
// 	const auto cs = clusters(ele);
// 	if (cs.size()==1)
// 		return cs.front();
// 	//logger::info(4,"calc_t::sims_t::matrix_t::clusters_t::cluster()","found multiple clusters for 1 corresponding element","returning empty");
// 	return {};
// }

const std::vector<cluster_t> calc_t::sims_t::matrix_t::clusters_t::clusters(const element_t& ele) const
{
	std::set<cluster_t> clusters;
	for (const auto& c : clusters_p)
	{
		if (c.isotopes.front().symbol == ele.symbol)
			clusters.insert(c);
	}
	return {clusters.begin(),clusters.end()};
}

const std::vector<cluster_t> calc_t::sims_t::matrix_t::clusters_t::clusters(const isotope_t& iso) const
{
	std::set<cluster_t> clusters;
	for (const auto& c : clusters_p)
	{
		if (c.isotopes.front() == iso)
			clusters.insert(c);
	}
	return {clusters.begin(),clusters.end()};
}

bool calc_t::sims_t::matrix_t::clusters_t::contains(const cluster_t& cluster) const
{
	if (clusters_p.find(cluster)!=clusters_p.end())
		return true;
	return false;
}

bool calc_t::sims_t::matrix_t::clusters_t::has_mixed_isotopes_in_clusters() const
{
	for (auto& C: clusters_p)
	{
		if (C.isotopes.size()==0)
			continue;
		for (auto& ref_iso : C.isotopes)
		{
			for (auto& iso : C.isotopes)
			{
				if (ref_iso==iso)
					continue;
                //logger::warning(3,"calc_t::sims_t::matrix_t::clusters_t::has_mixed_isotopes_in_clusters()","ref_iso!=iso",C.to_string(),"returning true");
				return true;
			}
		}
	}
	return false;
}

bool calc_t::sims_t::matrix_t::clusters_t::has_mixed_elements_in_clusters() const
{
	for (auto& C: clusters_p)
	{
		if (C.isotopes.size()==0)
			continue;
		for (auto& ref_iso : C.isotopes)
		{
			for (auto& iso : C.isotopes)
			{
				if (ref_iso.symbol==iso.symbol)
					continue;
                //logger::warning(3,"calc_t::sims_t::matrix_t::clusters_t::has_mixed_elements_in_clusters()","ref_iso.symbol==iso.symbol",C.to_string(),"returning true");
				return true;
			}
		}
	}
	return false;
}

std::vector<element_t> calc_t::sims_t::matrix_t::clusters_t::elements() const
{
	std::set<element_t> eles;
	sample_t::matrix_t s_mat(isotopes());
	for (const auto iso : isotopes())
		eles.insert(iso);
	return s_mat.elements;
}

/****************************************/
/***********     RSFs_t    **************/
/****************************************/

calc_t::sims_t::matrix_t::RSFs_t::RSFs_t(const clusters_t& matrix_clusters, 
										 const std::vector<calc_t::sims_t::Crel_to_Irel_data_polynomial_fit_t>& Crel_to_Irel_lin_fits, 
										 const std::vector<calc_t::sims_t::matrix_t::RSF_t>& RSFs) :
                                         matrix_clusters_p(matrix_clusters), Crel_to_Irel_lin_fits_p(Crel_to_Irel_lin_fits), RSFs_manually_set_p(RSFs),
                                         logger(global_logger,__FILE__,"calc_t::sims_t::matrix_t::RSFs_t")
{
    //logger::debug(11,"calc_t::sims_t::matrix_t::RSFs_t::RSFs_t","entering");
// 	std::cout << "RSFs" << std::endl;
// 	for (auto& rsf : RSFs)
// 		std::cout << rsf.to_string() << std::endl;
// 	std::cout << "STOP" << std::endl;
// 	std::cout << "RSFs_manually_set_p" << std::endl;
// 	for (auto& rsf : RSFs_manually_set_p)
// 		std::cout << rsf.to_string() << std::endl;
// 	std::cout << "STOP" << std::endl;
// 	std::cout << "RSFs_manually_set()" << std::endl;
// 	for (auto& rsf : RSFs_manually_set())
// 		std::cout << rsf.to_string() << std::endl;
// 	std::cout << "STOP" << std::endl;
}

calc_t::sims_t::matrix_t::RSFs_t::RSFs_t(const clusters_t& matrix_clusters, 
										 const std::vector<calc_t::sims_t::Crel_to_Irel_data_polynomial_fit_t>& Crel_to_Irel_lin_fits, 
										 const std::set<calc_t::sims_t::matrix_t::RSF_t>& RSFs) :
                                         matrix_clusters_p(matrix_clusters), Crel_to_Irel_lin_fits_p(Crel_to_Irel_lin_fits), RSFs_manually_set_p(RSFs.begin(),RSFs.end()),
                                         logger(global_logger,__FILE__,"calc_t::sims_t::matrix_t::RSFs_t")
{
	
}
/*getter functions for logical consts*/
const calc_t::sims_t::matrix_t::clusters_t& calc_t::sims_t::matrix_t::RSFs_t::matrix_clusters() const
{
	return matrix_clusters_p;
}
const std::vector<calc_t::sims_t::matrix_t::RSF_t>& calc_t::sims_t::matrix_t::RSFs_t::RSFs_manually_set() const
{
	return RSFs_manually_set_p;
}
const std::vector<calc_t::sims_t::Crel_to_Irel_data_polynomial_fit_t>& calc_t::sims_t::matrix_t::RSFs_t::Crel_to_Irel_lin_fits() const
{
	return Crel_to_Irel_lin_fits_p;
}


calc_t::sims_t::matrix_t::RSF_t calc_t::sims_t::matrix_t::RSFs_t::RSF(const Crel_to_Irel_data_polynomial_fit_t& Crel_to_Irel_lin_fit)
{
	if (Crel_to_Irel_lin_fit.polynom().successfully_fitted())
	{
		RSF_t T(Crel_to_Irel_lin_fit.zaehler(),Crel_to_Irel_lin_fit.nenner(),Crel_to_Irel_lin_fit.polynom().fit_parameters().at(1),Crel_to_Irel_lin_fit.abundance_ratio);
		return T;
	}
	return RSF_t{Crel_to_Irel_lin_fit.zaehler(),Crel_to_Irel_lin_fit.nenner()};
}

unsigned int calc_t::sims_t::matrix_t::RSFs_t::RSFs_count_from_clusters() const
{
	return (matrix_clusters_p.clusters().size()*matrix_clusters_p.clusters().size()-matrix_clusters_p.clusters().size());
}

const std::vector<calc_t::sims_t::matrix_t::RSF_t> calc_t::sims_t::matrix_t::RSFs_t::RSFs_from_fits() const
{
	std::vector<RSF_t> RSFs_set;
	for (auto& F : Crel_to_Irel_lin_fits())
	{
		const auto RSF_F = RSF(F);
		if (!RSF_F.is_set())
		{
			continue;
		}
		
		RSFs_set.push_back(RSF_F);
	}
	return RSFs_set;
}

calc_t::sims_t::matrix_t::RSFs_t calc_t::sims_t::matrix_t::RSFs_t::valid_RSFs_and_fits() const
{
	std::vector<RSF_t> valid_RSFs_s;
	for (auto& RSF : RSFs_manually_set())
	{
		if (!RSF.is_set() || RSF.is_nan() || RSF.is_inf())
			continue;
		valid_RSFs_s.push_back(RSF);
	}
	std::vector<Crel_to_Irel_data_polynomial_fit_t> valid_fits;
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
	std::vector<RSF_t> RSFs_copy = RSFs_manually_set();
	for (auto& Z : matrix_clusters().clusters())
	{
		for (auto& N : matrix_clusters().clusters())
		{
			if (Z==N) 
				continue;
			if (RSF(Z,N).is_set() && !RSF(N,Z).is_set())
			{
// 				RSF_t RSF_adder(N,Z,RSF(Z,N).invert());
// 				RSFs_copy.insert(RSF_adder);
				RSFs_copy.push_back(RSF_from_inverse(Z,N));
			}
		}
	}
	return {matrix_clusters(), Crel_to_Irel_lin_fits(),RSFs_copy};
}

calc_t::sims_t::matrix_t::RSFs_t calc_t::sims_t::matrix_t::RSFs_t::remove_Crel_to_Irel_fits_by_RSFs() const
{
	std::vector<Crel_to_Irel_data_polynomial_fit_t> residual_fits;
	for (auto& F : Crel_to_Irel_lin_fits())
	{
	if (tools::vec::find_in_vec(RSF(F),RSFs_manually_set())==nullptr)
		residual_fits.push_back(F);
	}
	return {matrix_clusters(),residual_fits,RSFs_manually_set()};
}

calc_t::sims_t::matrix_t::RSFs_t calc_t::sims_t::matrix_t::RSFs_t::improve_fits_by_removing_biggest_Irels(float relative_chisqr) const
{
	//Irel is X coordinate
	std::vector<Crel_to_Irel_data_polynomial_fit_t> new_fits;
	new_fits.reserve(Crel_to_Irel_lin_fits().size());
	for (auto& F : Crel_to_Irel_lin_fits())
	{
		auto new_map = F.Crel_to_Irel_map.sort_X_from_low_to_high();
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


calc_t::sims_t::matrix_t::RSFs_t calc_t::sims_t::matrix_t::RSFs_t::improve_fits_by_removing_outliners(float gof_reshold) const
{
	std::vector<Crel_to_Irel_data_polynomial_fit_t> new_fits;
	new_fits.reserve(Crel_to_Irel_lin_fits().size());
	for (auto& F : Crel_to_Irel_lin_fits())
	{
		if (F.polynom().gof()>gof_reshold)
		{
			new_fits.push_back(F);
		}
		else	
		{
            //logger::info(3,"calc_t::sims_t::matrix_t::RSFs_t::improve_fits_by_removing_outliners()","trying to improve: "+ F.to_string());
			const auto new_map = F.Crel_to_Irel_map.remove_outliners(F.polynom(),gof_reshold);
            //logger::info(4,"calc_t::sims_t::matrix_t::RSFs_t::improve_fits_by_removing_outliners()","new_map: "+ new_map.to_string());
			const fit_functions::polynom_t new_polynom = new_map.polynom(F.polynom().rank(),F.polynom().fit_parameters());
            //logger::info(4,"calc_t::sims_t::matrix_t::RSFs_t::improve_fits_by_removing_outliners()","new_polynom: "+ new_polynom.to_string());
			new_fits.push_back(Crel_to_Irel_data_polynomial_fit_t(F.zaehler(),F.nenner(),new_map,new_polynom));
// 			cout  << "F.Crel_to_Irel_map().Y()=" << F.Crel_to_Irel_map().Y().to_string_detailed() << std::endl;
// 			std::cout << "new_map.Y()=" << new_map.Y().to_string_detailed() << std::endl;
// 			exit(1);
		}
	}
	return RSFs_t(matrix_clusters(),new_fits,RSFs_manually_set());
}

calc_t::sims_t::matrix_t::RSFs_t calc_t::sims_t::matrix_t::RSFs_t::add_missing_RSFs() const
{
	return add_missing_RSFs_from_inverse().add_missing_RSFs_from_virtual_cluster();
}

const std::vector<calc_t::sims_t::matrix_t::RSF_t> calc_t::sims_t::matrix_t::RSFs_t::RSFs(const cluster_t& nenner) const
{
	std::vector<calc_t::sims_t::matrix_t::RSF_t> rsfs;
	for (auto& mc : matrix_clusters().clusters())
	{
		if (mc == nenner)
			continue;
		const auto rsf = RSF(mc,nenner);
// 		if (rsf.is_set())
		rsfs.push_back(rsf);
	}
	return rsfs;
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
		RSF_t rsf(zaehler,nenner,Crel_to_Irel.polynom().fit_parameters().at(1),Crel_to_Irel.abundance_ratio);
		return rsf;
	}
	return RSF_t(zaehler,nenner);
}

calc_t::sims_t::matrix_t::RSF_t calc_t::sims_t::matrix_t::RSFs_t::RSF_from_inverse(const cluster_t& zaehler, const cluster_t& nenner) const
{
	return RSF_t(zaehler,nenner,{RSF(nenner,zaehler).invert()});
}

const calc_t::sims_t::Crel_to_Irel_data_polynomial_fit_t& calc_t::sims_t::matrix_t::RSFs_t::RSF_with_worst_gof() const
{
	cluster_t Z,N;
	double gof_min=0;
	/*this will be catastrophical if Crel_to_Irel_lin_fits_p is empty*/
	if (Crel_to_Irel_lin_fits_p.size()==0)
    {
        //logger::fatal("calc_t::sims_t::matrix_t::RSFs_t::RSF_with_worst_gof()","Crel_to_Irel_lin_fits_p is empty, can NOT return nullptr as ref");
    }
	const calc_t::sims_t::Crel_to_Irel_data_polynomial_fit_t * result=nullptr;
	for (auto& fits : Crel_to_Irel_lin_fits_p)
	{
		if (result == nullptr || fits.polynom().gof() < gof_min)
		{
			Z = fits.zaehler();
			N = fits.nenner();
			gof_min = fits.polynom().gof();
			result = &fits;
		}
	}
	return *result;
}

calc_t::sims_t::matrix_t::RSFs_t calc_t::sims_t::matrix_t::RSFs_t::remove_RSFs(const std::set<RSF_t>& rsfs_s) const
{
	auto new_RSFs = RSFs_manually_set();
	for (auto& rsf : rsfs_s)
	{
		remove(new_RSFs.begin(),new_RSFs.end(),rsf);
// 		new_RSFs.erase(rsf);
	}
	std::vector<Crel_to_Irel_data_polynomial_fit_t> new_Crel_to_Irels_lin_fits;
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

bool calc_t::sims_t::matrix_t::RSFs_t::sort_rel_chisqr_to_fits_by_chisqr(const std::pair<double,Crel_to_Irel_data_polynomial_fit_t>& P1, const std::pair<double,Crel_to_Irel_data_polynomial_fit_t>& P2)
{
	return (P1.first > P2.first);
}

std::set<calc_t::sims_t::matrix_t::RSF_t> calc_t::sims_t::matrix_t::RSFs_t::RSFs_with_worst_fit(int n) const
{
	if (n>=Crel_to_Irel_lin_fits().size())
		return {};
	std::vector<std::pair<double,Crel_to_Irel_data_polynomial_fit_t>> rel_chisqr_to_fits;
	rel_chisqr_to_fits.reserve(Crel_to_Irel_lin_fits().size());
	for (Crel_to_Irel_data_polynomial_fit_t F : Crel_to_Irel_lin_fits())
	{
		rel_chisqr_to_fits.push_back(std::pair<double,Crel_to_Irel_data_polynomial_fit_t>(F.polynom().chisq_relative(),F));
	}
	sort(rel_chisqr_to_fits.begin(),rel_chisqr_to_fits.end(),sort_rel_chisqr_to_fits_by_chisqr);
	std::set<calc_t::sims_t::matrix_t::RSF_t> RSFs;
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
	std::vector<Crel_to_Irel_data_polynomial_fit_t> new_Crel_to_Irels_lin_fits;
	new_Crel_to_Irels_lin_fits.reserve(Crel_to_Irel_lin_fits().size());
	for (const auto& Crel_to_Irel : Crel_to_Irel_lin_fits())
	{
		if (Crel_to_Irel.polynom().chisq_relative()<=relativ_chisq)
			new_Crel_to_Irels_lin_fits.push_back(Crel_to_Irel);
	}
	return RSFs_t(matrix_clusters(),new_Crel_to_Irels_lin_fits,RSFs_manually_set());
}

calc_t::sims_t::matrix_t::RSFs_t calc_t::sims_t::matrix_t::RSFs_t::remove_RSFs_below_gof_treshold(float gof_treshold) const
{
	std::vector<Crel_to_Irel_data_polynomial_fit_t> new_Crel_to_Irels_lin_fits;
	new_Crel_to_Irels_lin_fits.reserve(Crel_to_Irel_lin_fits().size());
	for (const auto& Crel_to_Irel : Crel_to_Irel_lin_fits())
	{
		if (Crel_to_Irel.polynom().gof()>=gof_treshold)
			new_Crel_to_Irels_lin_fits.push_back(Crel_to_Irel);
	}
	return RSFs_t(matrix_clusters(),new_Crel_to_Irels_lin_fits,RSFs_manually_set());
}

calc_t::sims_t::matrix_t::RSFs_t calc_t::sims_t::matrix_t::RSFs_t::add_or_replace_RSF(const RSF_t& RSF) const
{
	std::vector<RSF_t> new_RSFs = RSFs_manually_set();
	if (tools::find_in_V(RSF,new_RSFs) == nullptr)
		new_RSFs.push_back(RSF);
	else
		std::replace(new_RSFs.begin(),new_RSFs.end(),RSF,RSF);
// 	new_RSFs.insert(RSF);
// 	new_RSFs.erase(RSF);
	return RSFs_t(matrix_clusters(),Crel_to_Irel_lin_fits(),new_RSFs);
}

calc_t::sims_t::matrix_t::RSFs_t calc_t::sims_t::matrix_t::RSFs_t::add_or_replace_RSFs(const std::vector<RSF_t>& RSFs_s) const
{
	auto new_RSFs = RSFs_manually_set();
	for (auto& RSF : RSFs_s)
	{
		if (tools::find_in_V(RSF,new_RSFs) == nullptr)
			new_RSFs.push_back(RSF);
		else
			std::replace(new_RSFs.begin(),new_RSFs.end(),RSF,RSF);
	}
	return RSFs_t(matrix_clusters(),Crel_to_Irel_lin_fits(),new_RSFs);
}

calc_t::sims_t::matrix_t::RSFs_t calc_t::sims_t::matrix_t::RSFs_t::symmetrical_RSFs() const
{
	std::vector<RSF_t> new_RSFs;
	new_RSFs.reserve(all_possible_RSFs().size()); // reserve theorethical maximum
	for (auto rsf : all_available_RSFs())
	{
		if (find(new_RSFs.begin(),new_RSFs.end(),rsf) != new_RSFs.end()) // rsf already in new_RSFs? -> continue
			continue;
		if (!rsf.is_set())
			continue;
		if (rsf.is_nan())
			continue;
		if (rsf.is_inf())
			continue;
		
		rsf << RSF(rsf.nenner(),rsf.zaehler()).invert(); 
		new_RSFs.push_back(RSF_t(rsf.zaehler(), rsf.nenner(), rsf.mean(), rsf.abundance_ratio));
        //logger::info(3,"calc_t::sims_t::matrix_t::RSFs_t::symmetrical_RSFs()",new_RSFs.back().to_string());
		new_RSFs.push_back(RSF_t(rsf.nenner(), rsf.zaehler(), rsf.mean().invert(), rsf.abundance_ratio.invert()));
        //logger::info(3,"calc_t::sims_t::matrix_t::RSFs_t::symmetrical_RSFs()",new_RSFs.back().to_string());
	}
// 	std::cout << std::endl << "new_RSFs: " << std::endl;
// 	for (auto& rsf : new_RSFs)
// 		std::cout << rsf.to_string() << std::endl;
// 	std::cout << "ENDE" << std::endl;
	return RSFs_t(matrix_clusters(),Crel_to_Irel_lin_fits(),new_RSFs);
}

calc_t::sims_t::matrix_t::RSFs_t calc_t::sims_t::matrix_t::RSFs_t::add_natural_abundances() const
{
	RSFs_t new_RSFs = *this;
	
	for (auto& rsf : new_RSFs.RSFs_manually_set_p)
	{
		if (rsf.abundance_ratio.is_set()) // already set
			continue;
		if (PSE.isotope(rsf.zaehler().isotopes.front().symbol,rsf.zaehler().isotopes.front().nucleons)==nullptr) 
		{
            //logger::error("calc_t::sims_t::matrix_t::RSFs_t::add_natural_abundances()","NOT found in PSE: "+rsf.zaehler().isotopes.front().to_string());
			continue;
		}
		if (PSE.isotope(rsf.nenner().isotopes.front().symbol,rsf.nenner().isotopes.front().nucleons)==nullptr)
		{
            //logger::error("calc_t::sims_t::matrix_t::RSFs_t::add_natural_abundances()","NOT found in PSE: "+rsf.nenner().isotopes.front().to_string());
			continue;
		}
		quantity::abundance_t abu_Z ({PSE.isotope(rsf.zaehler().isotopes.front().symbol,rsf.zaehler().isotopes.front().nucleons)->abundance});
		quantity::abundance_t abu_N ({PSE.isotope(rsf.nenner().isotopes.front().symbol,rsf.nenner().isotopes.front().nucleons)->abundance});
		rsf.abundance_ratio = abu_Z / abu_N;
        //logger::info(2,"calc_t::sims_t::matrix_t::RSFs_t::add_natural_abundances()", rsf.to_string());
	}
	
	for (auto& rsf : new_RSFs.Crel_to_Irel_lin_fits_p)
	{
		
		if (rsf.abundance_ratio.is_set()) // already set
			continue;
		if (PSE.isotope(rsf.zaehler().isotopes.front().symbol,rsf.zaehler().isotopes.front().nucleons)==nullptr) 
		{
            //logger::error("calc_t::sims_t::matrix_t::RSFs_t::add_natural_abundances()","NOT found in PSE: "+rsf.zaehler().isotopes.front().to_string());
			continue;
		}
		if (PSE.isotope(rsf.nenner().isotopes.front().symbol,rsf.nenner().isotopes.front().nucleons)==nullptr) 
		{
            //logger::error("calc_t::sims_t::matrix_t::RSFs_t::add_natural_abundances()","NOT found in PSE: "+rsf.nenner().isotopes.front().to_string());
			continue;
		}
		quantity::abundance_t abu_Z ({PSE.isotope(rsf.zaehler().isotopes.front().symbol,rsf.zaehler().isotopes.front().nucleons)->abundance});
		quantity::abundance_t abu_N ({PSE.isotope(rsf.nenner().isotopes.front().symbol,rsf.nenner().isotopes.front().nucleons)->abundance});
		rsf.abundance_ratio = abu_Z / abu_N;
        //logger::info(2,"calc_t::sims_t::matrix_t::RSFs_t::add_natural_abundances()", rsf.to_string());
	}
	return new_RSFs;
}

const std::vector<calc_t::sims_t::matrix_t::RSF_t> calc_t::sims_t::matrix_t::RSFs_t::all_available_RSFs() const
{
	auto fitted_RSFs = RSFs_from_fits();
// 	for (auto& r : fitted_RSFs)
// 		std::cout << "r_fitted: " << r.to_string() << std::endl;
	auto manual_RSFs = RSFs_manually_set();
// 	for (auto& r : manual_RSFs)
// 		std::cout << "r_man: " << r.to_string() << std::endl;
	std::set<calc_t::sims_t::matrix_t::RSF_t> all_RSFs; 
	
	if (manual_RSFs.size()>0)
		all_RSFs.insert(manual_RSFs.begin(),manual_RSFs.end());
	if (fitted_RSFs.size()>0)
		all_RSFs.insert(fitted_RSFs.begin(),fitted_RSFs.end());
// 	for (auto& r : all_RSFs)
// 		std::cout << "r_all: " << r.to_string() << std::endl;
	return {all_RSFs.begin(),all_RSFs.end()};
} 
const std::vector<calc_t::sims_t::matrix_t::RSF_t> calc_t::sims_t::matrix_t::RSFs_t::all_possible_RSFs() const
{
	std::vector<RSF_t> all_possible_RFS;
	int N = matrix_clusters().clusters().size();
	all_possible_RFS.reserve(N*N-N);
	for (auto& Z : matrix_clusters().clusters())
	{
		for (auto& N : matrix_clusters().clusters())
		{
			if (Z==N)
				continue;
			all_possible_RFS.push_back({Z,N,{}});
// 			all_possible_RFS.push_back({N,Z,{}});
		}
	}
	return {all_possible_RFS.begin(),all_possible_RFS.end()};
}
const std::vector<calc_t::sims_t::matrix_t::RSF_t> calc_t::sims_t::matrix_t::RSFs_t::unknown_RSFs() const
{
	std::vector<RSF_t> all_possible_RFS = all_possible_RSFs();
	//now substract the known
	for (auto& rsf : RSFs_manually_set())
		all_possible_RFS.erase(find(all_possible_RFS.begin(),all_possible_RFS.end(),rsf));
	for (auto& Crel_to_Irel : Crel_to_Irel_lin_fits())
		all_possible_RFS.erase(find(all_possible_RFS.begin(),all_possible_RFS.end(),RSF(Crel_to_Irel)));
// 		all_possible_RFS.erase(RSF(Crel_to_Irel));
	
	return all_possible_RFS;
}

//void calc_t::sims_t::matrix_t::RSFs_t::plot_now(double sleep_sec) const
//{
//	plot_t plot;
//// 	std::cout << std::endl << "Crel_to_Irel_lin_fits:" << Crel_to_Irel_lin_fits().size() << std::endl;
//	for (auto& RSF_fits : Crel_to_Irel_lin_fits())
//	{
//// 		RSF_fits.plot_to_screen(0);
//// 		continue;
//		plot_t::window_t w;
//		std::stringstream plot_title;
		
//		w.Y1.log10_scale=false;
//		auto Crel_to_Irel_map_withot_infs = RSF_fits.Crel_to_Irel_map.remove_inf();
		
//		w.Y1.add_points(Crel_to_Irel_map_withot_infs,RSF_fits.zaehler().to_string() + " / " + RSF_fits.nenner().to_string()," ro");
//		plot_title << "{" << RSF_fits.zaehler().to_string() << "} / {" << RSF_fits.nenner().to_string() <<  "}";
//		w.title = plot_title.str();
//		if (!RSF_fits.polynom().successfully_fitted())
//        {
//            //logger::warning(3,"calc_t::sims_t::matrix_t::RSFs_t::plot_now()",RSF_fits.to_string(),"continue");
//        }
//		else
//			w.Y1.add_polynom(Crel_to_Irel_map_withot_infs,RSF_fits.polynom(),RSF_fits.polynom().to_string());
//		plot.windows.push_back(w);
//	}
//	plot.to_screen("",0);
//}

/* this is a rather complicated method
 * it calculates the concentration of all matrix clusters, where the RSFs are given
 */
calc_t::sims_t::matrix_t::RSFs_t calc_t::sims_t::matrix_t::RSFs_t::add_missing_RSFs_from_virtual_cluster() const
{
	if (all_available_RSFs().size() == RSFs_count_from_clusters())
		return *this;
	if (unknown_RSFs().size()>2)
	{
        //logger::error("calc_t::sims_t::matrix_t::RSFs_t::add_missing_RSFs_from_virtual_cluster()","unknown_RSFs().size()>2","returning this");
		return *this;
	}
	
}


/****************************************/
/*****     concentration_c    ***********/
/****************************************/

// calc_t::sims_t::matrix_t::concentration_c::concentration_c(const std::set<RSF_t>& RSFs) : RSFs_p(RSFs)
// {
// }

calc_t::sims_t::matrix_t::concentration_c::concentration_c(const RSFs_t& RSFs, const measurements_::sims_t& measurement) : 
            measurement_p(measurement),matrix_clusters(measurement_p.clusters,RSFs.matrix_clusters().isotopes()),RSFs_p(RSFs),
            logger(global_logger,__FILE__,"calc_t::sims_t::matrix_t::concentration_c")
{
// 	clear_concentrations_of_mcs();
	if (use_elemental()) // add natural isotope ratios
	{
		set_natural_abundances_in_clusters();
		RSFs.add_natural_abundances(); 
	}
}

// const calc_t::sims_t::matrix_t::clusters_t calc_t::sims_t::matrix_t::concentration_c::matrix_clusters() const
// {
// 	return RSFs().matrix_clusters;
// }

calc_t::sims_t::matrix_t::concentration_c& calc_t::sims_t::matrix_t::concentration_c::concentrations_by_filling_up_all_concentrations_to_1()
{
	quantity::concentration_t residual_concentration({100},units::derived::atom_percent,quantity::dimensions::SI::relative);
	std::set<cluster_t> clusters_with_unknown_concentrations;
	for (const auto& C : matrix_clusters.clusters)
	{
// 		auto C = measurement_p.cluster(mc);
		if (C==nullptr)
		{
            //logger::error("calc_t::sims_t::matrix_t::concentration_c::concentrations_by_filling_up_all_concentrations_to_1()","measurement_p.cluster(mc)==nullptr",C->to_string(),"returning this");
			return *this;
		}
		if (!C->concentration.is_set())
		{
			clusters_with_unknown_concentrations.insert(C->isotopes);
			continue;
		}
		auto substract = (C->concentration.change_unit(units::derived::atom_percent));
		if (!substract.is_set())
		{
            //logger::error("calc_t::sims_t::matrix_t::concentration_c::concentrations_by_filling_up_all_concentrations_to_1()","!substract.is_set()",C->to_string(),"returning this");
			return *this;
		}
		if (use_elemental())
		{
			const auto iso = C->corresponding_isotope(matrix_clusters.isotopes());
			substract = substract / PSE.isotope(iso.symbol,iso.nucleons)->abundance;
		}
		residual_concentration = residual_concentration - substract;
	}
    //logger::info(3,"calc_t::sims_t::matrix_t::concentration_c::concentrations_by_filling_up_all_concentrations_to_1()","clusters_with_unknown_concentrations: " + to_string(clusters_with_unknown_concentrations.size()),"residual_concentration: " + residual_concentration.to_string());
	//ATTENTION does not work yet, ignores abundances
	if (clusters_with_unknown_concentrations.size()==1)
	{
		if (use_elemental())
		{
			const auto iso = clusters_with_unknown_concentrations.begin()->corresponding_isotope(matrix_clusters.isotopes());
			residual_concentration = residual_concentration * PSE.isotope(iso.symbol,iso.nucleons)->abundance;
		}
		measurement_p.cluster(*clusters_with_unknown_concentrations.begin())->concentration = residual_concentration;
	}
	return *this;
}

void calc_t::sims_t::matrix_t::concentration_c::set_natural_abundances_in_clusters()
{
	for (auto& C: matrix_clusters.clusters)
	{
		for (auto& I : C->isotopes)
		{
// 			if (I.abundance.is_set()) continue;
			const auto P = PSE.isotope(I.symbol,I.nucleons);
			if (P==nullptr)
				continue;
			
			I.abundance = quantity::abundance_t({P->abundance});
		}
	}
}

calc_t::sims_t::matrix_t::concentration_c& calc_t::sims_t::matrix_t::concentration_c::concentrations_by_RSFs()
{
	
	for (auto& C : matrix_clusters.clusters)
	{
		if (C==nullptr) continue;
		if (C->concentration.is_set()) continue;
		C->concentration = cluster(*C);
	}
	return *this;
}

bool calc_t::sims_t::matrix_t::concentration_c::use_elemental() const
{
	/// decide here, whether to take isotropical or elemental calculation
	/// for isotropical there have to be all isotopes (resp. their corresponding clusters) for all elements measured
	/// OR the abundances of all measured isotopes are known
	/// ... but there could be samples, where it is not necessary to monitor all isotopes of all elements, because they are not
	/// in the sample ... hmm: what to do? future florian may solve this minor logical issue
	
	///IF there is exactly 1 cluster per element, use elemental; ELSE use isotopical
	for (auto ele : matrix_clusters.elements())
	{
		const auto s = matrix_clusters.clusters_from_ele(ele).size();
		if (s!=1)
		{
            //logger::info(3,"calc_t::sims_t::matrix_t::concentration_c::use_elemental()","matrix_clusters.clusters("+ele.to_string()+")="+to_string(s)+"!=1","using isotopical");
			return false;
		}
	}
	return true;
}

quantity::concentration_t calc_t::sims_t::matrix_t::concentration_c::cluster(const cluster_t& cluster)
{
	const isotope_t cor_iso = cluster.corresponding_isotope(matrix_clusters.isotopes());
	if (!cor_iso.is_set())
	{
        //logger::error("calc_t::sims_t::matrix_t::concentration_c::cluster","corresponding isotope not found",cluster.to_string(),"returning empty");
		return {};
	}
	if (use_elemental())
	{
		return element(cor_iso)*PSE.isotope(cor_iso.symbol,cor_iso.nucleons)->abundance;
	}
	return isotope(cor_iso);
}

quantity::concentration_t calc_t::sims_t::matrix_t::concentration_c::isotope(const isotope_t& isotope)
{
	quantity::concentration_t cluster_conc({1},units::SI::one,quantity::dimensions::SI::relative);
	const auto cluster = matrix_clusters.cluster(isotope);
	if (cluster != nullptr)
		return {};
	const auto rsfs = RSFs().RSFs(isotope);
	
	for (const auto& rsf : rsfs)
	{
		if (!rsf.is_set())
		{
            //logger::error("calc_t::sims_t::matrix_t::concentration_c::isotope()","!rsf.is_set()",rsf.to_string(), "returning empty");
			return {};
		}
		calc_t::sims_t::Irel_t irel(measurement_p,rsf.zaehler(),rsf.nenner());
		if (!irel.from_clusters_pbp().is_set())
		{
            //logger::error("calc_t::sims_t::matrix_t::concentration_c::isotope()","!rsf.is_set()",rsf.to_string(), "returning empty");
			return {};
		}
		quantity::concentration_t relativ_concentration((rsf)*irel.from_clusters_pbp());
		cluster_conc += relativ_concentration;
	}
	quantity::concentration_t resulting_cluster_conc(cluster_conc.invert().change_unit(units::derived::atom_percent));
	return resulting_cluster_conc;
}

quantity::concentration_t calc_t::sims_t::matrix_t::concentration_c::element(const element_t& element)
{
	const auto clusters = matrix_clusters.clusters_from_ele(element);
	if (clusters.size()!=1)
	{
        //logger::error("calc_t::sims_t::matrix_t::concentration_c::element()","clusters.size()= " + to_string(clusters.size()) + " !=1", element.to_string(), "returning empty");
		return {};
	}
	
	quantity::concentration_t cluster_conc({1},units::SI::one,quantity::dimensions::SI::relative);
	const auto rsfs = RSFs().RSFs(*clusters.front());
	for (const auto& rsf : rsfs)
	{
// 		std::cout << std::endl << rsf.to_string() << std::endl;	
		if (!rsf.is_set())
		{
            //logger::warning(2,"calc_t::sims_t::matrix_t::concentration_c::element()","!rsf.is_set()",rsf.to_string(), "returning empty");
			return {};
		}
		calc_t::sims_t::Irel_t irel(measurement_p,rsf.zaehler(),rsf.nenner());
		if (!irel.from_clusters_pbp().is_set())
		{
            //logger::error("calc_t::sims_t::matrix_t::concentration_c::element()","!rsf.is_set()",rsf.to_string(), "returning empty");
			return {};
		}
		quantity::concentration_t relativ_concentration((rsf/rsf.abundance_ratio)*irel.from_clusters_pbp());
		cluster_conc += relativ_concentration;
// 		std::cout << std::endl << cluster_conc.to_string() << std::endl;
// 		std::cout << rsf.to_string() << std::endl;
// 		std::cout << rsf.abundance_ratio.to_string() << std::endl;
// 		std::cout << irel.from_clusters_pbp().to_string() << std::endl;
	}
	quantity::concentration_t resulting_cluster_conc(cluster_conc.invert().change_unit(units::derived::atom_percent));
    //logger::info(2,"calc_t::sims_t::matrix_t::concentration_c::element()",element.to_string() + ": " + resulting_cluster_conc.to_string());
	return resulting_cluster_conc;
}

/*
 * 	calculates the isotopical concentration of each cluster resp. corresponding isotope 
 *  OR, if not sufficient clusters resp. isotopes were measured, calculating elemental concentration, using natural abundance
 */
measurements_::sims_t calc_t::sims_t::matrix_t::concentration_c::measurement()
{
	return measurement_p;
}
 
const calc_t::sims_t::matrix_t::RSFs_t& calc_t::sims_t::matrix_t::concentration_c::RSFs() const
{
	return RSFs_p;
}

calc_t::sims_t::matrix_t::concentration_c& calc_t::sims_t::matrix_t::concentration_c::clear_concentrations_of_mcs()
{
	for (auto& C : matrix_clusters.clusters)
	{
// 		const auto C = measurement_p.cluster(mc);
		if (C==nullptr)
			continue;
		C->concentration.clear();
	}
	return *this;
}
 
