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
    ui->mgroups_treeview->update();

    QObject::connect(ui->files_treeView, SIGNAL(update_measurements_treeview()), ui->measurements_treeView, SLOT(update()));
    QObject::connect(ui->measurements_treeView, SIGNAL(update_mgroups_treeview()), ui->mgroups_treeview, SLOT(update()));
    QObject::connect(ui->mgroups_treeview, SIGNAL(update_measurements_treeview()), ui->measurements_treeView, SLOT(update()));
    QObject::connect(ui->sims_plot_measurement_widget, SIGNAL(auto_calc()),this, SLOT(auto_calc()) );
//    QObject::connect(ui->mgroups_treeview, SIGNAL(set_measurement_in_sims_plot_measurement_tree),ui->sims_plot_measurement_widget,SLOT(set_measurement));

//    ui->mgroups_treeview->header()->setStretchLastSection(false);
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
    ui->tofsims_export_directory_textEdit->setText(QString(""));
    ui->dsims_export_directory_textEdit->setText(QString(""));
#endif
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
//            dropped_files.push_back(urlList.at(i).toString().toStdString());
            dropped_files.push_back(urlList.at(i).toString().toStdString());
        }

        for (auto& L : dropped_files)
        {
            // file:///tmp/systemd-private-0ecda464d0ec48abbd82c45271cbe11f-systemd-logind.service-yYlCij
            tools::str::remove_substring_from_mainstring(&L,"file:");
            tools::str::remove_substring_from_mainstring(&L,"FILE:");
            tools::str::replace_chars(&L,"%23","#");
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
// there is a problem in recognizing the "u" (mikron letter); also olcdb is not parsed correctly (returns -1)
            tools::str::replace_chars(&L,"///","");
            //qt5 automatically replaces the windows style path delimiter with the unix path delimiter, lets change it back
            tools::str::replace_chars(&L,"/","\\"); // unix to windows style
            std::stringstream mu;
            mu << static_cast<char>(230);
            tools::str::replace_chars(&L,"µ",mu.str());
//            tools::str::replace_umlaute_to_windows_compatible(&L);
#endif
            logger.debug(__func__,"dropped_files").value(L,11);
        }
        //collecting and filtering
        filenames_collector_t filenames_collector;
        filenames_collector.add_files_or_folders(dropped_files,ui->scan_dropped_files_recursivly->isChecked());
        ///all filenames are unknown in the beginning
        const auto new_filenames = filenames_collector.filenames();
        logger.debug(__func__,"new_filenames").value(new_filenames.size(),10,tools::vec::combine_vec_to_string(new_filenames, ", "));
        int next_tof_file_idx = claus->tofsims.files.size();
        int next_dsims_file_idx = claus->dsims.files.size();
        int next_dektak6m_file_idx = claus->dektak6m.files.size();

        int dsims_measurements_size_before_adding = claus->dsims.measurements.size();
        int tofsims_measurements_size_before_adding = claus->tofsims.measurements.size();

        claus->parse_filenames(new_filenames);
        //unknown + tofsims + dsims + camera(blder) + xps + dektak6m + p17 + ...

        if (ui->tabWidget->currentIndex()==1 || ui->tabWidget->currentIndex()==2) //measurement tab or mgroup tab
        {
//            func_logger_t f_logger(logger,"tab: "+std::to_string(ui->tabWidget->currentIndex()));
//            auto flogger = logger.debug(__func__,"tab: "+std::to_string(ui->tabWidget->currentIndex()));
            std::string log_object("tab: " + std::to_string(ui->tabWidget->currentIndex()));
            logger.debug(__func__,log_object).enter();
            std::vector<unsigned int> indices;
            //tofsims
            for (int i = next_tof_file_idx; i<claus->tofsims.files.size(); i++)
            {

                auto& F = claus->tofsims.files.at(i);
                logger.info(__func__,log_object).value(F.name.filename(),10,"tofsims");
                claus->tofsims.add_file_to_measurement(F);
                indices.push_back(i);
            }
            tools::vec::erase(claus->tofsims.files,indices);

            indices.clear();
            //dsims
            for (int i = next_dsims_file_idx; i<claus->dsims.files.size(); i++)
            {
                auto& F = claus->dsims.files.at(i);
                logger.info(__func__,log_object).value(F.name.filename(),10,"dsims");
                claus->dsims.add_file_to_measurement(F);
                indices.push_back(i);
            }
            tools::vec::erase(claus->dsims.files,indices);

            indices.clear();
            //dektak6m
            for (int i = next_dektak6m_file_idx; i<claus->dektak6m.files.size(); i++)
            {
                auto& F = claus->dektak6m.files.at(i);
                logger.info(__func__,log_object).value(F.name.filename(),10,"dektak6m");
                claus->dektak6m.add_file_to_measurement(F);
                indices.push_back(i);
            }
            tools::vec::erase(claus->dektak6m.files,indices);

            //auto group newly added items, but not the old ones
            if (ui->tabWidget->currentIndex()==2)
            {
                std::vector<unsigned int> measurement_indices;
                //dsims
                measurement_indices.resize(claus->dsims.measurements.size()-dsims_measurements_size_before_adding);
                std::iota(measurement_indices.begin(),measurement_indices.end(),dsims_measurements_size_before_adding);
                claus->dsims.auto_group_measurements(measurement_indices);

                measurement_indices.clear();
                //tofsims
                measurement_indices.resize(claus->tofsims.measurements.size()-tofsims_measurements_size_before_adding);
                std::iota(measurement_indices.begin(),measurement_indices.end(),tofsims_measurements_size_before_adding);
                claus->tofsims.auto_group_measurements(measurement_indices);

//                ui->mgroups_treeview->resizeColumns();
            }
            logger.debug(__func__,log_object).exit();
        }

        ui->measurements_treeView->update();
        ui->files_treeView->update();
        ui->mgroups_treeview->update();

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
    else
        logger.error(__func__,"dropped file").value("has no URL");
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
        ui->tab_log_table->set_print_warning(false);
    else if (arg1==2)
        ui->tab_log_table->set_print_warning(true);
    ui->tab_log_table->update(*global_logger);
    logger.debug(__func__,"set_print_warning").value(std::to_string(arg1));
}

