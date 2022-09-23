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

#include "sims_plot_measurement_tree_t.hpp"

Q_DECLARE_METATYPE(cluster_t*);

sims_plot_measurement_tree_t::sims_plot_measurement_tree_t(QWidget *parent) :
    parent(parent),logger(global_logger,__FILE__,"plot_sims_measurement_clusters_t")
{
}

QStandardItem* sims_plot_measurement_tree_t::tree_cluster_item(cluster_t* cluster)
{
    QStandardItem* cluster_item = new QStandardItem();
    QVariant variant_data;
    variant_data.setValue(cluster);
    cluster_item->setData(variant_data);
    cluster_item->setText(QString(cluster->to_string().c_str()));
    return cluster_item;
}

void sims_plot_measurement_tree_t::set_measurement(measurements_::sims_t* new_measurement)
{
    measurement = new_measurement;
    logger.debug(__func__,new_measurement->to_string_short()).enter();
    //update this & sims_plot_window
}

bool sims_plot_measurement_tree_t::is_measurement() const
{
    if (measurement==nullptr)
        return false;
    return true;
}

bool sims_plot_measurement_tree_t::is_plot_window() const
{
    if (sims_plot_window==nullptr)
        return false;
    return true;
}

void sims_plot_measurement_tree_t::set_sims_plot_window(plotwindow_t* sims_plot_window_s)
{
    sims_plot_window = sims_plot_window_s;
}


void sims_plot_measurement_tree_t::plot(const quantity::intensity_t& intensity, const quantity::sputter_time_t& sputter_time)
{
    if (!is_plot_window())
        return;
    QCPAxisRect *wideAxisRect = new QCPAxisRect(sims_plot_window);
    wideAxisRect->axis(QCPAxis::atLeft,1);
    sims_plot_window->setInteraction(QCP::iRangeDrag,true);
    sims_plot_window->setInteraction(QCP::iRangeZoom,true);
    sims_plot_window->setInteraction(QCP::iSelectPlottables);
    sims_plot_window->setSelectionRectMode(QCP::srmZoom);

    sims_plot_window->clearGraphs();

    //each cluster_item in the treeWidget contains a pointer to its cluster_t

    sims_plot_window->addGraph();


}
