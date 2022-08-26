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


#ifndef LOGGERWINDOW_HPP
#define LOGGERWINDOW_HPP

#include <QMainWindow>
#include <vector>
#include "ui_loggerwindow.h"
#include <QString>
#include <QTableWidgetItem>
#include <iostream>

namespace Ui {
class loggerwindow;
}

class loggerwindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit loggerwindow(QWidget *parent = nullptr);
    ~loggerwindow();
//    enum color_t{none,white,red,blue,yellow,black,green,cyan,magenta};
    ///will add missing rows
//    void add_row(const std::vector<std::string>& row, color_t color) const;
//    void set_header(const std::vector<std::string>& header_s);
    Ui::loggerwindow *ui;
protected:
//    std::vector<std::string> header;
private:

};


#endif // LOGGERWINDOW_HPP
