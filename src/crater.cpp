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

#include "crater.hpp"


/************/
/*LINESCAN_T*/
/************/
crater_t::linescan_t::linescan_t()
{
}

crater_t::linescan_t::linescan_t(quantity_t xy, quantity_t z) : xy(xy),z(z)
{
}

fit_functions::asym2sig_t crater_t::linescan_t::fit_params()
{
	return asym2sig;
}

bool crater_t::linescan_t::is_set()
{
	if (!xy.is_set() || !z.is_set()) return false;
	return true;
}


quantity_t crater_t::linescan_t::fit()
{
	if (!is_set()) return {};
	quantity_t fitted_z_profile("linescan_z_fitted",{},z.unit());
// 	fitted_z_profile.name = "linescan_z_fitted";
// 	fitted_z_profile.data={};
	
	if (z.data.size()<21) return {};
	map<double,double> data_XY;
	// sampling rate: 3000 points for 600um scan length 10%=300 points window size is a good value
	tools::vec::combine_vecs_to_map(&xy.data,z.filter_impulse(0,2).data,&data_XY);
	if (!fitted) 
	{
		double chisq_rel = 1;
		for (int j=1;j<4;j++)
		{
			fit_functions::asym2sig_t asym2sig_p;
			asym2sig_p.fit(data_XY,NAN,NAN,NAN,NAN,j/4 * (data_XY.end()->first));
			if (asym2sig_p.chisq()<asym2sig.chisq() || asym2sig.chisq()==-1) 
				asym2sig=asym2sig_p;

			if (asym2sig.fitted() && asym2sig.chisq()/asym2sig.chisq0()<0.01 && asym2sig.chisq()/asym2sig.chisq0()>0) 
				break;
		}
		fitted=asym2sig.fitted();
	}
	if (!fitted) return fitted_z_profile;
	fitted_z_profile.data=asym2sig.fitted_y_data(xy.data);
// 	plot_t::fast_plot({xy,xy},{z,fitted_z_profile},"/tmp/linescan"+to_string((int)z.median().data[0]),false);
	return fitted_z_profile;
}

quantity_t crater_t::linescan_t::depth()
{	
	if (!is_set()) return {};
	quantity_t fitted_z_profile = fit();
	
	if (fit_params().gof()<0.4)
	{
		logger::warning(1,"crater_t::linescan_t::depth()", "fit_params().gof()<0.4",tools::to_string(fit_params().gof()),"skipping");
		return {};
	}
	
	double y0=asym2sig.fitted_y_data({0})[0];
	double m=(asym2sig.fitted_y_data({xy.data.back()})[0]-y0)/xy.data.back();
	quantity_t linescan_depth("total_sputter_depth",{},fitted_z_profile.unit());
	
	// fitted function value in the centre of the crater minus the valu at the same position on the (virtual) surface
	linescan_depth.data={abs(asym2sig.fitted_y_data({asym2sig.xc})[0] - (y0+m*asym2sig.xc))};
// 	linescan_depth.data={abs(asym2sig.fitted_y_data({asym2sig.xc})[0]-asym2sig.y0-asym2sig.m*asym2sig.xc)};
// 	linescan_depth.data={abs(asym2sig.fitted_y_data({asym2sig.xc})[0]-asym2sig.fitted_y_data({0})[0]-asym2sig.y0-asym2sig.m*asym2sig.xc)};
	
	return linescan_depth;
}

std::__cxx11::string crater_t::linescan_t::to_string(std::__cxx11::string prefix)
{
	stringstream ss;
	ss << prefix;
	ss << std::scientific << depth().to_string() <<"; " << fit_params().to_string();
	return ss.str();
}





/********************/
/** sputter_beam_t **/
/********************/
crater_t::sputter_beam_t::sputter_beam_t(sputter_current_t sputter_current_s, 
							 sputter_time_t sputter_time, 
							 sputter_depth_t sputter_depth_s) 
																	: 	sputter_current(sputter_current_s), 
																		sputter_time(sputter_time), 
																		sputter_depth(sputter_depth_s)
{
}

