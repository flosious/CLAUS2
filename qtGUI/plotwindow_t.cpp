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


#include "plotwindow_t.hpp"

plotwindow_t::plotwindow_t(QWidget *parent) : QCustomPlot(parent)
{
    setInteraction(QCP::iRangeDrag,true);
    setInteraction(QCP::iRangeZoom,true);
    setInteraction(QCP::iSelectPlottables);
    setSelectionRectMode(QCP::srmZoom);
}

void plotwindow_t::plot(const measurements_::sims_t &M)
{
    /*testing*/

    /****/
    clearGraphs();
    // create graph and assign data to it:
    addGraph();

    quantity::quantity_t X(M.crater.sputter_depth);
    if (!X.is_set())
    {
        X = M.crater.sputter_time;
    }
    if (!X.is_set())
        return;
    int g=0;

    for (auto& C : M.clusters)
    {
        if (!C.intensity.is_set()) continue;
        addGraph();
        auto xxx = X.data();
        auto yyy = C.intensity.data();
        graph(g)->setData(QVector<double>(xxx.begin(),xxx.end()), QVector<double>(yyy.begin(),yyy.end() ));
        xAxis->setLabel(X.name().c_str());
        yAxis->setLabel(C.intensity.name().c_str());
        g++;
    }
    xAxis->setRange(0,X.data().back());
    yAxis->setRange(1, 1E6);
    yAxis->setScaleType(QCPAxis::stLogarithmic);
    QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
    yAxis->setTicker(logTicker);
    replot();
}
