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
