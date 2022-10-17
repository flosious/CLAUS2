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
    static const std::map<int, QColor> iterate_colors;
    static QColor get_color_from_graph_id(int graph_id);
    class_logger_t class_logger;
    ///x-axis quantity; defines range of graph if value given
    quantity::quantity_t xAxis_quantity;
public:
//    enum class color_t {white,black,orange,};
    sims_plotwindow_t(QWidget *parent = nullptr);
    ///
    void set_xAxis_quantity(const quantity::quantity_t& X);
    ///x axis will be just the number of data points
    void unset_xAxis_quantity();
    ///returns the graph_id; -1 on error
    int add_Y_quantity(const quantity::quantity_t& Y);
};

#endif // SIMS_PLOTWINDOW_T_HPP
