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

mgroups_::sims_t::sims_t(measurements_::sims_t& measurement) : mgroup_t(measurement), logger(__FILE__,"mgroups_::sims_t")
{	

}

std::vector<measurements_::measurement_t*> mgroups_::sims_t::basic_measurements()
{
    auto Ms = measurements();
    std::vector<measurements_::measurement_t*> Ms_const(Ms.size());
    for (size_t i=0;i<Ms.size();i++)
    {
        Ms_const.at(i)=Ms.at(i);
    }
    return Ms_const;
}


const std::vector<const measurements_::sims_t*> mgroups_::sims_t::sims_t::measurements_const()
{
	const auto Ms = measurements();
	std::vector<const measurements_::sims_t*> Ms_const(Ms.size());
    for (size_t i=0;i<Ms.size();i++)
	{
		Ms_const.at(i)=Ms.at(i);
	}
	return Ms_const;
}

///will be overwritten by dsims/tofsims
std::vector<measurements_::sims_t *> mgroups_::sims_t::measurements()
{
    //logger::error("mgroups_::sims_t::measurements()","called base virtual function","tell florian");
	return {};
}

std::vector<measurements_::sims_t> mgroups_::sims_t::measurements_copy()
{
    //logger::error("mgroups_::sims_t::measurements_copy()","called base virtual function","tell florian");
	return {};
}


const msettings::sims_t* mgroups_::sims_t::settings() const
{
    //logger::error("mgroups_::sims_t::settings()","called base virtual function","tell florian");
	return nullptr;
}

mgroups_::sims_t::calc_t mgroups_::sims_t::calc()
{
	if (!set_matrix_isotopes_in_unknown_samples())
	{
        //logger::error("mgroups_::sims_t::calc()","!set_matrix_isotopes_in_unknown_samples()","ignoring");
	}
	return calc_t(*this);
}

void mgroups_::sims_t::check()
{
	quantity::quantity_t ST_resolution, SD_resolution;
	for (auto& M :measurements())
	{
		if (M->crater.sputter_time.is_set() && !ST_resolution.is_set())
			ST_resolution = M->crater.sputter_time.resolution();
		
		if (M->crater.sputter_depth.is_set() && !SD_resolution.is_set())
			SD_resolution = M->crater.sputter_depth.resolution();
		
		if (SD_resolution.is_set() && ST_resolution.is_set())
        {
            //logger::warning(1,"mgroups_::sims_t::check()","SD_resolution.is_set() && ST_resolution.is_set()",M->to_string());
        }
		if (!SD_resolution.is_set() && !ST_resolution.is_set())
        {
            //logger::warning(1,"mgroups_::sims_t::check()","!SD_resolution.is_set() && !ST_resolution.is_set()",M->to_string());
        }
		if (SD_resolution.is_set() && SD_resolution != M->crater.sputter_depth.resolution())
        {
            //logger::warning(1,"mgroups_::sims_t::check()","SD_resolution.is_set() && SD_resolution != M->crater.sputter_depth().resolution()",M->to_string());
        }
		if (ST_resolution.is_set() && ST_resolution != M->crater.sputter_time.resolution())
        {
            //logger::warning(1,"mgroups_::sims_t::check()","ST_resolution.is_set() && ST_resolution != M->crater.sputter_time.resolution()",M->to_string());
        }
	}
}

const std::map<sample_t::matrix_t, cluster_t::RSF_t> mgroups_::sims_t::matrix_to_RSF(const cluster_t& cluster)
{
	std::map<sample_t::matrix_t,cluster_t::RSF_t> mat_to_RSF;
	for (auto& M : measurements())
	{
        sample_t::matrix_t mat = M->sample.matrix();
		
		if (!mat.is_set()) continue;
		cluster_t* C = M->cluster(cluster);
		if (C==nullptr) continue;
		auto RSF =  C->RSF;
		if (!RSF.is_set()) continue;
        //logger::debug(21,"mgroups_::sims_t::matrix_to_RSF()",M->sample.to_string());
        //logger::debug(21,"mgroups_::sims_t::matrix_to_RSF()","matrix:"+mat.to_string(),"\trRSF:"+RSF.to_string());
		if (RSF.data().size()>1)
		{
            //logger::warning(3,"mgroups_::sims_t::matrix_to_RSF()","RSF is not a scalar, using median");
			RSF =  RSF.median();
		}
		if (mat_to_RSF.find(mat)==mat_to_RSF.end())
			mat_to_RSF.insert( {mat,RSF});
		else
			mat_to_RSF.at(mat) << RSF;
	}
	
	for (auto& m : mat_to_RSF)
		m.second = m.second.mean();
	return mat_to_RSF;
}

