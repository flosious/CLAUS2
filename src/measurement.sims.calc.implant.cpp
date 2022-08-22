/*
	Copyright (C) 2022 Florian Bärwolf
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

/***********************************************************************************/
/*******       measurements_::sims_t::calc_t::implant_c::quantity_c        *********/
/***********************************************************************************/

measurements_::sims_t::calc_t::implant_c::quantity_c::quantity_c(const quantity::quantity_t& Y) :Y(Y), logger(global_logger,__FILE__,"measurements_::sims_t::calc_t::implant_c::quantity_c")
{
}

unsigned int measurements_::sims_t::calc_t::implant_c::quantity_c::minimum_index_position()
{
	if (minimum_index_position_p>=0)
		return minimum_index_position_p;
	
	minimum_index_position_p = 0;
	
	if (!Y.is_set() || Y.data().size()<5)
		return minimum_index_position_p;
	
	int start, stop;
	if (!Y.is_set())
		return 0;
	auto coarse_minimum = minimum_index_position_coarse();
	auto coarse_max_right = Y.get_data_by_index(coarse_minimum,Y.data().size()-1).max_idx() + coarse_minimum;
	auto coarse_max_left = Y.get_data_by_index(0,coarse_minimum).max_idx();
	
	if (coarse_max_left >= coarse_minimum)
		start = 0;
	else 
		start = coarse_max_left;
	
	stop = coarse_max_right;
	
	const auto cut_Y = Y.get_data_by_index(start, stop).log10().polyfit(7);
	if (!cut_Y.is_set())
		return coarse_minimum;
	
	auto min_pos_finer = cut_Y.min_idx()+start;
	auto min_pos_finest = Y.get_data_by_index(min_pos_finer*3/4 , min_pos_finer*5/4).polyfit(2).min_idx() + min_pos_finer*3/4;
	
    //logger::debug(5,"measurements_::sims_t::calc_t::implant_c::quantity_c::minimum_index_position()","coarse_minimum=" + ::to_string(coarse_minimum)+"; min_pos_finer="+::to_string(min_pos_finer)+"; min_pos_finest="+::to_string(min_pos_finest));
	
// 	cout << "cut_Y.min_idx()=" << cut_Y.min_idx() << endl;
// 	plot_t P(false,false);
// 	quantity::quantity_t X("X",Y.data_X_1D(),units::SI::one);
// 	P.Y1.add_curve(X,Y.log10(),"");
// 	X = quantity::quantity_t("X",cut_Y.data_X_1D(),units::SI::one);
// 	P.Y1.add_curve(X+start,cut_Y,"");
// 	P.to_screen("measurements_::sims_t::calc_t::implant_c::quantity_c::minimum_index_position()",0);
	return min_pos_finest;
}

unsigned int measurements_::sims_t::calc_t::implant_c::quantity_c::minimum_index_position_coarse()
{
	
	
	vector<int> maxIdx, minIdx;
	int window_size= 0.01 * Y.data().size(); // 1%
	if (window_size==0)
		window_size = 1;
	//make a copy ;; don t change the original Y, its needed for maximum finding
	auto YY = Y.filter_gaussian(window_size,1).moving_window_median(window_size);
	double deviation = 0.01*statistics::get_mad_from_Y(Y.data());
// 	fit_functions::polynom_t poly(8,Y.data());
// 	double chi_rel = poly.chisq() / Y.data().size();
// 	double deviation = 20 * log10(chi_rel);
// 	deviation = 200 * log10(chi_rel);

	if (!statistics::get_extrema_indices(maxIdx,minIdx,YY.data(),deviation))
	{
        //logger::error("measurements_::sims_t::calc_t::implant_c::minimum_index_position()","statistics::get_extrema_indices","false","returning 0");
		return 0;
	}
	sort(maxIdx.begin(),maxIdx.end());
	sort(minIdx.begin(),minIdx.end());

	map<double,int> area_to_min_pos;
	
	int min_pos = YY.data().size()-1;
	int max_pos = -1;
	for (int i=maxIdx.size()-1;i>=0;i--)
	{
		if (min_pos < maxIdx.at(i)) 
			continue;
		for (int j=minIdx.size()-1;j>=0;j--)
		{
			if (minIdx.at(j)<maxIdx.at(i))
			{
				min_pos=  minIdx.at(j);
				max_pos = maxIdx.at(i);
				double area = YY.sum(min_pos,max_pos).data().at(0);
				area_to_min_pos.insert(pair<double,int> (area,min_pos));
				break;
			}
		}
	}
	
	if (area_to_min_pos.size()==0)
		return 0;
	
	min_pos = area_to_min_pos.rbegin()->second; // max area
    //logger::info(3,"measurements_::sims_t::calc_t::implant_c::minimum_index_position()","min_pos: " + ::to_string(min_pos));
	minimum_index_position_p = min_pos;
	return minimum_index_position_p;
}


