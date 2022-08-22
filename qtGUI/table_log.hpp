#ifndef TABLE_LOG_HPP
#define TABLE_LOG_HPP

#include <QTableWidget>
//#include "../src/log.hpp"


class table_log : public QTableWidget
{

public:
    table_log(QWidget *parent = nullptr);
    enum color_t{none,white,red,blue,yellow,black,green,cyan,magenta};
    ///will add missing rows
    void add_row(const std::vector<std::string>& row, color_t color);
    void set_header(const std::vector<std::string>& header_s);
protected:
    std::vector<std::string> header;

private:
    QWidget *parent;
};

#endif // TABLE_LOG_HPP
