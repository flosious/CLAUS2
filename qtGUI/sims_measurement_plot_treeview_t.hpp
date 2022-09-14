#ifndef SIMS_MEASUREMENT_PLOT_TREEVIEW_T_HPP
#define SIMS_MEASUREMENT_PLOT_TREEVIEW_T_HPP

#include <QTreeView>
#include "plotwindow.h"
#include "../src/measurement.hpp"
#include "../src/quantity.hpp"
#include "../src/log.hpp"

class sims_measurement_plot_treeview_t : public QTreeView
{
    Q_OBJECT
public:
    sims_measurement_plot_treeview_t();
    void set_plotwindow(plotwindow* window_s);
    void set_measurement(measurements_::sims_t* measurement_s);
    ///copys all the changes of measurement_copy to the original measurement pointer
    void apply_to_measurement();
protected:
    class_logger_t logger;
    QMenu* contextMenu;
    QWidget *parent;
    QStandardItemModel* model;
    measurements_::sims_t* measurement_copy;

    QStandardItem* cluster_quantity_item(quantity::quantity_t* cluster_quantity);
    QStandardItem* cluster_intensity_item(quantity::intensity_t* cluster_intensity);
    QStandardItem* cluster_concentration_item(quantity::concentration_t* cluster_concentration);
    QStandardItem* cluster_item(cluster_t* cluster);
    QStandardItem* reference_cluster(std::vector<cluster_t*> clusters);

    ///false if window is nullptr
    bool is_window_set() const;
    ///false if measurement is nullptr
    bool is_measurement_set() const;
private:
    plotwindow* window = nullptr;
    measurements_::sims_t* measurement = nullptr;;
    void set_model();
    void set_header();
    void set_actions();
    void set_contextMenu();
    void connect_signals_to_slots();

};

extern Logger global_logger;

#endif // SIMS_MEASUREMENT_PLOT_TREEVIEW_T_HPP
