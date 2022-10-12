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

measurements_::sims_t::calc_t::calc_t(sims_t& measurement, bool overwrite) : 
    M(measurement), SR(*this), SD(*this), SF(*this),RSF(*this),concentration(*this), overwrite(overwrite),matrix(*this) ,
    logger(global_logger,__FILE__,"measurements_::sims_t::calc_t")
{
}

measurements_::sims_t::calc_t::implant_c& measurements_::sims_t::calc_t::implant(cluster_t& cluster, double X_resolution_factor)
{
	
	if (implants_s.find(&cluster)!=implants_s.end())
		return implants_s.at(&cluster);
	
	implants_s.insert(std::pair<cluster_t * const,implant_c> (&cluster,implant_c(M,cluster,X_resolution_factor)));
	return implants_s.at(&cluster);
}


/*************************/
/**        SD_c         **/
/*************************/

measurements_::sims_t::calc_t::SD_c::SD_c(calc_t& calc) : M(calc.M), calc(calc), logger(global_logger,__FILE__,"measurements_::sims_t::calc_t::SD_c")
{
}

measurements_::sims_t::calc_t& measurements_::sims_t::calc_t::SD_c::from_SR(bool overwrite)
{
	M.crater.sputter_depth={};
	if (M.crater.SR.is_set() && M.crater.sputter_time.is_set())
	{
		if (overwrite || !M.crater.sputter_depth.is_set())
		{
			if (M.crater.SR.is_scalar())
				M.crater.sputter_depth = M.crater.SR * M.crater.sputter_time;
			else
				M.crater.sputter_depth = M.crater.SR.integrate_pbp(M.crater.sputter_time);
            logger.info(__func__,M.to_string_short()).value(M.crater.sputter_depth.to_string());
            //logger::debug(5,"measurements_::sims_t::calc_t::SD_c::from_SR()",M.crater.sputter_depth.to_string(),M.crater.sputter_time.to_string());
// 			std::cout << std::endl << M.crater.sputter_depth.to_string_detailed() << std::endl;
		}
	}
	return calc;
}

/************************************/
/**        concentration_c         **/
/************************************/

measurements_::sims_t::calc_t::concentration_c::concentration_c(calc_t& calc) : M(calc.M), calc(calc), logger(global_logger,__FILE__,"measurements_::sims_t::calc_t::concentration_c")
{
}

measurements_::sims_t::calc_t& measurements_::sims_t::calc_t::concentration_c::from_SF(bool overwrite)
{
	for (auto& C : M.clusters)
		if (overwrite || !C.concentration.is_set())
        {
            C.concentration = from_SF(C);
            logger.info(__func__,M.to_string_short()+"->"+C.to_string()).value(C.SF.to_string());
        }
	return calc;
}

quantity::concentration_t measurements_::sims_t::calc_t::concentration_c::from_SF(const cluster_t& cluster)
{
// 	if (!calc.overwrite && cluster.concentration.is_set())
// 		return {};
	if (!cluster.SF.is_set())
		return {};
	if (!cluster.intensity.is_set())
		return {};
	
	quantity::concentration_t concentration = cluster.SF * cluster.intensity;
// 	std::cout << std::endl << SF.to_string() << std::endl;
// 	std::cout << std::endl << cluster.intensity.to_string() << std::endl;
	return concentration;
}

/*************************/
/**        SR_c         **/
/*************************/
measurements_::sims_t::calc_t::SR_c::SR_c(calc_t& calc) : M(calc.M), calc(calc), logger(global_logger,__FILE__,"measurements_::sims_t::calc_t::SR_c")
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
        //logger::info(3,"measurements_::sims_t::calc_t::SR_c::from_crater_depths()",M.crater.SR.to_string() + " = " + M.crater.total_sputter_depth().to_string() + " / " + M.crater.total_sputter_time().to_string(),M.to_string_short());
        logger.info(__func__,M.to_string_short()).value(M.crater.SR.to_string(),10,M.crater.total_sputter_depth().to_string()+ "; " + M.crater.total_sputter_time().to_string() + "; " + M.crater.total_sputter_time(&M.clusters).to_string());
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
// 		std::cout << std::endl << M.crater.SR.to_string() << std::endl;
        logger.info(__func__,M.to_string_short()).value(M.crater.SR.to_string());
	}
	M.crater.SR = quantity::SR_t(M.crater.SR.mean());
	return calc;
}

