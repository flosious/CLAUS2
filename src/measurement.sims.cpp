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


measurements_::sims_t::sims_t(files::sims_t::name_t& filename, 
							  files::sims_t::contents_t& filecontents, 
							  list<sample_t>& samples_list, 
							  vector<files::jpg_t>* jpg_files,
							  vector<files::profiler_t>* profiler_files) : 
									measurement_t(filename,filecontents,samples_list)
{
	clusters = filecontents.clusters();
	
	/*JPEG files*/
	if (jpg_files!=nullptr)
	{
// 		for (vector<files::jpg_t>::reverse_iterator it=jpg_files->rbegin();it!=jpg_files->rend();it++)
		for (vector<files::jpg_t>::iterator it=jpg_files->begin();it!=jpg_files->end();it++)
		{
			measurement_t JM(it->name,samples_list);
			if (*this != JM) continue;
			crater.total_sputter_depths << it->name.total_sputter_depths();
// 			jpg_files->erase(next(it).base());
			jpg_files->erase(it);
			it--;
		}
	}
	
	/*PROFILER files*/
	if (profiler_files!=nullptr)
	{
// 		for (vector<files::profiler_t>::reverse_iterator it=profiler_files->rbegin();it!=profiler_files->rend();it++)
		for (vector<files::profiler_t>::iterator it=profiler_files->begin();it!=profiler_files->end();it++)
		{
			measurement_t PM(it->name,samples_list);
			if (*this != PM) continue;
			crater.linescans.push_back(it->contents.linescan());
			crater.total_sputter_depths << it->name.total_sputter_depths();
// 			profiler_files->erase(next(it).base());
			profiler_files->erase(it);
			it--;
		}
	}

}

measurements_::sims_t::sims_t(files::sims_t::name_t& filename, list<sample_t>& samples_list) : measurement_t(filename,samples_list)
{
	crater.total_sputter_depths = filename.total_sputter_depths();
}



std::__cxx11::string measurements_::sims_t::to_string(const std::__cxx11::string del) const
{
	stringstream ss;
	ss << measurement_t::to_string() << del;
	ss << "crater: " << crater.total_sputter_depths.to_string() << del;
	ss << "clusters: <" << clusters.size() << ">" ;
	return ss.str();
}
