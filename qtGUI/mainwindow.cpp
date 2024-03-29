﻿/*
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

//using log_f = function_logger_t logger(class_logger, __func__);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      log_c
{
    log_f;
//    function_logger_t logger(class_logger,__func__);
    logger.debug("MainWindow").enter();
    ui->setupUi(this);
    setAcceptDrops(true);

    ui->files_treeView->update();
    ui->measurements_treeView->update();
    ui->mgroups_treeview->update();

    QObject::connect(ui->files_treeView, SIGNAL(update_measurements_treeview()), ui->measurements_treeView, SLOT(update()));
    QObject::connect(ui->measurements_treeView, SIGNAL(update_mgroups_treeview()), ui->mgroups_treeview, SLOT(update()));
    QObject::connect(ui->mgroups_treeview, SIGNAL(update_measurements_treeview()), ui->measurements_treeView, SLOT(update()));

//    QObject::connect(ui->mgroups_treeview, SIGNAL(set_measurement_in_sims_plot_cluster_tree),ui->sims_plot_measurement_widget,SLOT(set_measurement));

//    ui->mgroups_treeview->header()->setStretchLastSection(false);
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
    ui->tofsims_export_directory_textEdit->setText(QString(""));
    ui->dsims_export_directory_textEdit->setText(QString(""));
#endif
    logger.debug("MainWindow").exit();
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
    log_f;
    logger.debug("this").enter();
    std::vector<std::string> dropped_files;
    const QMimeData* mimeData = e->mimeData();
    if (mimeData->hasUrls())
    {
        QList<QUrl> urlList = mimeData->urls();
        dropped_files.reserve(urlList.size());
        for (int i=0; i < urlList.size() ; i++)
        {
//            dropped_files.push_back(urlList.at(i).toString().toStdString());
//            urlList.at(i).toEncoded().toUShort();
//            urlList.at(i).toPercentEncoding(urlList.at(i).toString());
//            urlList.at(i).fromPercentEncoding(urlList.at(i).toEncoded());
            //works in win10 / NTFS; prints correctly in console
            dropped_files.push_back(QByteArray::fromPercentEncoding(urlList.at(i).toEncoded()).toStdString());
//            std::cout << "dropped_file: " <<  urlList.at(i).toString().toUtf8().toStdString() << std::endl;
        }

        for (auto& L : dropped_files)
        {
            // file:///tmp/systemd-private-0ecda464d0ec48abbd82c45271cbe11f-systemd-logind.service-yYlCij
            tools::str::remove_substring_from_mainstring(&L,"file:");
            tools::str::remove_substring_from_mainstring(&L,"FILE:");
//            tools::str::replace_chars(&L,"%23","#");
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
// there is a problem in recognizing the "u" (mikron letter); also olcdb is not parsed correctly (returns -1)
            tools::str::replace_chars(&L,"///","");
            //qt5 automatically replaces the windows style path delimiter with the unix path delimiter, lets change it back
            tools::str::replace_chars(&L,"/","\\"); // unix to windows style
            filenames_collector_t::convert_filename_with_path_to_utf8(L);
//            tools::str::replace_umlaute_to_windows_compatible(&L);
#endif
            logger.debug("dropped_files").value(L,11);
        }
        //collecting and filtering
        filenames_collector_t filenames_collector;
        filenames_collector.add_files_or_folders(dropped_files,ui->scan_dropped_files_recursivly->isChecked());

        ///all filenames are unknown in the beginning
        const auto new_filenames = filenames_collector.filenames();
        logger.debug("new_filenames").value(new_filenames.size(),10,tools::vec::combine_vec_to_string(new_filenames, ", "));
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
            logger.debug(log_object).enter();
            std::vector<unsigned int> indices;
            //tofsims
            for (int i = next_tof_file_idx; i<claus->tofsims.files.size(); i++)
            {

                auto& F = claus->tofsims.files.at(i);
                logger.info(log_object).value(F.name.filename(),10,"tofsims");
                claus->tofsims.add_file_to_measurement(F);
                indices.push_back(i);
            }
            tools::vec::erase(claus->tofsims.files,indices);

            indices.clear();
            //dsims
            for (int i = next_dsims_file_idx; i<claus->dsims.files.size(); i++)
            {
                auto& F = claus->dsims.files.at(i);
                logger.info(log_object).value(F.name.filename(),10,"dsims");
                claus->dsims.add_file_to_measurement(F);
                indices.push_back(i);
            }
            tools::vec::erase(claus->dsims.files,indices);

            indices.clear();
            //dektak6m
            for (int i = next_dektak6m_file_idx; i<claus->dektak6m.files.size(); i++)
            {
                auto& F = claus->dektak6m.files.at(i);
                logger.info(log_object).value(F.name.filename(),10,"dektak6m");
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
            logger.debug(log_object).exit();
        }

        /*some testing, can be deleted*/
