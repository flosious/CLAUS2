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
	//check reference_clusters
}
