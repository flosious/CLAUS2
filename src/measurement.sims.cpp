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

#include "measurement.hpp"




/************************************/
/* *measurements_::sims_t::filter_t */
/************************************/
measurements_::sims_t::filter_t::filter_t(const measurements_::sims_t& M) : M(M)
{
}

measurements_::sims_t measurements_::sims_t::filter_t::impulses()
{
	sims_t M_filtered = M;
	for (auto& C:M_filtered.clusters)
	{
		C = C.filter_impulse();
	}
	return M_filtered;
}

/***********************************/
/****** measurements_::sims_t ******/
/***********************************/

matrix_clusters_c measurements_::sims_t::matrix_clusters()
{

	return matrix_clusters_c(clusters,reference_isotopes);
}

measurements_::sims_t::filter_t measurements_::sims_t::filter() const
{
	return filter_t(*this); 
}

void measurements_::sims_t::add_clusters(std::vector<cluster_t>& clusters)
{
	for (auto& C: clusters)
	{
		if (tools::find_in_V(C,clusters)==nullptr)
			clusters.push_back(C);
	}
}

bool measurements_::sims_t::are_clusters_in_measurement(const std::vector<cluster_t>& clusters_s) const
{
	for (auto& C : clusters_s)
	{
		if (cluster(C)==nullptr)
			return false;
	}
	return true;
}

bool measurements_::sims_t::are_clusters_in_measurement(const cluster_t& cluster_s) const
{
	if (cluster(cluster_s)==nullptr)
		return false;
	return true;
}

bool measurements_::sims_t::are_intensities_of_clusters_set(const std::vector<cluster_t>& clusters_s) const
{
	for (auto& C : clusters_s)
	{
		if (cluster(C)==nullptr)
			return false;
		if (!cluster(C)->intensity.is_set())
			return false;
	}
	return true;
}

bool measurements_::sims_t::are_intensities_of_clusters_set(const cluster_t& cluster_s) const
{
	if (cluster(cluster_s)==nullptr)
		return false;
	if (!cluster(cluster_s)->intensity.is_set())
		return false;
	return true;
}

measurements_::sims_t::calc_t measurements_::sims_t::calc(bool overwrite)
{
	return calc_t{(*this),overwrite};
}

measurements_::sims_t::sims_t(files_::sims_t::name_t& filename, files_::sims_t::contents_t& filecontents, std::string method, database_t& sql_wrapper, std::vector<const quantity::total_sputter_depth_t*> total_sputter_dephs) :
    measurement_t(filename,method,sql_wrapper), clusters(filecontents.clusters()), reference_isotopes(sample.matrix().isotopes()),
    class_logger(__FILE__,"measurements_::sims_t")
{
//    set_default_export_location();
	crater.total_sputter_depths << filename.total_sputter_depths();
	for (auto& tsd : total_sputter_dephs)
		if (tsd!=nullptr)
			crater.total_sputter_depths << *tsd;
}

//void measurements_::sims_t::set_default_export_location()
//{
//    export_location = tools::file::extract_directory_from_filename(filename_with_path);
//    export_location = tools::file::check_directory_string(export_location);
//    if (olcdb >= 0)
//        export_location += std::to_string(olcdb);
//    else
//        export_location += "unknown olcdb";
//    tools::file::check_directory_string(export_location);
//}

std::string measurements_::sims_t::to_string(const std::string del)
{
    //logger::debug(31,"measurements_::sims_t::to_string()","","","entering");
	std::stringstream ss;
	ss << measurement_t::to_string() << del;
// 	if (crater.total_sputter_depths.is_set() || crater.linescans.size()>0)
	ss << "crater: ";
	if (crater.sputter_depth.is_set())
		ss << crater.sputter_depth.to_string() << del;
	if (crater.sputter_time.is_set())
		ss << crater.sputter_time.to_string() << del;
	if (crater.total_sputter_depths.is_set())
		ss << crater.total_sputter_depths.to_string() << del;
	if (crater.linescans.size()>0)
		ss << "linescans: <" << crater.linescans.size() <<">" << del;
	ss << "clusters: <" << clusters.size() << ">" << del;
    //logger::debug(31,"measurements_::sims_t::to_string()","","","exiting");
	return ss.str();
}