//        claus->dsims.mgroups.front().set_export_location(claus->dsims.mgroups.front().export_location() + "/test/");
//        logger.error("test").value(claus->dsims.mgroups.front().measurements().front()->export_location);
        /******************************/

        ui->measurements_treeView->update();
        ui->files_treeView->update();
        ui->mgroups_treeview->update();

        //aishu special treatment
        if (claus->aishu.files.size()>0)
        {
            logger.debug("aishu.files").enter();
            for (auto& af : claus->aishu.files)
            {
                af.export_to_aishu_format();
            }
            logger.debug("aishu.files").exit();
        }



        logger.debug("this").exit();
        //focus tab to files_treeView
//        ui->tabWidget->setCurrentWidget(ui->tab_files);
//        ui->tabWidget->setCurrentIndex(0);

    }
    else
        logger.error("dropped file").value("has no URL");
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if ( (e->modifiers() == Qt::ControlModifier) && (e->key() == Qt::Key_Q))
    {
        QApplication::quit();
    }
//    else
//        MainWindow::keyPressEvent(e);
}

void MainWindow::on_button_files_to_measurements_clicked()
{
    log_f;
    logger.debug("this").enter();
    ui->files_treeView->selections_to_measurements();
    logger.debug("this").exit();
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
    log_f;
    logger.debug("this").value("row=" + std::to_string( index.row() ) + "; column=" + std::to_string( index.column() ));

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
    log_f;
    if (arg1==0)
        ui->tab_log_table->set_print_warning(false);
    else if (arg1==2)
        ui->tab_log_table->set_print_warning(true);
    ui->tab_log_table->update();
    logger.debug("set_print_warning").value(std::to_string(arg1));
}

void MainWindow::on_tab_log_info_stateChanged(int arg1)
{
    log_f;
    if (arg1==0)
        ui->tab_log_table->set_print_info(false);
    else if (arg1==2)
        ui->tab_log_table->set_print_info(true);
    ui->tab_log_table->update();
    logger.debug("set_print_info").value(std::to_string(arg1));
}

void MainWindow::on_tab_log_debug_stateChanged(int arg1)
{
    log_f;
    if (arg1==0)
        ui->tab_log_table->set_print_debug(false);
    else if (arg1==2)
        ui->tab_log_table->set_print_debug(true);
    ui->tab_log_table->update();
    logger.debug("set_print_debug").value(std::to_string(arg1));
}

void MainWindow::on_tab_log_clear_button_clicked()
{
    ui->tab_log_table->clearContents();
    ui->tab_log_table->setRowCount(0);
    logger_t::clear_messages();
}

void MainWindow::on_files_treeView_customContextMenuRequested(const QPoint &pos)
{
    log_f;
    logger.debug("this").enter();
    QModelIndex index = ui->files_treeView->indexAt(pos);
    if (index.isValid())
    {
        logger.debug("pos").value("x="+std::to_string(pos.x()) + "; y="+std::to_string(pos.y()));
        ui->files_treeView->contextMenu->exec(ui->files_treeView->viewport()->mapToGlobal(pos));
    }
}

void MainWindow::on_measurements_treeView_customContextMenuRequested(const QPoint &pos)
{
    log_f;
    QModelIndex index = ui->measurements_treeView->indexAt(pos);
    if (index.isValid())
    {
        logger.debug("pos").value("x="+std::to_string(pos.x()) + "; y="+std::to_string(pos.y()));
        ui->measurements_treeView->contextMenu->exec(ui->measurements_treeView->viewport()->mapToGlobal(pos));
    }
}

