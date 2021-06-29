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

measurements_::sims_t::calc_t::calc_t(sims_t& measurement) : M(measurement)
{
}

measurements_::sims_t::calc_t::implant_c measurements_::sims_t::calc_t::implant(cluster_t& cluster)
{
	return {M,cluster};
}

// measurements_::sims_t& measurements_::sims_t::calc_t::sputter_depth() const
// {
// 	M.crater.sputter_depth={};
// 	if (M.crater.SR.is_set() && M.crater.sputter_time.is_set())
// 		M.crater.sputter_depth = M.crater.SR * M.crater.sputter_time;
// 	return M;
// }

measurements_::sims_t::calc_t::SD_c measurements_::sims_t::calc_t::SD()
{
	return SD_c(M);
}

measurements_::sims_t::calc_t::SR_c measurements_::sims_t::calc_t::SR()
{
	return SR_c(M);
}

measurements_::sims_t::calc_t::SF_c measurements_::sims_t::calc_t::SF()
{
	return SF_c(M);
}

measurements_::sims_t::calc_t::RSF_c measurements_::sims_t::calc_t::RSF()
{
	return RSF_c(M);
}

measurements_::sims_t::calc_t::concentration_c measurements_::sims_t::calc_t::concentration()
{
	return concentration_c(M);
}

// measurements_::sims_t& measurements_::sims_t::calc_t::concentration(cluster_t& cluster) const
// {
// 	cluster.concentration={};
// 	if (cluster.SF.is_set() && cluster.intensity.is_set())
// 		cluster.concentration = cluster.SF * cluster.intensity;
// 	return M;
// }
// 
// measurements_::sims_t& measurements_::sims_t::calc_t::concentrations() const
// {
// 	for (auto& C:M.clusters)
// 		concentration(C);
// 	return M;
// }

/*************************/
/**        SD_c         **/
/*************************/

measurements_::sims_t::calc_t::SD_c::SD_c(measurements_::sims_t& measurement) : M(measurement)
{
}

measurements_::sims_t& measurements_::sims_t::calc_t::SD_c::from_SR()
{
	M.crater.sputter_depth={};
	if (M.crater.SR.is_set() && M.crater.sputter_time.is_set())
	{
		M.crater.sputter_depth = M.crater.SR * M.crater.sputter_time;
		logger::info(3,"measurements_::sims_t::calc_t::SD_c::from_SR()","M.crater.SR.is_set() && M.crater.sputter_time.is_set()");
	}
	return M;
}

/************************************/
/**        concentration_c         **/
/************************************/

measurements_::sims_t::calc_t::concentration_c::concentration_c(measurements_::sims_t& measurement) : M(measurement)
{
}

measurements_::sims_t& measurements_::sims_t::calc_t::concentration_c::from_SF()
{
	for (auto& C : M.clusters)
		from_SF(C);
	
	return M;
}

measurements_::sims_t& measurements_::sims_t::calc_t::concentration_c::from_SF(cluster_t& cluster)
{
	cluster.concentration={};
	if (cluster.SF.is_set() && cluster.intensity.is_set())
		cluster.concentration = cluster.SF * cluster.intensity;
	return M;
}

/*************************/
/**        SR_c         **/
/*************************/
measurements_::sims_t::calc_t::SR_c::SR_c(measurements_::sims_t& measurement) : M(measurement)
{
}

measurements_::sims_t& measurements_::sims_t::calc_t::SR_c::from_crater_depths()
{
	M.crater.SR={};
	if (M.crater.total_sputter_depth().is_set() && M.crater.total_sputter_time(&M.clusters).is_set())
	{
		M.crater.SR = M.crater.total_sputter_depth()/M.crater.total_sputter_time();
		logger::info(3,"measurements_::sims_t::calc_t::SR_c::from_crater_depths()",M.crater.SR.to_string() + " = " + M.crater.total_sputter_depth().to_string() + " / " + M.crater.total_sputter_time().to_string());
	}
	return M;
}

