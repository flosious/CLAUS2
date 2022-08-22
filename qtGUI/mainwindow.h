#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QEvent>
#include <QMouseEvent>
#include <QMimeData>
#include <QFileSystemModel>
#include <QStandardItemModel>
#include <QList>
#include "ui_mainwindow.h"
#include <string>
#include <iostream>
#include "../src/processor.hpp"
#include "../src/filenames_collector_t.hpp"
#include "../src/log.hpp"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    class_logger_t logger;

private slots:

    void on_button_files_to_measurements_clicked();
    void on_remove_files_duplicates_clicked();
    void on_measurements_treeView_clicked(const QModelIndex &index);

    void on_tab_log_warning_stateChanged(int arg1);

    void on_tab_log_info_stateChanged(int arg1);

    void on_tab_log_debug_stateChanged(int arg1);

    void on_tab_log_clear_button_clicked();

protected:
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *e) override;

public:
    Ui::MainWindow *ui;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


};

extern Logger global_logger;
extern processor *claus;
#endif // MAINWINDOW_H
