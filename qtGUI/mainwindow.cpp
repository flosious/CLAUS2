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


#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      logger(global_logger,__FILE__,"MainWindow")
{
    logger.debug(__func__,"MainWindow").enter();
    ui->setupUi(this);
    setAcceptDrops(true);

    ui->files_treeView->update();
    ui->measurements_treeView->update();

    QObject::connect(ui->files_treeView, SIGNAL(update_measurements_treeview()), ui->measurements_treeView, SLOT(update()));

    logger.debug(__func__,"MainWindow").exit();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls())
    {
        e->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *e)
{
    logger.debug(__func__,"this").enter();
    std::vector<std::string> dropped_files;
    const QMimeData* mimeData = e->mimeData();
    if (mimeData->hasUrls())
    {
        QList<QUrl> urlList = mimeData->urls();
        dropped_files.reserve(urlList.size());
        for (int i=0; i < urlList.size() ; i++)
        {
              dropped_files.push_back(urlList.at(i).toString().toStdString());
        }

        for (auto& L : dropped_files)
        {
            // file:///tmp/systemd-private-0ecda464d0ec48abbd82c45271cbe11f-systemd-logind.service-yYlCij
            tools::str::remove_substring_from_mainstring(&L,"file://");
            tools::str::remove_substring_from_mainstring(&L,"FILE://");
            tools::str::replace_chars(&L,"%23","#");
            logger.debug(__func__,"dropped_files").value(L,11);
        }
        //collecting and filtering
        filenames_collector_t filenames_collector;
        filenames_collector.add_files_or_folders(dropped_files,ui->scan_dropped_files_recursivly->isChecked());
        ///all filenames are unknown in the beginning
        const auto new_filenames = filenames_collector.filenames();

        int next_tof_file_idx = claus->tofsims.files.size();
        int next_dsims_file_idx = claus->dsims.files.size();
        int next_dektak6m_file_idx = claus->dektak6m.files.size();
        claus->parse_filenames(new_filenames);
        //unknown + tofsims + dsims + camera(blder) + xps + dektak6m + p17 + ...

        if (ui->tabWidget->currentIndex()==1) //measurement tab
        {
            logger.debug(__func__,"measurement Tab").enter();
            vector<unsigned int> indices;
            //tofsims
            for (int i = next_tof_file_idx; i<claus->tofsims.files.size(); i++)
            {

                auto& F = claus->tofsims.files.at(i);
                logger.info(__func__,"measurement Tab").value(F.name.filename(),10,"tofsims");
                claus->tofsims.add_file_to_measurement(F);
                indices.push_back(i);
            }
            tools::vec::erase(claus->tofsims.files,indices);

            indices.clear();
            //dsims
            for (int i = next_dsims_file_idx; i<claus->dsims.files.size(); i++)
            {
                auto& F = claus->dsims.files.at(i);
                logger.info(__func__,"measurement Tab").value(F.name.filename(),10,"dsims");
                claus->dsims.add_file_to_measurement(F);
                indices.push_back(i);
            }
            tools::vec::erase(claus->dsims.files,indices);

            indices.clear();
            //dektak6m
            for (int i = next_dektak6m_file_idx; i<claus->dektak6m.files.size(); i++)
            {
                auto& F = claus->dektak6m.files.at(i);
                logger.info(__func__,"measurement Tab").value(F.name.filename(),10,"dektak6m");
                claus->dektak6m.add_file_to_measurement(F);
                indices.push_back(i);
            }
            tools::vec::erase(claus->dektak6m.files,indices);

            logger.debug(__func__,"measurement Tab").exit();
        }
        ui->measurements_treeView->update();
        ui->files_treeView->update();
        //aishu special treatment
        if (claus->aishu.files.size()>0)
        {
            logger.debug(__func__,"aishu.files").enter();
            for (auto& af : claus->aishu.files)
            {
                af.export_to_aishu_format();
            }
            logger.debug(__func__,"aishu.files").exit();
        }
        logger.debug(__func__,"this").exit();
        //focus tab to files_treeView
//        ui->tabWidget->setCurrentWidget(ui->tab_files);
//        ui->tabWidget->setCurrentIndex(0);

    }
}

