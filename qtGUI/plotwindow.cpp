#include "plotwindow.h"

plotwindow::plotwindow(QWidget *parent) : QCustomPlot(parent)
{
    setInteraction(QCP::iRangeDrag,true);
    setInteraction(QCP::iRangeZoom,true);
    setInteraction(QCP::iSelectPlottables);
    setSelectionRectMode(QCP::srmZoom);
}

void plotwindow::plot(const measurements_::sims_t &M)
{

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