void MainWindow::on_tab_log_info_stateChanged(int arg1)
{
    if (global_logger==nullptr)
        return;
    if (arg1==0)
        ui->tab_log_table->set_print_info(false);
    else if (arg1==2)
        ui->tab_log_table->set_print_info(true);
    ui->tab_log_table->update(*global_logger);
    logger.debug(__func__,"set_print_info").value(std::to_string(arg1));
}

void MainWindow::on_tab_log_debug_stateChanged(int arg1)
{
    if (global_logger==nullptr)
        return;

    if (arg1==0)
        ui->tab_log_table->set_print_debug(false);
    else if (arg1==2)
        ui->tab_log_table->set_print_debug(true);
    ui->tab_log_table->update(*global_logger);
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

void MainWindow::on_measurements_treeView_customContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = ui->measurements_treeView->indexAt(pos);
    if (index.isValid())
    {
        logger.debug(__func__,"pos").value("x="+std::to_string(pos.x()) + "; y="+std::to_string(pos.y()));
        ui->measurements_treeView->contextMenu->exec(ui->measurements_treeView->viewport()->mapToGlobal(pos));
    }
}

void MainWindow::on_mgroups_treeview_clicked(const QModelIndex &index)
{
    measurements_::sims_t* M = nullptr;

    M = ui->mgroups_treeview->dsims_section.get_measurement_from_QIndex(index);
    if (M!=nullptr)
    {
        ui->sims_plot_measurement_widget->update(M);
        return;
    }

    M = ui->mgroups_treeview->tofsims_section.get_measurement_from_QIndex(index);
    if (M!=nullptr)
    {
        ui->sims_plot_measurement_widget->update(M);
        return;
    }

}

void MainWindow::on_mgroups_treeview_customContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = ui->mgroups_treeview->indexAt(pos);
    if (index.isValid())
    {
//        logger.debug(__func__,"pos").value("x="+std::to_string(pos.x()) + "; y="+std::to_string(pos.y()));
        ui->mgroups_treeview->contextMenu->exec(ui->mgroups_treeview->viewport()->mapToGlobal(pos));
    }
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if (index==7) // log tab
    {
        logger.debug(__func__,"this").enter();
        ui->tab_log_table->update(*global_logger);
    }
}

