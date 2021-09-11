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

measurements_::sims_t::calc_t::calc_t(sims_t& measurement, bool overwrite) : M(measurement), SR(*this), SD(*this), SF(*this),RSF(*this),concentration(*this), overwrite(overwrite)
{
}

measurements_::sims_t::calc_t::implant_c& measurements_::sims_t::calc_t::implant(cluster_t& cluster, double X_resolution_factor)
{
	if (implants_s.find(&cluster)!=implants_s.end())
		return implants_s.at(&cluster);
	
	implants_s.insert(pair<cluster_t * const,implant_c> (&cluster,implant_c(M,cluster,X_resolution_factor)));
	return implants_s.at(&cluster);
}


/*************************/
/**        SD_c         **/
/*************************/

measurements_::sims_t::calc_t::SD_c::SD_c(calc_t& calc) : M(calc.M), calc(calc)
{
}

measurements_::sims_t::calc_t& measurements_::sims_t::calc_t::SD_c::from_SR(bool overwrite)
{
	M.crater.sputter_depth={};
	if (M.crater.SR.is_set() && M.crater.sputter_time.is_set())
	{
		if (overwrite || !M.crater.sputter_depth.is_set())
		{
			M.crater.sputter_depth = M.crater.SR * M.crater.sputter_time;
			logger::info(3,"measurements_::sims_t::calc_t::SD_c::from_SR()","M.crater.SR.is_set() && M.crater.sputter_time.is_set()");
		}
	}
	return calc;
}

/************************************/
/**        concentration_c         **/
/************************************/

measurements_::sims_t::calc_t::concentration_c::concentration_c(calc_t& calc) : M(calc.M), calc(calc)
{
}

measurements_::sims_t::calc_t& measurements_::sims_t::calc_t::concentration_c::from_SF(bool overwrite)
{
	for (auto& C : M.clusters)
		if (overwrite || !C.concentration.is_set())
			C.concentration = from_SF(C);
	return calc;
}

concentration_t measurements_::sims_t::calc_t::concentration_c::from_SF(const cluster_t& cluster)
{
// 	if (!calc.overwrite && cluster.concentration.is_set())
// 		return {};
	if (!cluster.SF.is_set())
		return {};
	if (!cluster.intensity.is_set())
		return {};
	
	concentration_t concentration = cluster.SF * cluster.intensity;
	return concentration;
}

/*************************/
/**        SR_c         **/
/*************************/
measurements_::sims_t::calc_t::SR_c::SR_c(calc_t& calc) : M(calc.M), calc(calc)
{
}

measurements_::sims_t::calc_t& measurements_::sims_t::calc_t::SR_c::from_crater_depths(bool overwrite)
{
	if (!calc.overwrite && M.crater.SR.is_set())
		return calc;
	M.crater.SR={};
	if (M.crater.total_sputter_depth().is_set() && M.crater.total_sputter_time(&M.clusters).is_set())
	{
		M.crater.SR = M.crater.total_sputter_depth()/M.crater.total_sputter_time();
		logger::info(3,"measurements_::sims_t::calc_t::SR_c::from_crater_depths()",M.crater.SR.to_string() + " = " + M.crater.total_sputter_depth().to_string() + " / " + M.crater.total_sputter_time().to_string());
	}
	return calc;
}

measurements_::sims_t::calc_t& measurements_::sims_t::calc_t::SR_c::from_implant_max(bool overwrite)
{
	if (!calc.overwrite && M.crater.SR.is_set())
		return calc;
	M.crater.SR={};
	unsigned int start_idx;
	for (auto& C:M.clusters)
	{
		M.crater.SR  << from_implant_max(C);
	}
	M.crater.SR = SR_t(M.crater.SR.mean());
	return calc;
}

SR_t measurements_::sims_t::calc_t::SR_c::from_implant_max(cluster_t& C) const
{
	if (!C.intensity.is_set() && !C.concentration.is_set())	return {};
	if (!M.crater.sputter_time.is_set()) return {};
	if (!C.intensity.is_set()) return {};
	
	SR_t SR = calc.implant(C).SR();
	if (SR.is_set())
		logger::debug(11,"measurements_::sims_t::calc_t::SR_c::from_implant_max()","calc.implant(C).SR()=",SR.to_string());
	return SR;
}

/*************************/
/**        SF_c         **/
/*************************/
measurements_::sims_t::calc_t::SF_c::SF_c(calc_t& calc) : M(calc.M), calc(calc)
{
}

