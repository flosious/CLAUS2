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

mgroups_::sims_t::calc_t::calc_t(sims_t& MG) : MG(MG), SRs(*this), SDs(*this), SFs(*this), RSFs(*this), matrices(*this), concentrations(*this), measurements(MG.measurements())
{
	
// 	if (save_calc_esults)
// 	{
// 		for (auto& M : measurements)
// 		{
// 			MG.saved_calc_results.push_back(M->calc());
// 		}
// 	}
// 	for (auto& c : saved_calc_results)
// 	{
// 		cout << c.first->crater.sputter_time.to_string() << endl;
// 		cout << c.second.M.crater.sputter_time.to_string() << endl;
// 	}
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

// void mgroups_::sims_t::set_matrix_isotopes_in_measurements()
// {
// 	for (auto& M: measurements())
// 		M->matrix_isotopes = matrix_isotopes();
// }

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
			intensity_t Iref = M->matrix_clusters().intensity_sum();
// 			cout << endl << Iref.to_string_detailed() << endl;
			
			if (C.intensity.is_set() && Iref.is_set() && !M->matrix_clusters().is_cluster_in_matrix(C))
			{
				logger::debug(11,"mgroups_::sims_t::calc_t::normalize_to_ref_intensity()","Iref="+Iref.to_string_detailed());
				C.intensity = ((C.intensity / Iref ) * Iref.median());
// 				cout << endl << C.intensity.to_string_detailed() << endl;
			}
		}
// 		M->plot_now(0);
	}
	return *this;
}

// map<sample_t::matrix_t,SR_t> mgroups_::sims_t::calc_t::matrices_to_SR()
// {
// 	vector<pair<sample_t::matrix_t,SR_t>> mats_to_SR;
// 	map<sample_t::matrix_t,SR_t> mats_to_SR_map;
// 	
// 	for (auto& M : measurements)
// 	{
// 		if (!M->sample->matrix().is_set()) continue;
// 		if (!M->crater.SR.is_set()) continue;
// 		mats_to_SR.push_back(pair<sample_t::matrix_t,SR_t>(M->sample->matrix(),M->crater.SR));
// 	}
// 	
// 	// now there could be entries with same matrices in vector mats_to_SR, lets filter them and take their mean
// 	for (int i=0;i<mats_to_SR.size();i++)
// 	{
// 		SR_t SR = mats_to_SR.at(i).second;
// 		for (int j=i+1;j<mats_to_SR.size();j++)
// 		{
// 			if (mats_to_SR.at(i).first == mats_to_SR.at(j).first)
// 				SR << mats_to_SR.at(j).second;
// 		}
// 		if (SR.data.size() > 1)
// 			SR = SR_t(SR.mean()); // take the mean, but don't tell anyone
// 		mats_to_SR_map.insert(pair<sample_t::matrix_t,SR_t> (mats_to_SR.at(i).first,SR));
// 	}
// 	
// 	return mats_to_SR_map;
// }
// 
// map<sample_t::matrix_t,RSF_t> mgroups_::sims_t::calc_t::matrices_to_RSF(const cluster_t& cluster)
// {
// 	map<sample_t::matrix_t,RSF_t> mat_to_RSF_map;
// 	vector<pair<sample_t::matrix_t,RSF_t>> mat_to_RSF;
// 	for (auto& M : measurements)
// 	{
// 		if (!M->sample->matrix().is_set()) continue;
// 		RSF_t RSF = M->cluster(cluster)->RSF;
// 		if (!RSF.is_set()) continue;
// 		mat_to_RSF.push_back(pair<sample_t::matrix_t,RSF_t>(M->sample->matrix(),RSF));
// 	}
// 	
// 	// now there could be entries with same matrices in vector mat_to_RSF, lets filter them and take their mean
// 	for (int i=0;i<mat_to_RSF.size();i++)
// 	{
// 		RSF_t RSF = mat_to_RSF.at(i).second;
// 		for (int j=i+1;j<mat_to_RSF.size();j++)
// 		{
// 			if (mat_to_RSF.at(i).first == mat_to_RSF.at(j).first)
// 				RSF << mat_to_RSF.at(j).second;
// 		}
// 		if (RSF.data.size() > 1)
// 			RSF = SR_t(RSF.mean()); // take the mean, but don't tell anyone
// 		mat_to_RSF_map.insert(pair<sample_t::matrix_t,RSF_t> (mat_to_RSF.at(i).first,RSF));
// 	}
// 	
// 	return mat_to_RSF_map;
// }


