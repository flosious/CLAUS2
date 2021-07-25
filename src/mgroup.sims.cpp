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

mgroups_::sims_t::sims_t(measurements_::sims_t& measurement) : mgroup_t(measurement)
{	
}

///will be overwritten by dsims/tofsims
vector<measurements_::sims_t *> mgroups_::sims_t::measurements()
{
	return {};
}

// vector<cluster_t> mgroups_::sims_t::reference_clusters()
// {
// 	
// 	set<cluster_t> all_ref_clusters;
// 	
// 	for (auto& M : measurements())
// 	{
// 		for (auto& ref_c : M->reference_clusters())
// 			all_ref_clusters.insert(ref_c->isotopes);
// 	}
// 
// 	/*intersection of all reference clusters over all clusters in each measurement*/
// 	for (auto& M : measurements())
// 	{
// 		for (auto ref_c=all_ref_clusters.begin();ref_c!=all_ref_clusters.end();ref_c++)
// 			if(find(M->clusters.begin(),M->clusters.end(),*ref_c)==M->clusters.end())
// 			{
// 				all_ref_clusters.erase(ref_c);
// 				ref_c--;
// // 				logger::error("mgroups_::sims_t::reference_clusters(): missing cluster in measurements", ref_c->name());
// 				logger::error("mgroups_::sims_t::reference_clusters()","missing cluster",ref_c->to_string() + " in " + M->to_string(),"keep calculating, but results might be wrong");
// 			}
// 	}
// 	
// 	return {all_ref_clusters.begin(),all_ref_clusters.end()};
// }

mgroups_::sims_t::calc_t mgroups_::sims_t::calc()
{
	if (!set_matrix_isotopes_in_unknown_samples())
	{
		logger::error("mgroups_::sims_t::calc()","!set_matrix_isotopes_in_unknown_samples()","ignoring");
	}
	return calc_t(*this);
}

void mgroups_::sims_t::check()
{
	quantity_t ST_resolution, SD_resolution;
	for (auto& M :measurements())
	{
		if (M->crater.sputter_time.is_set() && !ST_resolution.is_set())
			ST_resolution = M->crater.sputter_time.resolution();
		
		if (M->crater.sputter_depth.is_set() && !SD_resolution.is_set())
			SD_resolution = M->crater.sputter_depth.resolution();
		
		if (SD_resolution.is_set() && ST_resolution.is_set())
			logger::warning(1,"mgroups_::sims_t::check()","SD_resolution.is_set() && ST_resolution.is_set()",M->to_string());
// 			logger::warning("mgroups_::sims_t::check: SD_resolution.is_set() && ST_resolution.is_set()", to_string());
		if (!SD_resolution.is_set() && !ST_resolution.is_set())
			logger::warning(1,"mgroups_::sims_t::check()","!SD_resolution.is_set() && !ST_resolution.is_set()",M->to_string());
// 			logger::warning("mgroups_::sims_t::check: !SD_resolution.is_set() && !ST_resolution.is_set()", to_string());
		if (SD_resolution.is_set() && SD_resolution != M->crater.sputter_depth.resolution())
			logger::warning(1,"mgroups_::sims_t::check()","SD_resolution.is_set() && SD_resolution != M->crater.sputter_depth().resolution()",M->to_string());
// 			logger::warning("mgroups_::sims_t::check: sputter_depth().resolution() differs", to_string());
		if (ST_resolution.is_set() && ST_resolution != M->crater.sputter_time.resolution())
			logger::warning(1,"mgroups_::sims_t::check()","ST_resolution.is_set() && ST_resolution != M->crater.sputter_time.resolution()",M->to_string());
// 			logger::warning("mgroups_::sims_t::check: sputter_time.resolution() differs", to_string());
	}
}

const map<sample_t::matrix_t,RSF_t> mgroups_::sims_t::matrix_to_RSF(const cluster_t& cluster)
{
	map<sample_t::matrix_t,RSF_t> mat_to_RSF;
	for (auto& M : measurements())
	{
		sample_t::matrix_t mat = M->sample->matrix();
		
		if (!mat.is_set()) continue;
		cluster_t* C = M->cluster(cluster);
		if (C==nullptr) continue;
		RSF_t RSF =  C->RSF;
		if (!RSF.is_set()) continue;
		logger::debug(21,"mgroups_::sims_t::matrix_to_SRs()",M->sample->to_string());
		logger::debug(21,"mgroups_::sims_t::matrix_to_RSF()","matrix:"+mat.to_string(),"\trRSF:"+RSF.to_string());
		if (RSF.data.size()>1)
		{
			logger::warning(3,"mgroups_::sims_t::matrix_to_RSF()","RSF is not a scalar, using median");
			RSF =  RSF.median();
		}
		if (mat_to_RSF.find(mat)==mat_to_RSF.end())
			mat_to_RSF.insert(pair<sample_t::matrix_t, RSF_t> (mat,RSF));
		else
			mat_to_RSF.at(mat) << RSF;
	}
	
	for (auto& m : mat_to_RSF)
		m.second = RSF_t(m.second.mean());
	return mat_to_RSF;
}