const std::string measurements_::sims_t::to_string_short(const std::string del) const
{
    //logger::debug(31,"measurements_::sims_t::to_string()","","","entering");
	std::stringstream ss;
	ss << measurement_t::to_string();
	return ss.str();
}

void measurements_::sims_t:: export_origin_ascii(std::string path, const std::string delimiter)
{
	if (clusters.size()==0)
	{
        //logger::warning(1,"measurements_::sims_t::export_origin_ascii","clusters.size()==0",to_string());
		return;
	}
// 	std::cout << measurements_::sims_t::to_string() << std::endl;
    //logger::debug(6,"measurements_::sims_t::export_origin_ascii","path",path,"starting export: " + to_string_short());
	std::vector<origin_t::column_t> cols;
	/*time-intensity-profile*/
	std::stringstream comment,longname;
	comment << "crater ";
    if (sample.simple_name!="")
        comment << sample.simple_name;
	else
        comment << sample.lot << sample.lot_split << "_w" << sample.wafer_string();
	if (crater.sputter_depth.is_set())
	{
		longname << "sputter_depth";
		cols.push_back({crater.sputter_depth.data(),longname.str(),crater.sputter_depth.unit().to_string(),comment.str()});
	}
	else if (crater.sputter_time.is_set())
	{
		longname << "sputter_time";
		cols.push_back({crater.sputter_time.data(),longname.str(),crater.sputter_time.unit().to_string(),comment.str()});
	}
	for (auto& C : clusters)
	{
		comment.str("");
		longname.str("");
		comment << "^" << C.to_string(" ^");
		longname << "^" << C.to_string(" ^") ;
		
		quantity::concentration_t C_out = C.concentration;
		
		for (const auto& iso : C.isotopes)
		{
			if (!iso.abundance.is_set())
				continue;
			//calculate elemental concentration 
			C_out = C.concentration / iso.abundance;
			comment.str("");
			longname.str("");
			comment << iso.symbol;
			longname << iso.symbol;
			break; // error prone to clusters with more than 2 different isotopes
		}
		
        if (sample.simple_name!="")
            comment << " " << sample.simple_name;
		else
            comment <<" " << sample.lot << sample.lot_split << "_w" << sample.wafer_string();
        if (sample.chip.is_set())
            comment << "_" << sample.chip.to_string();
        if (sample.monitor!="")
            comment << "_m" << sample.monitor;
		if (C_out.is_set()) 
		{
			if (C_out.dimension() == quantity::dimensions::SI::relative)
				C_out = C_out.change_unit("at%");
			else
				C_out = C_out.change_unit(units::derived::atoms_per_ccm);
			cols.push_back({C_out.data(),longname.str() + " concentration",C_out.unit().to_string(),comment.str()});
		}
	}
	comment.str("");
	longname.str("");
	comment << "crater ";
	longname << "sputter_time";
    if (sample.simple_name!="")
        comment << sample.simple_name;
	else
        comment << sample.lot << sample.lot_split << "_w" << sample.wafer_string();
    if (sample.chip.is_set())
            comment << "_" << sample.chip.to_string();
    if (sample.monitor!="")
        comment << "_m" << sample.monitor;
	if (crater.sputter_time.is_set())
	{
		cols.push_back({crater.sputter_time.data(),longname.str(),crater.sputter_time.unit().to_string(),comment.str()});
	}
	for (auto& C : clusters)
	{
		std::ostringstream comment,longname;
		comment << "^" << C.to_string(" ^") ;
		longname << "^" << C.to_string(" ^") ;
		
        if (sample.simple_name!="")
            comment << " " << sample.simple_name;
		else
            comment << " " << sample.lot << sample.lot_split << "_w" << sample.wafer_string();
        if (sample.chip.is_set())
            comment << "_" << sample.chip.to_string();
        if (sample.monitor!="")
            comment << "_m" << sample.monitor;
		if (C.intensity.is_set()) 
			cols.push_back({C.intensity.data(),longname.str() + " intensity",C.intensity.unit().to_string(),comment.str()});
	}
	if (crater.sputter_current().is_set())
	{
		comment.str("");
		comment << "crater ";
        if (sample.simple_name!="")
            comment << sample.simple_name;
		else
            comment << sample.lot << sample.lot_split << "_w" << sample.wafer_string();
        if (sample.chip.is_set())
            comment << "_" << sample.chip.to_string();
        if (sample.monitor!="")
            comment << "m_" << sample.monitor;
		cols.push_back({crater.sputter_current().data(),"crater "+ crater.sputter_current().name(),crater.sputter_current().unit().to_string(),comment.str()});
	}
	
	std::vector<std::vector<std::string>> matrix;
	for (auto& col : cols)
		matrix.push_back(col.vec());
	matrix = tools::mat::transpose_matrix(matrix);
	std::string out = tools::mat::format_matrix_to_string(&matrix);
	if (path!="")
	{
        path = tools::file::check_directory_string(path);
	}
    else
    {
        path = tools::file::check_directory_string(export_location);
    }
    tools::file::mkpath(path,0750);
	std::stringstream filename;
	filename << olcdb;
    if (sample.simple_name!="")
        filename << "_" << sample.simple_name;
	else
        filename << "_" << sample.lot << sample.lot_split << "_w" << sample.wafer_string();
	
    if (sample.monitor!="")
        filename << "_m" << sample.monitor;
    if (sample.chip.is_set())
        filename << "_c" << sample.chip.to_string();
    if (group_id!="")
        filename << "_g" << group_id;
	if (repetition!="")
		filename << "_r" << repetition;
	filename << ".txt";
// 	cout  << std::endl << "sample.wafer: " << sample.wafer << std::endl;
// 	cout  << std::endl << "sample.wafer_string(): " << sample.wafer_string() << std::endl;
	tools::file::write_to_file(path+filename.str(),&out,false);
}