quantity::SR_t measurements_::sims_t::calc_t::SR_c::from_implant_max(cluster_t& C)
{
	if (!C.intensity.is_set() && !C.concentration.is_set())	return {};
	if (!M.crater.sputter_time.is_set()) return {};
	if (!C.intensity.is_set()) return {};
// 	isotope_t I_iso = C.corresponding_isotope(M.reference_isotopes);
// 	auto I = M.sample.implant(I_iso);
// 	if (!I.depth_at_concentration_maxium.is_set())
// 	{
// 		logger::info(5,"measurements_::sims_t::calc_t::SR_c::from_implant_max("+C.to_string()+")","depth_at_concentration_maxium not std::set in DB",M.to_string_short());
// 		return {};
// 	}
	quantity::SR_t SR = calc.implant(C).SR_from_max();
	if (SR.is_set())
    {
        logger.info(__func__,C.to_string()).value(SR.to_string());
        //logger::debug(11,"measurements_::sims_t::calc_t::SR_c::from_implant_max()","calc.implant(C).SR()=",SR.to_string(),M.to_string_short());
    }
    else
        logger.debug(__func__,C.to_string()).value(SR.to_string()); // for debugging
	
// 	std::cout << std::endl << "from_implant_max SR:" << SR.to_string() << std::endl;
	
	return SR;
}

/*************************/
/**        SF_c         **/
/*************************/
measurements_::sims_t::calc_t::SF_c::SF_c(calc_t& calc) : M(calc.M), calc(calc), logger(global_logger,__FILE__,"measurements_::sims_t::calc_t::SF_c")
{
}

measurements_::sims_t::calc_t& measurements_::sims_t::calc_t::SF_c::from_db_dose(bool overwrite)
{
	for (auto& C: M.clusters)
	{
		if (overwrite || !C.SF.is_set())
        {
			C.SF = from_db_dose(C);
            logger.info(__func__,M.to_string_short()+"->"+C.to_string()).value(C.SF.to_string());
        }
	}
	return calc;
}

quantity::SF_t measurements_::sims_t::calc_t::SF_c::from_db_dose(cluster_t& cluster)
{
	isotope_t I_iso = cluster.corresponding_isotope(M.reference_isotopes);
    auto I = M.sample.implant(I_iso);
	if (!I.dose.is_set())
	{
        //logger::info(5,"measurements_::sims_t::calc_t::SF_c::from_db_dose("+cluster.to_string()+")","dose not std::set in DB",M.to_string_short());
		return {};
	}
	return calc.implant(cluster).SF_from_dose();;
}

measurements_::sims_t::calc_t& measurements_::sims_t::calc_t::SF_c::from_db_max(bool overwrite)
{
	for (auto& C: M.clusters)
	{
		if (overwrite || !C.SF.is_set())
		{
			C.SF = from_db_max(C);
            logger.info(__func__,M.to_string_short()+"->"+C.to_string()).value(C.SF.to_string());
		}
	}
	return calc;
}

quantity::SF_t measurements_::sims_t::calc_t::SF_c::from_db_max(cluster_t& cluster)
{
	isotope_t I_iso = cluster.corresponding_isotope(M.reference_isotopes);
    auto I = M.sample.implant(I_iso);
	if (!I.concentration_maximum.is_set())
	{
        //logger::info(5,"measurements_::sims_t::calc_t::SF_c::from_db_max("+cluster.to_string()+")","concentration_maximum not std::set in DB",M.to_string_short());
		return {};
	}
	return calc.implant(cluster).SF_from_max();;
}


/*************************/
/**       RSF_c         **/
/*************************/
measurements_::sims_t::calc_t::RSF_c::RSF_c(calc_t& calc) : M(calc.M), calc(calc), logger(global_logger,__FILE__,"measurements_::sims_t::calc_t::RSF_c")
{
}