const std::map< sample_t::matrix_t, SR_t > mgroups_::sims_t::matrix_to_SRs()
{
	map<sample_t::matrix_t,SR_t> mat_to_SRs;
	for (auto& M : measurements())
	{
		sample_t::matrix_t mat = M->sample->matrix();
		if (!mat.is_set()) continue;
		SR_t SR = M->crater.SR;
		if (!SR.is_set()) continue;
		logger::debug(21,"mgroups_::sims_t::matrix_to_SRs()",M->sample->to_string());
		logger::debug(21,"mgroups_::sims_t::matrix_to_SRs()","matrix:"+mat.to_string(),"\tSR:"+SR.to_string());
		if (SR.data.size()>1)
		{
			logger::warning(3,"mgroups_::sims_t::matrix_to_SRs()","SR is not a scalar, using median");
			SR = SR.median();
		}
		if (mat_to_SRs.find(mat)==mat_to_SRs.end())
			mat_to_SRs.insert(pair<sample_t::matrix_t, SR_t> (mat,SR));
		else
		{
			mat_to_SRs.at(mat) << SR;
		}
	}
	
	for (auto& m : mat_to_SRs)
		m.second = m.second.mean();
	return mat_to_SRs;
}

const std::map< sample_t::matrix_t, intensity_t > mgroups_::sims_t::matrix_to_intensity_sum()
{
	map<sample_t::matrix_t,intensity_t> mat_to_I;
	for (auto& M : measurements())
	{
		sample_t::matrix_t mat = M->sample->matrix();
		if (!mat.is_set()) continue;
		intensity_t I = M->matrix_clusters().intensity_sum();
		if (!I.is_set()) continue;
		if (mat_to_I.find(mat)==mat_to_I.end())
			mat_to_I.insert(pair<sample_t::matrix_t, intensity_t> (mat,I.quantile(0.10)));
		else
			mat_to_I.at(mat) << I.quantile(0.10);
	}
	return mat_to_I;
}

vector<isotope_t> mgroups_::sims_t::matrix_isotopes()
{
	set<isotope_t> isos;
	for (auto& M : measurements())
	{
		if (!M->sample->matrix().is_set()) continue;
		vector<isotope_t> mat_isos=M->sample->matrix().isotopes;
		isos.insert(mat_isos.begin(),mat_isos.end());
// 		const vector<isotope_t> matrix_isos_in_M = M->matrix_clusters().isotopes();
// 		isos.insert(matrix_isos_in_M.begin(),matrix_isos_in_M.end());
	}
	vector<isotope_t> isos_vec = {isos.begin(),isos.end()};
	
	// keep abundance and/or substance_amount if there is only 1 known matrix from all reference samples within this group
	if (matrices().size()==1) 
	{
		logger::info(3,"mgroups_::sims_t::matrix_isotopes()","all references have same matrix, applying for whole group",matrices().begin()->to_string());
		return isos_vec;
	}
	
	// delete abundance and substance_amount because there are different ones in the group. calculate later
	for (auto& I : isos_vec)
	{
		I.substance_amount.clear();
		I.abundance.clear();
	}
	
	return isos_vec;
}

vector<cluster_t> mgroups_::sims_t::matrix_clusters()
{
	set<cluster_t> clusters;
	for (auto& M : measurements())
	{
		for (auto& C : M->matrix_clusters().clusters)
		{
			clusters.insert({C->isotopes});
		}
	}
	return {clusters.begin(),clusters.end()};
}

bool mgroups_::sims_t::set_matrix_isotopes_in_unknown_samples()
{
	const vector<isotope_t> mat_isos = matrix_isotopes();
	if (mat_isos.size()==0)
	{
		logger::error("mgroups_::sims_t::set_matrix_isotopes_in_unknown_samples()","matrix_isotopes()==0 unknown matrices","please include atleast 1 tabulated reference sample; check database","returning false");
		return false;
	}
	for (auto& M : measurements())
		if (M->sample->matrix().isotopes.size()==0)
			M->sample->matrix().isotopes=mat_isos;
	return true;
}

set<sample_t::matrix_t> mgroups_::sims_t::matrices()
{
	set<sample_t::matrix_t> mats;
	for (auto& M : measurements())
		if (M->sample->matrix().is_set())
			mats.insert(M->sample->matrix());
// 	cout << endl << "mats.size()=" << mats.size() << endl;
// 	for (auto& m : mats)
// 		cout << m.to_string() << endl;
	return mats;
}

set<cluster_t> mgroups_::sims_t::clusters()
{
	set<cluster_t> cs;
	
	for (auto& M : measurements())
	{
		for (auto& C : M->clusters)
		{
			cs.insert({C.isotopes});
		}
	}
	
	return cs;
}

// measurements_::dsims_t::matrix_clusters_c& mgroups_::sims_t::common_matrix_clusters()
// {
// 	set<cluster_t> common_clusters;
// 	if (matrix_clusters_s.clusters.size()>0)
// 		return matrix_clusters_s;
// 	for (auto& m:measurements())
// 	{
// 		vector<cluster_t*> cs =  m->matrix_clusters(m->sample->matrix().isotopes).clusters;
// 		for (auto& c : cs)
// 			common_clusters.insert(c->isotopes);
// 	}
// 	return matrix_clusters_s;
// }
// 
