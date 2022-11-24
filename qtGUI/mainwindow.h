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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../src/definitions.hpp"
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
#include "qobject.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE



class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    class_logger_t class_logger;
//    QProgressBar *files_loading_progress_bar;
private slots:

    void on_button_files_to_measurements_clicked();
    void on_remove_files_duplicates_clicked();
    void on_measurements_treeView_clicked(const QModelIndex &index);

    void on_tab_log_warning_stateChanged(int arg1);

    void on_tab_log_info_stateChanged(int arg1);

    void on_tab_log_debug_stateChanged(int arg1);

    void on_tab_log_clear_button_clicked();
    void on_files_treeView_customContextMenuRequested(const QPoint &pos);
    void on_measurements_treeView_customContextMenuRequested(const QPoint &pos);

    void on_mgroups_treeview_clicked(const QModelIndex &index);

    void on_mgroups_treeview_customContextMenuRequested(const QPoint &pos);

    void on_tabWidget_currentChanged(int index);

    void on_calc_button_clicked();

    void on_export_button_clicked();

    void on_check_pbp_stateChanged(int arg1);


    void on_test_button_clicked();

protected:
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
public:
    Ui::MainWindow *ui;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:


};

extern processor *claus;
#endif // MAINWINDOW_H
