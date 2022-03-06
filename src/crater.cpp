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

crater_t::linescan_t::linescan_t(quantity::quantity_t xy, quantity::quantity_t z) : xy(xy),z(z)
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


quantity::quantity_t crater_t::linescan_t::fit()
{
	if (!is_set()) return {};
	
// 	fitted_z_profile.name = "linescan_z_fitted";
// 	fitted_z_profile.data()={};
	
	if (z.data().size()<21) 
		return {};
	map<double,double> data_XY;
	// sampling rate: 3000 points for 600um scan length 10%=300 points window size is a good value
	tools::vec::combine_vecs_to_map(&xy.data(),&z.filter_impulse(0,2).data(),&data_XY);
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
	if (!fitted) 
		return {};
	quantity::quantity_t fitted_z_profile("linescan_z_fitted",asym2sig.fitted_y_data(xy.data()),z.unit(),quantity::dimensions::SI::length);

// 	plot_t::fast_plot({xy,xy},{z,fitted_z_profile},"/tmp/linescan"+to_string((int)z.median().data()[0]),false);
	return fitted_z_profile;
}

quantity::quantity_t crater_t::linescan_t::depth()
{
	if (!is_set()) return {};
	quantity::quantity_t fitted_z_profile = fit();
	
	if (fit_params().gof()<0.4)
	{
		logger::warning(1,"crater_t::linescan_t::depth()", "fit_params().gof()<0.4",tools::to_string(fit_params().gof()),"skipping");
		return {};
	}
	
	double y0=asym2sig.fitted_y_data({0})[0];
	double m=(asym2sig.fitted_y_data({xy.data().back()})[0]-y0)/xy.data().back();
	
	
	// fitted function value in the centre of the crater minus the valu at the same position on the (virtual) surface
	vector<double> D ={abs(asym2sig.fitted_y_data({asym2sig.xc})[0] - (y0+m*asym2sig.xc))};
// 	linescan_depth.data()={abs(asym2sig.fitted_y_data({asym2sig.xc})[0]-asym2sig.y0-asym2sig.m*asym2sig.xc)};
// 	linescan_depth.data()={abs(asym2sig.fitted_y_data({asym2sig.xc})[0]-asym2sig.fitted_y_data({0})[0]-asym2sig.y0-asym2sig.m*asym2sig.xc)};
	
	return {"total_sputter_depth",D,fitted_z_profile.unit(),quantity::dimensions::SI::length};
}

string crater_t::linescan_t::to_string(string prefix)
{
	stringstream ss;
	ss << prefix;
	ss << std::scientific << depth().to_string() <<"; " << fit_params().to_string();
	return ss.str();
}





/********************/
/** sputter_beam_t **/
/********************/
crater_t::sputter_beam_t::sputter_beam_t(quantity::current_t sputter_current_s, 
							 quantity::sputter_time_t sputter_time, 
							 quantity::depth_t sputter_depth_s) 
																	: 	sputter_current(sputter_current_s), 
																		sputter_time(sputter_time), 
																		sputter_depth(sputter_depth_s)
{
}

const string crater_t::sputter_beam_t::to_string(const string del) const
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

quantity::sputter_time_t& crater_t::total_sputter_time(vector<cluster_t>* clusters)
{
	if (total_sputter_time_s.is_set())
		return total_sputter_time_s;
	
	if (sputter_time.is_set())
	{
		total_sputter_time_s = sputter_time.max();
	}
	
	if (clusters==nullptr || clusters->size()==0) 
		return total_sputter_time_s;
	
	for (auto& C : *clusters)
	{
		if (!C.sputter_time.is_set()) continue;
		quantity::sputter_time_t TST = C.sputter_time.max();
		if (TST > total_sputter_time() || !total_sputter_time().is_set())
			total_sputter_time_s= TST;
	}
	
	return total_sputter_time_s;
}

quantity::depth_t crater_t::total_sputter_depth()
{
	quantity::depth_t TSD;
	if (total_sputter_depths.is_set())
		TSD = total_sputter_depths;
	if (linescans.size()>0)
	{
		for (auto& LS : linescans)
		{
			if (LS.fit_params().gof()>0.5)
				TSD << LS.depth();
		}
	}
	if (TSD.is_set())
		return TSD.median();
	return {};
}

const quantity::quantity_t* crater_t::X() const
{
	const quantity::quantity_t* X;
	if (sputter_depth.is_set())
		return &sputter_depth;
	if (sputter_time.is_set())
		return &sputter_time;
	return nullptr;
}


// quantity::sputter_time_t crater_t::common_sputter_time(vector<cluster_t>& clusters)
quantity::quantity_t crater_t::common_X_quantity(const vector<quantity::quantity_t>& X_quantities)
{
	vector<double> mins;
	vector<double> maxs;	
	
	double resolution;
	
	/*asuming same units*/
	for (auto& ST : X_quantities)
	{
		mins.push_back(statistics::get_min_from_Y(ST.data()));
		maxs.push_back(statistics::get_max_from_Y(ST.data()));
	}
	quantity::quantity_t X(X_quantities.begin()->name(),X_quantities.begin()->unit());
	resolution = abs(X_quantities.begin()->data().front() - X_quantities.begin()->data().back()); 
	
	
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
	
	vector<double> X_data(common_dimension_data_size);
	for (int i=0;i<common_dimension_data_size;i++)
		X_data.at(i)=resolution*i+x_min;
	
	return {X,X_data};
}

quantity::depth_t crater_t::common_sputter_depth(vector<cluster_t>& clusters)
{
	if(clusters.size()==0)
	{
		logger::error("crater_t::common_sputter_depth()","clusters.size()==0","","returning empty");
		return {};
	}
	vector<quantity::quantity_t> sputter_depths; 
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
	return quantity::depth_t{common_X_quantity(sputter_depths)};
}

quantity::sputter_time_t crater_t::common_sputter_time(vector<cluster_t>& clusters)
{
	if(clusters.size()==0)
	{
		logger::error("crater_t::common_sputter_time","clusters.size()==0","","returning empty");
		return {};
	}
	vector<quantity::quantity_t> sputter_times; 
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
	return quantity::sputter_time_t{common_X_quantity(sputter_times)};
}

crater_t crater_t::change_resolution(quantity::depth_t sputter_depth_res)
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
	if (sputter_depth_res.data().size()==1 && sputter_depth_res.data().at(0)==0)
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

crater_t crater_t::change_resolution(quantity::sputter_time_t sputter_time_res)
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
	if (sputter_time_res.data().size()==1 && sputter_time_res.data().at(0)==0)
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

quantity::current_t& crater_t::sputter_current()
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
