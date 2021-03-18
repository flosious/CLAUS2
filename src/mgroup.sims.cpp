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

vector<measurements_::sims_t *> mgroups_::sims_t::measurements()
{
	return {};
}

vector<cluster_t> mgroups_::sims_t::reference_clusters()
{
	
	set<cluster_t> all_ref_clusters;
	
	for (auto& M : measurements())
	{
		for (auto& ref_c : M->reference_clusters())
			all_ref_clusters.insert(ref_c->isotopes);
	}

	/*intersection of all reference clusters over all clusters in each measurement*/
	for (auto& M : measurements())
	{
		for (auto ref_c=all_ref_clusters.begin();ref_c!=all_ref_clusters.end();ref_c++)
			if(find(M->clusters.begin(),M->clusters.end(),*ref_c)==M->clusters.end())
			{
				all_ref_clusters.erase(ref_c);
				ref_c--;
			}
	}
	
	return {all_ref_clusters.begin(),all_ref_clusters.end()};
}
