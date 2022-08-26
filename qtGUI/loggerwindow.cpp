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


#include "loggerwindow.hpp"

loggerwindow::loggerwindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::loggerwindow)
{
    ui->setupUi(this);
//    ui->tableWidget->setRowCount(logger.messages().size());
//    ui->tableWidget->setColumnCount(7);
}

loggerwindow::~loggerwindow()
{
    delete ui;
}

//void loggerwindow::add_row(const std::vector<std::string>& row, color_t color) const
//{
//    ui->tableWidget->setRowCount(ui->tableWidget->rowCount()+1);
//    for (int i=0;i<row.size();i++)
//    {

//        auto *item = new QTableWidgetItem(row.at(i).c_str());
////        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
//        item->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsSelectable|Qt::ItemIsEnabled);
//        switch (color)
//        {
//        case white:
//            item->setForeground(QBrush(QColor(255,255,255)));
//            break;
//        case red:
//            item->setForeground(QBrush(QColor(255,0,0)));
//            break;
//        case blue:
//            item->setForeground(QBrush(QColor(0,0,255)));
//            break;
//        case green:
//            item->setForeground(QBrush(QColor(0,255,0)));
//            break;
//        case black:
//            item->setForeground(QBrush(QColor(0,0,0)));
//            break;
//        case yellow:
//            item->setForeground(QBrush(QColor(255,255,0)));
//            break;
//        case cyan:
//            item->setForeground(QBrush(QColor(0,255,255)));
//            break;
//        case magenta:
//            item->setForeground(QBrush(QColor(255,0,255)));
//            break;
//        }
////        std::cout << "adding row item: " << row.at(i) << " at: row="<< ui->tableWidget->rowCount()-1 << " column=" << i << std::endl;
//        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,i,item);
//    }
//}

//void loggerwindow::set_header(const std::vector<std::string>& header_s)
//{
//    header = header_s;
//    ui->tableWidget->setColumnCount(header.size());
//    QStringList header_labels;
//    header_labels.reserve(header.size());
//    for (const auto& h : header)
//        header_labels << h.c_str();
//    ui->tableWidget->setHorizontalHeaderLabels(header_labels);
//}
