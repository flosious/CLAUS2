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

#ifndef PLOT_T_HPP
#define PLOT_T_HPP


#include <string>

#include <math.h>
#include "tools.hpp"
#include "quantity.hpp"
#include "cluster.hpp"
// #include "measurement.hpp"

using namespace std;

class plot_t : public mglDraw
{
private:
	
	class axis_t
	{
	public:
		class range_t
		{
		private:
			
		public:
			///asuming y axis starts at low values and grows to bigger (e.g. from 1 to 1000)
			range_t(const quantity::quantity_t* Ys);
			range_t(const vector<const quantity::quantity_t*> Ys);
			range_t(double start, double stop);
			///changes start and stop to log10 values
			range_t log10() const;
			double start=-1;
			double stop=1;
			string to_string() const;
		};
	private:
		///defines a curve on an axis
		class points_t
		{
		public:
			points_t(const quantity::map_t& XY, const string legende, const string color=" +");
// 			points_t(const quantity::quantity_t* X,const quantity::quantity_t* Y, const string legende, const string color=" +");
			const quantity::map_t XY;
// 			const quantity::quantity_t X;
// 			const quantity::quantity_t Y;
			const string color;
			const string legende="";
		};
		class line_t
		{
		public:
			const double x_start;
			const double y_start;
			const double x_stop;
			const double y_stop;
			const string color;
			const string text;
			///adds an arrow from start --> stop
			line_t(double x_start, double y_start, double x_stop, double y_stop, string color="", string text="");
		};
		vector<line_t> lines;
		double start=-1;
		double stop=-1;
		///prefered output units
// 		static string unit_string(const unit_t& unit);
// 		static string unit_string(quantity::quantity_t& quantity);
	public:
		///all the curves for this axis
		vector<points_t> curves;
		vector<points_t> points;
// 		vector<points_t> polynoms;
		///not working
		void add_polynom(const quantity::map_t& XY, const fit_functions::polynom_t& polynom_s, const string legend="", const string color="", const unsigned int  points=100);
		void add_points(const quantity::map_t& XY, const string legend="", const string color=" +");
		///e.g. "Intensity [c/s]"
		void add_points(const quantity::quantity_t& X, const quantity::quantity_t& Y, const string legend="", const string color=" +");
		///checks all curves for consistency (same physical quantities and units)
		bool check();
		///adds a new curve to this axis
		void add_curve(const quantity::map_t& XY, const string legend="");
		///Y without X, using index position as X
		void add_curve(const quantity::quantity_t& Y, const string legend="");
		void add_curve(const quantity::quantity_t& X, const quantity::quantity_t& Y, const string legend="");
		void add_curve(const vector<double>& X, const vector<double>& Y, const string legend="");
		///draws the axis to graph object
		void draw(mglGraph * gr, double x_origin=0);
		///set log10_scale usage to true
		bool log10_scale=false;
		string color="k";
		///returns the actual range for all curves for this axis
		range_t range();
		///sets a range for all curves for this axis
		void range(double start_s, double stop_s, bool log10_scale_s);
		
		void add_line(double x_start, double y_star, double x_stop, double y_stop, string color="", string text="");
		void add_arrow(double x_start, double y_star, double x_stop, double y_stop, string color="rA", string text="");
	}; // axis_t
	int Draw(mglGraph * gr) override;
public:
	class window_t
	{
	private:
	public:
		string title="";
		axis_t Y1;
		axis_t Y2;
		axis_t Y3;
		void draw(mglGraph * gr);
	}; //window_t
	plot_t(bool Y1_log10=true, bool Y2_log10=true, bool Y3_log10=false);
	vector<window_t> windows;
	axis_t Y1;
	axis_t Y2;
	axis_t Y3;
// 	plot_t(const quantity::quantity_t& X, const quantity::quantity_t& Y, string legend);
// 	plot_t(const quantity::quantity_t& X, const quantity::quantity_t& Y1, string Y1_legend, const quantity::quantity_t& Y2, string Y2_legend);
// 	plot_t(const quantity::quantity_t& X, vector<pair<const quantity::quantity_t*,string>> Y1_quantities_to_legends, vector<pair<const quantity::quantity_t*,string>> Y2_quantities_to_legends);
	
	void to_screen(const string window_title="", double sleep_sec=2);
	void to_file(const string filename);
	///concentrations/intensities vs depth
// 	static plot_t depth_profiles(const vector<const measurements_::sims_t*> Ms);
	///concentrations/intensities vs time
// 	static plot_t time_profiles(const vector<const measurements_::sims_t*> Ms);
	///tries to plot depth_profiles and then time_profiles
// 	static plot_t measurement(const measurements_::sims_t& M);
};

#endif //PLOT_T_HPP