measurements_::sims_t::calc_t & measurements_::sims_t::calc_t::RSF_c::from_SF_mean_ref(bool overwrite)
{
	for (auto& C: M.clusters)
	{
		if (overwrite || !C.RSF.is_set())
        {
			C.RSF = from_SF_mean_ref(C);
            logger.info(__func__,M.to_string_short()+"->"+C.to_string()).value(C.RSF.to_string());
        }
	}
	return calc;
}

cluster_t::RSF_t measurements_::sims_t::calc_t::RSF_c::from_SF_mean_ref(const cluster_t& cluster)
{
	cluster_t::RSF_t RSF;
	if (cluster.RSF.reference_clusters().size()==0)
		RSF = M.matrix_clusters().intensity_sum().mean() * cluster.SF; 
	else
		for (const auto& ref_c : cluster.RSF.reference_clusters())
		{
			quantity::intensity_t sum;
			const auto ref_c_in_M = M.cluster(ref_c);
			if (ref_c_in_M==nullptr || !ref_c_in_M->intensity.is_set()) // reference_cluster does not exist in this measurement, asume its intensity is 0
			{
                //logger::warning(3,"measurements_::sims_t::calc_t::RSF_c::from_SF_mean_ref()",cluster.to_string()+"-RSF reference cluster("+ ref_c.to_string() +") not found in M: " + M.to_string_short() ,"asuming intensity is 0",M.to_string_short());
				continue;
			}
			sum += ref_c_in_M->intensity;
		}
	return RSF;
}



/*************************/
/**      implant_c      **/
/*************************/

// measurements_::sims_t::calc_t::implant_c::implant_c(sims_t& measurement, cluster_t& cluster, double X_resolution_factor) :
// 	M(measurement), cluster(cluster), X_resolution_factor(X_resolution_factor), implant_parameters(M.sample.implant(cluster.corresponding_isotope(M.reference_isotopes)))
// {
// 	// it is not relevant yet, whether the sample has implanted parameters: 
// 	// maybe the user needs the implant values for other reasons or the implant parameters were not introduced in the DB yet
// 	
// 	// OLD
// 	fit_maximum_intensity_val_and_pos();
// 	logger::debug(11,"measurements_::sims_t::calc_t::implant_c::implant_c()","new implant object created",cluster.to_string(),M.to_string_short());
// }

// unsigned int measurements_::sims_t::calc_t::implant_c::minimum_index_position()
// {
// 	if (cluster.intensity.is_set())
// 		return minimum_index_position(cluster.intensity);
// 	return minimum_index_position(cluster.concentration);
// }


// unsigned int measurements_::sims_t::calc_t::implant_c::minimum_index_position(std::vector<double> data)
// {
// 	return minimum_index_position({"dummy",data,units::SI::one});
// }
// 
// /*
//  * lets keep it stupid simple:
//  * 0)	asume the quantity.data() are ordered from lowest to highest corresponding X
//  * 1)	find the nearest local minimum, beginning from the end (highest X-axis value)
//  */
// unsigned int measurements_::sims_t::calc_t::implant_c::minimum_index_position(quantity::quantity_t Y)
// {
// 	
// 	if (!Y.is_set() || Y.data().size()<5)
// 		return 0;
// 	std::vector<int> maxIdx, minIdx;
// 	int window_size= 0.01 * Y.data().size(); // 1%
// 	Y = Y.filter_gaussian(window_size,1).moving_window_median(window_size);
// 	double deviation = 0.01*statistics::get_mad_from_Y(Y.data());
// // 	fit_functions::polynom_t poly(8,Y.data());
// // 	double chi_rel = poly.chisq() / Y.data().size();
// // 	double deviation = 20 * log10(chi_rel);
// // 	deviation = 200 * log10(chi_rel);
// 
// 	if (!statistics::get_extrema_indices(maxIdx,minIdx,Y.data(),deviation))
// 	{
// 		logger::error("measurements_::sims_t::calc_t::implant_c::minimum_index_position()","statistics::get_extrema_indices","false","returning 0");
// 		return 0;
// 	}
// 	sort(maxIdx.begin(),maxIdx.end());
// 	sort(minIdx.begin(),minIdx.end());
// 
// 	std::map<double,int> area_to_min_pos;
// 	
// 	int min_pos = Y.data().size()-1;
// 	int max_pos = -1;
// 	for (int i=maxIdx.size()-1;i>=0;i--)
// 	{
// 		if (min_pos < maxIdx.at(i)) 
// 			continue;
// 		for (int j=minIdx.size()-1;j>=0;j--)
// 		{
// 			if (minIdx.at(j)<maxIdx.at(i))
// 			{
// 				min_pos=  minIdx.at(j);
// 				max_pos = maxIdx.at(i);
// 				double area = Y.sum(min_pos,max_pos).data().at(0);
// 				area_to_min_pos.insert(std::pair<double,int> (area,min_pos));
// 				break;
// 			}
// 		}
// 	}
// 	
// 	if (area_to_min_pos.size()==0)
// 		return 0;
// 	
// 	min_pos = area_to_min_pos.rbegin()->second; // max area
// 	logger::info(3,"measurements_::sims_t::calc_t::implant_c::minimum_index_position()","min_pos: " + ::to_string(min_pos));
// 	return min_pos;
// }

