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


#ifndef TABLE_LOG_T_HPP
#define TABLE_LOG_T_HPP

#include <QTableWidget>
#include "../src/log.hpp"


class table_log_t : public QTableWidget
{

public:
    table_log_t(QWidget *parent = nullptr);
    enum color_t{none,white,red,blue,yellow,black,green,cyan,magenta};
    enum column_t{col_type, col_file, col_class, col_function, col_object, col_description, col_details, col_LAST};
    ///appends at the end
    void append_message(const logger_message_t& message);
    void append_messages(const std::vector<logger_message_t>& messages);
    ///will add missing rows
//    void add_row(const QList<QTableWidgetItem*>& row, color_t color);
    void set_row(unsigned int row_idx, const logger_message_t& message);
    void update(const logger_t& logger);
    void set_print_warning(bool state);
    void set_print_info(bool state);
    void set_print_debug(bool state);
    bool print_warning();
    bool print_info();
    bool print_debug();
protected:
    QTableWidgetItem* item(const logger_message_t& message, column_t col);
    void set_header();
#ifdef QT_DEBUG
    bool print_debug_p=true;
#else
    bool print_debug_p=false;
#endif
    bool print_warning_p=true;
    bool print_info_p=true;
private:
    static std::map<column_t,std::string> column_to_name;
    static std::map<color_t, QColor> color_to_QColor;
    static std::map<logger_message_t::mtype_t,color_t> message_type_to_color;
    std::vector<std::string> header;
    QWidget *parent;
};

#endif // TABLE_LOG_T_HPP
