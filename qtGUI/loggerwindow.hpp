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
