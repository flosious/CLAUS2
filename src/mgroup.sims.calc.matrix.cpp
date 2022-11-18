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

mgroups_::sims_t::calc_t::matrix_c::matrix_c(calc_t& calc)
    : MG(calc.MG), calc(calc), measurements(calc.measurements),
      class_logger(__FILE__,"mgroups_::sims_t::calc_t::matrix_c")
{
}

mgroups_::sims_t::calc_t& mgroups_::sims_t::calc_t::matrix_c::median_const_from_db(bool overwrite)
{
	for (auto& mat_C_to_I : MG.matrix_cluster_to_matrix_iso_substance_amount())
	{
		cluster_t* C = mat_C_to_I.first;
		
		if (!C->concentration.is_set() || overwrite)
		{
// 			logger::debug(7,"mgroups_::sims_t::calc_t::matrix_c::median_const_from_db()",C->to_string() + " substance_amount",mat_C_to_I.second.to_string());
			C->concentration = (quantity::concentration_t( (C->intensity / C->intensity.median() ) * mat_C_to_I.second)).change_unit("at%");
            //logger::debug(13,"mgroups_::sims_t::calc_t::matrix_c::median_const_from_db()",C->to_string(),C->concentration.to_string());
		}
	}	
	return calc;
}

mgroups_::sims_t::calc_t& mgroups_::sims_t::calc_t::matrix_c::median_const_from_reference_isotopes(bool overwrite)
{
	MG.set_reference_isotopes_in_measurements();
	for (auto& M : MG.measurements())
		M->calc().matrix.median_const_from_reference_isotopes(overwrite);
	return calc;
}

mgroups_::sims_t::calc_t& mgroups_::sims_t::calc_t::matrix_c::const_from_db(quantity::quantity_t (*operation) (quantity::quantity_t), bool overwrite)
{
	for (auto& mat_C_to_I : MG.matrix_cluster_to_matrix_iso_substance_amount())
	{
		cluster_t* C = mat_C_to_I.first;
// 		isotope_t* mat_iso = mat_C_to_I.second;
		if (!C->concentration.is_set() || overwrite)
		{
			C->concentration = quantity::concentration_t( (C->intensity / operation(C->intensity) ) * mat_C_to_I.second).change_unit("at%");
            //logger::debug(13,"mgroups_::sims_t::calc_t::matrix_c::const_from_db()",C->concentration.to_string());
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
                //logger::error("mgroups_::sims_t::calc_t::matrix_c::intensities_are_set()","could NOT find cluster " + C.to_string() + " in measurement " + M->to_string(),"returning false");
				result = false;
				continue;
			}
			if (!CC->intensity.is_set())
			{
                //logger::error("mgroups_::sims_t::calc_t::matrix_c::intensities_are_set()","intensity is not std::set in cluster " + C.to_string() + " in measurement " + M->to_string(),"returning false");
				result = false;
				continue;
			}
		}
	}
	return result;
}

/*
 * this function interpolates each matrix cluster from given matrix isotopes from reference samples (database)
 * rank = {0,1,0} means linear RSFs without offset (0*c0*x^0 + 1*c1*x^1 + 0*c2*x^2)
 */ 
mgroups_::sims_t::calc_t& mgroups_::sims_t::calc_t::matrix_c::interpolate(const std::vector<unsigned int> polynom_rank, bool overwrite)
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
        //logger::error("mgroups_::sims_t::calc_t::matrix_c::interpolate()","MG.check_matrix_cluster_consistency returned false","returning");
		return calc;
	}
	if (!intensities_are_set())
	{
        //logger::error("mgroups_::sims_t::calc_t::matrix_c::interpolate()","not all intensities are set","returning");
		return calc;
	}
	return calc;
}

mgroups_::sims_t::calc_t& mgroups_::sims_t::calc_t::matrix_c::linear_interpolated_elemental_concentration(bool overwrite)
{
	if (!MG.check_matrix_cluster_consistency())
	{
        //logger::error("mgroups_::sims_t::calc_t::matrix_c::linear_interpolated_elemental_concentration()","MG.check_matrix_cluster_consistency returned false","returning");
		return calc;
	}
	if (!intensities_are_set())
	{
        //logger::error("mgroups_::sims_t::calc_t::matrix_c::linear_interpolated_elemental_concentration()","not all intensities are set","returning");
		return calc;
	}
	
	
	return calc;
}

