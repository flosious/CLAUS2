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

#include <QTreeView>
#include <QStandardItem>
//#include "qcustomplot.h"
#include "sims_plotwindow_t.hpp"
#include "../src/measurement.hpp"
#include "../src/cluster.hpp"
#include "../src/log.hpp"
#include "../src/processor.hpp"

Q_DECLARE_METATYPE(quantity::quantity_t); // saved in Qt::UserRole+1; the copy of the original data (+2)
Q_DECLARE_METATYPE(quantity::quantity_t*); // saved in Qt::UserRole+2; the pointer to the original data
Q_DECLARE_METATYPE(int); //graph_id saved in Qt::UserRole+3

class sims_plot_cluster_tree_t : public QTreeView
{
    Q_OBJECT
private:
    ///columns; keep LAST in the row at last position for automated iteration
    enum class column_t{root, LAST};


    ///for the header
    static std::map<column_t,std::string> column_names;
    QWidget *parent = nullptr;
    class_logger_t logger;
    measurements_::sims_t* measurement = nullptr;
    QStandardItemModel* model;
    static void set_graph_id_in_item(int graph_id, QStandardItem *item);
    ///-1 on error
    static int get_graph_id_from_index(const QModelIndex& idx);
    ///true if index contains data (Qt::UserRole+1 && Qt::UserRole+2) set
    static bool index_has_data(const QModelIndex& idx);
    static quantity::quantity_t* get_quantity_in_item_from_Qindex(const QModelIndex& idx);
    ///the pointer to the originally quantity within its cluster
    static quantity::quantity_t* get_quantity_in_cluster_from_Qindex(const QModelIndex& idx);
    ///will populate item.setData with the quantities information in UserRole+1 and UserRole+2 within the 0th column
    static void set_quantity_in_QStandardItem(QStandardItem* quantity_item, quantity::quantity_t& quantity);
    template<typename quantity_or_cluster>
    QList<QStandardItem*> row(quantity_or_cluster& q_or_c)
    {
        QList<QStandardItem*> row_;
        int size=static_cast<int>(column_t::LAST);
        row_.reserve(size);
        for (int c=0; c<size;c++)
        {
            column_t col = static_cast<column_t>(c);
            row_ << item(q_or_c, col);
        }
        return row_;
    }
    QStandardItem* item(quantity::quantity_t& quantity, column_t col);
    QStandardItem* item(cluster_t& cluster, column_t col);

    sims_plotwindow_t* sims_plotwindow = nullptr;
    ///false if nullptr
    bool is_plot_window() const;
    ///false if nullptr
    bool is_measurement() const;
    void set_model();
    void update_sims_plotwindow();
    void graph_id_from_Qindex(const QModelIndex& idx);
    void set_header();
//    void add_quantity_to_plot(quantity::quantity_t* quantity);
//    void remove_quantity_from_plot(quantity::quantity_t* quantity);


public:
    sims_plot_cluster_tree_t(QWidget *parent = nullptr);

    void update();
    ///clears the graph; sets the measurement_pointer to NULL
    void unset_measurement();
    void set_sims_plotwindow(sims_plotwindow_t* sims_plotwindow_s);
public slots:
    ///set sims_plotwindow to new_measurement
    void set_measurement(measurements_::sims_t* new_measurement);
signals:
    ///updates MG_tab_measurement_plotwindow
//    void update_plotwindow();
};
#endif // SIMS_PLOT_MEASUREMENT_TREE_T_H