// quantity::depth_t measurements_::sims_t::calc_t::implant_c::minimum_sputter_depth_position() const
// {
// 	if (!M.crater.sputter_depth.is_set())
// 		return {};
// 	if (cluster.intensity.is_set())
// 		return quantity::quantity_t{M.crater.sputter_depth, M.crater.sputter_depth.data().at(minimum_index_position(cluster.intensity))};
// 	else if (cluster.concentration.is_set())
// 		return quantity::quantity_t{M.crater.sputter_depth, M.crater.sputter_depth.data().at(minimum_index_position(cluster.concentration))};
// 	logger::error("measurements_::sims_t::calc_t::implant_c::minimum_sputter_depth_position()","neither intensity nor concentration set",M.to_string_short(),"returning 0");
// 	return {};
// }
// 
// quantity::sputter_time_t measurements_::sims_t::calc_t::implant_c::minimum_sputter_time_position() const
// {
// 	if (!M.crater.sputter_time.is_set())
// 		return {};
// 	if (cluster.intensity.is_set())
// 		return quantity::quantity_t{M.crater.sputter_time, M.crater.sputter_time.data().at(minimum_index_position(cluster.intensity))};
// 	else if (cluster.concentration.is_set())
// 		return quantity::quantity_t{M.crater.sputter_time, M.crater.sputter_time.data().at(minimum_index_position(cluster.concentration))};
// 	logger::error("measurements_::sims_t::calc_t::implant_c::minimum_sputter_depth_position()","neither intensity nor concentration set",M.to_string_short(),"returning 0");
// 	return {};
// }
// 
// quantity::quantity_t measurements_::sims_t::calc_t::implant_c::minimum_starting_position() const
// {
// 	if (M.crater.X()==nullptr)
// 	{
// 		logger::error("measurements_::sims_t::calc_t::implant_c::minimum_starting_position()","M.crater.X()==nullptr","neither sputter_time nor sputter_depth set: "+ M.to_string_short(),"return empty");
// 		return {};
// 	}
// 
// 	if (cluster.intensity.is_set())
// 		return {*M.crater.X(),M.crater.X()->data().at(minimum_index_position(cluster.intensity))};
// 	else if (cluster.concentration.is_set())
// 		return {*M.crater.X(),M.crater.X()->data().at(minimum_index_position(cluster.concentration))};
// 	logger::error("measurements_::sims_t::calc_t::implant_c::minimum_starting_position()","neither intensity nor concentration set: " + M.to_string_short(),"","returning 0");
// 	return {};
// }

// quantity::SF_t measurements_::sims_t::calc_t::implant_c::RSF()
// {
// 	if (!cluster.SF.is_set())
// 	{
// 		logger::debug(6,"measurements_::sims_t::calc_t::implant_c::RSF()","!SF(cluster).is_set()",M.to_string_short(),"return empty");
// 		return {};
// 	}
// 	return cluster.SF * M.matrix_clusters().intensity_sum();
// }