/*************************/
/*****   SR_c  **********/
/*************************/

mgroups_::sims_t::calc_t::SR_c::SR_c(calc_t& calc) : MG(calc.MG), calc(calc), measurements(calc.measurements)
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
// 		cout<< "B\t" << M->crater.sputter_time.to_string() << endl;
		if (!overwrite && M->crater.SR.is_set()) continue;
		
// 		for (auto& m : MG.saved_calc_results)
// 		{
// 			if (m.M != *M) continue;
// 			m.SR.from_implant_max();
// 			cout << M->crater.SR.to_string() << endl;
// 		}
		
// 		if (save_calc_esults)
// 		{
// 			MG.saved_calc_results.at(M).SR.from_implant_max();
// 		}
// 		else
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
		if (!M->sample->matrix().is_set()) continue;
		for (auto& mat_to_SR : mat_to_SRs)
		{
			if (M->sample->matrix() == mat_to_SR.first)
			{
				if (mat_to_SR.second.is_vector())
				{
					/*	this is unhandled at the moment, but will be important when reference samples with layer of different matrices are used.
					 *	one could use all the mean(SR point at matrix concentration) ... yeah maybe later...;-)
					 */
					logger::error("mgroups_::sims_t::calc_t::SR_c::from_others()","SR is a vector, dont know how to handle, tell florian",M->to_string(),"ignoring");
				}
				if (overwrite || !M->crater.SR.is_set())
					M->crater.SR = mat_to_SR.second;
			}
		}
	}
	
	return calc;
}


/*************************/
/******   SD_c  **********/
/*************************/

mgroups_::sims_t::calc_t::SD_c::SD_c(calc_t& calc) : MG(calc.MG), calc(calc), measurements(calc.measurements)
{
}

mgroups_::sims_t::calc_t & mgroups_::sims_t::calc_t::SD_c::from_SR()
{
	for (auto& M : measurements)
	{
		if (!overwrite && M->crater.sputter_depth.is_set()) continue;
// 		if (save_calc_esults)
// 			MG.saved_calc_results.at(M).SD.from_SR();
// 		else
			M->calc().SD.from_SR();
// 		M->calc().SD.from_SR();
	}
	return calc;
}

/*************************/
/*****   SF_c  **********/
/*************************/

mgroups_::sims_t::calc_t::SF_c::SF_c(calc_t& calc) : MG(calc.MG), calc(calc), measurements(calc.measurements)
{
}

mgroups_::sims_t::calc_t& mgroups_::sims_t::calc_t::SF_c::from_RSF_median_ref(bool overwrite)
{
	for (auto& rsf_to_ref : RSFs_to_ref_intensities())
	{
		if (overwrite || !rsf_to_ref.first->SF.is_set())
		{
			rsf_to_ref.first->SF = SF_t (rsf_to_ref.first->RSF / rsf_to_ref.second.median()); // this is the median of a sum; not the sum of the medians!
		}
	}
	return calc;
}

const map<cluster_t*,intensity_t> mgroups_::sims_t::calc_t::SF_c::RSFs_to_ref_intensities()
{
	map<cluster_t*,intensity_t> rsf_to_ref_intensity;
	for (auto& M : measurements)
	{
		auto matrix_clusters = M->matrix_clusters();
		if (!matrix_clusters.intensity_sum().is_set())
		{
			logger::error("mgroups_::sims_t::calc_t::RSF_c::RSFs_to_ref_intensities()","!M->reference_intensity().is_set()","continue");
			continue;
		}
		for (auto& C : M->clusters)
		{
			if (!C.RSF.is_set()) continue;
				rsf_to_ref_intensity.insert(pair<cluster_t*,intensity_t> (&C,matrix_clusters.intensity_sum()));
		}
	}
	return rsf_to_ref_intensity;
}