void MainWindow::on_mgroups_treeview_clicked(const QModelIndex &index)
{
    log_f;
    measurements_::sims_t* M = nullptr;
//    index.row();
//    index.siblingAtColumn(0);
//    logger.debug("index").value("row: "+ std::to_string(index.row()) + " col: "+ std::to_string(index.column()));
//    logger.debug("dsims").enter();
//    ui->mgroups_treeview->dsims_section.show_selection_in_log({index.siblingAtColumn(0)});
//    logger.debug("tofsims").enter();
//    ui->mgroups_treeview->tofsims_section.show_selection_in_log({index.siblingAtColumn(0)});

//    ui->mgroups_treeview->saveState();

    M = ui->mgroups_treeview->dsims_section.get_measurement_from_QIndex(index.siblingAtColumn(0));
    if (M!=nullptr)
    {
        ui->sims_plot_measurement_widget->update(M);
        return;
    }



    M = ui->mgroups_treeview->tofsims_section.get_measurement_from_QIndex(index.siblingAtColumn(0));
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
    log_f;
    if (index==7) // log tab
    {
        logger.debug(std::to_string(index)).enter();
        ui->tab_log_table->update();
    }
}

void MainWindow::on_calc_button_clicked()
{
    log_f;
    /*fullAuto SIMS*/
    if (ui->check_fullAuto->isChecked())
    {
        for (auto& MG : claus->tofsims.mgroups)
        {
            MG.calc().full_auto();
        }
        for (auto& MG : claus->dsims.mgroups)
        {
            MG.calc().full_auto();
        }
        return;
    }



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
        // RSFs from known samples
        if (ui->check_percentile->isChecked())
        {
            calc.SFs.from_implant_dose().SFs.from_implant_max().RSFs.from_SF_percentile_ref(0.25).RSFs.copy_to_same_matrices().RSFs.interpolate_from_known_sample_matrices();
        }
        else
        {
            calc.SFs.from_implant_dose().SFs.from_implant_max().RSFs.from_SF_median_ref().RSFs.copy_to_same_matrices().RSFs.interpolate_from_known_sample_matrices();
        }
        // SFs of unknown samples
        if (ui->check_pbp->isChecked())
        {
            logger.info("calculation Method").signal("pbp");
            calc.SFs.from_RSF_pbp_ref().SFs.from_implant_max(); //pbp
        }
        else if (ui->check_percentile->isChecked())
        {
            logger.info("calculation Method").signal("percentile0.1");
            calc.SFs.from_RSF_percentile_ref(0.1).SFs.from_implant_max(); //median
        }
        else
        {
            logger.info("calculation Method").signal("median");
            calc.SFs.from_RSF_median_ref().SFs.from_implant_max(); //median
        }
        // C
        calc.concentrations.from_SF();

        /*change numerical sputter_depth resolution*/
        for (auto& M : MG.measurements_p)
        {
            if (!M.crater.sputter_depth.is_set())
                continue;
            auto new_SD_res = M.crater.sputter_depth.change_resolution(M.crater.sputter_depth.min().ceil_().data().front(),0.1,M.crater.sputter_depth.max().floor_().data().front());
            if (new_SD_res.is_set())
            {
                if (M.change_sputter_depth_resolution(new_SD_res))
                    logger.info("new_SD_res").value(new_SD_res.resolution().to_string_short());
                else
                    logger.info("new_SD_res").value("could not change res to: " + new_SD_res.resolution().to_string_short());
            }
        }
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
        // RSFs from known samples
        if (ui->check_percentile->isChecked())
        {
            calc.SFs.from_implant_dose().SFs.from_implant_max().RSFs.from_SF_percentile_ref(0.1).RSFs.copy_to_same_matrices().RSFs.interpolate_from_known_sample_matrices();
//            calc.SFs.from_implant_dose().SFs.from_implant_max().RSFs.from_SF_median_ref().RSFs.copy_to_same_matrices().RSFs.interpolate_from_known_sample_matrices();
        }
        else
        {
            calc.SFs.from_implant_dose().SFs.from_implant_max().RSFs.from_SF_median_ref().RSFs.copy_to_same_matrices().RSFs.interpolate_from_known_sample_matrices();
        }
        // SFs of unknown samples
        if (ui->check_pbp->isChecked())
        {
            logger.info("calculation Method").signal("pbp");
            calc.SFs.from_RSF_pbp_ref().SFs.from_implant_max(); //pbp
        }
        else if (ui->check_percentile->isChecked())
        {
            logger.info("calculation Method").signal("percentile0.1");
            calc.SFs.from_RSF_percentile_ref(0.1).SFs.from_implant_max(); //median
        }
        else
        {
            logger.info("calculation Method").signal("median");
            calc.SFs.from_RSF_median_ref().SFs.from_implant_max(); //median
        }
        // C
        calc.concentrations.from_SF();

        /*change numerical sputter_depth resolution*/
        for (auto& M : MG.measurements_p)
        {
            if (!M.crater.sputter_depth.is_set())
                continue;
            auto new_SD_res = M.crater.sputter_depth.change_resolution(M.crater.sputter_depth.min().ceil_().data().front(),0.1,M.crater.sputter_depth.max().floor_().data().front());
            if (new_SD_res.is_set())
            {
                if (M.change_sputter_depth_resolution(new_SD_res))
                    logger.info("new_SD_res").value(new_SD_res.resolution().to_string_short());
                else
                    logger.info("new_SD_res").value("could not change res to: " + new_SD_res.resolution().to_string_short());
            }
        }
    }
}