// const quantity::map_t& measurements_::sims_t::calc_t::implant_c::fitted_curve() const
// {
// 	return fitted_curve_p;
// }
// 
// double measurements_::sims_t::calc_t::implant_c::X_resolution_factor_() const
// {
// 	return X_resolution_factor;
// }
// 
// ///TODO HERE
// void measurements_::sims_t::calc_t::implant_c::fit_maximum_intensity_val_and_pos(double seconds_for_fit_plot)
// {
// 	// current X resoluttion
// 	quantity::sputter_time_t res_old = M.crater.sputter_time.diff().mean().absolute();
// 	quantity::sputter_time_t res_new = res_old*X_resolution_factor;
// 	quantity::intensity_t Y;
// 	crater_t crater;
// 	if (res_new.is_set())
// 	{
// 		crater = M.crater.change_resolution(res_new);
// 		Y = cluster.intensity.interp(M.crater.sputter_time,crater.sputter_time);
// 		logger::debug(11,"measurements_::sims_t::calc_t::implant_c::fit_maximum_intensity_val_and_pos()", "res_new:" + res_new.to_string(), "res_old:"+ res_old.to_string());
// 	}
// 	else
// 	{
// 		Y = cluster.intensity;
// 		crater = M.crater;
// 		logger::warning(3,"measurements_::sims_t::calc_t::implant_c::fit_maximum_intensity_val_and_pos()","!res_new.is_set()","could not calculate new resolution","using old");
// 	}
// 	
// 	
// // 	unsigned int start_idx = minimum_index_position(Y);
// 	unsigned int minimum_idx = minimum_index_position(Y);
// 
// 	int max_idx = statistics::get_max_index_from_Y(Y.remove_data_from_begin(minimum_idx).polyfit(17).data()) + minimum_idx;
// // 	std::cout << std::endl << "max_idx: " << max_idx << std::endl;
// 	int delta = (max_idx - minimum_idx)*8/10;
// 	int stop_idx = max_idx + delta;
// 	unsigned int start_idx = max_idx - delta;
// // 	auto reduced_q = Y.get_data_by_index(start_idx,stop_idx);
// 	auto reduced_q = Y.get_data_by_index(start_idx,stop_idx);
// 	auto reduced_q_x = crater.sputter_time.get_data_by_index(start_idx,stop_idx);
// 	auto intensity_poly6 = reduced_q.polyfit(6);
// 	maximum_pos_index_s = statistics::get_max_index_from_Y(intensity_poly6.data()) + start_idx;
// 	std::stringstream ss;
// 	ss << "; Y.data().size()=" << Y.data().size();
// 	ss << "; minimum_idx=" << minimum_idx;
// 	ss << "; crater.sputter_time.at(minimum_idx)=" << crater.sputter_time.at(minimum_idx).to_string();
// 	ss << "; start_idx=" << start_idx;
// 	ss << "; stop_idx=" << stop_idx;
// 	ss << "; delta=" << delta;
// 	ss << "; reduced_q.data().size()=" << reduced_q.data().size();
// 	ss << "; reduced_q_x.front()=" << reduced_q_x.front().to_string();
// 	ss << "; maximum_pos_index_s=" << maximum_pos_index_s;
// 	ss << "; intensity_poly6.data().size()=" << intensity_poly6.data().size();
// 	if (maximum_pos_index_s-start_idx > intensity_poly6.data().size()-2) // maximum at the end of profile -> no clear maximum
// 	{
// 		logger::error("measurements_::sims_t::calc_t::implant_c::fit_maximum_intensity_val_and_pos()","maximum at the end of profile? " + ss.str());
// 		sputter_time_at_maximum_s = {};
// 		maximum_intensity_s = {};
// 	}
// 	else
// 	{
// 		sputter_time_at_maximum_s = intensity_poly6.x_at_max(crater.sputter_time.get_data_by_index(start_idx,stop_idx));
// 		maximum_intensity_s = intensity_poly6.max();
// 	}
// 	
// 	logger::info(3,"measurements_::sims_t::calc_t::implant_c::fit_maximum_intensity_val_and_pos()",ss.str());
// // 	std::cout << std::endl << "start_idx: " << start_idx << std::endl;
// // 	std::cout << "stop_idx: " << stop_idx << std::endl;
// 	
// 	logger::debug(11,"measurements_::sims_t::calc_t::implant_c::fit_maximum_intensity_val_and_pos()",cluster.to_string(),Y.to_string(), M.to_string_short()+ " minimum_index_position()=" + tools::to_string(start_idx)+ "\tmax_idx=" + tools::to_string(max_idx) + "\tstop_idx=" + tools::to_string(stop_idx));
// 	logger::debug(7,"measurements_::sims_t::calc_t::implant_c::fit_maximum_intensity_val_and_pos()",M.to_string_short()+" maximum_pos_index_s="+tools::to_string(maximum_pos_index_s),"sputter_time_at_maximum: " + sputter_time_at_maximum_s.to_string() +" ;maximum_intensity: " + maximum_intensity_s.to_string());
// 	
// 	fitted_curve_p = quantity::map_t(crater.sputter_time.get_data_by_index(start_idx,stop_idx),intensity_poly6);
// // 	seconds_for_fit_plot=0;
// 	if (seconds_for_fit_plot>=0)
// 	{
// 		plot_t plot(true,true); //log, lin
// 		plot.Y1.range(1,1E6,true);
// 		plot.Y2.range(1,1E6,true);
// 		plot.Y3.range(1,1E6,true);
// 		plot.Y1.add_curve(M.crater.sputter_time,cluster.intensity,cluster.to_string());
// // 		plot.Y2.add_curve(crater.sputter_time.get_data_by_index(start_idx,stop_idx),intensity_poly6,cluster.to_string());
// 		plot.Y2.add_curve(fitted_curve_p,cluster.to_string());
// 		plot.to_screen(M.to_string() + "fit_maximum_intensity_val_and_pos()",seconds_for_fit_plot);
// 	}
// }

