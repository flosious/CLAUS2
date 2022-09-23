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



#ifndef SIMS_PLOT_MEASUREMENT_TREE_T_H
#define SIMS_PLOT_MEASUREMENT_TREE_T_H

#include <QTreeWidget>
#include <QStandardItem>
//#include "qcustomplot.h"
#include "plotwindow_t.hpp"
#include "../src/measurement.hpp"
#include "../src/cluster.hpp"
#include "../src/log.hpp"

class sims_plot_measurement_tree_t : public QTreeWidget
{
    Q_OBJECT
private:
    ///columns; keep LAST in the row at last position for automated iteration
    enum column_t{cluster,LAST};
    QWidget *parent = nullptr;
    class_logger_t logger;
    measurements_::sims_t* measurement = nullptr;

    QStandardItem* tree_cluster_item(cluster_t* cluster);
    QStandardItem* tree_item(cluster_t* cluster, column_t col);
    QList<QStandardItem*> tree_cluster_row(cluster_t* cluster);
    plotwindow_t* sims_plot_window = nullptr;
    ///false if nullptr
    bool is_plot_window() const;
    ///false if nullptr
    bool is_measurement() const;
    void plot(const quantity::intensity_t& intensity, const quantity::sputter_time_t& sputter_time);
    void plot(const quantity::concentration_t& concentration, const quantity::sputter_time_t& sputter_time);
    void plot(const quantity::intensity_t& intensity, const quantity::sputter_depth_t& sputter_depth);
    void plot(const quantity::concentration_t& concentration, const quantity::sputter_depth_t& sputter_depth);
    void update_sims_plot_window();
public:
    sims_plot_measurement_tree_t(QWidget *parent = nullptr);
    ///clears the graph; sets the measurement_pointer to NULL
    void unset_measurement();
    void set_sims_plot_window(plotwindow_t* sims_plot_window_s);
public slots:
    ///set sims_plot_window to new_measurement
    void set_measurement(measurements_::sims_t* new_measurement);
signals:
    ///updates MG_tab_measurement_plotwindow
//    void update_plotwindow();
};

#endif // SIMS_PLOT_MEASUREMENT_TREE_T_H