measurements_::sims_t::calc_t& measurements_::sims_t::calc_t::SF_c::from_db_dose(bool overwrite)
{
	for (auto& C: M.clusters)
	{
		if (overwrite || !C.SF.is_set())
			C.SF = from_db_dose(C);
	}
	return calc;
}

SF_t measurements_::sims_t::calc_t::SF_c::from_db_dose(cluster_t& cluster)
{
	return calc.implant(cluster).SF_from_dose();;
}

measurements_::sims_t::calc_t& measurements_::sims_t::calc_t::SF_c::from_db_max(bool overwrite)
{
	for (auto& C: M.clusters)
	{
		if (overwrite || !C.SF.is_set())
		{
			C.SF = from_db_max(C);
		}
	}
	return calc;
}

SF_t measurements_::sims_t::calc_t::SF_c::from_db_max(cluster_t& cluster)
{
	return calc.implant(cluster).SF_from_max();;
}


/*************************/
/**       RSF_c         **/
/*************************/
measurements_::sims_t::calc_t::RSF_c::RSF_c(calc_t& calc) : M(calc.M), calc(calc)
{
}

measurements_::sims_t::calc_t & measurements_::sims_t::calc_t::RSF_c::from_SF_mean_ref(bool overwrite)
{
	for (auto& C: M.clusters)
	{
		if (overwrite || !C.RSF.is_set())
			C.RSF = from_SF_mean_ref(C);
	}
	return calc;
}

RSF_t measurements_::sims_t::calc_t::RSF_c::from_SF_mean_ref(const cluster_t& cluster)
{
	RSF_t RSF = M.matrix_clusters().intensity_sum().mean() * cluster.SF;
	return RSF;
}



/*************************/
/**      implant_c      **/
/*************************/

measurements_::sims_t::calc_t::implant_c::implant_c(sims_t& measurement, cluster_t& cluster, double X_resolution_factor) : M(measurement), cluster(cluster), X_resolution_factor(X_resolution_factor)
{
	logger::debug(11,"measurements_::sims_t::calc_t::implant_c::implant_c()","new implant object created",cluster.to_string(),M.to_string());
}

unsigned int measurements_::sims_t::calc_t::implant_c::minimum_index_position()
{
	if (cluster.intensity.is_set())
		return minimum_index_position(cluster.intensity);
	return minimum_index_position(cluster.concentration);
}


unsigned int measurements_::sims_t::calc_t::implant_c::minimum_index_position(vector<double> data)
{
	return minimum_index_position({"dummy",data,units::SI::one});
}

/*
 * lets keep it stupid simple:
 * 0)	asume the quantity.data are ordered from lowest to highest corresponding X
 * 1)	find the nearest local minimum, beginning from the end (highest X-axis value)
 */
