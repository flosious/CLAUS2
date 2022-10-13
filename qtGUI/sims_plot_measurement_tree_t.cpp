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

std::map<sims_plot_measurement_tree_t::column_t,std::string> sims_plot_measurement_tree_t::column_names{
    {column_t::root,"cluster"}
};


void sims_plot_measurement_tree_t::set_graph_id_in_item(int graph_id, QStandardItem *item)
{
    QVariant variant_data;
    //copy of the quantity saved in "+1"
    variant_data.setValue(graph_id);
    item->setData(variant_data,Qt::UserRole+3);
    variant_data.clear();
}

int get_graph_id_from_item(const QModelIndex& idx)
{
    const int role = 3;
    if (!idx.data(Qt::UserRole+role).isValid())
            return -1;
    if (idx.data(Qt::UserRole+role).userType() != qMetaTypeId<int>())
        return false;
    return idx.data(Qt::UserRole+role).value<int>();
}

bool sims_plot_measurement_tree_t::index_has_data(const QModelIndex& idx)
{
    if (!idx.data(Qt::UserRole+1).isValid())
            return false;
    if (!idx.data(Qt::UserRole+2).isValid())
            return false;

    if (idx.data(Qt::UserRole+1).userType() != qMetaTypeId<quantity::quantity_t>())
        return false;
    if (idx.data(Qt::UserRole+2).userType() != qMetaTypeId<quantity::quantity_t*>())
        return false;

    return true;
}

quantity::quantity_t* sims_plot_measurement_tree_t::get_quantity_in_item_from_Qindex(const QModelIndex& idx)
{
    if (!index_has_data(idx))
        return nullptr;
    return idx.data(Qt::UserRole+1).value<quantity::quantity_t*>();
}

quantity::quantity_t* sims_plot_measurement_tree_t::get_quantity_in_cluster_from_Qindex(const QModelIndex& idx)
{
    if (!index_has_data(idx))
        return nullptr;
    return idx.data(Qt::UserRole+2).value<quantity::quantity_t*>();
}

void sims_plot_measurement_tree_t::set_quantity_in_QStandardItem(QStandardItem* quantity_item, quantity::quantity_t& quantity)
{
    QVariant variant_data;

    //copy of the quantity saved in "+1"
    variant_data.setValue(quantity);
    quantity_item->setData(variant_data,Qt::UserRole+1);

    variant_data.clear();

    //location of the original quantity saved in "+2"
    variant_data.setValue(&quantity);
    quantity_item->setData(variant_data, Qt::UserRole+2);
}

sims_plot_measurement_tree_t::sims_plot_measurement_tree_t(QWidget *parent) :
    parent(parent),
    logger(__FILE__,"sims_plot_measurement_tree_t"),
    model(new QStandardItemModel)
{
    set_model();
    set_header();
    update();
}

void sims_plot_measurement_tree_t::set_measurement(measurements_::sims_t* new_measurement)
{
    unset_measurement();
    if (new_measurement==nullptr)
        return;
    measurement = new_measurement;
    logger.debug(__func__,new_measurement->to_string_short()).enter();

    //set xAxis in sims_plotwindow
    if (measurement->crater.sputter_depth.is_set())
        sims_plot_window->set_xAxis_quantity(measurement->crater.sputter_depth);
    else if (measurement->crater.sputter_time.is_set())
        sims_plot_window->set_xAxis_quantity(measurement->crater.sputter_time);
    else
        logger.error(__func__,measurement->to_string_short()).value("no x axis in crater");

    //update this & sims_plot_window
    update();
}

QStandardItem* sims_plot_measurement_tree_t::item(quantity::quantity_t& quantity, column_t col)
{
    QStandardItem* quantity_item = new QStandardItem;
    switch (col)
    {
        case column_t::root:
        {
            quantity_item->setText(quantity.name().c_str());
            set_quantity_in_QStandardItem(quantity_item,quantity);
            int graph_id = sims_plot_window->add_Y_quantity(quantity);
            set_graph_id_in_item(graph_id,quantity_item);
            quantity_item->setEditable(false);
            quantity_item->setCheckable(true);
            quantity_item->setCheckState(Qt::CheckState::Checked);
            break;
        }
        case column_t::LAST:
        {
            logger.error(__func__,"column_t::LAST called").signal(quantity.to_string());
        }
    }

    return quantity_item;
}

QStandardItem* sims_plot_measurement_tree_t::item(cluster_t& cluster, column_t col)
{
    QStandardItem* cluster_item = new QStandardItem;
    switch (col)
    {
    case column_t::root:
        cluster_item->setText(cluster.to_string().c_str());
        if (cluster.intensity.is_set())
            cluster_item->appendRow(row(cluster.intensity));
        if (cluster.concentration.is_set())
            cluster_item->appendRow(row(cluster.concentration));
        cluster_item->setEditable(false);
        cluster_item->setCheckable(true);
        cluster_item->setCheckState(Qt::CheckState::Checked);
        break;
    case column_t::LAST:
        logger.error(__func__,"column_t::LAST called").signal(cluster.to_string());
    }
    return cluster_item;
}

bool sims_plot_measurement_tree_t::is_plot_window() const
{
    if (sims_plot_window==nullptr)
        return false;
    return true;
}

bool sims_plot_measurement_tree_t::is_measurement() const
{
    if (measurement==nullptr)
        return false;
    return true;
}

void sims_plot_measurement_tree_t::set_model()
{
    model->clear();
    setSelectionMode(QTreeView::SingleSelection);
    setModel(model);
}

void sims_plot_measurement_tree_t::set_header()
{
    QStringList list;
    int size = static_cast<int>(column_t::LAST);
    list.reserve(size);
    for (int i=0;i<size;i++)
    {
        column_t col = static_cast<column_t>(i);
        if (column_names.find(col)==column_names.end())
        {
            logger.fatal(__func__,"column_names").value(std::to_string(i),10,"col not found in column_names <"+std::to_string(size) +">");
            return;
        }
        list.push_back(QString(column_names[col].c_str()));
    }
    model->setHorizontalHeaderLabels(list);
}

void sims_plot_measurement_tree_t::unset_measurement()
{
    measurement = nullptr;
    ///remove all rows
    model->removeRows(0,model->rowCount());
}

void sims_plot_measurement_tree_t::set_sims_plot_window(sims_plotwindow_t* sims_plot_window_s)
{
    sims_plot_window = sims_plot_window_s;
}


void sims_plot_measurement_tree_t::update()
{
    if (!is_plot_window())
        return;
    if (!is_measurement())
        return;
//    clear();
    QList<QTreeWidgetItem*> cluster_items;
    cluster_items.reserve(measurement->clusters.size());
    for (auto& C : measurement->clusters)
    {
        model->appendRow(row(C));
    }
    expandAll();
    sims_plot_window->clearGraphs();
//    QCPAxisRect *wideAxisRect = new QCPAxisRect(sims_plot_window);
//    wideAxisRect->axis(QCPAxis::atLeft,1);
//    sims_plot_window->setInteraction(QCP::iRangeDrag,true);
//    sims_plot_window->setInteraction(QCP::iRangeZoom,true);
//    sims_plot_window->setInteraction(QCP::iSelectPlottables);
//    sims_plot_window->setSelectionRectMode(QCP::srmZoom);

//    sims_plot_window->clearGraphs();

    //each cluster_item in the treeWidget contains a pointer to its cluster_t

//    sims_plot_window->addGraph();


}