const std::__cxx11::string crater_t::sputter_beam_t::to_string(const string del) const
{
	stringstream out;
	out << sputter_time.to_string() << del << sputter_depth.to_string() << del << sputter_current.to_string();
	return out.str();
}


/**************/
/** CRATER_T **/
/**************/
crater_t::crater_t()
{
}

 const string crater_t::to_string(const string del) const
 {
	 return "not set at the mom";
 }

// quantity_t crater_t::set_common_base_points(vector<cluster_t>& clusters)
// {
// 	if(clusters.size()==0)
// 	{
// 		logger::error("crater_t::set_common_base_points()","clusters.size()==0","","returning false");
// 		return {};
// 	}
// 	vector<sputter_time_t> sputter_times;
// 	vector<sputter_depth_t> sputter_depths;
// // 	if (sputter_beam.sputter_time.is_set())
// // 		sputter_times.push_back(sputter_beam.sputter_time);
// // 	if (sputter_beam.sputter_depth.is_set())
// // 		sputter_depths.push_back(sputter_beam.sputter_depth);
// 	for (int i=0;i<clusters.size();i++)
// 	{
// 		if (clusters.at(i).sputter_depth().is_set())
// 			sputter_depths.push_back(clusters.at(i).sputter_depth());
// 		if (clusters.at(i).sputter_time.is_set())
// 			sputter_times.push_back(clusters.at(i).sputter_time);
// 	}
// 	
// 	if (sputter_times.size() < clusters.size() && sputter_depths.size() < clusters.size())
// 	{
// // 		logger::error("crater_t::get_common_X_dimension_vector: use just sputter_time OR sputter_depth for all clusters");
// 		logger::error("crater_t::set_common_base_points()","sputter_times.size() < clusters.size() && sputter_depths.size() < clusters.size()","use just sputter_time OR sputter_depth for all clusters","returning false");
// 		return {};
// 	}
// 	if (sputter_times.size() > 0 && sputter_depths.size() > 0 && sputter_times.size() != sputter_depths.size())
// // 		logger::warning("crater_t::get_common_X_dimension_vector: sputter_times and sputter_depths sometimes set and sometimes not");
// 		logger::warning(1,"crater_t::set_common_base_points()","sputter_times.size() > 0 && sputter_depths.size() > 0 && sputter_times.size() != sputter_depths.size()","sputter_times and sputter_depths sometimes set and sometimes not");
// 	if (sputter_times.size() > 0 && sputter_times.size() != clusters.size())
// 		logger::error("crater_t::set_common_base_points()","sputter_times.size()>0 && sputter_times.size()!=clusters.size()","check raw data for exported sputter_times");
// // 		logger::error("crater_t::get_common_X_dimension_vector: sputter_times.size() != clusters.size()","check raw data for exported sputter_times");
// 	if (sputter_depths.size() > 0 && sputter_depths.size() != clusters.size())
// 		logger::error("crater_t::set_common_base_points()","sputter_depths.size() > 0 && sputter_depths.size() != clusters.size()","check raw data for exported sputter_dephs");
// // 		logger::error("crater_t::get_common_X_dimension_vector: sputter_depths.size() != clusters.size()","check raw data for exported sputter_dephs");
// 	
// 	vector<double> mins;
// 	vector<double> maxs;	
// 	quantity_t X;
// 	double resolution;
// 	if (sputter_depths.size()==clusters.size())
// 	{
// 		/*asuming same units*/
// 		for (auto& SD : sputter_depths)
// 		{
// 			mins.push_back(statistics::get_min_from_Y(SD.data));
// 			maxs.push_back(statistics::get_max_from_Y(SD.data));
// 		}
// 		X = sputter_depth_t({},sputter_depths.begin()->unit());
// 		resolution = abs(sputter_depths.begin()->data.front() - sputter_depths.begin()->data.back()); 
// 	}
// 	else
// 	{
// 		/*asuming same units*/
// 		for (auto& ST : sputter_times)
// 		{
// 			mins.push_back(statistics::get_min_from_Y(ST.data));
// 			maxs.push_back(statistics::get_max_from_Y(ST.data));
// 		}
// 		X = sputter_time_t({},sputter_times.begin()->unit());
// 		resolution = abs(sputter_times.begin()->data.front() - sputter_times.begin()->data.back()); 
// 	}
// 	
// 	if (mins.size()==0 || maxs.size()==0)
// 	{
// 		logger::error("crater_t::set_common_base_points()", "mins.size()==0 || maxs.size()==0","","returning false");
// 		return {};
// 	}
// 	
// 	double x_min = statistics::get_max_from_Y(mins);
// 	double x_max = statistics::get_min_from_Y(maxs);
// 	
// 	if (mins.size()>1)
// 		resolution = abs( (x_min - statistics::get_min_from_Y(mins)) / (mins.size()-1) );
// 	
// 	if (resolution<=0)
// 	{
// 		logger::error("crater_t::set_common_base_points()","resolution<=0","","returning false");
// 		return {};
// 	}
// 	
// 	const int common_dimension_data_size = floor(x_max-x_min)/resolution+1;
// 	if (common_dimension_data_size<3)
// 	{
// 		logger::error("crater_t::set_common_base_points()","common_dimension_data_size<3",tools::to_string(common_dimension_data_size),"returning false");
// 		return {};
// 	}
// 	
// 	X.data.resize(common_dimension_data_size);
// 	for (int i=0;i<common_dimension_data_size;i++)
// 		X.data.at(i)=resolution*i+x_min;
// 	
// // 	sputter_time_t common_ST;
// // 	sputter_depth_t common_SD;
// // 	if (sputter_time.name() == X.name())
// // 		sputter_time = X;
// // 	else if (sputter_depth_s.name() == X.name())
// // 		sputter_depth_s = X;
// // 	else
// // 	{
// // 		logger::error("crater_t::set_common_base_points()","sputter_time.name() != sputter_depth_s.name() != X.name()","could not recognize X axis","returning false");
// // 		return {};
// // 	}
// 	logger::debug(5,"crater_t::set_common_base_points()","","","SUCCESS");
// 	return X;
// }

