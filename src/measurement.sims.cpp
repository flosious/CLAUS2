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

void measurements_::sims_t::add_clusters(vector<cluster_t>& clusters_s)
{
	cluster_t* C_p;
	for (auto& C: clusters_s)
	{
		C_p = tools::find_in_V(C,clusters_s);
		if (C_p==nullptr)
			clusters.push_back(C);
	}
}

measurements_::sims_t::sims_t(filenames::sims_t& filename, files::sims_t& file, list<sample_t>& samples_list) : measurement_t(filename,file,samples_list)
{
	clusters = file.clusters();
}

std::__cxx11::string measurements_::sims_t::to_string(const std::__cxx11::string del) const
{
	stringstream ss;
	ss << measurement_t::to_string() << del;
	ss << "clusters: <" << clusters.size() << ">" ;
	return ss.str();
}