void MainWindow::auto_calc()
{
    /*tof sims*/
    for (auto& MG : claus->tofsims.mgroups)
    {
        if (MG.measurements_p.size()==0)
            continue;
        MG.set_reference_isotopes_in_measurements();
        MG.set_natural_abundances_in_matrix_clusters();
        auto calc = MG.calc();
        calc.matrices.median_const_from_reference_isotopes();
        calc.SRs.from_crater_depths().SRs.from_implant_max();
        if (MG.matrix_clusters().size()>1)
        {
            calc_t::sims_t::matrix_t mat(MG.matrix_isotopes(),MG.measurements_copy());
            auto RSFs = mat.RSFs().add_natural_abundances().remove_RSFs_below_gof_treshold(0.2).symmetrical_RSFs();
//            RSFs.plot_now(0);
            for (auto& M:MG.measurements())
            {
                calc_t::sims_t::matrix_t::concentration_c Concentration(RSFs,*M);
                const auto M_with_Cs = Concentration.concentrations_by_RSFs().concentrations_by_filling_up_all_concentrations_to_1().measurement();
                ///copy the results - this is really ugly
                for (auto C : M_with_Cs.clusters)
                {
                    if (!C.concentration.is_set())
                        continue;
                    *M->cluster(C) = C;
                }
            }
        }
        // SR + SD
        calc.SRs.copy_to_same_matrices().SRs.interpolate_from_known_sample_matrices({1,1}).SDs.from_SR();
        // SF + RSF
        calc.SFs.from_implant_dose().SFs.from_implant_max().RSFs.from_SF_median_ref().RSFs.copy_to_same_matrices().RSFs.interpolate_from_known_sample_matrices();
        calc.SFs.from_RSF_pbp_ref().SFs.from_implant_max();
        // C
        calc.concentrations.from_SF();
        /*exporting*/

        std::string export_path = ui->tofsims_export_directory_textEdit->toPlainText().toStdString();
        if (export_path=="")
            export_path = tools::file::extract_directory_from_filename(MG.measurements_p.front().filename_with_path);
//        if (export_path=="")
//            export_path = ui->tofsims_export_directory_textEdit->placeholderText().toStdString();
        export_path = tools::file::check_directory_string(export_path);
        logger.info(__func__,MG.to_string()).signal("exporting to: " + export_path);
        MG.export_origin_ascii(export_path);
    }


    /*D sims*/
    for (auto& MG : claus->dsims.mgroups)
    {
        if (MG.measurements_p.size()==0)
            continue;
        MG.set_reference_isotopes_in_measurements();
        MG.set_natural_abundances_in_matrix_clusters();
        auto calc = MG.calc();
        calc.matrices.median_const_from_reference_isotopes();
        calc.SRs.from_crater_depths().SRs.from_implant_max();
        if (MG.matrix_clusters().size()>1)
        {
            calc_t::sims_t::matrix_t mat(MG.matrix_isotopes(),MG.measurements_copy());
            auto RSFs = mat.RSFs().add_natural_abundances().remove_RSFs_below_gof_treshold(0.2).symmetrical_RSFs();
//            RSFs.plot_now(0);
            for (auto& M:MG.measurements())
            {
                calc_t::sims_t::matrix_t::concentration_c Concentration(RSFs,*M);
                const auto M_with_Cs = Concentration.concentrations_by_RSFs().concentrations_by_filling_up_all_concentrations_to_1().measurement();
                ///copy the results - this is really ugly
                for (auto C : M_with_Cs.clusters)
                {
                    if (!C.concentration.is_set())
                        continue;
                    *M->cluster(C) = C;
                }
            }
        }
        // SR + SD
        calc.SRs.copy_to_same_matrices().SRs.interpolate_from_known_sample_matrices({1,1}).SDs.from_SR();
        // SF + RSF
        calc.SFs.from_implant_dose().SFs.from_implant_max().RSFs.from_SF_median_ref().RSFs.copy_to_same_matrices().RSFs.interpolate_from_known_sample_matrices();
        calc.SFs.from_RSF_pbp_ref().SFs.from_implant_max();
        // C
        calc.concentrations.from_SF();
        /*exporting*/
        std::string export_path = ui->dsims_export_directory_textEdit->toPlainText().toStdString();
        if (export_path=="")
            export_path = tools::file::extract_directory_from_filename(MG.measurements_p.front().filename_with_path);

//        if (export_path=="")
//            export_path = ui->dsims_export_directory_textEdit->placeholderText().toStdString();

        export_path = tools::file::check_directory_string(export_path);
        logger.info(__func__,MG.to_string()).signal("exporting to: " + export_path);
        MG.export_origin_ascii(export_path);
    }
}