mgroups_::sims_t::calc_t & mgroups_::sims_t::calc_t::SF_c::from_implant_dose(bool overwrite)
{
	for (auto& M : measurements)
	{
		for (auto& C : M->clusters)
		{
			//skip cluster belonging to matrix
			if (M->matrix_clusters().is_cluster_in_matrix(C)) continue;
			if (overwrite || !C.SF.is_set())
				C.SF = M->calc().SF.from_db_dose(C);
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
			if (M->matrix_clusters().is_cluster_in_matrix(C)) continue;
			if (overwrite || !C.SF.is_set()) 
				C.SF = M->calc().SF.from_db_max(C);
		}
	}
	return calc;
}

/*************************/
/*****  RSF_c    *********/
/*************************/

mgroups_::sims_t::calc_t::RSF_c::RSF_c(calc_t& calc) : MG(calc.MG), calc(calc), measurements(calc.measurements)
{
}

const map<cluster_t*,intensity_t> mgroups_::sims_t::calc_t::RSF_c::SFs_to_ref_intensities()
{
	map<cluster_t*,intensity_t> sf_to_ref_intensity;
	for (auto& M : measurements)
	{
		auto matrix_clusters = M->matrix_clusters();
		if (!matrix_clusters.intensity_sum().is_set())
		{
			logger::error("mgroups_::sims_t::calc_t::RSF_c::SFs_to_ref_intensities()","!M->reference_intensity().is_set()","continue");
			continue;
		}
		for (auto& C : M->clusters)
		{
			if (!C.SF.is_set()) continue;
			sf_to_ref_intensity.insert(pair<cluster_t*,intensity_t> (&C,matrix_clusters.intensity_sum()));
		}
	}
	return sf_to_ref_intensity;
}

mgroups_::sims_t::calc_t& mgroups_::sims_t::calc_t::RSF_c::from_SF_mean_ref(bool overwrite)
{
	for (auto& sf_to_ref : SFs_to_ref_intensities())
	{
		if (overwrite || !sf_to_ref.first->RSF.is_set())
			sf_to_ref.first->RSF = RSF_t (sf_to_ref.first->SF * sf_to_ref.second.mean()); // this is the mean of a sum; not the sum of the means!
	}
	return calc;
}

mgroups_::sims_t::calc_t& mgroups_::sims_t::calc_t::RSF_c::from_SF_median_ref(bool overwrite)
{
	for (auto& sf_to_ref : SFs_to_ref_intensities())
	{
		if (overwrite || !sf_to_ref.first->RSF.is_set())
			sf_to_ref.first->RSF = RSF_t (sf_to_ref.first->SF * sf_to_ref.second.median()); // this is the median of a sum; not the sum of the medians!
	}
	return calc;
}

mgroups_::sims_t::calc_t& mgroups_::sims_t::calc_t::RSF_c::from_SF_pbp_ref(bool overwrite)
{
	for (auto& sf_to_ref : SFs_to_ref_intensities())
	{
		if (overwrite || !sf_to_ref.first->RSF.is_set())
			sf_to_ref.first->RSF = RSF_t (sf_to_ref.first->SF * sf_to_ref.second); // this is the median of a sum; not the sum of the medians!
	}
	return calc;
}

mgroups_::sims_t::calc_t & mgroups_::sims_t::calc_t::RSF_c::copy_to_same_matrices(bool overwrite)
{
	auto cs = MG.clusters();
	
	for (auto& C : cs)
	{
		//for the specific cluster C
		auto mat_to_RSF = MG.matrix_to_RSF(C);
		if (mat_to_RSF.size()==0) continue; // no RSFs at all for this cluster at all
		for (auto& M : measurements)
		{
			if (!M->sample->matrix().is_set()) continue; // unknown matrix
			auto mat = mat_to_RSF.find(M->sample->matrix());
			if (mat==mat_to_RSF.end()) continue; // no RSF for this matrix and cluster
			if (mat->second.is_vector())
			{
				/*	this is unhandled at the moment, but will be important when reference samples with layer of different matrices are used.
				*	one could use all the mean(RSF point at matrix concentration) ... yeah maybe later...;-)
				*/
				logger::error("mgroups_::sims_t::calc_t::RSF_c::from_others()","RSF is a vector, dont know how to handle, tell florian",M->to_string(),"ignoring");
			}
			if (overwrite || !M->cluster(C)->RSF.is_set())
				M->cluster(C)->RSF = mat->second;
		}
	}
	return calc;
}



