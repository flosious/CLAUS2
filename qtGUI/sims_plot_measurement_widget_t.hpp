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

#ifndef SIMS_PLOT_MEASUREMENT_WIDGET_T_H
#define SIMS_PLOT_MEASUREMENT_WIDGET_T_H

#include <QWidget>
#include "ui_sims_plot_measurement_widget_t.h"
#include "../src/measurement.hpp"
#include "../src/log.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class sims_plot_measurement_widget_t;
}
QT_END_NAMESPACE

class sims_plot_measurement_widget_t : public QWidget
{
    Q_OBJECT
public:
    explicit sims_plot_measurement_widget_t(QWidget *parent = nullptr);
    ~sims_plot_measurement_widget_t();
//    void set_measurement(measurements_::sims_t* new_measurement);
    void update(measurements_::sims_t* new_measurement);
public slots:
    void on_clusters_button_clicked();
signals:
    void auto_calc();
private:
    Ui::sims_plot_measurement_widget_t *ui;
    class_logger_t logger;
};



#endif // SIMS_PLOT_MEASUREMENT_WIDGET_T_H