quantity::quantity_t measurements_::sims_t::calc_t::implant_c::quantity_c::minimum_position()
{
	return Y.at(minimum_index_position());
}

unsigned int measurements_::sims_t::calc_t::implant_c::quantity_c::maximum_index_position()
{
	if (maximum_index_position_p >= 0)
		return maximum_index_position_p;
	
	maximum_index_position_p = 0;
	
	if (!Y.is_set() || Y.data().size()<5)
		return maximum_index_position_p;
	
	/// get the rough position of the maximum
	maximum_index_position_p = statistics::get_max_index_from_Y(Y.remove_data_from_begin(minimum_index_position()).polyfit(17).data()) + minimum_index_position();
	
	int delta = (maximum_index_position_p - minimum_index_position())*8/10;
	int stop_idx = maximum_index_position_p + delta;
	unsigned int start_idx = maximum_index_position_p - delta;

	auto reduced_q = Y.get_data_by_index(start_idx,stop_idx);
	auto intensity_poly6 = reduced_q.polyfit(6);
	/// get fine position of the maximum
	maximum_index_position_p = statistics::get_max_index_from_Y(intensity_poly6.data());
	
	//debugging
//	double seconds_for_fit_plot=0;
//	if (seconds_for_fit_plot>=0)
//	{
//		plot_t plot(true,true); //log, lin
//		plot.Y1.range(1,1E6,true);
//		plot.Y2.range(1,1E6,true);
//		plot.Y3.range(1,1E6,true);
//		plot.Y1.add_curve(Y,Y.to_string());
//		plot.Y2.add_curve(intensity_poly6,intensity_poly6.to_string());
//		plot.to_screen("measurements_::sims_t::calc_t::implant_c::quantity_c::maximum_index_position()",seconds_for_fit_plot);
//	}
	
	return maximum_index_position_p;
}

quantity::quantity_t measurements_::sims_t::calc_t::implant_c::quantity_c::maximum_position()
{
	double vicinity_range = 0.01;
	int vicinity_start = (vicinity_range) * Y.data().size();
	int vicinity_end = (1 - vicinity_range) * Y.data().size();
	
	if (maximum_index_position() >= vicinity_end)
    {
        //logger::warning(3,"measurements_::sims_t::calc_t::implant_c::quantity_c::maximum_position()","maximum close at end");
    }
	if (maximum_index_position() <= vicinity_start)
    {
        //logger::warning(3,"measurements_::sims_t::calc_t::implant_c::quantity_c::maximum_position()","maximum close at start");
    }
	
	return Y.at(maximum_index_position());
}

quantity::quantity_t measurements_::sims_t::calc_t::implant_c::quantity_c::background(int last_points) const
{
// 	return Y.get_data_by_index_rel(0.99,1).mean();
	return Y.get_data_by_index(Y.data().size()-(last_points+1),Y.data().size()-1).mean();
}

