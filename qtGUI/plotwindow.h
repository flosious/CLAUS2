#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include "qcustomplot.h"
#include "../src/measurement.hpp"

class plotwindow : public QCustomPlot
{
public:
    plotwindow(QWidget *parent = nullptr);
    void plot(const measurements_::sims_t& M);
};

#endif // PLOTWINDOW_H