// int measurements_::sims_t::calc_t::implant_c::maximum_pos_index() const
// {
// // 	if (maximum_pos_index_s<0)
// // 		fit_maximum_intensity_val_and_pos();
// 	return maximum_pos_index_s;
// }
// 
// quantity::intensity_t measurements_::sims_t::calc_t::implant_c::background_intensity()
// {
// 	plot_t plot(true,true); //log, lin
// 	plot.Y1.range(1,1E6,true);
// 
// 
// 	plot.Y3.range(1E-6,1,true);
// 	plot.Y1.add_curve(M.crater.sputter_time,cluster.intensity,cluster.to_string());
// 
// 	quantity::quantity_t Q = ( cluster.intensity/maximum_intensity() ).moving_window_mean().moving_window_iqr();
// 
// 	plot.Y3.add_curve(M.crater.sputter_time,Q,cluster.to_string());
// 
// 	int grade = 17;
// 
// 	Q = cluster.intensity.absolute().moving_window_mean();
// 	plot.Y2.add_curve(M.crater.sputter_time,Q,cluster.to_string());
// 
// 	plot.to_screen("measurements_::sims_t::calc_t::implant_c::fit_maximum_intensity_val_and_pos() "+ M.to_string_short(),0);
// 	return {};
// }

// quantity::dose_t measurements_::sims_t::calc_t::implant_c::dose() const
// {
// 	if (!M.crater.sputter_depth.is_set() || !cluster.concentration.is_set()) 
// 		return {};
// 	
// 	return cluster.concentration.integrate(M.crater.sputter_depth.change_unit("cm"),minimum_sputter_depth_position());
// }
// 
// const quantity::sputter_time_t&  measurements_::sims_t::calc_t::implant_c::sputter_time_at_maximum() const
// {
// // 	if (!sputter_time_at_maximum_s.is_set())
// // 		fit_maximum_intensity_val_and_pos();
// 
// 	return sputter_time_at_maximum_s;
// 
// }
// 
// const quantity::intensity_t& measurements_::sims_t::calc_t::implant_c::maximum_intensity() const
// {
// // 	if (!maximum_intensity_s.is_set())
// // 		fit_maximum_intensity_val_and_pos();
// 
// 	return maximum_intensity_s;
// 
// }