/***********************************************************************************/
/*******         measurements_::sims_t::calc_t::implant_c::map_c           *********/
/***********************************************************************************/

measurements_::sims_t::calc_t::implant_c::map_c::map_c() : logger(global_logger,__FILE__,"measurements_::sims_t::calc_t::implant_c::map_c")
{
	minimum_index_position_p=-1;
	maximum_index_position_p=-1;
}
measurements_::sims_t::calc_t::implant_c::map_c::map_c(const quantity::map_t& XY_map) : XY_map_p(XY_map), logger(global_logger,__FILE__,"measurements_::sims_t::calc_t::implant_c::map_c")
{
// 	quantity_c Y_c(XY_map.Y());
// 	minimum_index_position_p = Y_c.minimum_index_position();
// 	maximum_index_position_p = Y_c.maximum_index_position();
// 	XY_map_without_surface_p = XY_map.get_data_by_index(minimum_index_position_p,XY_map.size()-1);
}

const quantity::map_t& measurements_::sims_t::calc_t::implant_c::map_c::XY_map() const
{
	return XY_map_p;
}

quantity::map_t measurements_::sims_t::calc_t::implant_c::map_c::XY_map_without_surface()
{
	return XY_map_p.get_data_by_index(minimum_index_position(),XY_map_p.size()-1);
}

quantity::quantity_t measurements_::sims_t::calc_t::implant_c::map_c::background_value(int last_points) const
{
	return XY_map_p.Y().get_data_by_index(XY_map_p.size()-last_points-1,XY_map_p.size()).mean();
}

quantity::quantity_t measurements_::sims_t::calc_t::implant_c::map_c::minimum_pos()
{
	auto q_min = XY_map_p.X().at(minimum_index_position());
	return q_min;
}

quantity::quantity_t measurements_::sims_t::calc_t::implant_c::map_c::minimum_value()
{
	return XY_map_p.Y().at(minimum_index_position());
}

quantity::quantity_t measurements_::sims_t::calc_t::implant_c::map_c::maximum_pos()
{
	return XY_map_p.X().at(maximum_index_position());
}

quantity::quantity_t measurements_::sims_t::calc_t::implant_c::map_c::maximum_value()
{
	return XY_map_p.Y().at(maximum_index_position());
}

quantity::quantity_t measurements_::sims_t::calc_t::implant_c::map_c::area()
{
	auto Q = XY_map_without_surface().integral();
	return Q;
}

quantity::quantity_t measurements_::sims_t::calc_t::implant_c::map_c::implanted_area()
{
// 	cout << endl << "area(): " << area().to_string() << endl;
// 	cout << "background_area: " << background_area().to_string() << endl;
	auto Q = area() - background_area();
// 	cout <<  "implanted_area: " << Q.to_string() << endl;
	return Q;
}

quantity::quantity_t measurements_::sims_t::calc_t::implant_c::map_c::background_area()
{
	auto Q = ( XY_map_without_surface().X().max() - XY_map_without_surface().X().min() ) * background_value();
	return Q;
}

unsigned int measurements_::sims_t::calc_t::implant_c::map_c::minimum_index_position()
{
	if (minimum_index_position_p>=0)
		return minimum_index_position_p;
	minimum_index_position_p = quantity_c(XY_map_p.Y()).minimum_index_position();
	return minimum_index_position_p;
}

unsigned int measurements_::sims_t::calc_t::implant_c::map_c::maximum_index_position()
{
	if (maximum_index_position_p>=0)
		return maximum_index_position_p;
	maximum_index_position_p = quantity_c(XY_map_p.Y()).maximum_index_position();
	return maximum_index_position_p;
}