const std::map< sample_t::matrix_t, quantity::SR_t > mgroups_::sims_t::matrix_to_SRs()
{
	std::map<sample_t::matrix_t,quantity::SR_t> mat_to_SRs;
	for (auto& M : measurements())
	{
        sample_t::matrix_t mat = M->sample.matrix();
		if (!mat.is_set()) continue;
		quantity::SR_t SR = M->crater.SR;
		if (!SR.is_set()) continue;
        //logger::debug(21,"mgroups_::sims_t::matrix_to_SRs()",M->sample.to_string());
        //logger::debug(21,"mgroups_::sims_t::matrix_to_SRs()","matrix:"+mat.to_string(),"\tSR:"+SR.to_string());
		if (SR.data().size()>1)
		{
            //logger::warning(3,"mgroups_::sims_t::matrix_to_SRs()","SR is not a scalar, using median");
			SR = SR.median();
		}
		if (mat_to_SRs.find(mat)==mat_to_SRs.end())
			mat_to_SRs.insert(std::pair<sample_t::matrix_t, quantity::SR_t> (mat,SR));
		else
		{
			mat_to_SRs.at(mat) << SR;
		}
	}
	
	for (auto& m : mat_to_SRs)
		m.second = m.second.mean();
	return mat_to_SRs;
}

// const std::map< sample_t::matrix_t, quantity::intensity_t > mgroups_::sims_t::matrix_to_intensity_sum()
// {
// 	std::map<sample_t::matrix_t,quantity::intensity_t> mat_to_I;
// 	for (auto& M : measurements())
// 	{
// 		sample_t::matrix_t mat = M->sample.matrix();
// 		if (!mat.is_set()) continue;
// 		quantity::intensity_t I = M->matrix_clusters().intensity_sum();
// 		if (!I.is_set()) continue;
// 		if (mat_to_I.find(mat)==mat_to_I.end())
// 			mat_to_I.insert(std::pair<sample_t::matrix_t, quantity::intensity_t> (mat,I.quantile(0.10)));
// 		else
// 			mat_to_I.at(mat) << I.quantile(0.10);
// 	}
// 	return mat_to_I;
// }

// const std::vector< std::pair<const sample_t::matrix_t&, const measurements_::sims_t::matrix_clusters_c>> mgroups_::sims_t::sample_mats_to_mat_clusters()
// {
// 	std::vector< std::pair<const sample_t::matrix_t&, const measurements_::sims_t::matrix_clusters_c>> Smat_to_Cmat;
// 	for (auto& M : measurements())
// 	{
// 		if (!M->sample.matrix().is_set()) continue;
// 		if (M->matrix_clusters().clusters.size()==0) continue;
// 		Smat_to_Cmat.push_back(std::pair<const sample_t::matrix_t&, const measurements_::sims_t::matrix_clusters_c&> (M->sample.matrix(),M->matrix_clusters()));
// 	}
// 	return Smat_to_Cmat;
// }

measurements_::sims_t* mgroups_::sims_t::measurement ( const measurements_::sims_t& M )
{
	for (auto& M_in_MG : measurements())
		if (*M_in_MG==M)
			return M_in_MG;
	return nullptr;
}



const std::vector<isotope_t> mgroups_::sims_t::matrix_isotopes()
{
// 	if (matrix_isotopes_p.size()>0)
// 		return matrix_isotopes_p;
	std::set<isotope_t> isos;
// 	std::cout << "F1" << std::endl;
	for (auto& M : measurements())
	{
// 		std::cout << "F7" << std::endl;
        if (!M->sample.matrix().is_set()) continue;
// 		std::cout << "F4" << std::endl;
        std::vector<isotope_t> mat_isos=M->sample.matrix().isotopes();
// 		std::cout << "F5" << std::endl;
		isos.insert(mat_isos.begin(),mat_isos.end());
// 		std::cout << "F6" << std::endl;
	}
// 	std::cout << "F2" << std::endl;
	std::vector<isotope_t> isos_vec = {isos.begin(),isos.end()};
	
	// keep abundance and/or substance_amount if there is only 1 known matrix from all reference samples within this group
	if (matrices().size()==1) 
	{
        //logger::info(3,"mgroups_::sims_t::matrix_isotopes()","found just 1 matrix, using its substance_amount for all matrix isotopes in this group",matrices().begin()->to_string());
		return isos_vec;
	}
// 	std::cout << "F3" << std::endl;
	// delete abundance and substance_amount because there are different ones in the group. calculate later
	for (auto& I : isos_vec)
	{
		I.substance_amount.clear();
		I.abundance.clear();
	}
	
	for (auto& mi : isos_vec)
    {
        //logger::info(3,"mgroups_::sims_t::matrix_isotopes()",mi.to_string());
    }
	if (isos.size()==0)
    {
        //logger::warning(1,"no matrix isotopes","");
    }
	
	return isos_vec;
}


