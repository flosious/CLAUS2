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
//            std::cout << urlList.at(i).toString().toStdString() << std::endl;
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

        int next_tof_file_idx = claus->tofsims.files().size();
        int next_dsims_file_idx = claus->dsims.files.size();
        claus->parse_filenames(new_filenames);
        //unknown + tofsims + dsims + camera(blder) + xps + dektak6m + p17 + ...

        if (ui->tabWidget->currentIndex()==1) //measurement tab
        {
            logger.debug(__func__,"measurement Tab").enter();
            vector<unsigned int> indices;
            //tofsims
            for (int i = next_tof_file_idx; i<claus->tofsims.files().size(); i++)
            {

                auto& F = claus->tofsims.files().at(i);
                logger.info(__func__,"measurement Tab").value(F.name.filename(),10,"tofsims");
                claus->tofsims.add_to_measurement(F);
                indices.push_back(i);
            }
            tools::vec::erase(claus->tofsims.files(),indices);

            indices.clear();
            //dsims
            for (int i = next_dsims_file_idx; i<claus->dsims.files.size(); i++)
            {
                auto& F = claus->dsims.files.at(i);
                logger.info(__func__,"measurement Tab").value(F.name.filename(),10,"dsims");
                claus->dsims.add_to_measurement(F);
                indices.push_back(i);
            }
            tools::vec::erase(claus->dsims.files,indices);

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
//    std::vector<unsigned int> rows;

//    //tofsims
//    rows = ui->files_treeView->tofsims_entries().get_selected_rows();
//    for (auto file_idx : rows)
//    {
//        auto& file = claus->tofsims.files().at(file_idx);
//        claus->tofsims.add_to_measurement(file);
//        logger.info(__func__,"tofsims.file").value(file.name.to_string(),10,"tofsims");
//    }
//    logger.debug(__func__,"tofsims.files").signal("deleting...");
//    tools::vec::erase(claus->tofsims.files(),rows);

//    //dsims
//    std::vector<files_::dsims_t*> dsims_files; // testing
//    rows = ui->files_treeView->dsims_entries().get_selected_rows();
//    for (auto file_idx : rows)
//    {
//        auto& file = claus->dsims.files.at(file_idx);
//        dsims_files.push_back(&file); // testing
//        claus->dsims.add_to_measurement(file);
//        logger.info(__func__,"dsims.file").value(file.name.to_string(),10,"dsims");
//    }
//    logger.debug(__func__,"dsims.files").signal("deleting...");

//    ///testing...
//    auto filtered_files = claus->dsims.filter_files(dsims_files).by_olcdb(53430).files();
//    for (auto& ff : filtered_files)
//    {
//        logger.info(__func__,"filtered_file").value((ff->name.filename()));
//    }
//    ///

//    tools::vec::erase(claus->dsims.files,rows);

//    ui->files_treeView->update();
//    ui->measurements_treeView->update();
    logger.debug(__func__,"this").exit();
}

void MainWindow::on_remove_files_duplicates_clicked()
{
// 	std::vector<files_::tofsims_t> TF_copy = claus->tofsims.files();
    std::set<files_::tofsims_t> TT(claus->tofsims.files().begin(),claus->tofsims.files().end());
    claus->tofsims.files() = {TT.begin(),TT.end()};

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
        auto& M = claus->tofsims.measurements().at(ui->measurements_treeView->tofsims_entries().get_selected_rows().back());
        ui->measurement_plot_window->plot(M);
    }
}

void MainWindow::on_tab_log_warning_stateChanged(int arg1)
{
    if (arg1==0)
        global_logger->set_print_warning(false);
    else if (arg1==2)
        global_logger->set_print_warning(true);
    logger.debug(__func__,"set_print_warning").value(std::to_string(arg1));
}

void MainWindow::on_tab_log_info_stateChanged(int arg1)
{
    if (arg1==0)
        global_logger->set_print_info(false);
    else if (arg1==2)
        global_logger->set_print_info(true);
    logger.debug(__func__,"set_print_info").value(std::to_string(arg1));
}

void MainWindow::on_tab_log_debug_stateChanged(int arg1)
{
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