bool measurements_::sims_t::calc_t::implant_c::map_c::is_implanted(double abs_treshold,  double rel_treshold)
{
	if (!implanted_area().is_set())
		return false;
	if (!background_area().is_set())
		return false;
	auto Q_tresh = (XY_map().X().max() -XY_map().X().min()) * quantity::quantity_t(XY_map_p.Y() ,abs_treshold );
    //logger::debug(4,"measurements_::sims_t::calc_t::implant_c::map_c::is_implanted","implanted_area(): " + implanted_area().to_string_short() + "; background_area(): "+background_area().to_string_short()+"; abs_treshold=" + ::to_string(abs_treshold) + "; Q_tresh: "+ Q_tresh.to_string_short()+ "; rel_treshold=" + ::to_string(rel_treshold), "return true");
	if ( implanted_area() < background_area() * rel_treshold ) 
	{
        //logger::debug(5,"measurements_::sims_t::calc_t::implant_c::map_c::is_implanted","implanted_area() < background_area() * rel_treshold","returning false");
		return false;
	}
	
	if (implanted_area() < Q_tresh)
	{
        //logger::debug(5,"measurements_::sims_t::calc_t::implant_c::map_c::is_implanted","implanted_area() < Q_tresh","returning false");
		return false;
	}
	//but what if background_area() = 0? --> use abs_treshold
	return true;
}



/***********************************************************************************/
/*******            measurements_::sims_t::calc_t::implant_c               *********/
/***********************************************************************************/

measurements_::sims_t::calc_t::implant_c::implant_c(sims_t& M, cluster_t& cluster, double X_resolution_factor) :
    implant_parameters(M.sample.implant(cluster.corresponding_isotope(M.reference_isotopes))), M(M), cluster(cluster), X_resolution_factor(X_resolution_factor),
    logger(global_logger,__FILE__,"measurements_::sims_t::calc_t::implant_c")
{
// 	auto mapper = M.concentration_vs_sputter_depth(cluster);
// 	if (mapper.is_set())
// 		C_vs_SD_p = map_c (mapper.change_resolution(mapper.X().resolution()/X_resolution_factor));
// 	
// 	mapper = M.concentration_vs_sputter_time(cluster);
// 	if (mapper.is_set())
// 		C_vs_ST_p = map_c (mapper.change_resolution(mapper.X().resolution()/X_resolution_factor));
// 	
// 	mapper = M.intensity_vs_sputter_depth(cluster);
// 	if (mapper.is_set())
// 		I_vs_SD_p = map_c (mapper.change_resolution(mapper.X().resolution()/X_resolution_factor));
// 	
// 	mapper = M.intensity_vs_sputter_time(cluster);
// 	if (mapper.is_set())
// 		I_vs_ST_p = map_c (mapper.change_resolution(mapper.X().resolution()/X_resolution_factor));
	
	
    //logger::debug(11,"measurements_::sims_t::calc_t::implant_c::implant_c()","new implant object created",cluster.to_string(),M.to_string_short());
}

quantity::concentration_t measurements_::sims_t::calc_t::implant_c::background_concentration()
{
	if (!is_implanted())
		return {};
	quantity::quantity_t Q_out;
	if (C_vs_SD().XY_map().is_set())
		Q_out = C_vs_SD().background_value();
	else if (C_vs_ST().background_value().is_set())
		Q_out = C_vs_ST().background_value();
	
	if (!cluster.implant_parameters.background_concentration.is_set() && Q_out.is_set())
		cluster.implant_parameters.background_concentration = Q_out;
	return Q_out;
}

quantity::intensity_t measurements_::sims_t::calc_t::implant_c::background_intensity()
{	
	if (!is_implanted())
		return {};
	quantity::quantity_t Q_out;
	if (I_vs_SD().XY_map().is_set())
		Q_out = I_vs_SD().background_value();
	else if (I_vs_ST().XY_map().is_set())
		Q_out = I_vs_ST().background_value();
	
	if (!cluster.implant_parameters.background_intensity.is_set() && Q_out.is_set())
		cluster.implant_parameters.background_intensity = Q_out;
	return Q_out;
}

