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

mgroups_::sims_t::calc_t::calc_t(sims_t& MG) : MG(MG), jiang(MG), SRs(MG), SDs(MG), SFs(MG), RSFs(MG), matrices(MG), concentrations(MG)
{
}

void mgroups_::sims_t::calc_t::toggle_overwrite(bool overwrite)
{
	SRs.overwrite=overwrite;
	SDs.overwrite=overwrite;
	SFs.overwrite=overwrite;
	RSFs.overwrite=overwrite;
	matrices.overwrite=overwrite;
	concentrations.overwrite=overwrite;
}

/*************************/
/*****  jiang_c  *********/
/*************************/

mgroups_::sims_t::calc_t::jiang_c::jiang_c(sims_t & MG) : MG(MG)
{
}

/*************************/
/*****   SR_c  **********/
/*************************/

mgroups_::sims_t::calc_t::SR_c::SR_c(sims_t & MG) : MG(MG)
{
}

mgroups_::sims_t& mgroups_::sims_t::calc_t::SR_c::from_crater_depths()
{
	for (auto& M : MG.measurements())
	{
		if (!overwrite && M->crater.SR.is_set()) continue;
		M->calc().SR().from_crater_depths();
	}
	return MG;
}

mgroups_::sims_t & mgroups_::sims_t::calc_t::SR_c::from_implant_max()
{
	for (auto& M : MG.measurements())
	{
		if (!overwrite && M->crater.SR.is_set()) continue;
		M->calc().SR().from_implant_max();
	}
	return MG;
}


/*************************/
/*****   SD_c  **********/
/*************************/

mgroups_::sims_t::calc_t::SD_c::SD_c(sims_t & MG) : MG(MG)
{
}

mgroups_::sims_t & mgroups_::sims_t::calc_t::SD_c::from_SR()
{
	for (auto& M : MG.measurements())
	{
		if (!overwrite && M->crater.sputter_depth.is_set()) continue;
		M->calc().SD().from_SR();
	}
	return MG;
}

/*************************/
/*****   SF_c  **********/
/*************************/

mgroups_::sims_t::calc_t::SF_c::SF_c(sims_t & MG) : MG(MG)
{
}

mgroups_::sims_t & mgroups_::sims_t::calc_t::SF_c::from_implant_dose()
{
	for (auto& M : MG.measurements())
	{
		for (auto& C : M->clusters)
		{
			//skip cluster belonging to matrix
			if (M->matrix_clusters(M->sample->matrix().isotopes).is_cluster_in_matrix(C)) continue;
			if (!overwrite && C.SF.is_set()) continue;
			M->calc().SF().from_db_dose(C);
		}
	}
	return MG;
}

mgroups_::sims_t & mgroups_::sims_t::calc_t::SF_c::from_implant_max()
{
	for (auto& M : MG.measurements())
	{
		for (auto& C : M->clusters)
		{
			if (!overwrite && C.SF.is_set()) continue;
			//skip cluster belonging to matrix
			if (M->matrix_clusters(M->sample->matrix().isotopes).is_cluster_in_matrix(C)) continue;
			M->calc().SF().from_db_max(C);
		}
	}
	return MG;
}

/*************************/
/*****  RSF_c    *********/
/*************************/

mgroups_::sims_t::calc_t::RSF_c::RSF_c(sims_t & MG) : MG(MG)
{
}



/*************************/
/**   concentration_c  **/
/*************************/

mgroups_::sims_t::calc_t::concentration_c::concentration_c(sims_t & MG) : MG(MG)
{
}

mgroups_::sims_t& mgroups_::sims_t::calc_t::concentration_c::from_SF()
{
	for (auto& M : MG.measurements())
	{
		for (auto& C : M->clusters)
		{
			if (!overwrite && C.concentration.is_set()) continue;
			//skip cluster belonging to matrix
			if (M->matrix_clusters(M->sample->matrix().isotopes).is_cluster_in_matrix(C)) continue;
			M->calc().concentration().from_SF(C);
		}
	}
	return MG;
}

/*************************/
/*****   matrix_c *******/
/*************************/

mgroups_::sims_t::calc_t::matrix_c::matrix_c(sims_t & MG) : MG(MG)
{
}