void MainWindow::on_button_files_to_measurements_clicked()
{
    logger.debug(__func__,"this").enter();
    ui->files_treeView->selections_to_measurements();
    logger.debug(__func__,"this").exit();
}

void MainWindow::on_remove_files_duplicates_clicked()
{
// 	std::vector<files_::tofsims_t> TF_copy = claus->tofsims.files;
    std::set<files_::tofsims_t> TT(claus->tofsims.files.begin(),claus->tofsims.files.end());
    claus->tofsims.files = {TT.begin(),TT.end()};

    std::set<files_::dsims_t> DT(claus->dsims.files.begin(),claus->dsims.files.end());
    claus->dsims.files = {DT.begin(),DT.end()};

    std::set<files_::aishu_t> AF(claus->aishu.files.begin(),claus->aishu.files.end());
    claus->aishu.files = {AF.begin(),AF.end()};

    std::set<std::string> FF(claus->unknown_filenames.begin(),claus->unknown_filenames.end());
    claus->unknown_filenames = {FF.begin(),FF.end()};

    ui->files_treeView->update();
}

void MainWindow::on_measurements_treeView_clicked(const QModelIndex &index)
{
    logger.debug(__func__,"this").value("row=" + std::to_string( index.row() ) + "; column=" + std::to_string( index.column() ));

    if (ui->measurements_treeView->dsims_entries().get_selected_rows().size()>0)
    {
        ui->measurement_plot_window->clearGraphs();
        auto& M = claus->dsims.measurements.at(ui->measurements_treeView->dsims_entries().get_selected_rows().back());
        ui->measurement_plot_window->plot(M);
    }

    if (ui->measurements_treeView->tofsims_entries().get_selected_rows().size()>0)
    {
        ui->measurement_plot_window->clearGraphs();
        auto& M = claus->tofsims.measurements.at(ui->measurements_treeView->tofsims_entries().get_selected_rows().back());
        ui->measurement_plot_window->plot(M);
    }
}

void MainWindow::on_tab_log_warning_stateChanged(int arg1)
{
    if (global_logger==nullptr)
        return;
    if (arg1==0)
        global_logger->set_print_warning(false);
    else if (arg1==2)
        global_logger->set_print_warning(true);
    logger.debug(__func__,"set_print_warning").value(std::to_string(arg1));
}

void MainWindow::on_tab_log_info_stateChanged(int arg1)
{
    if (global_logger==nullptr)
        return;
    if (arg1==0)
        global_logger->set_print_info(false);
    else if (arg1==2)
        global_logger->set_print_info(true);
    logger.debug(__func__,"set_print_info").value(std::to_string(arg1));
}

void MainWindow::on_tab_log_debug_stateChanged(int arg1)
{
    if (global_logger==nullptr)
        return;
    if (arg1==0)
        global_logger->set_print_debug(false);
    else if (arg1==2)
        global_logger->set_print_debug(true);
    logger.debug(__func__,"set_print_debug").value(std::to_string(arg1));
}

void MainWindow::on_tab_log_clear_button_clicked()
{
    ui->tab_log_table->clearContents();
    ui->tab_log_table->setRowCount(0);
    logger.clear_messages();
}

void MainWindow::on_files_treeView_customContextMenuRequested(const QPoint &pos)
{
    logger.debug(__func__,"this").enter();
    QModelIndex index = ui->files_treeView->indexAt(pos);
    if (index.isValid())
    {
        logger.debug(__func__,"pos").value("x="+std::to_string(pos.x()) + "; y="+std::to_string(pos.y()));
        ui->files_treeView->contextMenu->exec(ui->files_treeView->viewport()->mapToGlobal(pos));
    }
}