total_sputter_time_t crater_t::total_sputter_time(vector<cluster_t>* clusters)
{
	if (total_sputter_time().is_set())
		return total_sputter_time();
	
	if (sputter_time.is_set())
		total_sputter_time() = total_sputter_time_t(sputter_time.max());
	
	if (clusters==nullptr || clusters->size()==0) 
		return total_sputter_time();
	
	for (auto& C : *clusters)
	{
		if (!C.sputter_time.is_set()) continue;
		total_sputter_time_t TST(C.sputter_time.max());
		if (TST > total_sputter_time() || !total_sputter_time().is_set())
			total_sputter_time() = TST;
	}
	
	return total_sputter_time();
}

total_sputter_depth_t crater_t::total_sputter_depth()
{
	total_sputter_depth_t TSD;
	if (total_sputter_depths.is_set())
		TSD = total_sputter_depths;
	if (linescans.size()>0)
	{
		for (auto& LS : linescans)
			if (LS.fit_params().gof()>0.5)
				TSD << LS.depth();
	}
	if (TSD.is_set())
		return TSD.median();
	return {};
}

const quantity_t * crater_t::X() const
{
	const quantity_t* X;
	if (sputter_depth.is_set())
		return &sputter_depth;
	if (sputter_time.is_set())
		return &sputter_time;
	return nullptr;
}


