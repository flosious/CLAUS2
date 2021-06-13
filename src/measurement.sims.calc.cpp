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

measurements_::sims_t::calc_t::implant_t measurements_::sims_t::calc_t::implant(cluster_t& cluster)
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
			M.crater.SR << implant_t(M,C).SR();
// 			M.crater.SR << implant_t(M,C).SR().crater.SR; ///collecting skalar values of SR
	}
	///M.crater.SR is now populated with SRs from all different clusters
	M.crater.SR = M.crater.SR.mean();
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
	cluster.SF=	implant_t(M,cluster).SF_from_dose();
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
	cluster.SF=implant_t(M,cluster).SF_from_max();
	return M;
}


/*************************/
/**       RSF_c         **/
/*************************/
measurements_::sims_t::calc_t::RSF_c::RSF_c(measurements_::sims_t& measurement) : M(measurement)
{
}

/*************************/
/**      implant_t      **/
/*************************/

measurements_::sims_t::calc_t::implant_t::implant_t(sims_t& measurement, cluster_t& cluster) : M(measurement), cluster(cluster)
{
}

/*
 * lets keep it stupid simple:
 * 0)	asume the quantity.data are ordered from lowest to highest corresponding X
 * 1)	find the nearest local minimum, beginning from the end (highest X-axis value)
 */
unsigned int measurements_::sims_t::calc_t::implant_t::minimum_starting_position(quantity_t Y)
{
	if (!Y.is_set())
		return 0;
	vector<int> maxIdx, minIdx;
	Y = Y.filter_gaussian(5,4);
	if (!statistics::get_extrema_indices(maxIdx,minIdx,Y.data,(Y.mad()*1).data.at(0)))
	{
		logger::error("measurements_::sims_t::calc_t::implant_t::minimum_starting_position()","statistics::get_extrema_indices","false","returning 0");
		return 0;
	}
	sort(maxIdx.begin(),maxIdx.end());
	sort(minIdx.begin(),minIdx.end());
	unsigned int implant_max_idx = maxIdx.back();
// 	int implant_min_idx=-1;
	for (int i=minIdx.size()-1;i>=0;i--)
	{
		if (minIdx.at(i)<implant_max_idx)
			return  minIdx.at(i);
	}
// 	for (int i=minIdx.size()-1;i>=0;i--)
// 		cout << "min=" << minIdx.at(i) << endl;
// 	for (int i=maxIdx.size()-1;i>=0;i--)
// 		cout << "max=" << maxIdx.at(i) << endl;
	
	logger::error("measurements_::sims_t::calc_t::implant_t::minimum_starting_position()","could not find implant min idx","","returning 0");
	return 0;
}

quantity_t measurements_::sims_t::calc_t::implant_t::minimum_starting_position()
{
	if (M.crater.X()==nullptr)
	{
		logger::error("measurements_::sims_t::calc_t::implant_t::minimum_starting_position()","M.crater.X()==nullptr","neither sputter_time nor sputter_depth set","return empty");
		return {};
	}
	if (cluster.intensity.is_set())
		return {*M.crater.X(),M.crater.X()->data.at(minimum_starting_position(cluster.intensity))};
	else if (cluster.concentration.is_set())
		return {*M.crater.X(),M.crater.X()->data.at(minimum_starting_position(cluster.concentration))};
	logger::error("measurements_::sims_t::calc_t::implant_t::minimum_starting_position()","neither intensity nor concentration set","","returning 0");
	return {};
}

// SF_t measurements_::sims_t::calc_t::implant_t::SF()
// {
// 	
// 	if (!cluster.intensity.is_set() || !M.crater.sputter_depth.is_set())
// 	{
// 		logger::debug(6,"measurements_::sims_t::calc_t::implant_t::SF()","!cluster.intensity.is_set() || !M.crater.sputter_depth.is_set()","","return empty");
// 		return {};
// 	}
// 	isotope_t I_iso = cluster.corresponding_isotope(M.matrix_cluster().isotopes());
// 	auto I = M.sample->database().implant(I_iso);
// 	if (I.dose.is_set())
// 		logger::info(3,"measurements_::sims_t::calc_t::implant_t::SF()","database: dose(" + I_iso.to_string()+")="+I.dose.to_string());
// 	else
// 	{
// 		logger::debug(6,"measurements_::sims_t::calc_t::implant_t::SF()","NOT set: database dose(" + I_iso.to_string()+")","","return empty");
// 		return {};
// 	}
// 	return SF_t(I.dose / cluster.intensity.integrate(M.crater.sputter_depth,minimum_starting_position(cluster.intensity)));
// }

RSF_t measurements_::sims_t::calc_t::implant_t::RSF()
{
	
	if (!cluster.SF.is_set())
	{
		logger::debug(6,"measurements_::sims_t::calc_t::implant_t::RSF()","!SF(cluster).is_set()","","return empty");
		return {};
	}
	return cluster.SF * M.matrix_cluster().intensity_sum();
}

