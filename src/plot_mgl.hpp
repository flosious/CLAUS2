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


using namespace std;

class plot_t : public mglDraw
{
private:
    class_logger_t logger;
	class axis_t
	{
    private:
	public:
		class range_t
		{
		private:
            class_logger_t logger;
			double backgound_minimum_c(const std::vector<const quantity::quantity_t*> Ys) const;
			double backgound_minimum_p;
		public:
			///asuming y axis starts at low values and grows to bigger (e.g. from 1 to 1000)
			range_t(const quantity::quantity_t* Ys);
			range_t(const std::vector<const quantity::quantity_t*> Ys);
			range_t(double start, double stop);
			///changes start and stop to log10 values
			range_t log10() const;
			double background_minimum() const;
			double start=-1;
			double stop=1;
			std::string to_string() const;
		};
	private:
		///defines a curve on an axis
		class points_t
		{
        private:
            class_logger_t logger;
		public:
			points_t(const quantity::map_t& XY, const std::string legende, const std::string color=" +");
			const quantity::map_t XY;
			const std::string color;
			const std::string legende="";
		};
		class line_t
		{
        private:
            class_logger_t logger;
		public:
			const double x_start;
			const double y_start;
			const double x_stop;
			const double y_stop;
			const std::string color;
			const std::string text;
			///adds an arrow from start --> stop
			line_t(double x_start, double y_start, double x_stop, double y_stop, std::string color="", std::string text="");
		};
		std::vector<line_t> lines;
		double start=-1;
		double stop=-1;
	public:
		///all the curves for this axis
		std::vector<points_t> curves;
		std::vector<points_t> points;
// 		std::vector<points_t> polynoms;
		///not working
		void add_polynom(const quantity::map_t& XY, const fit_functions::polynom_t& polynom_s, const std::string legend="", const std::string color="", const unsigned int  points=100);
		void add_points(const quantity::map_t& XY, const std::string legend="", const std::string color=" +");
		///e.g. "Intensity [c/s]"
		void add_points(const quantity::quantity_t& X, const quantity::quantity_t& Y, const std::string legend="", const std::string color=" +");
		///checks all curves for consistency (same physical quantities and units)
		bool check();
		///adds a new curve to this axis
		void add_curve(const quantity::map_t& XY, const std::string legend="");
		///Y without X, using index position as X
		void add_curve(const quantity::quantity_t& Y, const std::string legend="");
		void add_curve(const quantity::quantity_t& X, const quantity::quantity_t& Y, const std::string legend="");
		void add_curve(const std::vector<double>& X, const std::vector<double>& Y, const std::string legend="");
		///draws the axis to graph object
		void draw(mglGraph * gr, double x_origin=0);
		///set log10_scale usage to true
		bool log10_scale=false;
		std::string color="k";
		///returns the actual range for all curves for this axis
		range_t range();
		///sets a range for all curves for this axis
		void range(double start_s, double stop_s, bool log10_scale_s);
		
		void add_line(double x_start, double y_star, double x_stop, double y_stop, std::string color="", std::string text="");
		void add_arrow(double x_start, double y_star, double x_stop, double y_stop, std::string color="rA", std::string text="");
	}; // axis_t
	int Draw(mglGraph * gr) override;
public:
	class window_t
	{
	private:
	public:
		std::string title="";
		axis_t Y1;
		axis_t Y2;
		axis_t Y3;
		void draw(mglGraph * gr);
	}; //window_t
	plot_t(bool Y1_log10=true, bool Y2_log10=true, bool Y3_log10=false);
	std::vector<window_t> windows;
	axis_t Y1;
	axis_t Y2;
	axis_t Y3;

	void to_screen(const std::string window_title="", double sleep_sec=2);
	void to_file(const std::string filename);
};
extern Logger global_logger;
#endif //PLOT_T_HPP
