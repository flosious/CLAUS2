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
			quantity::intensity_t Iref = M->matrix_clusters().intensity_sum();
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

// map<sample_t::matrix_t,quantity::SR_t> mgroups_::sims_t::calc_t::matrices_to_SR()
// {
// 	vector<pair<sample_t::matrix_t,quantity::SR_t>> mats_to_SR;
// 	map<sample_t::matrix_t,quantity::SR_t> mats_to_SR_map;
// 	
// 	for (auto& M : measurements)
// 	{
// 		if (!M->sample->matrix().is_set()) continue;
// 		if (!M->crater.SR.is_set()) continue;
// 		mats_to_SR.push_back(pair<sample_t::matrix_t,quantity::SR_t>(M->sample->matrix(),M->crater.SR));
// 	}
// 	
// 	// now there could be entries with same matrices in vector mats_to_SR, lets filter them and take their mean
// 	for (int i=0;i<mats_to_SR.size();i++)
// 	{
// 		quantity::SR_t SR = mats_to_SR.at(i).second;
// 		for (int j=i+1;j<mats_to_SR.size();j++)
// 		{
// 			if (mats_to_SR.at(i).first == mats_to_SR.at(j).first)
// 				SR << mats_to_SR.at(j).second;
// 		}
// 		if (SR.data.size() > 1)
// 			SR = quantity::SR_t(SR.mean()); // take the mean, but don't tell anyone
// 		mats_to_SR_map.insert(pair<sample_t::matrix_t,quantity::SR_t> (mats_to_SR.at(i).first,SR));
// 	}
// 	
// 	return mats_to_SR_map;
// }
// 
// map<sample_t::matrix_t,quantity::SF_t> mgroups_::sims_t::calc_t::matrices_to_RSF(const cluster_t& cluster)
// {
// 	map<sample_t::matrix_t,quantity::SF_t> mat_to_RSF_map;
// 	vector<pair<sample_t::matrix_t,quantity::SF_t>> mat_to_RSF;
// 	for (auto& M : measurements)
// 	{
// 		if (!M->sample->matrix().is_set()) continue;
// 		quantity::SF_t RSF = M->cluster(cluster)->RSF;
// 		if (!RSF.is_set()) continue;
// 		mat_to_RSF.push_back(pair<sample_t::matrix_t,quantity::SF_t>(M->sample->matrix(),RSF));
// 	}
// 	
// 	// now there could be entries with same matrices in vector mat_to_RSF, lets filter them and take their mean
// 	for (int i=0;i<mat_to_RSF.size();i++)
// 	{
// 		quantity::SF_t RSF = mat_to_RSF.at(i).second;
// 		for (int j=i+1;j<mat_to_RSF.size();j++)
// 		{
// 			if (mat_to_RSF.at(i).first == mat_to_RSF.at(j).first)
// 				RSF << mat_to_RSF.at(j).second;
// 		}
// 		if (RSF.data.size() > 1)
// 			RSF = quantity::SR_t(RSF.mean()); // take the mean, but don't tell anyone
// 		mat_to_RSF_map.insert(pair<sample_t::matrix_t,quantity::SF_t> (mat_to_RSF.at(i).first,RSF));
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
			rsf_to_ref.first->SF = quantity::SF_t (rsf_to_ref.first->RSF / rsf_to_ref.second.median()); // this is the median of a sum; not the sum of the medians!
		}
	}
	return calc;
}

const map<cluster_t*,quantity::intensity_t> mgroups_::sims_t::calc_t::SF_c::RSFs_to_ref_intensities()
{
	map<cluster_t*,quantity::intensity_t> rsf_to_ref_intensity;
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
				rsf_to_ref_intensity.insert(pair<cluster_t*,quantity::intensity_t> (&C,matrix_clusters.intensity_sum()));
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

const map<cluster_t*,quantity::intensity_t> mgroups_::sims_t::calc_t::RSF_c::SFs_to_ref_intensities()
{
	map<cluster_t*,quantity::intensity_t> sf_to_ref_intensity;
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
			sf_to_ref_intensity.insert(pair<cluster_t*,quantity::intensity_t> (&C,matrix_clusters.intensity_sum()));
		}
	}
	return sf_to_ref_intensity;
}

mgroups_::sims_t::calc_t& mgroups_::sims_t::calc_t::RSF_c::from_SF_mean_ref(bool overwrite)
{
	for (auto& sf_to_ref : SFs_to_ref_intensities())
	{
		if (overwrite || !sf_to_ref.first->RSF.is_set())
			sf_to_ref.first->RSF = quantity::SF_t (sf_to_ref.first->SF * sf_to_ref.second.mean()); // this is the mean of a sum; not the sum of the means!
	}
	return calc;
}