SR_t measurements_::sims_t::calc_t::implant_t::SR()
{

	if (!cluster.intensity.is_set() || !M.crater.sputter_time.is_set())
	{
		logger::debug(6,"measurements_::sims_t::calc_t::implant_t::SR()","!cluster.intensity.is_set() || !M.crater.sputter_time.is_set()","","return empty");
		return {};
	}
	isotope_t I_iso = cluster.corresponding_isotope(M.matrix_cluster().isotopes());
	auto I = M.sample->database().implant(I_iso);
	if (I.depth_at_concentration_maxium.is_set())
		logger::info(3,"measurements_::sims_t::calc_t::implant_t::SR()",M.sample->to_string() + " database: depth_at_concentration_maxium(" + I_iso.to_string()+")="+I.depth_at_concentration_maxium.to_string());
	else
	{
		logger::debug(6,"measurements_::sims_t::calc_t::implant_t::SR()",M.sample->to_string() +" unknown depth_at_concentration_maxium(" + I_iso.to_string()+")","","return empty");
		return {};
	}
	unsigned int start_idx = minimum_starting_position(cluster.intensity);
	quantity_t ST_max= 	cluster.intensity.remove_data_from_begin(start_idx).polyfit(17).max_at_x(M.crater.sputter_time.remove_data_from_begin(start_idx));
	plot_t plot(true,true); //log, lin
	plot.Y1.range(1,1E6,true);
	plot.Y2.range(1,1E6,true);
	plot.Y1.add_curve(M.crater.sputter_time,cluster.intensity,cluster.to_string());
	plot.Y2.add_curve(M.crater.sputter_time.remove_data_from_begin(start_idx),cluster.intensity.remove_data_from_begin(start_idx).polyfit(17),cluster.to_string());
	plot.to_screen("measurements_::sims_t::calc_t::implant_t::SR()",1);
	return I.depth_at_concentration_maxium / ST_max;
}

SF_t measurements_::sims_t::calc_t::implant_t::SF_from_max()
{
	if (!cluster.intensity.is_set() )
	{
		logger::debug(6,"measurements_::sims_t::calc_t::implant_t::SF_from_max()","!cluster.intensity.is_set()","","return empty");
		return {};
	}
	isotope_t I_iso = cluster.corresponding_isotope(M.matrix_cluster().isotopes());
	auto I = M.sample->database().implant(I_iso);
	if (I.concentration_maximum.is_set())
		logger::info(3,"measurements_::sims_t::calc_t::implant_t::SF_from_max()",M.sample->to_string() + " database: concentration_maximum(" + I_iso.to_string()+")="+I.concentration_maximum.to_string());
	else
	{
		logger::debug(6,"measurements_::sims_t::calc_t::implant_t::SF_from_max()",M.sample->to_string() +" NOT set: database dose(" + I_iso.to_string()+")","","return empty");
		return {};
	}
	return SF_t(I.dose / cluster.intensity.integrate(M.crater.sputter_depth,minimum_starting_position(cluster.intensity)));
}

SF_t measurements_::sims_t::calc_t::implant_t::SF_from_dose()
{
	if (!cluster.intensity.is_set() || !M.crater.sputter_depth.is_set())
	{
		logger::debug(6,"measurements_::sims_t::calc_t::implant_t::SF_from_dose()","!cluster.intensity.is_set() || !M.crater.sputter_depth.is_set()","","return empty");
		return {};
	}
	isotope_t I_iso = cluster.corresponding_isotope(M.matrix_cluster().isotopes());
	auto I = M.sample->database().implant(I_iso);
	if (I.dose.is_set())
		logger::info(3,"measurements_::sims_t::calc_t::implant_t::SF_from_dose()",M.sample->to_string() +" database: dose(" + I_iso.to_string()+")="+I.dose.to_string());
	else
	{
		logger::debug(6,"measurements_::sims_t::calc_t::implant_t::SF_from_dose()",M.sample->to_string() +" NOT set: database dose(" + I_iso.to_string()+")","","return empty");
		return {};
	}
	
	quantity_t max = cluster.intensity.remove_data_from_begin(minimum_starting_position(cluster.intensity)).polyfit(17).max();

	if (cluster.intensity.data.back() / max.data.front() > 0.1)
		logger::warning(2,"measurements_::sims_t::calc_t::implant_t::SF_from_dose()","intensity background higher than 10\% of maximum","recommanding: sputter deeper, substract background, use implant maximum");
	return SF_t(I.dose / cluster.intensity.integrate(M.crater.sputter_depth.change_unit({"cm"}),minimum_starting_position(cluster.intensity)));
}