// quantity::SR_t measurements_::sims_t::calc_t::implant_c::SR() const
// {
// 	if (M.matrix_clusters().is_cluster_in_matrix(cluster))
// 	{
// 		logger::debug(6,"measurements_::sims_t::calc_t::implant_c::SR()","matrix cluster true",M.to_string_short() + ": " + cluster.to_string(),"return empty");
// 		return {};
// 	}
// 	if (!cluster.intensity.is_set() || !M.crater.sputter_time.is_set())
// 	{
// 		logger::debug(6,"measurements_::sims_t::calc_t::implant_c::SR()",M.to_string_short() + ": !cluster.intensity.is_set() || !M.crater.sputter_time.is_set()","","return empty");
// 		return {};
// 	}
// 	isotope_t I_iso = cluster.corresponding_isotope(M.reference_isotopes);
// 
// 	auto I = M.sample.implant(I_iso);
// 	if (I.depth_at_concentration_maxium.is_set())
// 		logger::info(3,"measurements_::sims_t::calc_t::implant_c::SR()",M.sample.to_name() + " database: depth_at_concentration_maxium(" + I_iso.to_string()+")="+I.depth_at_concentration_maxium.to_string());
// 	else
// 	{
// 		logger::debug(6,"measurements_::sims_t::calc_t::implant_c::SR()",M.sample.to_name() +" unknown depth_at_concentration_maxium(" + I_iso.to_string()+")","","return empty");
// 		return {};
// 	}
// // 	std::cout << std::endl << sputter_time_at_maximum().to_string() << std::endl;
// // 	std::cout << I.depth_at_concentration_maxium.to_string() << std::endl;
// 	return I.depth_at_concentration_maxium / sputter_time_at_maximum();
// }

// quantity::SF_t measurements_::sims_t::calc_t::implant_c::SF_from_max()
// {
// 	if (M.matrix_clusters().is_cluster_in_matrix(cluster))
// 	{
// 		logger::debug(6,"measurements_::sims_t::calc_t::implant_c::SF_from_max()","matrix cluster true",cluster.to_string(),"return empty");
// 		return {};
// 	}
// 	if (!cluster.intensity.is_set() )
// 	{
// 		logger::debug(6,"measurements_::sims_t::calc_t::implant_c::SF_from_max()","!cluster.intensity.is_set()","","return empty");
// 		return {};
// 	}
// 	isotope_t I_iso = cluster.corresponding_isotope(M.reference_isotopes);
// 	auto I = M.sample.implant(I_iso);
// 	if (I.concentration_maximum.is_set())
// 		logger::info(3,"measurements_::sims_t::calc_t::implant_c::SF_from_max()",M.sample.to_string() + " database: concentration_maximum(" + I_iso.to_string()+")="+I.concentration_maximum.to_string());
// 	else
// 	{
// 		logger::debug(6,"measurements_::sims_t::calc_t::implant_c::SF_from_max()",M.sample.to_string() +" NOT set: database dose(" + I_iso.to_string()+")","","return empty");
// 		return {};
// 	}
// 	
// 	return I.concentration_maximum / maximum_intensity();
// }

// quantity::SF_t measurements_::sims_t::calc_t::implant_c::SF_from_dose()
// {
// 	if (M.matrix_clusters().is_cluster_in_matrix(cluster))
// 	{
// 		logger::debug(6,"measurements_::sims_t::calc_t::implant_c::SF_from_dose()","matrix cluster true",cluster.to_string(),"return empty");
// 		return {};
// 	}
// 	if (!cluster.intensity.is_set() || !M.crater.sputter_depth.is_set())
// 	{
// 		logger::debug(6,"measurements_::sims_t::calc_t::implant_c::SF_from_dose()","!cluster.intensity.is_set() || !M.crater.sputter_depth.is_set()","","return empty");
// 		return {};
// 	}
// 	isotope_t I_iso = cluster.corresponding_isotope(M.reference_isotopes);
// 	auto I = M.sample.implant(I_iso);
// 	if (I.dose.is_set())
// 		logger::info(3,"measurements_::sims_t::calc_t::implant_c::SF_from_dose()",M.sample.to_string() +" database: dose(" + I_iso.to_string()+")="+I.dose.to_string());
// 	else
// 	{
// 		logger::debug(6,"measurements_::sims_t::calc_t::implant_c::SF_from_dose()",M.sample.to_string() +" NOT set: database dose(" + I_iso.to_string()+")","","return empty");
// 		return {};
// 	}
// 	
// 	if (!maximum_intensity().is_set())
// 	{
// 		logger::error("measurements_::sims_t::calc_t::implant_c::SF_from_dose()","!maximum_intensity().is_set()","returning empty");
// 		return {};
// 	}
// 	if (cluster.intensity.data().back() / maximum_intensity().data().front() > 0.1)
// 	{
// 		logger::error("measurements_::sims_t::calc_t::implant_c::SF_from_dose()","intensity background higher than 10\% of maximum","recommending: sputter deeper, substract background, use implant maximum","returning empty");
// 		return {};
// 	}
// 	auto min_pos = minimum_index_position(cluster.intensity);
// 	logger::debug(11,"measurements_::sims_t::calc_t::implant_c::SF_from_dose()","min_pos="+tools::to_string(min_pos),"cluster.intensity="+cluster.intensity.to_string());
// 	auto area = cluster.intensity.integrate(M.crater.sputter_depth,min_pos); // change_unit(units::prefixes::centi*units::SI::meter)
// 	quantity::SF_t SF = (I.dose / area);
// 	//change_unit(units::derived::counts/units::SI::second*units::SI::meter*units::prefixes::nano
// 	logger::debug(11,"measurements_::sims_t::calc_t::implant_c::SF_from_dose()","SF="+SF.to_string(),"area="+area.to_string());
// 	return SF;
// }



