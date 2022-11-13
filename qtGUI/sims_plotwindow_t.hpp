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

#ifndef SIMS_PLOTWINDOW_T_HPP
#define SIMS_PLOTWINDOW_T_HPP

#include <map>
#include <QVector>
#include "qcustomplot.h"
#include "../src/measurement.hpp"
#include "../src/cluster.hpp"
#include "../src/quantity.hpp"
#include "../src/definitions.hpp"
/*
    should allow multiple measurements (each may differenct clusters in size)
    carying about:
        - line color + style settings
        - xAxis (sputter_time or sputter_depth)
*/
class sims_plotwindow_t : public QCustomPlot
{
private:
    class axis_t
    {
    protected:
        quantity::quantity_t quantity_p;
        void set_label();
        class_logger_t class_logger;
//        quantity::quantity_t range_minima;
//        quantity::quantity_t range_maxima;
        void set_quantity(const quantity::quantity_t& quantity_s);
    public:
        axis_t(QCPAxis *qcpAxis_s, const quantity::quantity_t& quantity_s={});
//        axis_t(const quantity::quantity_t& quantity_s, QCPAxisRect *rect);
        QCPAxis *qcpAxis = nullptr;
        bool change_unit(unit_t new_unit);

        void unset();
        void set_scale_linear();
        void set_scale_logarithmic();
        void set_range(quantity::quantity_t start, quantity::quantity_t stop);
        void rescale_to_full_multiple_of(double multiple=1);
        void set_visible(bool visibility);
        const quantity::quantity_t& quantity() const;
    };
public:
    class x_axis_t : public axis_t
    {
    public:
        enum class options{sputter_time,sputter_depth,sputter_points,LAST};
    private:
        options option = options::sputter_time;
    protected:
        sims_plotwindow_t* sims_plotwindow=nullptr;
        void update_quantity();
    public:

        x_axis_t(sims_plotwindow_t* sims_plotwindow,QCPAxis *qcpAxis_s, options option = options::sputter_time);
        quantity::quantity_t quantity_from_measurement(const measurements_::sims_t& measurement) const;
        quantity::quantity_t quantity_from_crater(const crater_t& crater) const;
        void change(options axis_option);
    };
private:
    static const std::map<int, QColor> iterate_colors;
    static QColor get_color_from_graph_id(int graph_id);

    class_logger_t class_logger;

    ///sets the axis to the given axisRect
    class axes_t
    {
    private:
        void set_x_axis_as_sputter_time();
        void set_intensity_axis();
        void set_concentration_abs_axis();
        void set_concentration_rel_axis();
    public:
        axes_t(QCPAxisRect *axisRect_s, sims_plotwindow_t *sims_plotwindow);
        axis_t intensity;
        axis_t concentration_abs;
        axis_t concentration_rel;
//        axis_t sputter_depth;
//        axis_t sputter_time;
        x_axis_t x;
    };
    axis_t* get_y_axis(const quantity::quantity_t& Y);
public:
    sims_plotwindow_t(QWidget *parent = nullptr);
    axes_t axes;
    ///returns the graph_id; -1 on error
    int add_graph(quantity::quantity_t X, quantity::quantity_t Y);
    int add_graph(const crater_t& crater, quantity::quantity_t Y);
    ///returns the graph_id; -1 on error
    int add_graph(const quantity::map_t& XY);
    ///removes the graph from the plot
    void del_graph(int graph_id);

    ///
//    void set_xAxis_quantity(const quantity::quantity_t& X);
    ///x axis will be just the number of data points
//    void unset_xAxis_quantity();
    ///returns the graph_id; -1 on error; distributes to axis depeding on quantitys dimension
//    int add_Y_quantity(const quantity::quantity_t& Y);

    /*this is new and untested*/


    ///returns graph_id
//    int add_to_relative_concentration_axis(const quantity::concentration_t& concentration);
    ///returns graph_id
//    int add_to_absolute_concentration_axis(const quantity::concentration_t& concentration);
    ///returns graph_id
//    int add_to_intensity_axis(const quantity::intensity_t& intensity);
};

#endif // SIMS_PLOTWINDOW_T_HPP