measurements_::sims_t& measurements_::sims_t::calc_t::SR_c::from_implant_max()
{
	M.crater.SR={};
	unsigned int start_idx;
	for (auto& C:M.clusters)
	{
		if (!C.intensity.is_set() && !C.concentration.is_set())	continue;
		if (!M.crater.sputter_time.is_set()) continue;
		
// 		concentration_t C_max_db = M.sample->implant(C.corresponding_isotope(M.matrix_cluster().isotopes())).concentration_maximum;
// 		if (!C_max_db.is_set()) continue;
		
		if (C.intensity.is_set())
		{
			SR_t SR = implant_c(M,C).SR();
// 			cout << C.to_string() << " = " <<SR.to_string() << endl;
// 			if (SR.is_set())
			M.crater.SR << SR;
			
// 			cout << implant_c(M,C).SR().to_string() << endl;
		}
		
// 			M.crater.SR << implant_c(M,C).SR().crater.SR; ///collecting skalar values of SR
	}
// 	cout << "M.crater.SR = " << M.crater.SR.to_string() << endl;
	///M.crater.SR is now populated with SRs from all different clusters
	M.crater.SR = M.crater.SR.mean();
// 	cout << "M.crater.SR.mean() = " << M.crater.SR.to_string() << endl;
	return M;
}

/*************************/
/**        SF_c         **/
/*************************/
measurements_::sims_t::calc_t::SF_c::SF_c(measurements_::sims_t& measurement) : M(measurement)
{
}

measurements_::sims_t& measurements_::sims_t::calc_t::SF_c::from_db_dose()
{
	for (auto& C: M.clusters)
		from_db_dose(C);
	return M;
}

measurements_::sims_t& measurements_::sims_t::calc_t::SF_c::from_db_dose(cluster_t& cluster)
{
	cluster.SF=	implant_c(M,cluster).SF_from_dose();
	return M;
}

measurements_::sims_t& measurements_::sims_t::calc_t::SF_c::from_db_max()
{
	for (auto& C: M.clusters)
		from_db_max(C);
	return M;
}

measurements_::sims_t& measurements_::sims_t::calc_t::SF_c::from_db_max(cluster_t& cluster)
{
	cluster.SF=implant_c(M,cluster).SF_from_max();
	return M;
}


/*************************/
/**       RSF_c         **/
/*************************/
measurements_::sims_t::calc_t::RSF_c::RSF_c(measurements_::sims_t& measurement) : M(measurement)
{
}

measurements_::sims_t & measurements_::sims_t::calc_t::RSF_c::from_SF_mean_ref()
{
	for (auto& C: M.clusters)
		from_SF_mean_ref(C);
	return M;
}

measurements_::sims_t & measurements_::sims_t::calc_t::RSF_c::from_SF_mean_ref(cluster_t& cluster)
{
// 	if (!cluster.SF.is_set())
// 		return M;
	cluster.RSF = M.matrix_clusters(M.sample->matrix().isotopes).intensity_sum().mean() * cluster.SF;
	return M;
}



/*************************/
/**      implant_c      **/
/*************************/

measurements_::sims_t::calc_t::implant_c::implant_c(sims_t& measurement, cluster_t& cluster) : M(measurement), cluster(cluster)
{
}


/*
 * lets keep it stupid simple:
 * 0)	asume the quantity.data are ordered from lowest to highest corresponding X
 * 1)	find the nearest local minimum, beginning from the end (highest X-axis value)
 */
unsigned int measurements_::sims_t::calc_t::implant_c::minimum_starting_position(quantity_t Y)
{
	if (!Y.is_set() || Y.data.size()<5)
		return 0;
	vector<int> maxIdx, minIdx;
	Y = Y.filter_gaussian(10,1).log10();
// 	double deviation = 2*abs(Y.data.at(12)-Y.data.at(11));
	double deviation = 0.05*statistics::get_mad_from_Y(Y.data);
// 	Y = Y.moving_window_mean(10);
	Y = Y.polyfit(17);

	
	

	if (!statistics::get_extrema_indices(maxIdx,minIdx,Y.data,deviation))
	{
		logger::error("measurements_::sims_t::calc_t::implant_c::minimum_starting_position()","statistics::get_extrema_indices","false","returning 0");
		return 0;
	}
	sort(maxIdx.begin(),maxIdx.end());
	sort(minIdx.begin(),minIdx.end());

	map<double,int> area_to_min_pos;
	
	int min_pos = Y.data.size()-1;
	int max_pos = -1;
	for (int i=maxIdx.size()-1;i>=0;i--)
	{
		if (min_pos < maxIdx.at(i)) continue;
		for (int j=minIdx.size()-1;j>=0;j--)
		{
			if (minIdx.at(j)<maxIdx.at(i))
			{
				min_pos=  minIdx.at(j);
				max_pos = maxIdx.at(i);
				double area = Y.sum(min_pos,max_pos).data.at(0);
				area_to_min_pos.insert(pair<double,int> (area,min_pos));
				break;
			}
		}
	}
	
	if (area_to_min_pos.size()==0)
		return 0;
	
	min_pos = area_to_min_pos.rbegin()->second; // max area

	return min_pos;
}