/*************************/
/**   concentration_c  **/
/*************************/

mgroups_::sims_t::calc_t::concentration_c::concentration_c(calc_t& calc) : MG(calc.MG), calc(calc), measurements(calc.measurements)
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

/*************************/
/*****   matrix_c *******/
/*************************/

mgroups_::sims_t::calc_t::matrix_c::matrix_c(calc_t& calc) : MG(calc.MG), calc(calc), measurements(calc.measurements)
{
}

const map<cluster_t*,isotope_t*> mgroups_::sims_t::calc_t::matrix_c::matrix_cluster_to_iso()
{
	map<cluster_t*,isotope_t*> matrix_C_to_I;
	for (auto& M : measurements)
	{
		if (!M->sample->matrix().is_set()) continue;
		if (!M->matrix_clusters().intensity_sum().is_set()) continue;
		for (int i=0;i<M->sample->matrix().isotopes.size();i++)
		{
			cluster_t* C = M->matrix_clusters().cluster(M->sample->matrix().isotopes.at(i));
			if (C == nullptr) 
			{
				logger::debug(13,"mgroups_::sims_t::calc_t::matrix_c::matrix_cluster_to_iso()","M->matrix_clusters().cluster("+M->sample->matrix().isotopes.at(i).to_string()+")==nullptr","doing nothing");
				continue;
			}
			if (!C->intensity.is_set())
			{
				logger::warning(4,"mgroups_::sims_t::calc_t::matrix_c::matrix_cluster_to_iso()",C->to_string()+" intensity not set","doing nothing");
				continue;
			}
// 			isotope_t* I = &((.isotopes).at(i));
			matrix_C_to_I.insert(pair<cluster_t*,isotope_t*> (C,&M->sample->matrix().isotopes.at(i)));
// 			C->concentration = concentration_t( (C->intensity / C->intensity.median() ) * mat_iso.substance_amount);
// 			logger::debug(11,"mgroups_::sims_t::calc_t::matrix_c::median_const_from_db()","mat_iso.substance_amount="+mat_iso.substance_amount.to_string(),"C->concentration=" + C->concentration.to_string());
		}
	}
	return matrix_C_to_I;
}

mgroups_::sims_t::calc_t& mgroups_::sims_t::calc_t::matrix_c::median_const_from_db(bool overwrite)
{
	for (auto& mat_C_to_I : matrix_cluster_to_iso())
	{
		cluster_t* C = mat_C_to_I.first;
		isotope_t* mat_iso = mat_C_to_I.second;
		if (!C->concentration.is_set() || overwrite)
			C->concentration = concentration_t( (C->intensity / C->intensity.median() ) * mat_iso->substance_amount);
	}	
	return calc;
// 	return const_from_db(quantity_t::mean);
}

mgroups_::sims_t::calc_t& mgroups_::sims_t::calc_t::matrix_c::const_from_db(quantity_t (*operation) (quantity_t), bool overwrite)
{
	for (auto& mat_C_to_I : matrix_cluster_to_iso())
	{
		cluster_t* C = mat_C_to_I.first;
		isotope_t* mat_iso = mat_C_to_I.second;
		if (!C->concentration.is_set() || overwrite)
			C->concentration = concentration_t( (C->intensity / operation(C->intensity) ) * mat_iso->substance_amount);
// 			C->concentration = concentration_t( (C->intensity / C->intensity.operation() ) * mat_iso->substance_amount);
	}	
	return calc;
}

/*
 * this function interpolates each matrix cluster from gives matrix isotopes from reference samples (database)
 * rank = {0,1,0} means linear RSFs without offset (0*c0*x^0 + 1*c1*x^1 + 0*c2*x^2)
 */ 
mgroups_::sims_t::calc_t& mgroups_::sims_t::calc_t::matrix_c::interpolate(vector<int> rank)
{
	for (auto& mc : MG.matrix_clusters())
	{
		//get the reference_intensities of all measurements and their known matrix_concentration for this isotope
	}
	return calc;
}



