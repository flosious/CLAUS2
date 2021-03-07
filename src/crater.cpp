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

/*LINESCAN_T*/

linescan_t::linescan_t()
{
}

linescan_t::linescan_t(quantity_t xy, quantity_t z) : xy(xy),z(z)
{
}

fit_functions::asym2sig_t linescan_t::fit_params()
{
	return asym2sig;
}

bool linescan_t::is_set()
{
	if (!xy.is_set() || !z.is_set()) return false;
	return true;
}


quantity_t linescan_t::fit()
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

quantity_t linescan_t::depth()
{	
	if (!is_set()) return {};
	quantity_t fitted_z_profile = fit();

	double y0=asym2sig.fitted_y_data({0})[0];
	double m=(asym2sig.fitted_y_data({xy.data.back()})[0]-y0)/xy.data.back();
	quantity_t linescan_depth("total_sputter_depth",{},fitted_z_profile.unit());
	
	// fitted function value in the centre of the crater minus the valu at the same position on the (virtual) surface
	linescan_depth.data={abs(asym2sig.fitted_y_data({asym2sig.xc})[0] - (y0+m*asym2sig.xc))};
// 	linescan_depth.data={abs(asym2sig.fitted_y_data({asym2sig.xc})[0]-asym2sig.y0-asym2sig.m*asym2sig.xc)};
// 	linescan_depth.data={abs(asym2sig.fitted_y_data({asym2sig.xc})[0]-asym2sig.fitted_y_data({0})[0]-asym2sig.y0-asym2sig.m*asym2sig.xc)};
	
	return linescan_depth;
}

std::__cxx11::string linescan_t::to_string(std::__cxx11::string prefix)
{
	stringstream ss;
	ss << prefix;
	ss << std::scientific << depth().to_string() <<"; " << fit_params().to_string();
	return ss.str();
}