void mgroups_::sims_t::set_reference_isotopes_in_measurements()
{
	const auto matrix_isos = matrix_isotopes();
	for (auto& M : measurements())
	{
		M->reference_isotopes = matrix_isos;
        //logger::info(3,"mgroups_::sims_t::set_reference_isotopes_in_measurements()",M->to_string_short() + " was set");
	}
}

const std::vector<isotope_t> mgroups_::sims_t::isotopes_corresponding_to_matrix_clusters()
{
	const auto matrix_isos = matrix_isotopes();
	
	std::set<isotope_t> isos;
	for (auto& MC : matrix_clusters())
		isos.insert(MC.corresponding_isotope(matrix_isos));
	return {isos.begin(),isos.end()};
}

const std::vector<cluster_t> mgroups_::sims_t::matrix_clusters()
{
	std::set<cluster_t> clusters;
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
	const std::vector<isotope_t> mat_isos = matrix_isotopes();
	if (mat_isos.size()==0)
	{
        //logger::error("mgroups_::sims_t::set_matrix_isotopes_in_unknown_samples()","matrix_isotopes()==0 unknown matrices","please include atleast 1 tabulated reference sample; check database","returning false");
		return false;
	}
	for (auto& M : measurements())
    {
        if (M->sample.matrix().elements.size()==0)
        {
            M->sample.set_matrix(mat_isos);
        }
    }
	return true;
}

std::set<sample_t::matrix_t> mgroups_::sims_t::matrices()
{
	std::set<sample_t::matrix_t> mats;
	for (auto& M : measurements())
        if (M->sample.matrix().is_set())
            mats.insert(M->sample.matrix());
// 	std::cout << std::endl << "mats.size()=" << mats.size() << std::endl;
// 	for (auto& m : mats)
// 		std::cout << m.to_string() << std::endl;
	return mats;
}

std::set<cluster_t> mgroups_::sims_t::clusters()
{
	std::set<cluster_t> cs;
	
	for (auto& M : measurements())
	{
		for (auto& C : M->clusters)
		{
			cs.insert({C.isotopes});
		}
	}
	
	return cs;
}

std::string mgroups_::sims_t::to_string_short() const
{
	std::stringstream out;
    out  << olcdb << "_g"<<group_id << "_" << tool_name() << "_" << settings()->to_string_short();
	return out.str();
}

void mgroups_::sims_t::export_origin_ascii(std::string path, const std::string delimiter)
{
	for (auto& M : measurements())
	{
        M->export_origin_ascii();
	}
}

bool mgroups_::sims_t::check_cluster_consistency()
{
	auto Ms = measurements();
	if (Ms.size()==0)
	{
        //logger::error("mgroups_::sims_t::check_cluster_consistency()","no measurements","returning false");
		return false;
	}
	const auto& ref_clusters = clusters();
	for (int i=0;i<Ms.size();i++)
	{
		auto M = Ms.at(i);
		for (auto& Cref : ref_clusters)
		{
			if (tools::find_in_V(Cref,M->clusters)==nullptr)
			{
                //logger::warning(3,"mgroups_::sims_t::check_cluster_consistency()","missing cluster " + Cref.to_string() +" in measurement " + M->to_string());
				return false;
			}
		}
	}
	return true;
}

bool mgroups_::sims_t::check_matrix_cluster_consistency()
{
	auto Ms = measurements();
	if (Ms.size()==0)
	{
        //logger::error("mgroups_::sims_t::check_matrix_cluster_consistency()","no measurements","returning false");
		return false;
	}
	const auto& ref_clusters = matrix_clusters();
	for (int i=0;i<Ms.size();i++)
	{
		auto M = Ms.at(i);
		for (auto& Cref : ref_clusters)
		{
			if (tools::find_in_V(Cref,M->matrix_clusters().cluster_names())==nullptr)
			{
                //logger::error("mgroups_::sims_t::check_matrix_cluster_consistency()","missing matrix_cluster " + Cref.to_string() +" in measurement " + M->to_string());
				return false;
			}
		}
	}
	return true;
}

void mgroups_::sims_t::set_natural_abundances_in_matrix_clusters()
{
	for (auto& M : measurements())
		M->matrix_clusters().set_natural_abundances();
}