// sputter_time_t crater_t::common_sputter_time(vector<cluster_t>& clusters)
quantity_t crater_t::common_X_quantity(const vector<quantity_t>& X_quantities)
{
	vector<double> mins;
	vector<double> maxs;	
	
	double resolution;
	
	/*asuming same units*/
	for (auto& ST : X_quantities)
	{
		mins.push_back(statistics::get_min_from_Y(ST.data));
		maxs.push_back(statistics::get_max_from_Y(ST.data));
	}
	quantity_t X(X_quantities.begin()->name(),X_quantities.begin()->unit());
	resolution = abs(X_quantities.begin()->data.front() - X_quantities.begin()->data.back()); 
	
	
	if (mins.size()==0 || maxs.size()==0)
	{
		logger::error("crater_t::common_X_quantity()", "mins.size()==0 || maxs.size()==0","","returning false");
		return {};
	}
	
	double x_min = statistics::get_max_from_Y(mins);
	double x_max = statistics::get_min_from_Y(maxs);
	
	if (mins.size()>1)
		resolution = abs( (x_min - statistics::get_min_from_Y(mins)) / (mins.size()-1) );
	
	if (resolution<=0)
	{
		logger::error("crater_t::common_X_quantity()","resolution<=0","","returning false");
		return {};
	}
	else
		logger::debug(11,"crater_t::common_X_quantity()","resolution: ", tools::to_string(resolution));
	
	const int common_dimension_data_size = floor(x_max-x_min)/resolution+1;
	if (common_dimension_data_size<3)
	{
		logger::error("crater_t::common_X_quantity()","common_dimension_data_size<3",tools::to_string(common_dimension_data_size),"returning false");
		return {};
	}
	else
		logger::debug(11,"crater_t::common_X_quantity()","common_dimension_data_size: ", tools::to_string(common_dimension_data_size));
	
	X.data.resize(common_dimension_data_size);
	for (int i=0;i<common_dimension_data_size;i++)
		X.data.at(i)=resolution*i+x_min;
	
	return X;
}

sputter_depth_t crater_t::common_sputter_depth(vector<cluster_t>& clusters)
{
	if(clusters.size()==0)
	{
		logger::error("crater_t::common_sputter_depth()","clusters.size()==0","","returning empty");
		return {};
	}
	vector<quantity_t> sputter_depths; 
	for (int i=0;i<clusters.size();i++)
	{
		if (clusters.at(i).sputter_depth.is_set())
			sputter_depths.push_back(clusters.at(i).sputter_depth);
	}
	logger::debug(7,"crater_t::common_sputter_depth","sputter_depths.size()="+tools::to_string(sputter_depths.size()) + " clusters.size()="+tools::to_string(clusters.size()));
	if (sputter_depths.size() != clusters.size() )
	{
		logger::error("crater_t::common_sputter_depth()","sputter_depths.size() != clusters.size()","check exported clusters for sputter_depth","returning empty");
		return {};
	}
	return sputter_depth_t{common_X_quantity(sputter_depths)};
}

sputter_time_t crater_t::common_sputter_time(vector<cluster_t>& clusters)
{
	if(clusters.size()==0)
	{
		logger::error("crater_t::common_sputter_time","clusters.size()==0","","returning empty");
		return {};
	}
	vector<quantity_t> sputter_times; 
	for (int i=0;i<clusters.size();i++)
	{
		if (clusters.at(i).sputter_time.is_set())
			sputter_times.push_back(clusters.at(i).sputter_time);
	}
	if (sputter_times.size() != clusters.size() )
	{
		logger::error("crater_t::common_sputter_time","sputter_times.size() != clusters.size()","check exported clusters for sputter_time","returning empty");
		return {};
	}
	return sputter_time_t{common_X_quantity(sputter_times)};
}


// sputter_time_t& crater_t::sputter_time
// {
// 	if (sputter_time.is_set())
// 		return sputter_time;
// // 	if (clusters == nullptr || !set_common_base_points(*clusters))
// // 		logger::error("crater_t::sputter_time","clusters == nullptr || !set_common_base_points(*clusters)","","returning empty");
// // 	if (clusters == nullptr)
// // 		logger::debug(5,"crater_t::sputter_time","clusters == nullptr");
// // 	else if (!set_common_base_points(*clusters))
// // 		logger::debug(5,"crater_t::sputter_time","!set_common_base_points(*clusters)");
// 	return sputter_time;
// }

// sputter_depth_t& crater_t::sputter_depth()
// {
// 	if (sputter_depth_s.is_set())
// 		return sputter_depth_s;
// // 	if (sputter_time(clusters).is_set() && SR.is_set())
// // 	{
// // 		if (SR.data.size()==1)
// // 			sputter_depth_s = SR * sputter_time;
// // 		else
// // 			sputter_depth_s = (sputter_time.diff() * SR).sum();
// // 		return sputter_depth_s;
// // 	}
// 	return sputter_depth_s;
// }

