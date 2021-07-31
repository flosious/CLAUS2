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


measurements_::dsims_t::dsims_t(files_::dsims_t& dsims_file, 
								list<sample_t>& samples_list,
								database_t& sql_wrapper,
								vector<files_::jpg_t>* jpg_files,
								vector<files_::profiler_t>* profiler_files
   							) : 
				sims_t(dsims_file.name,dsims_file.contents,samples_list,"dsims",sql_wrapper,jpg_files,profiler_files), 
				settings(dsims_file.name,dsims_file.contents)
// 				clusters(dsims_file.contents.clusters)
{	
	crater.sputter_beam = dsims_file.contents.Ipr();
	
	if (dsims_file.contents.total_sputtering_time().is_set())
		crater.total_sputter_time_s = total_sputter_time_t(dsims_file.contents.total_sputtering_time());
	else if (crater.sputter_beam.sputter_time.is_set())
		crater.total_sputter_time_s = total_sputter_time_t(crater.sputter_beam.sputter_time.max());
	else
		crater.total_sputter_time_s = total_sputter_time_t(dsims_file.contents.total_acquisition_time());
	
	if (clusters.size()==0)
	{
		logger::error("measurements_::dsims_t::dsims_t()","clusters.size()==0","","return");
		return;
	}
	for (auto& c : clusters)
	{
		if (c.sputter_depth.is_set() || c.sputter_time.is_set()) 
			continue;
		logger::error("measurements_::dsims_t::dsims_t","neither sputer_depth or sputter_time is set in cluster",c.to_string(),"aboarting");
		return;
	}
	
	/*regularize data*/
	if (clusters.begin()->sputter_depth.is_set())
	{
		crater.sputter_depth = crater.common_sputter_depth(clusters);
		logger::debug(7,"measurements_::dsims_t::dsims_t","crater.sputter_depth()=" + crater.sputter_depth.to_string());
	}
	if (!crater.sputter_depth.is_set())
	{
		crater.sputter_time = crater.common_sputter_time(clusters);
		logger::debug(7,"measurements_::dsims_t::dsims_t","crater.sputter_time=" + crater.sputter_time.to_string());
	}
	
	if (!crater.sputter_time.is_set() && !crater.sputter_depth.is_set())
	{
		logger::error("measurements_::dsims_t::dsims_t","!crater.sputter_time.is_set() && !crater.sputter_depth().is_set()","check all clusters for sputter_time or depth","returning");
		return;
	}
	
	if (crater.sputter_beam.sputter_current.is_set())
	{
		if (crater.sputter_beam.sputter_depth.is_set() && crater.sputter_depth.is_set())
		{
			logger::debug(11,"measurements_::dsims_t::dsims_t()","crater.sputter_beam.sputter_depth:",crater.sputter_beam.sputter_depth.to_string());
			logger::debug(11,"measurements_::dsims_t::dsims_t()","crater.sputter_depth:",crater.sputter_depth.to_string());
			crater.sputter_beam.sputter_current = crater.sputter_beam.sputter_current.interp(crater.sputter_beam.sputter_depth,crater.sputter_depth);
			crater.sputter_beam.sputter_depth.clear();
		}
		else if (crater.sputter_beam.sputter_time.is_set() && crater.sputter_time.is_set())
		{
			logger::debug(10,"measurements_::dsims_t::dsims_t()","crater.sputter_beam.sputter_current.interp(crater.sputter_beam.sputter_time,crater.sputter_time)=",crater.sputter_beam.sputter_current.interp(crater.sputter_beam.sputter_time,crater.sputter_time).to_string());
			crater.sputter_beam.sputter_current = crater.sputter_beam.sputter_current.interp(crater.sputter_beam.sputter_time,crater.sputter_time);
			crater.sputter_beam.sputter_time.clear();
		}
		else
			logger::warning(2,"measurements_::dsims_t::dsims_t","could not interp crater.sputter_beam.sputter_current","","skipping");
	}
	
	for (auto& C: clusters)
	{
		if (C.sputter_time.is_set() && crater.sputter_time.is_set())
		{
			C = C.interpolate(crater.sputter_time,C.sputter_time);
			logger::debug(7,"measurements_::dsims_t::dsims_t","C.change_sputter_time",C.to_string());
			C.sputter_time.clear();
		}
		else if (C.sputter_depth.is_set() && crater.sputter_depth.is_set())
		{
			C = C.interpolate(crater.sputter_depth,C.sputter_depth);
			logger::debug(7,"measurements_::dsims_t::dsims_t","C.change_sputter_depth",C.to_string());
			C.sputter_depth.clear();
		}
		else
			logger::error("measurements_::dsims_t::dsims_t","could not change sputter_time or sputter_depth for cluster",C.to_string(),"skipping");
	}
}

bool measurements_::dsims_t::operator!=(const measurements_::dsims_t& obj) const
{
	return !operator==(obj);
}

bool measurements_::dsims_t::operator==(const measurements_::dsims_t& obj) const
{
	if (measurement_t::operator!=(obj)) return false;
	if (settings!=obj.settings) return false;
	return true;
}