mgroups_::sims_t::calc_t& mgroups_::sims_t::calc_t::RSF_c::from_SF_median_ref(bool overwrite)
{
	for (auto& sf_to_ref : SFs_to_ref_intensities())
	{
		if (overwrite || !sf_to_ref.first->RSF.is_set())
			sf_to_ref.first->RSF = quantity::SF_t (sf_to_ref.first->SF * sf_to_ref.second.median()); // this is the median of a sum; not the sum of the medians!
	}
	return calc;
}

mgroups_::sims_t::calc_t& mgroups_::sims_t::calc_t::RSF_c::from_SF_pbp_ref(bool overwrite)
{
	for (auto& sf_to_ref : SFs_to_ref_intensities())
	{
		if (overwrite || !sf_to_ref.first->RSF.is_set())
			sf_to_ref.first->RSF = quantity::SF_t (sf_to_ref.first->SF * sf_to_ref.second); // this is the median of a sum; not the sum of the medians!
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

/****************************************************/
/**********   Crel_to_Irel_c      *******************/
/****************************************************/


mgroups_::sims_t::calc_t::Crel_to_Irel_c::Crel_to_Irel_c(const cluster_t& zaehler,const cluster_t& nenner,calc_t& calc) :
																				zaehler(zaehler), nenner(nenner), calc(calc)
{
}

const quantity::concentration_t mgroups_::sims_t::calc_t::Crel_to_Irel_c::Crel_template = quantity::concentration_t({},units::SI::one);

const quantity::intensity_t mgroups_::sims_t::calc_t::Crel_to_Irel_c::Irel(const measurements_::sims_t& M) const
{
	const cluster_t* Z = M.cluster(zaehler);
	const cluster_t* N = M.cluster(nenner);
	if (Z == nullptr || N == nullptr)
	{
		logger::error("mgroups_::sims_t::calc_t::Crel_to_Irel_c::Irel()","nullptr in M= " + M.to_string_short(),zaehler.to_string(),nenner.to_string());
		return {};
	}
	if (!Z->intensity.is_set() || !N->intensity.is_set())
		return {};
	return {(Z->intensity) / (N->intensity)};
}

const quantity::intensity_t mgroups_::sims_t::calc_t::Crel_to_Irel_c::Irel_from_truncated_medians(const measurements_::sims_t& M) const
{
	const cluster_t* Z = M.cluster(zaehler);
	const cluster_t* N = M.cluster(nenner);
	if (Z == nullptr || N == nullptr)
	{
		logger::error("mgroups_::sims_t::calc_t::Crel_to_Irel_c::Irel()","nullptr in M= " + M.to_string_short(),zaehler.to_string(),nenner.to_string());
		return {};
	}
	if (!Z->intensity.is_set() || !N->intensity.is_set())
		return {};
	const auto s = Z->intensity.data.size()/2;
	return {(Z->intensity.remove_data_from_begin(s).median()) / (N->intensity.remove_data_from_begin(s).median())};
}

const quantity::concentration_t mgroups_::sims_t::calc_t::Crel_to_Irel_c::Crel_from_sample(const measurements_::sims_t& M) const
{
	quantity::concentration_t Crel;
	const auto matrix_isotopes = calc.MG.matrix_isotopes();
	auto Zmat_iso = M.sample->matrix().isotope(zaehler.corresponding_isotope(matrix_isotopes));
	auto Nmat_iso = M.sample->matrix().isotope(nenner.corresponding_isotope(matrix_isotopes));
	if (Nmat_iso == nullptr || !Nmat_iso->substance_amount.is_set()) 
		return {}; //division by 0
	if (Zmat_iso == nullptr || !Zmat_iso->substance_amount.is_set())
		return quantity::concentration_t({0},Crel_template.unit()); // always add 0
	return quantity::concentration_t((Zmat_iso->substance_amount / Nmat_iso->substance_amount).data,Crel_template.unit());
}

const pair<quantity::quantity_t,quantity::quantity_t> mgroups_::sims_t::calc_t::Crel_to_Irel_c::known_Crels_from_sample_matrix_to_Irels_truncated_median() const
{
	quantity::quantity_t Crels, Irels;
	for (auto& M : calc.measurements)
	{
		if (!Crel_from_sample(*M).is_set()) continue;
// 		if (!Irel(*M).is_set()) continue;
// 		Irels << Irel(*M).remove_data_from_begin(Irel(*M).data.size()/2).median(); // this is the truncated_median from Irel and NOT Irel from truncated_medians; This is different!
		if (!Irel_from_truncated_medians(*M).is_set()) continue;
		Irels << Irel_from_truncated_medians(*M);
		Crels << Crel_from_sample(*M);
	}
	return {Crels,Irels};
}

const pair<quantity::quantity_t,quantity::quantity_t> mgroups_::sims_t::calc_t::Crel_to_Irel_c::known_Crels_from_Clusters_to_Irels() const
{
	quantity::quantity_t Crel, Irel;
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
// 	Crel_template = quantity::quantity_t(Crel.name(),{},Crel.unit());
	logger::debug(7,"mgroups_::sims_t::calc_t::Crel_to_Irel_c::known_Crels_from_Clusters_to_Irels()","Z="+zaehler.to_string()+" N="+nenner.to_string(),"Irel=" + Irel.to_string(), "Crel=" + Crel.to_string());
	
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
mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c::polynom_fit_Crel_to_Irel_c(const cluster_t& zaehler,const cluster_t& nenner,calc_t& calc, vector<unsigned int> rank, vector<double> fit_start_parameters) :
																				Crel_to_Irel_c(zaehler,nenner,calc),
																				polynom(rank,fit_start_parameters)
{
}

void mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c::plot_to_screen(double sleep_sec) const
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
	const quantity::quantity_t x_res = Crel_to_Irel.second.resolution()/data_points_for_fit_resolution;
	const quantity::quantity_t X(title_X.str(),(Crel_to_Irel.second.change_resolution(x_res/data_points_for_fit_resolution)).data,units::SI::one); 
	logger::debug(15,"mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c::plot_to_screen()",Crel_to_Irel.second.to_string_detailed());
	logger::debug(15,"mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c::plot_to_screen()",x_res.to_string_detailed());
	logger::debug(15,"mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c::plot_to_screen()",X.to_string_detailed());
	const quantity::quantity_t Y(title_Y.str(),polynom.fitted_y_data(X.data),units::SI::one);
	plot.Y1.add_curve(X,Y,"fit rank: " + tools::vec::to_string(polynom.rank));
	plot.Y1.add_points((quantity::quantity_t{X,Crel_to_Irel.second.data}),(quantity::quantity_t{Y,Crel_to_Irel.first.data}),"data"," ro");
	plot.to_screen(title_window.str(),sleep_sec);
}

quantity::concentration_t mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c::Crel(measurements_::sims_t& M) const
{
// 	logger::debug(11,"mgroups_::sims_t::calc_t::matrix_c::polynom_c::polynom_fit_Crel_to_Irel_c::Crel()","Irel= " + Irel(M).to_string(),"Crel=");
	quantity::concentration_t result(polynom.fitted_y_data(Irel(M).data),Crel_template.unit());
	logger::debug(11,"mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c::Crel()","Irel= " + Irel(M).to_string_detailed(),"Crel=" + result.to_string_detailed());
// 	return {polynom.fitted_y_data(Irel(M).data),Crel_template.unit()};
	return result;
}

bool mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c::execute_fit_polynom()
{
	const auto Crel_to_Irel = known_Crels_from_sample_matrix_to_Irels_truncated_median();
	
	if (!Crel_to_Irel.first.is_set() || !Crel_to_Irel.second.is_set())
	{
		logger::warning(4,"mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c::execute_fit_polynom()","known_Crels_from_sample_matrix_to_Irels_truncated_median() is not set","returning false");
		return false;
	}
	map<double,double> X_to_Y_map;
	tools::vec::combine_vecs_to_map(&Crel_to_Irel.second.data,&Crel_to_Irel.first.data, &X_to_Y_map); // Crel(Irel)
	if (X_to_Y_map.size()<1 || X_to_Y_map.size() < polynom.rank.size())
	{
		logger::warning(3,"mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c::execute_fit_polynom()","to less data points: " + tools::to_string(X_to_Y_map.size()),"returning false");
		return false;
	}
	if (!polynom.fit(X_to_Y_map))
	{
		//this should essentially never happen; if there is a GSL error, GSL handler will catch the exception
		logger::error("mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c::execute_fit_polynom()","fit returned error","returning false");
		logger::debug(11,"mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c::execute_fit_polynom()","X_to_Y_map.size()=" + tools::to_string(X_to_Y_map.size()));
		logger::debug(12,"mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c::execute_fit_polynom()","X_to_Y_map",tools::to_string(X_to_Y_map));
		logger::debug(12,"mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c::execute_fit_polynom()","rank",tools::vec::to_string(polynom.rank));
		return false;
	}
	else
	{
		logger::info(4,"mgroups_::sims_t::calc_t::polynom_fit_Crel_to_Irel_c::execute_fit_polynom()","fit successful","zaehler=" + zaehler.to_string()+" nenner=" + nenner.to_string());
	}
	return true;
}