/*************************/
/**      matrix_c       **/
/*************************/

measurements_::sims_t::calc_t::matrix_c::matrix_c(calc_t& calc) : calc(calc), M(calc.M), logger(global_logger,__FILE__,"measurements_::sims_t::calc_t::matrix_c")
{
}

measurements_::sims_t::calc_t & measurements_::sims_t::calc_t::matrix_c::median_const_from_db(bool overwrite)
{
    const auto& sample_mat = M.sample.matrix();
	auto M_copy = M; // work with a copy and overwrite at the end
	const auto& mat_clusters = M_copy.matrix_clusters();
	
	if (!sample_mat.is_set()) 
		return calc;
	if (!mat_clusters.intensity_sum().is_set()) 
		return calc;
	
	for (auto& C : mat_clusters.clusters)
	{
		const auto& corresponding_iso = C->corresponding_isotope(M.reference_isotopes);
		if (!corresponding_iso.is_set())
			continue;
		const auto& iso_in_matrix = sample_mat.isotope(corresponding_iso);
		if (iso_in_matrix==nullptr)
		{
            //logger::info(5,"measurements_::sims_t::calc_t::matrix_c::mean_const_from_db","corresponding isotope from cluster " + C->to_string()+ " not found in " + sample_mat.to_string() );
			continue;
		}
		if (!iso_in_matrix->substance_amount.is_set())
		{
            //logger::error("measurements_::sims_t::calc_t::matrix_c::mean_const_from_db","isotope in sample matrix has unknown substance amount");
			return calc;
		}
		if (!C->concentration.is_set() || overwrite)
        {
			C->concentration = (quantity::concentration_t( (C->intensity / C->intensity.median() ) * iso_in_matrix->substance_amount)).change_unit("at%");
            logger.info(__func__,M.to_string_short()+"->"+C->to_string()).value(C->concentration.to_string());
        }
	}

	M = M_copy; // if everything was successfull, then overwrite original
	return calc;
}

measurements_::sims_t::calc_t & measurements_::sims_t::calc_t::matrix_c::median_const_from_reference_isotopes(bool overwrite)
{
// 	auto M_copy = M; // work with a copy and overwrite at the end
// 	auto mat_clusters = M.matrix_clusters();
	
	for (auto& ref_iso : M.reference_isotopes)
	{
		if (!ref_iso.is_set() || !ref_iso.substance_amount.is_set())
			continue;
		auto C = M.cluster(ref_iso);
		if (C==nullptr || !C->intensity.is_set())
			continue;
		if (!C->concentration.is_set() || overwrite)
        {
			C->concentration = (quantity::concentration_t( (C->intensity / C->intensity.median() ) * ref_iso.substance_amount)).change_unit("at%");
            logger.info(__func__,M.to_string_short()+"->"+C->to_string()).value(C->concentration.to_string());
        }
	}
	return calc;
}