isotope_t measurements_::sims_t::isotope_corresponding_to_cluster(const cluster_t& cluster)
{
	return cluster.corresponding_isotope(reference_isotopes);
}


std::set<cluster_t *> measurements_::sims_t::clusters_corresponding_to_isotope(const isotope_t& isotope)
{
	std::set<cluster_t*> Cs;
	for (auto& C : clusters)
	{
		if (find(clusters.begin(),clusters.end(),C)!=clusters.end())
		{
			Cs.insert(&C);
		}
	}
	if (Cs.size()==0)
    {
        //logger::error("measurements_::sims_t::clusters_corresponding_to_isotope()","can not fond corresponding cluster","","return empty");
    }
	return Cs;
}

bool measurements_::sims_t::add(measurements_::sims_t& adder)
{
	if (*this != adder)
		return false;
	crater.total_sputter_depths << adder.crater.total_sputter_depths;
	crater.linescans.insert(crater.linescans.end(),adder.crater.linescans.begin(),adder.crater.linescans.end());
	if (!crater.sputter_beam.sputter_current.is_set())
		crater.sputter_beam = adder.crater.sputter_beam;
	return true;
}

cluster_t* measurements_::sims_t::cluster(const isotope_t& iso)
{
	for (auto& C : clusters)
	{
		if (iso==C.corresponding_isotope(reference_isotopes))
			return &C;
	}
	return nullptr;
}

cluster_t* measurements_::sims_t::cluster(const cluster_t& cluster_s)
{
	for (auto& C : clusters)
		if (cluster_s==C)
			return &C;
	return nullptr;
}