quantity_t measurements_::sims_t::calc_t::implant_c::minimum_starting_position()
{
	if (M.crater.X()==nullptr)
	{
		logger::error("measurements_::sims_t::calc_t::implant_c::minimum_starting_position()","M.crater.X()==nullptr","neither sputter_time nor sputter_depth set","return empty");
		return {};
	}
	if (cluster.intensity.is_set())
		return {*M.crater.X(),M.crater.X()->data.at(minimum_starting_position(cluster.intensity))};
	else if (cluster.concentration.is_set())
		return {*M.crater.X(),M.crater.X()->data.at(minimum_starting_position(cluster.concentration))};
	logger::error("measurements_::sims_t::calc_t::implant_c::minimum_starting_position()","neither intensity nor concentration set","","returning 0");
	return {};
}

RSF_t measurements_::sims_t::calc_t::implant_c::RSF()
{
	
	if (!cluster.SF.is_set())
	{
		logger::debug(6,"measurements_::sims_t::calc_t::implant_c::RSF()","!SF(cluster).is_set()","","return empty");
		return {};
	}
	return cluster.SF * M.matrix_clusters().intensity_sum();
}

void measurements_::sims_t::calc_t::implant_c::fit_maximum_intensity_val_and_pos(double seconds_for_fit_plot)
{
	unsigned int start_idx = minimum_starting_position(cluster.intensity);
	int max_idx = statistics::get_max_index_from_Y(cluster.intensity.remove_data_from_begin(start_idx).polyfit(6).data) + start_idx;
	int stop_idx = (max_idx-start_idx) * 2 + start_idx;
	auto intensity_poly6 = cluster.intensity.get_data_by_index(start_idx,stop_idx).polyfit(6);
	sputter_time_at_maximum_s = intensity_poly6.max_at_x(M.crater.sputter_time.get_data_by_index(start_idx,stop_idx));
	maximum_intensity_s = intensity_poly6.max();
	
	if (seconds_for_fit_plot>=0)
	{
		plot_t plot(true,true); //log, lin
		plot.Y1.range(1,1E6,true);
		plot.Y2.range(1,1E6,true);
		plot.Y1.add_curve(M.crater.sputter_time,cluster.intensity,cluster.to_string());
		plot.Y2.add_curve(M.crater.sputter_time.get_data_by_index(start_idx,stop_idx),cluster.intensity.get_data_by_index(start_idx,stop_idx).polyfit(6),cluster.to_string());
		plot.to_screen("measurements_::sims_t::calc_t::implant_c::fit_maximum_intensity_val_and_pos()",seconds_for_fit_plot);
	}
}

const sputter_time_t & measurements_::sims_t::calc_t::implant_c::sputter_time_at_maximum()
{
	if (sputter_time_at_maximum_s.is_set())
		return sputter_time_at_maximum_s;
	fit_maximum_intensity_val_and_pos();
	if (!sputter_time_at_maximum_s.is_set())
		logger::error("measurements_::sims_t::calc_t::implant_c::sputter_time_at_maximum()","could not calculate sputter_time_at_maximum","","returning empty");
	return sputter_time_at_maximum_s;
}