const std::map<cluster_t*,quantity::substance_amount_t> mgroups_::sims_t::matrix_cluster_to_matrix_iso_substance_amount()
{
	std::map<cluster_t*,quantity::substance_amount_t> matrix_C_to_I;
	const auto MCs = matrix_clusters();
	const auto MIs = matrix_isotopes();
	for (auto& M : measurements())
	{
        if (!M->sample.matrix().is_set()) continue;
		for (auto& MC : MCs)
		{
			if (M->cluster(MC)==nullptr)
			{
                //logger::error("mgroups_::sims_t::matrix_cluster_to_matrix_iso_substance_amount()","matrix cluster: "+ MC.to_string() + " not found in measurement " + M->to_string_short(),"skipping");
				continue;
			}
			auto corresponding_isotope = MC.corresponding_isotope(MIs);
			quantity::substance_amount_t amount;
            if (M->sample.matrix().isotope(corresponding_isotope)==nullptr)
				amount = quantity::substance_amount_t({0});
			else
                amount = M->sample.matrix().isotope(corresponding_isotope)->substance_amount;
			matrix_C_to_I.insert(std::pair<cluster_t*,quantity::substance_amount_t> (M->cluster(MC),amount) );
// 			logger::debug(9,"mgroups_::sims_t::matrix_cluster_to_matrix_iso_substance_amount()","MC="+MC.to_string()+" " + tools::to_string(M->cluster(MC)),"isotope="+corresponding_isotope.to_string() + " abu="+abu.to_string(),M->to_string());
		}
	}
	return matrix_C_to_I;
}

quantity::table_t mgroups_::sims_t::matrix_concentrations()
{
	quantity::table_t table;
	for (const auto& I : matrix_isotopes())
		table.add(matrix_concentrations(I));
	return table;
}

quantity::table_t mgroups_::sims_t::matrix_concentrations(const isotope_t& isotope)
{
	quantity::table_t table;
	data_collectors_t::sims_t dc(measurements());
	const auto col = dc.get_column_concentrations_from_sample_matrix(isotope);
	table.add(col);
	return table;
}


quantity::table_t mgroups_::sims_t::matrix_concentrations(const element_t& element)
{
	quantity::table_t table;
	data_collectors_t::sims_t dc(measurements());
	const auto col = dc.get_column_concentrations_from_sample_matrix(element);
	table.add(col);
	return table;
}

quantity::table_t mgroups_::sims_t::intensities(const cluster_t& cluster)
{
	quantity::table_t table;
	data_collectors_t::sims_t collect_data(measurements());
	const auto col = collect_data.get_column_intensities_from_cluster(cluster);
	table.add(col);
	return table;
}

quantity::table_t mgroups_::sims_t::intensities()
{
	quantity::table_t table;
	for (const auto& C : clusters())
		table.add(intensities(C));
	return table;
}

quantity::table_t mgroups_::sims_t::ref_intensities()
{
	quantity::table_t table;
	for (const auto& C : matrix_clusters())
		table.add(intensities(C));
	return table;
}

quantity::table_t mgroups_::sims_t::concentrations(const cluster_t& cluster)
{
	quantity::table_t table;
	data_collectors_t::sims_t collect_data(measurements());
	const auto col = collect_data.get_column_concentrations_from_cluster(cluster);
	table.add(col);
	return table;
}

quantity::table_t mgroups_::sims_t::concentrations()
{
	quantity::table_t table;
	for (const auto& C : clusters())
		table.add(concentrations(C));
	return table;
}

quantity::table_t mgroups_::sims_t::RSFs(const cluster_t& cluster)
{
	quantity::table_t table;
	data_collectors_t::sims_t collect_data(measurements());
	const auto col = collect_data.get_column_RSFs_from_cluster(cluster);
	table.add(col);
	return table;
// 	for (const auto& C : MG.clusters())
// 	{
// 		auto col = collect_data.get_column_SFs_from_cluster(C);
// 		col.name = C.to_string();
// 		if (col.is_set())
// 			table.add(col);
// 	}
// 	for (const auto& C : MG.matrix_clusters())
// 	{
// 		auto col = collect_data.get_column_intensities_from_cluster(C);
// 		col.name = C.to_string();
// 		if (col.is_set())
// 			table.add(col);
// 	}
// 	return table.remove_empty_lines();
}

quantity::table_t mgroups_::sims_t::RSFs()
{
	quantity::table_t table;
	for (const auto& C : clusters())
		table.add(RSFs(C));
	return table;
}

quantity::table_t mgroups_::sims_t::SFs(const cluster_t& cluster)
{
	quantity::table_t table;
	data_collectors_t::sims_t collect_data(measurements());
	const auto col = collect_data.get_column_SFs_from_cluster(cluster);
	table.add(col);
	return table;
}

quantity::table_t mgroups_::sims_t::SFs()
{
	quantity::table_t table;
	for (const auto& C : clusters())
		table.add(SFs(C));
	return table;
}

quantity::table_t mgroups_::sims_t::SRs()
{
	quantity::table_t table;
	data_collectors_t::sims_t collect_data(measurements());
	table.add(collect_data.get_column_sputter_rates());
	return table;
}

quantity::table_t mgroups_::sims_t::measurement_names()
{
	quantity::table_t table;
	data_collectors_t::sims_t collect_data(measurements());
	const auto col = collect_data.measurement_line_names();
	table.add(col);
	return table;
}
