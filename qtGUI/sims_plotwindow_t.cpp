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

#include "sims_plotwindow_t.hpp"

const std::map<int, QColor> sims_plotwindow_t::iterate_colors
{
    { 0,QColor(0,        0,      0)}, //black

    { 1,QColor(255,      0,      0)}, //red
    { 2,QColor(0,        255,    0)}, //green
    { 3,QColor(0,        0,      255)}, //blue

    { 4,QColor(255,      255,    0)},
    { 5,QColor(0,        255,    255)},
    { 6,QColor(255,        0,    255)},

    { 7,QColor(255/2,    255/1,  255/2)},
    { 8,QColor(255/2,    255/2,  255/1)},
    { 9,QColor(255/1,    255/2,  255/2)}
};

sims_plotwindow_t::sims_plotwindow_t(QWidget *parent) : logger(__FILE__,"sims_plotwindow_t")
{
//    axisRect()->axis(QCPAxis::atLeft, 1);
}

QColor sims_plotwindow_t::get_color_from_graph_id(int graph_id)
{
    return iterate_colors.at(graph_id%10);
}

void sims_plotwindow_t::set_xAxis_quantity(const quantity::quantity_t& X)
{
    if (X.is_set())
    {
        xAxis_quantity = X;
        xAxis->setLabel(X.name().c_str());
        xAxis->setRange(0,X.data().back());
    }
}


int sims_plotwindow_t::add_Y_quantity(const quantity::quantity_t& Y)
{
    // intensity axis
    yAxis->setScaleType(QCPAxis::stLogarithmic);
    yAxis->setRange(1, 1E6);
    QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
    yAxis->setTicker(logTicker);
    // concentration axis
    yAxis2->setScaleType(QCPAxis::stLogarithmic);

    int graph_id = -1;
    if (!Y.is_set() )
    {
        return -1;
    }
    if (!xAxis_quantity.is_set())
    {
        xAxis_quantity = quantity::quantity_t("#",Y.data_X_1D(),units::SI::one); // get the index positions, that means a vector of 0,1,2,3,4,5,6,...,size()-1
    }
    if (xAxis_quantity.data().size() != Y.data().size())
    {
        return -1;
    }
    ///intensity?
    if (Y.dimension() == quantity::intensity_t().dimension())
    {
        addGraph(xAxis, yAxis); // left + bottom
        graph_id = graphCount()-1;
//        graph(graph_id)->
//        graph(graph_id)->setPen(QColor(50, 50, 50, 255));
//        graph(graph_id)->setLineStyle(QCPGraph::lsNone);
//        graph(graph_id)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
//        graph(graph_id)->setName(Y.name().c_str());
//        graph(graph_id)->setData(QVector<double>::fromStdVector(xAxis_quantity.data()),QVector<double>::fromStdVector(Y.data()));
//        graph(graph_id)->rescaleAxes();
    }
    ///concentration?
    else if (Y.dimension() == quantity::concentration_t().dimension())
    {
        addGraph(xAxis, yAxis2); // right + bottom
        graph_id = graphCount()-1;
//        graph(graph_id)->setPen(QColor(50, 50, 50, 255));
//        graph(graph_id)->setLineStyle(QCPGraph::lsNone);
//        graph(graph_id)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
//        graph(graph_id)->setName(Y.name().c_str());
//        graph(graph_id)->setData(QVector<double>::fromStdVector(xAxis_quantity.data()),QVector<double>::fromStdVector(Y.data()));
//        graph(graph_id)->rescaleAxes();
    }
    logger.debug(__func__,Y.to_string_short()).value("graph_id="+ std::to_string(graph_id));
    if (graph_id>=0)
    {
        graph(graph_id)->setName(Y.name().c_str());
        graph(graph_id)->setData(QVector<double>::fromStdVector(xAxis_quantity.data()), QVector<double>::fromStdVector(Y.data()) );
        graph(graph_id)->setPen(get_color_from_graph_id(graph_id));
        replot();
    }
    return graph_id;
}