quantity::dose_t measurements_::sims_t::calc_t::implant_c::dose()
{
	quantity::quantity_t Q_out;
	
	if (!is_implanted())
		return {};
	
	if (C_vs_SD().XY_map().is_set())
		Q_out = C_vs_SD().area().change_unit(units::derived::atoms_per_scm);
	
	if (Q_out.is_set() && !cluster.implant_parameters.dose.is_set())
		cluster.implant_parameters.dose = Q_out;
	if (C_vs_SD().minimum_pos().is_set() && !cluster.implant_parameters.sputter_depth_at_min.is_set())
		cluster.implant_parameters.sputter_depth_at_min = C_vs_SD().minimum_pos();
	if (C_vs_SD().minimum_value().is_set() && !cluster.implant_parameters.min_concentration.is_set())
		cluster.implant_parameters.min_concentration = C_vs_SD().minimum_value();
	
	
	return Q_out;
}
quantity::SR_t measurements_::sims_t::calc_t::implant_c::SR_from_max()
{
	quantity::quantity_t ST_at_max;
	if (!implant_parameters.depth_at_concentration_maxium.is_set())
		return {};
	if (!is_implanted())
		return {};
	if (I_vs_ST().XY_map().is_set())
		ST_at_max =  I_vs_ST().maximum_pos();
	else if (C_vs_ST().XY_map().is_set())
		ST_at_max =  C_vs_ST().maximum_pos();
	
	if (ST_at_max.is_set() && !cluster.implant_parameters.sputter_time_at_max.is_set())
			cluster.implant_parameters.sputter_time_at_max = ST_at_max;
	
	
	return implant_parameters.depth_at_concentration_maxium / ST_at_max;
}
quantity::SF_t measurements_::sims_t::calc_t::implant_c::SF()
{
	if (SF_from_dose().is_set())
		return SF_from_dose();
	return SF_from_max();
}
quantity::SF_t measurements_::sims_t::calc_t::implant_c::SF_from_dose()
{
	if (!implant_parameters.dose.is_set())
		return {};
	if (!is_implanted())
		return {};
	
	if (I_vs_SD().minimum_pos().is_set() && !cluster.implant_parameters.sputter_depth_at_min.is_set())
		cluster.implant_parameters.sputter_depth_at_min = I_vs_SD().minimum_pos();
	if (I_vs_SD().minimum_value().is_set() && !cluster.implant_parameters.min_intensity.is_set())
		cluster.implant_parameters.min_intensity = I_vs_SD().minimum_value();
	
	quantity::SF_t SF_p = ( implant_parameters.dose / I_vs_SD().area() ).change_unit(units::derived::atoms_per_ccm/units::derived::counts*units::SI::second);
    //logger::info(3,"measurements_::sims_t::calc_t::implant_c::SF_from_dose()",M.to_string_short() + " " + cluster.to_string() + " " + SF_p.to_string());
	
	return SF_p;
}

quantity::SF_t measurements_::sims_t::calc_t::implant_c::SF_from_max()
{
	if (!implant_parameters.concentration_maximum.is_set())
		return {};
	if (!is_implanted())
		return {};
	quantity::intensity_t I_max = I_vs_ST().maximum_value();
	if (!I_max.is_set())
		I_max = I_vs_SD().maximum_value();
	
	if (I_max.is_set() && !cluster.implant_parameters.min_intensity.is_set())
		cluster.implant_parameters.max_intensity = I_max;
	
	quantity::SF_t SF_p = ( implant_parameters.concentration_maximum / I_max ). change_unit(units::derived::atoms_per_ccm/units::derived::counts*units::SI::second);
    //logger::info(3,"measurements_::sims_t::calc_t::implant_c::SF_from_max()",M.to_string_short() + " " + cluster.to_string() + " " + SF_p.to_string());
		
	return SF_p;
}

