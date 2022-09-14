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
#include <QApplication>
#include "../src/processor.hpp"
#include "../src/tools.hpp"
#include "../src/log.hpp"

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
    MainWindow w; // creating the mainwindow, it needs claus
    w.show();
//    global_logger->set_window(w.ui->tab_log_table); //give the logger a gui to plot
//    global_logger->window_header(); // create the header
//    global_logger->update_window(); // show whatever is already logged


    return a.exec();
}