crater_t crater_t::change_resolution(sputter_depth_t sputter_depth_res)
{
	if (!sputter_depth.is_set())
	{
		logger::error("crater_t::change_resolution","!sputter_depth().is_set()","","returning this");
		return *this;
	}
	if (!sputter_depth_res.is_set())
	{
		logger::error("crater_t::change_resolution","!sputter_depth_res.is_set()","","returning this");
		return *this;
	}
	if (sputter_depth_res.data.size()==1 && sputter_depth_res.data.at(0)==0)
	{
		logger::error("crater_t::change_resolution","!sputter_depth_res is 0","","returning this");
		return *this;
	}
	crater_t copy_C = *this;
	copy_C.sputter_depth = sputter_depth.change_resolution(sputter_depth_res);
	if (sputter_time.is_set())
		copy_C.sputter_time = sputter_time.interp(sputter_depth,copy_C.sputter_depth);
	if (SR.is_set())
		copy_C.SR = SR.interp(sputter_depth,copy_C.sputter_depth);
	if (sputter_current().is_set())
		copy_C.sputter_current() = sputter_current().interp(sputter_depth,copy_C.sputter_depth);
	return copy_C;
}

crater_t crater_t::change_resolution(sputter_time_t sputter_time_res)
{
	if (!sputter_time.is_set())
	{
		logger::error("crater_t::change_resolution","!sputter_depth().is_set()","","returning this");
		return *this;
	}
	if (!sputter_time_res.is_set())
	{
		logger::error("crater_t::change_resolution","!sputter_depth_res.is_set()","","returning this");
		return *this;
	}
	if (sputter_time_res.data.size()==1 && sputter_time_res.data.at(0)==0)
	{
		logger::error("crater_t::change_resolution","!sputter_depth_res is 0","","returning this");
		return *this;
	}
	crater_t copy_C = *this;
	copy_C.sputter_time = sputter_time.change_resolution(sputter_time_res);
	if (sputter_depth.is_set())
		copy_C.sputter_depth = sputter_depth.interp(sputter_time,copy_C.sputter_time);
	if (SR.is_set())
		copy_C.SR = SR.interp(sputter_time,copy_C.sputter_time);
	if (sputter_current().is_set())
		copy_C.sputter_current() = sputter_current().interp(sputter_time,copy_C.sputter_time);
	return copy_C;
}

