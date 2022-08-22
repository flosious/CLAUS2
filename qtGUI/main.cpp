#include "mainwindow.h"
#include <QApplication>
#include "../src/processor.hpp"
#include "../src/tools.hpp"
#include "../src/log.hpp"
#include "loggerwindow.hpp"

//global scope
processor *claus = nullptr;
//Logger global_logger = std::make_shared<logger_t>();
Logger global_logger;

//Logger global_logger;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    global_logger = std::make_shared<logger_t>();
    processor KLAUS(tools::str::args_to_vec(argc, argv)); // processor needs a logger printing to std::cout
    //define global variable
    claus = &KLAUS;
//    auto l = std::make_shared<loggerwindow>();
//    loggerwindow l;
//    l.show();
//    global_logger = std::make_shared<logger_t>((l.ui->tableWidget));
//    global_logger->window_header();
    MainWindow w; // creating the mainwindow, it needs claus
    w.show();
    global_logger->set_window(w.ui->tab_log_table); //give the logger a gui to plot
    global_logger->window_header(); // create the header
    global_logger->update_window(); // show whatever is already logged


    return a.exec();
}