measurements_::sims_t::calc_t::implant_c::map_c& measurements_::sims_t::calc_t::implant_c::C_vs_SD()
{
	if (!C_vs_SD_p.XY_map().is_set())
	{
		auto mapper = M.concentration_vs_sputter_depth(cluster);
		if (mapper.is_set())
			C_vs_SD_p = map_c (mapper.change_resolution(mapper.X().resolution()/X_resolution_factor));
	}
	return C_vs_SD_p;
}
measurements_::sims_t::calc_t::implant_c::map_c& measurements_::sims_t::calc_t::implant_c::C_vs_ST()
{
	if (!C_vs_ST_p.XY_map().is_set())
	{
		auto mapper = M.concentration_vs_sputter_time(cluster);
		if (mapper.is_set())
			C_vs_ST_p = map_c (mapper.change_resolution(mapper.X().resolution()/X_resolution_factor));
	}
	return C_vs_ST_p;
}
measurements_::sims_t::calc_t::implant_c::map_c& measurements_::sims_t::calc_t::implant_c::I_vs_SD()
{
	if (!I_vs_SD_p.XY_map().is_set())
	{
		auto mapper = M.intensity_vs_sputter_depth(cluster);
// 		cout << endl << "mapper: " << mapper.to_string() << endl;
// 		cout << "mapper.res: " << mapper.X().resolution().to_string() << endl;
// 		cout <<"X_resolution_factor: " << X_resolution_factor << endl;
// 		cout << "mapper.X().resolution()/X_resolution_factor: " << (mapper.X().resolution()/X_resolution_factor).to_string() << endl;
		if (mapper.is_set())
			I_vs_SD_p = map_c (mapper.change_resolution(mapper.X().resolution()/X_resolution_factor));
// 		cout << "I_vs_SD_p: " << I_vs_SD_p.XY_map().to_string() << endl;
// 		cout << "I_vs_SD_p.res: " << I_vs_SD_p.XY_map().X().resolution().to_string() << endl;
	}
	
	return I_vs_SD_p;
}
measurements_::sims_t::calc_t::implant_c::map_c& measurements_::sims_t::calc_t::implant_c::I_vs_ST()
{
	if (!I_vs_ST_p.XY_map().is_set())
	{
		auto mapper = M.intensity_vs_sputter_time(cluster);
		if (mapper.is_set())
			I_vs_ST_p = map_c (mapper.change_resolution(mapper.X().resolution()/X_resolution_factor));
	}
	return I_vs_ST_p;
}

void measurements_::sims_t::calc_t::implant_c::set_parameters_in_cluster()
{
	if (C_vs_SD_p.XY_map().is_set())
	{
		
	}
}

bool measurements_::sims_t::calc_t::implant_c::is_implanted()
{
	if (I_vs_ST().XY_map().is_set() && !I_vs_ST().is_implanted())
	{
        //logger::debug(5,"measurements_::sims_t::calc_t::implant_c::is_implanted","I_vs_ST().XY_map().is_set() && !I_vs_ST().is_implanted()");
		return false;
	}
	if (C_vs_SD().XY_map().is_set() && !C_vs_SD().is_implanted())
	{
        //logger::debug(5,"measurements_::sims_t::calc_t::implant_c::is_implanted","C_vs_SD().XY_map().is_set() && !C_vs_SD().is_implanted()");
		return false;
	}
	if (I_vs_SD().XY_map().is_set() && !I_vs_SD().is_implanted())
	{
        //logger::debug(5,"measurements_::sims_t::calc_t::implant_c::is_implanted","I_vs_SD().XY_map().is_set() && !I_vs_SD().is_implanted()");
		return false;
	}
	if (C_vs_ST().XY_map().is_set() && !C_vs_ST().is_implanted())
	{
        //logger::debug(5,"measurements_::sims_t::calc_t::implant_c::is_implanted","C_vs_ST().XY_map().is_set() && !C_vs_ST().is_implanted()");
		return false;
	}
    //logger::debug(5,"measurements_::sims_t::calc_t::implant_c::is_implanted",cluster.to_string() + " " + M.sample.to_string(),"returning true");
	return true;
}
