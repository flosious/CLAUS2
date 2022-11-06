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



#ifndef PLOTWINDOW_T_H
#define PLOTWINDOW_T_H

#include "qcustomplot.h"
#include "../src/measurement.hpp"

class plotwindow_t : public QCustomPlot
{
private:
    class axis_t
    {
    private:
        bool log_scale=false;
    public:
        axis_t();
        ///change to logarithmic scale
        void set_log_scale();
        ///change to linear scale
        void set_lin_scale();
        int add_quantity(const quantity::quantity_t& q);
    };
//protected:
//    class axis_t
//    {
//    private:
//        std::string name_p;
//        unit_t unit_p;
//        quantity::quantity_t::dimension_t dimension_p;
//        QCPAxis *qcpAxis;
//    public:
//        axis_t(const std::string& name, const unit_t& unit, const quantity::quantity_t::dimension_t& dimension, QCPAxis *qcpAxis);
////        axis_t(const std::string& name, const unit_t& unit, QCPAxisRect *axisRect, AxisType axisType);
//        std::string name() const;
//        unit_t unit() const;
//        bool change_unit(const unit_t& new_unit);
//    };

//    QCPAxisRect *axisRect_p;
//    axis_t x;
public:
    plotwindow_t(QWidget *parent = nullptr);
    void plot(const measurements_::sims_t& M);
};

#endif // PLOTWINDOW_T_H
