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
public:
    plotwindow_t(QWidget *parent = nullptr);
    void plot(const measurements_::sims_t& M);
};

#endif // PLOTWINDOW_T_H