unsigned int measurements_::sims_t::calc_t::implant_c::minimum_index_position(quantity_t Y)
{
	
	if (!Y.is_set() || Y.data.size()<5)
		return 0;
	vector<int> maxIdx, minIdx;
	Y = Y.filter_gaussian(10,1).moving_window_median(5);
// 	Y = Y.filter_gaussian(10,1).log10() +0.001;
// 	double deviation = 2*abs(Y.data.at(12)-Y.data.at(11));
	double deviation = 0.01*statistics::get_mad_from_Y(Y.data);
// 	Y = Y.moving_window_mean(10);
	fit_functions::polynom_t poly(8);
	poly.fit(Y.data);
	double chi_rel = poly.chisq() / Y.data.size();
// 	cout << "chi_rel=" << chi_rel << endl;
// 	deviation = 4.5E-8 * pow(chi_rel,5);
// 	deviation = 215 * log10(chi_rel);
	deviation = 200 * log10(chi_rel);
// 	deviation = 250;
// 	cout << "deviation=" << deviation << endl;

	if (!statistics::get_extrema_indices(maxIdx,minIdx,Y.data,deviation))
	{
		logger::error("measurements_::sims_t::calc_t::implant_c::minimum_index_position()","statistics::get_extrema_indices","false","returning 0");
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

// quantity_t measurements_::sims_t::calc_t::implant_c::sputter_depth_from_minimum()
// {
// 	if (!M.crater.sputter_depth.is_set())
// 		return {};
// }
// 
// quantity_t measurements_::sims_t::calc_t::implant_c::sputter_time_from_minimum()
// {
// 	if (!M.crater.sputter_time.is_set())
// 		return {};
// }

sputter_depth_t measurements_::sims_t::calc_t::implant_c::minimum_sputter_depth_position() const
{
	if (!M.crater.sputter_depth.is_set())
		return {};
	if (cluster.intensity.is_set())
		return quantity_t{M.crater.sputter_depth, M.crater.sputter_depth.data.at(minimum_index_position(cluster.intensity))};
	else if (cluster.concentration.is_set())
		return quantity_t{M.crater.sputter_depth, M.crater.sputter_depth.data.at(minimum_index_position(cluster.concentration))};
	logger::error("measurements_::sims_t::calc_t::implant_c::minimum_sputter_depth_position()","neither intensity nor concentration set","","returning 0");
	return {};
}

sputter_time_t measurements_::sims_t::calc_t::implant_c::minimum_sputter_time_position() const
{
	if (!M.crater.sputter_time.is_set())
		return {};
	if (cluster.intensity.is_set())
		return quantity_t{M.crater.sputter_time, M.crater.sputter_time.data.at(minimum_index_position(cluster.intensity))};
	else if (cluster.concentration.is_set())
		return quantity_t{M.crater.sputter_time, M.crater.sputter_time.data.at(minimum_index_position(cluster.concentration))};
	logger::error("measurements_::sims_t::calc_t::implant_c::minimum_sputter_depth_position()","neither intensity nor concentration set","","returning 0");
	return {};
}

quantity_t measurements_::sims_t::calc_t::implant_c::minimum_starting_position() const
{
	if (M.crater.X()==nullptr)
	{
		logger::error("measurements_::sims_t::calc_t::implant_c::minimum_starting_position()","M.crater.X()==nullptr","neither sputter_time nor sputter_depth set","return empty");
		return {};
	}

	if (cluster.intensity.is_set())
		return {*M.crater.X(),M.crater.X()->data.at(minimum_index_position(cluster.intensity))};
	else if (cluster.concentration.is_set())
		return {*M.crater.X(),M.crater.X()->data.at(minimum_index_position(cluster.concentration))};
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
	// current X resoluttion
	sputter_time_t res_old = M.crater.sputter_time.diff().mean().absolute();
	sputter_time_t res_new = res_old*X_resolution_factor;
	intensity_t Y;
	unsigned int start_idx;
	crater_t crater;
	if (res_new.is_set())
	{
		crater = M.crater.change_resolution(res_new);
		Y = cluster.intensity.interp(M.crater.sputter_time,crater.sputter_time);
		logger::debug(11,"measurements_::sims_t::calc_t::implant_c::fit_maximum_intensity_val_and_pos()", "res_new:" + res_new.to_string(), "res_old:"+ res_old.to_string());
	}
	else
	{
		Y = cluster.intensity;
		crater = M.crater;
		logger::warning(3,"measurements_::sims_t::calc_t::implant_c::fit_maximum_intensity_val_and_pos()","!res_new.is_set()","could not calculate new resolution","using old");
	}
	
// 	logger::warning(3,"measurements_::sims_t::calc_t::implant_c::fit_maximum_intensity_val_and_pos()","Y:" + Y.to_string(), "cluster.intensity:" + cluster.intensity.to_string());
	
	start_idx = minimum_index_position(Y);
	int max_idx = statistics::get_max_index_from_Y(Y.remove_data_from_begin(start_idx).polyfit(17).data) + start_idx;
	int delta = (max_idx - start_idx)*8/10;
	int stop_idx = max_idx + delta;
	start_idx = max_idx - delta;
	auto intensity_poly6 = Y.get_data_by_index(start_idx,stop_idx).polyfit(6);
	maximum_pos_index_s = statistics::get_max_index_from_Y(intensity_poly6.data) + start_idx;
	
	if (maximum_pos_index_s > intensity_poly6.data.size()-2) // maximum at the end of profile -> no clear maximum
	{
		sputter_time_at_maximum_s = {};
		maximum_intensity_s = {};
	}
	else
	{
		sputter_time_at_maximum_s = intensity_poly6.max_at_x(crater.sputter_time.get_data_by_index(start_idx,stop_idx));
		maximum_intensity_s = intensity_poly6.max();
	}
	
	logger::debug(11,"measurements_::sims_t::calc_t::implant_c::fit_maximum_intensity_val_and_pos()",cluster.to_string(),Y.to_string(), " minimum_index_position()=" + tools::to_string(start_idx)+ "\tmax_idx=" + tools::to_string(max_idx) + "\tstop_idx=" + tools::to_string(stop_idx));
	logger::debug(7,"measurements_::sims_t::calc_t::implant_c::fit_maximum_intensity_val_and_pos()","maximum_pos_index_s="+tools::to_string(maximum_pos_index_s),"max: " + sputter_time_at_maximum_s.to_string() +" " + maximum_intensity_s.to_string());
	
	if (seconds_for_fit_plot>=0)
	{
		plot_t plot(true,true); //log, lin
		plot.Y1.range(1,1E6,true);
		plot.Y2.range(1,1E6,true);
		plot.Y3.range(1,1E6,true);
		plot.Y1.add_curve(M.crater.sputter_time,cluster.intensity,cluster.to_string());
		plot.Y2.add_curve(crater.sputter_time,intensity_poly6,cluster.to_string());
// 		sputter_time_t SD_res = M.crater.sputter_time.resolution()*5;
// 		auto DM = M.change_resolution(SD_res);
// 		intensity_t YY;
// 		if (M.cluster(cluster)!=nullptr)
// 			logger::debug(7,"measurements_::sims_t::calc_t::implant_c::fit_maximum_intensity_val_and_pos()",cluster.to_string(),M.crater.sputter_time.resolution().to_string(),cluster.intensity.max_at_x(M.crater.sputter_time).to_string());
// 		if (DM.cluster(cluster)!=nullptr)
// 		{
// 			YY = DM.cluster(cluster)->intensity;
// 			logger::debug(7,"measurements_::sims_t::calc_t::implant_c::fit_maximum_intensity_val_and_pos()",cluster.to_string(),DM.crater.sputter_time.resolution().to_string(),YY.max_at_x(DM.crater.sputter_time).to_string());
// 		}
// 		else
// 		{
// 			logger::debug(7,"measurements_::sims_t::calc_t::implant_c::fit_maximum_intensity_val_and_pos()",DM.to_string());
// 			for (auto& CC: DM.clusters)
// 				logger::debug(7,"measurements_::sims_t::calc_t::implant_c::fit_maximum_intensity_val_and_pos()",CC.to_string());
// 		}
// 		plot.Y3.add_curve(DM.crater.sputter_time,YY.polyfit(17),cluster.to_string());
// 		plot.Y2.add_curve(crater.sputter_time.get_data_by_index(start_idx,stop_idx),Y.get_data_by_index(start_idx,stop_idx).polyfit(6),cluster.to_string());
		plot.to_screen("measurements_::sims_t::calc_t::implant_c::fit_maximum_intensity_val_and_pos()",seconds_for_fit_plot);
	}
}

int measurements_::sims_t::calc_t::implant_c::maximum_pos_index()
{
	if (maximum_pos_index_s<0)
		fit_maximum_intensity_val_and_pos();
	return maximum_pos_index_s;
}

intensity_t measurements_::sims_t::calc_t::implant_c::background_intensity()
{
	plot_t plot(true,true); //log, lin
	plot.Y1.range(1,1E6,true);


	plot.Y3.range(1E-6,1,true);
	plot.Y1.add_curve(M.crater.sputter_time,cluster.intensity,cluster.to_string());

	quantity_t Q = ( cluster.intensity/maximum_intensity() ).moving_window_mean().moving_window_iqr();

	plot.Y3.add_curve(M.crater.sputter_time,Q,cluster.to_string());

	int grade = 17;

	Q = cluster.intensity.absolute().moving_window_mean();
	plot.Y2.add_curve(M.crater.sputter_time,Q,cluster.to_string());

	plot.to_screen("measurements_::sims_t::calc_t::implant_c::fit_maximum_intensity_val_and_pos()",0);
	return {};
}

dose_t measurements_::sims_t::calc_t::implant_c::dose() const
{
	if (!M.crater.sputter_depth.is_set() || !cluster.concentration.is_set()) 
		return {};
	
	return cluster.concentration.integrate(M.crater.sputter_depth.change_unit({"cm"}),minimum_sputter_depth_position());
}

const sputter_time_t&  measurements_::sims_t::calc_t::implant_c::sputter_time_at_maximum()
{
	if (!sputter_time_at_maximum_s.is_set())
		fit_maximum_intensity_val_and_pos();

	return sputter_time_at_maximum_s;

}

const intensity_t& measurements_::sims_t::calc_t::implant_c::maximum_intensity()
{
	if (!maximum_intensity_s.is_set())
		fit_maximum_intensity_val_and_pos();

	return maximum_intensity_s;

}

SR_t measurements_::sims_t::calc_t::implant_c::SR()
{
	if (M.matrix_clusters().is_cluster_in_matrix(cluster))
	{
		logger::debug(6,"measurements_::sims_t::calc_t::implant_c::SR()","matrix cluster true",cluster.to_string(),"return empty");
		return {};
	}
	if (!cluster.intensity.is_set() || !M.crater.sputter_time.is_set())
	{
		logger::debug(6,"measurements_::sims_t::calc_t::implant_c::SR()","!cluster.intensity.is_set() || !M.crater.sputter_time.is_set()","","return empty");
		return {};
	}
	isotope_t I_iso = cluster.corresponding_isotope(M.sample->matrix().isotopes());
	
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
	if (M.matrix_clusters().is_cluster_in_matrix(cluster))
	{
		logger::debug(6,"measurements_::sims_t::calc_t::implant_c::SF_from_max()","matrix cluster true",cluster.to_string(),"return empty");
		return {};
	}
	if (!cluster.intensity.is_set() )
	{
		logger::debug(6,"measurements_::sims_t::calc_t::implant_c::SF_from_max()","!cluster.intensity.is_set()","","return empty");
		return {};
	}
	isotope_t I_iso = cluster.corresponding_isotope(M.sample->matrix().isotopes());
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
	if (M.matrix_clusters().is_cluster_in_matrix(cluster))
	{
		logger::debug(6,"measurements_::sims_t::calc_t::implant_c::SF_from_dose()","matrix cluster true",cluster.to_string(),"return empty");
		return {};
	}
	if (!cluster.intensity.is_set() || !M.crater.sputter_depth.is_set())
	{
		logger::debug(6,"measurements_::sims_t::calc_t::implant_c::SF_from_dose()","!cluster.intensity.is_set() || !M.crater.sputter_depth.is_set()","","return empty");
		return {};
	}
	isotope_t I_iso = cluster.corresponding_isotope(M.sample->matrix().isotopes());
	auto I = M.sample->implant(I_iso);
	if (I.dose.is_set())
		logger::info(3,"measurements_::sims_t::calc_t::implant_c::SF_from_dose()",M.sample->to_string() +" database: dose(" + I_iso.to_string()+")="+I.dose.to_string());
	else
	{
		logger::debug(6,"measurements_::sims_t::calc_t::implant_c::SF_from_dose()",M.sample->to_string() +" NOT set: database dose(" + I_iso.to_string()+")","","return empty");
		return {};
	}
	

	if (!maximum_intensity().is_set())
	{
		logger::error("measurements_::sims_t::calc_t::implant_c::SF_from_dose()","!maximum_intensity().is_set()","returning empty");
		return {};
	}
	if (cluster.intensity.data.back() / maximum_intensity().data.front() > 0.1)
		logger::warning(2,"measurements_::sims_t::calc_t::implant_c::SF_from_dose()","intensity background higher than 10\% of maximum","recommanding: sputter deeper, substract background, use implant maximum");
	auto min_pos = minimum_index_position(cluster.intensity);
	logger::debug(11,"measurements_::sims_t::calc_t::implant_c::SF_from_dose()","min_pos="+tools::to_string(min_pos),"cluster.intensity="+cluster.intensity.to_string());
	auto area = cluster.intensity.integrate(M.crater.sputter_depth.change_unit(units::prefixes::centi*units::SI::meter),min_pos);
	SF_t SF = (I.dose / area);
	logger::debug(11,"measurements_::sims_t::calc_t::implant_c::SF_from_dose()","SF="+SF.to_string(),"area="+area.to_string());
	return SF;
}




/*************************/
/**      matrix_c       **/
/*************************/

measurements_::sims_t::calc_t::matrix_c::matrix_c(calc_t& calc) : calc(calc), M(calc.M)
{
}

measurements_::sims_t::calc_t & measurements_::sims_t::calc_t::matrix_c::mean_const_from_db(bool overwrite)
{
	if (!M.sample->matrix().is_set()) return calc;
	if (!M.matrix_clusters().intensity_sum().is_set()) return calc;
		
	return calc;
}