const intensity_t & measurements_::sims_t::calc_t::implant_c::maximum_intensity()
{
	if (maximum_intensity_s.is_set())
		return maximum_intensity_s;
	fit_maximum_intensity_val_and_pos();
	if (!maximum_intensity_s.is_set())
		logger::error("measurements_::sims_t::calc_t::implant_c::maximum_intensity()","could not calculate maximum_intensity","","returning empty");
	return maximum_intensity_s;
}

SR_t measurements_::sims_t::calc_t::implant_c::SR()
{
	//TODO something wrong here
	if (!cluster.intensity.is_set() || !M.crater.sputter_time.is_set())
	{
		logger::debug(6,"measurements_::sims_t::calc_t::implant_c::SR()","!cluster.intensity.is_set() || !M.crater.sputter_time.is_set()","","return empty");
		return {};
	}
	isotope_t I_iso = cluster.corresponding_isotope(M.sample->matrix().isotopes);
	
	auto I = M.sample->implant(I_iso);
	if (I.depth_at_concentration_maxium.is_set())
		logger::info(3,"measurements_::sims_t::calc_t::implant_c::SR()",M.sample->to_name() + " database: depth_at_concentration_maxium(" + I_iso.to_string()+")="+I.depth_at_concentration_maxium.to_string());
	else
	{
		logger::debug(6,"measurements_::sims_t::calc_t::implant_c::SR()",M.sample->to_name() +" unknown depth_at_concentration_maxium(" + I_iso.to_string()+")","","return empty");
		return {};
	}
	
	return I.depth_at_concentration_maxium / sputter_time_at_maximum();
}

SF_t measurements_::sims_t::calc_t::implant_c::SF_from_max()
{
	if (!cluster.intensity.is_set() )
	{
		logger::debug(6,"measurements_::sims_t::calc_t::implant_c::SF_from_max()","!cluster.intensity.is_set()","","return empty");
		return {};
	}
	isotope_t I_iso = cluster.corresponding_isotope(M.sample->matrix().isotopes);
	auto I = M.sample->implant(I_iso);
	if (I.concentration_maximum.is_set())
		logger::info(3,"measurements_::sims_t::calc_t::implant_c::SF_from_max()",M.sample->to_string() + " database: concentration_maximum(" + I_iso.to_string()+")="+I.concentration_maximum.to_string());
	else
	{
		logger::debug(6,"measurements_::sims_t::calc_t::implant_c::SF_from_max()",M.sample->to_string() +" NOT set: database dose(" + I_iso.to_string()+")","","return empty");
		return {};
	}
	
	return I.concentration_maximum / maximum_intensity();
}

SF_t measurements_::sims_t::calc_t::implant_c::SF_from_dose()
{
	if (!cluster.intensity.is_set() || !M.crater.sputter_depth.is_set())
	{
		logger::debug(6,"measurements_::sims_t::calc_t::implant_c::SF_from_dose()","!cluster.intensity.is_set() || !M.crater.sputter_depth.is_set()","","return empty");
		return {};
	}
	isotope_t I_iso = cluster.corresponding_isotope(M.sample->matrix().isotopes);
	auto I = M.sample->implant(I_iso);
	if (I.dose.is_set())
		logger::info(3,"measurements_::sims_t::calc_t::implant_c::SF_from_dose()",M.sample->to_string() +" database: dose(" + I_iso.to_string()+")="+I.dose.to_string());
	else
	{
		logger::debug(6,"measurements_::sims_t::calc_t::implant_c::SF_from_dose()",M.sample->to_string() +" NOT set: database dose(" + I_iso.to_string()+")","","return empty");
		return {};
	}
	
// 	quantity_t max = cluster.intensity.remove_data_from_begin(minimum_starting_position(cluster.intensity)).polyfit(17).max();
// 	if (cluster.intensity.data.back() / max.data.front() > 0.1)
	if (cluster.intensity.data.back() / maximum_intensity().data.front() > 0.1)
		logger::warning(2,"measurements_::sims_t::calc_t::implant_c::SF_from_dose()","intensity background higher than 10\% of maximum","recommanding: sputter deeper, substract background, use implant maximum");
	return SF_t(I.dose / cluster.intensity.integrate(M.crater.sputter_depth.change_unit({"cm"}),minimum_starting_position(cluster.intensity)));
}