void MainWindow::on_export_button_clicked()
{
    log_f;

    /*tof sims*/
    for (auto& MG : claus->tofsims.mgroups)
    {
        MG.export_origin_ascii();
    }

    /*D sims*/
    for (auto& MG : claus->dsims.mgroups)
    {
        MG.export_origin_ascii();
    }

}

void MainWindow::on_check_pbp_stateChanged(int arg1)
{
//    if (ui->check_pbp->isChecked() && ui->check_percentile->isChecked())
//        ui->check_percentile->setChecked(false);
}

void MainWindow::on_test_button_clicked()
{
    log_f;
    logger.debug("this").enter();
    std::map<double,double> target, data;
    std::vector<double> x;
    for (int i=50;i<150;i++)
    {
        x.push_back(i);
    }
    fit_functions::pseudo_voigt_t pseudo_voigt(0,1e4,0.25,48,1);
    fit_functions::pseudo_voigt_t pseudo_voigt_target(0,1e5,0.25,50,1);
    std::vector<double> y = pseudo_voigt.y_data(x);
    data = tools::vec::combine_vecs_to_map(x,y);
    std::vector<double> target_y = pseudo_voigt_target.y_data(x);
    target = tools::vec::combine_vecs_to_map(x,target_y);
    pseudo_voigt.fit_A=true;
    pseudo_voigt.fit_y0=false;
    pseudo_voigt.fit_m=false;
    pseudo_voigt.fit_xc=true;
    pseudo_voigt.fit_w=false;

    std::cout << "pseudo_voigt: " << pseudo_voigt.to_string() << std::endl;
    pseudo_voigt.fit(target);
    auto y_fitted = pseudo_voigt.y_data(x);

    std::cout << "x\t\ty_target\t\ty\t\ty_fitted\t\terror" << std::endl;
//    for (int i=0;i<x.size();i++)
//        std::cout << x.at(i) <<"\t\t" << target_y.at(i) <<"\t\t" << y.at(i) << "\t\t" << y_fitted.at(i) << "\t\t" << abs(target_y.at(i)-y_fitted.at(i)) << std::endl;

    std::cout << "pseudo_voigt_target: " << pseudo_voigt_target.to_string() << std::endl;
    std::cout << "pseudo_voigt fitted: " << pseudo_voigt.to_string() << std::endl;
}

void MainWindow::on_check_percentile_stateChanged(int arg1)
{
//    if (ui->check_pbp->isChecked() && ui->check_percentile->isChecked())
//        ui->check_pbp->setChecked(false);
}

void MainWindow::on_check_fullAuto_clicked()
{
    //
}