const cluster_t* measurements_::sims_t::cluster(const cluster_t& cluster_s) const
{
	for (auto& C : clusters)
		if (cluster_s==C)
			return &C;
	return nullptr;
}

///returns a copy of the RSF of the specified cluster_s
cluster_t::RSF_t measurements_::sims_t::RSF(const cluster_t& cluster_s) const
{
	auto c = cluster(cluster_s);
	if (c==nullptr)
		return {};
	return c->RSF;
}
///returns a copy of the SF of the specified cluster_s
quantity::SF_t measurements_::sims_t::SF(const cluster_t& cluster_s) const
{
	auto c = cluster(cluster_s);
	if (c==nullptr)
		return {};
	return c->SF;
}
///returns a copy of the intensity of the specified cluster_s
quantity::intensity_t measurements_::sims_t::intensity(const cluster_t& cluster_s) const
{
	auto c = cluster(cluster_s);
	if (c==nullptr)
		return {};
	return c->intensity;
}
///returns a copy of the concentration of the specified cluster_s
quantity::concentration_t measurements_::sims_t::concentration(const cluster_t& cluster_s) const
{
	auto c = cluster(cluster_s);
	if (c==nullptr)
		return {};
	return c->concentration;
}

quantity::map_t measurements_::sims_t::concentration_vs_sputter_depth(const cluster_t& cluster) const
{
	return {crater.sputter_depth,concentration(cluster)};
}
quantity::map_t measurements_::sims_t::concentration_vs_sputter_time(const cluster_t& cluster) const
{
	return {crater.sputter_time,concentration(cluster)};
}
quantity::map_t measurements_::sims_t::intensity_vs_sputter_depth(const cluster_t& cluster) const
{
	return {crater.sputter_depth,intensity(cluster)};
}
quantity::map_t measurements_::sims_t::intensity_vs_sputter_time(const cluster_t& cluster) const
{
	return {crater.sputter_time,intensity(cluster)};
}

bool measurements_::sims_t::change_sputter_depth_resolution(quantity::sputter_depth_t sputter_depth_res)
{
    log_f;
	if (!sputter_depth_res.is_set())
	{
        //logger::error("measurements_::sims_t::change_resolution","!sputter_depth_res.is_set()","","returning this");
        return false;
	}
	if (sputter_depth_res.data().size()==1 && sputter_depth_res.data().at(0)<=0)
	{
        //logger::error("measurements_::sims_t::change_resolution","!sputter_depth_res is <= 0","","returning this");
        return false;
	}
    auto copy_M = *this;
	copy_M.crater = copy_M.crater.change_resolution(sputter_depth_res);
    logger.debug("copy_M.crater.sputter_depth").value(copy_M.crater.sputter_depth.to_string_detailed());
	if (!copy_M.crater.sputter_depth.is_set())
        return false;
	for (auto& C: copy_M.clusters)
	{
		C = C.interpolate(copy_M.crater.sputter_depth,crater.sputter_depth);
        if (!C.is_set())
        {
            return false;
        }
	}
    *this = copy_M;
    return true;
}
bool measurements_::sims_t::change_sputter_time_resolution(quantity::sputter_time_t sputter_time_res)
{
	if (!sputter_time_res.is_set())
	{
        //logger::error("measurements_::sims_t::change_resolution","!sputter_time_res.is_set()","","returning this");
        return false;
	}
	if (sputter_time_res.data().size()==1 && sputter_time_res.data().at(0)<=0)
	{
        //logger::error("measurements_::sims_t::change_resolution","!sputter_time_res is <= 0","","returning this");
        return false;
	}
	sims_t copy_M = *this;
	copy_M.crater = copy_M.crater.change_resolution(sputter_time_res);
	if (!copy_M.crater.sputter_time.is_set()) 
        return false;
	for (auto& C: copy_M.clusters)
	{
		C = C.interpolate(copy_M.crater.sputter_time,crater.sputter_time);
		if (!C.is_set())
		{
            return false;
		}
	}
    *this = copy_M;
    return true;
}

