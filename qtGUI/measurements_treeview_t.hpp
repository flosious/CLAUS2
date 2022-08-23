#ifndef MEASUREMENTS_TREEVIEW_T_HPP
#define MEASUREMENTS_TREEVIEW_T_HPP

#include <iostream>
#include <QTreeView>
#include <QKeyEvent>

#include <QMainWindow>
#include <QEvent>
#include <QMouseEvent>
#include <QMimeData>
#include <QFileSystemModel>
#include <QStandardItemModel>
#include <QList>
#include <string>
#include <iostream>
#include "../src/processor.hpp"
#include "../src/filenames_collector_t.hpp"
#include "qcustomplot.h"

class measurements_treeview_t : public QTreeView
{
    Q_OBJECT
private:
    class_logger_t logger;
    ///this is the order of parent points in the treeview; leave "LAST" at last position
    enum methods {tofsims, dsims, dektak6m, p17, xps, LAST};
    static std::map<methods,std::string> method_names;

    class parent_entry_t
    {
    public:
        ///returns a vector of row indices
        std::vector<unsigned int> get_selected_rows() const;
    protected:
        parent_entry_t(methods method_id, measurements_treeview_t *measurements_treeview);
        ///transform a string matrix to a proper items matrix useable by QtModels
        std::vector<QStandardItem*> itemCols(const std::vector<std::vector<std::string>>& lines_and_cols);
        methods method_id;
        ///the current model used in treeview
        measurements_treeview_t *measurements_treeview=nullptr;
    };
    class tofsims_t : public parent_entry_t
    {
    private:
        class_logger_t logger;
    public:
        tofsims_t(measurements_treeview_t *measurements_treeview);
        std::vector<QStandardItem*> itemCols();
    };
    class dsims_t : public parent_entry_t
    {
    private:
        class_logger_t logger;
    public:
        dsims_t(measurements_treeview_t *measurements_treeview);
        std::vector<QStandardItem*> itemCols();
    };
    QStandardItemModel* model;
protected:
    int total_lines_count() const;
    void keyPressEvent(QKeyEvent *e) override;
//    void dragMoveEvent(QDragMoveEvent *e) override;
//    void dragEnterEvent(QDragEnterEvent *e) override;
//    void dropEvent(QDropEvent *e) override; //somehow this is not recognized :(
    ///sets the model
    void createModel();
    void update(methods method_id, const std::vector<QStandardItem*>& itemCols);
    QWidget *parent;
    QCustomPlot *measurement_plot_window;
public:
    measurements_treeview_t(QWidget *parent = nullptr);
    tofsims_t tofsims_entries();
    dsims_t dsims_entries();
    void set_plot_window(QCustomPlot *measurement_plot_window_s);
//    parent_entry_t dsims_entries();
//    parent_entry_t xps_entries();
//    parent_entry_t dektak6m_entries();
//    parent_entry_t p17_entries();
//    parent_entry_t camera_images_entries();
    ///will reload from processor: klaus()->"method"->measurements
public slots:
    void update();
};
extern processor *claus;
extern Logger global_logger;
#endif // MEASUREMENTS_TREEVIEW_T_HPP