sputter_current_t& crater_t::sputter_current()
{
// 	logger::debug(10,"crater_t::sputter_current()","sputter_beam.sputter_current()",sputter_beam.sputter_current.to_string());
	if (sputter_beam.sputter_time.is_set() && sputter_time.is_set())
	{
		sputter_beam.sputter_current = sputter_beam.sputter_current.interp(sputter_beam.sputter_time,sputter_time);
		sputter_beam.sputter_time.clear();
	}
	else if (sputter_beam.sputter_depth.is_set() && sputter_depth.is_set())
	{
		sputter_beam.sputter_current = sputter_beam.sputter_current.interp(sputter_beam.sputter_depth,sputter_depth);
		sputter_beam.sputter_depth.clear();
	}
	else if (sputter_beam.sputter_depth.is_set() || sputter_beam.sputter_time.is_set())
	{
		logger::error("crater_t::sputter_current()","can not interpolate sputter_beam, there seem to be differences in exported clusters sputter_time / depth","","return empty");
		sputter_beam.sputter_current.clear();
	}
	return sputter_beam.sputter_current;
}
// crater_t crater_t::change_sputter_depth(sputter_depth_t new_sputter_depth, vector<cluster_t>& clusters)
// {
// 	if (!sputter_depth().is_set()) return {};
// 	if (!new_sputter_depth.is_set()) 
// 	{
// 		logger::error("crater_t::change_sputter_depth()", "!new_sputter_depth.is_set()","","returning empty");
// 		return {};
// 	}
// 	quantity_t old_sputter_depth = sputter_time.change_unit(new_sputter_depth.unit()); 
// 	if (old_sputter_depth.unit() != new_sputter_depth.unit())// same units?
// 	{
// 		logger::error("crater_t::change_sputter_depth","old_sputter_depth.unit() != new_sputter_depth.unit()");
// 		return {};
// 	}
// 	crater_t new_crater;
// 	new_crater.sputter_depth_s = new_sputter_depth;
// 	map<double,double> XY_data;
// 	if (sputter_time.is_set())
// 	{
// 		XY_data.clear();
// 		tools::vec::combine_vecs_to_map(&old_sputter_depth.data,&sputter_time.data,&XY_data);
// 		new_crater.sputter_time = sputter_time_t(statistics::interpolate_data_XY(XY_data,new_sputter_depth.data),sputter_depth().unit());
// 	}
// 	if (SR.is_set())
// 	{
// 		XY_data.clear();
// 		tools::vec::combine_vecs_to_map(&old_sputter_depth.data,&SR.data,&XY_data);
// 		new_crater.SR = SR_t(statistics::interpolate_data_XY(XY_data,new_sputter_depth.data),SR.unit());
// 	}
// // 	if (clusters==nullptr || clusters->size()==0)
// 	if (clusters.size()==0)
// 		return new_crater;
// 	
// 	for (auto C : clusters)
// 	{
// 		if (!C.concentration().is_set() && !C.intensity().is_set())
// 		{
// 			logger::error("crater_t::change_sputter_depth()","!C.concentration().is_set() && !C.intensity().is_set()","","skipping");
// 			continue;
// 		}
// 		if (C.sputter_depth().is_set())
// 		{
// 			C = C.change_sputter_depth(new_crater.sputter_depth());
// 			continue;
// 		}
// 		if (C.sputter_time.is_set())
// 		{
// 			//do nothing
// 			continue;
// 		}
// 		C.sputter_depth_p = old_sputter_depth;
// 		C = C.change_sputter_depth(new_crater.sputter_depth());
// 	}
// 	
// 	return new_crater;
// }
// 
// crater_t crater_t::change_sputter_time(sputter_time_t new_sputter_time, vector<cluster_t>& clusters)
// {
// 	if (!sputter_time.is_set()) return {};
// 	if (!new_sputter_time.is_set()) 
// 	{
// 		logger::error("crater_t::change_sputter_time","!new_sputter_time.is_set()","","returning empty");
// 		return {};
// 	}
// 	quantity_t old_sputter_time = sputter_time.change_unit(new_sputter_time.unit()); 
// 	if (old_sputter_time.unit() != new_sputter_time.unit())// same units?
// 	{
// 		logger::error("crater_t::change_sputter_time","old_sputter_time.unit() != new_sputter_time.unit()",old_sputter_time.unit().to_string()+"!="+new_sputter_time.unit().to_string(),"returning empty");
// 		return {};
// 	}
// 	crater_t new_crater;
// 	new_crater.sputter_time = new_sputter_time;
// 	map<double,double> XY_data;
// 	if (sputter_depth().is_set())
// 	{
// 		XY_data.clear();
// 		tools::vec::combine_vecs_to_map(&old_sputter_time.data,&sputter_depth().data,&XY_data);
// 		new_crater.sputter_depth_s = sputter_depth_t(statistics::interpolate_data_XY(XY_data,new_sputter_time.data),sputter_depth().unit());
// 	}
// 	if (SR.is_set())
// 	{
// 		XY_data.clear();
// 		tools::vec::combine_vecs_to_map(&old_sputter_time.data,&SR.data,&XY_data);
// 		new_crater.SR = SR_t(statistics::interpolate_data_XY(XY_data,new_sputter_time.data),SR.unit());
// 	}
// // 	if (clusters==nullptr || clusters->size()==0)
// 	if (clusters.size()==0)
// 		return new_crater;
// 	
// 	for (auto& C : clusters)
// 	{
// 		if (!C.concentration().is_set() && !C.intensity().is_set())
// 		{
// 			logger::error("crater_t::change_sputter_time","!C.concentration().is_set() && !C.intensity().is_set()","","skipping");
// 			continue;
// 		}
// 		if (C.sputter_time.is_set())
// 		{
// 			C = C.change_sputter_time(new_crater.sputter_time);
// 			continue;
// 		}
// 		if (C.sputter_depth().is_set())
// 		{
// 			//do nothing
// 			continue;
// 		}
// 		C.sputter_time_p = old_sputter_time;
// 		C = C.change_sputter_time(new_crater.sputter_time);
// 	}
// 	
// 	return new_crater;
// }

