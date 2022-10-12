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
    along with this programessage.  If not, see <https://www.gnu.org/licenses/>.
*/


#include "table_log_t.hpp"

table_log_t::table_log_t(QWidget *parent) : parent(parent)
{
    set_header();
}

std::map<logger_message_t::mtype_t,table_log_t::color_t> table_log_t::message_type_to_color
{
    {logger_message_t::fatal,       black},
    {logger_message_t::error,       magenta},
    {logger_message_t::warning,     yellow},
    {logger_message_t::info,        green},
    {logger_message_t::debug,       cyan}
};
std::map<table_log_t::color_t,QColor> table_log_t::color_to_QColor
{
    {white,     QColor(255,255,255)},
    {red,       QColor(255,0,0)},
    {blue,      QColor(0,0,255)},
    {green,     QColor(0,255,0)},
    {black,     QColor(0,0,0)},
    {yellow,    QColor(255,255,0)},
    {cyan,      QColor(0,255,255)},
    {magenta,   QColor(255,0,255)}
};
std::map<table_log_t::column_t,std::string> table_log_t::column_to_name
{
    {col_type,          "TYPE"},
    {col_file,          "FILE"},
    {col_class,         "CLASS"},
    {col_function,      "FUNCTION"},
    {col_object,        "OBJECT"},
    {col_description,   "DESCRIPTION"},
    {col_details,       "DETAILS"}
};

QTableWidgetItem* table_log_t::item(const logger_message_t& message, column_t col)
{
    QTableWidgetItem *item_p = new QTableWidgetItem;
    switch (col)
    {
    case col_type:
        item_p->setText(message.type_string().c_str());
        break;
    case col_class:
        item_p->setText(message.class_name.c_str());
        break;
    case col_description:
        item_p->setText(message.message_description.c_str());
        break;
    case col_details:
        item_p->setText(message.message_details.c_str());
        break;
    case col_file:
        item_p->setText(message.source_file_name.c_str());
        break;
    case col_function:
        item_p->setText(message.func_name.c_str());
        break;
    case col_object:
        item_p->setText(message.object_name.c_str());
        break;
    }
//    item_p->setFlags(Qt::ItemIsSelectable);
    item_p->setForeground(QBrush(color_to_QColor.at(message_type_to_color.at(message.mtype))));
    return item_p;
}

void table_log_t::append_message(const logger_message_t& message)
{
    int row = rowCount();
    setRowCount(row + 1);
    set_row(row, message);
}

void table_log_t::append_messages(const std::vector<logger_message_t>& messages)
{
    int idx = rowCount();
    setRowCount(idx + messages.size());
    for (int i=0;i<messages.size();i++)
    {
        set_row(idx, messages.at(i));
        idx++;
    }
}

void table_log_t::set_row(unsigned int row_idx, const logger_message_t& message)
{
    for (int c=0;c<col_LAST;c++)
    {
        column_t col = static_cast<column_t>(c);
        setItem(row_idx,c,item(message,col));
    }
}

void table_log_t::set_header()
{
//    header = {"TYPE","FILE","CLASS","FUNCTION","OBJECT","DESCRIPTION","DETAILS"};
    setColumnCount(col_LAST);
    QStringList header_labels;
    header_labels.reserve(col_LAST);
    for (int i=0;i<col_LAST;i++)
    {
        column_t col = static_cast<column_t>(i);
        header_labels << column_to_name.at(col).c_str();
//        std::cout << "SET HEADER: " << header_labels.at(i).toStdString() << std::endl;
    }

    setHorizontalHeaderLabels(header_labels);
}
void table_log_t::update(const logger_t& logger)
{
    clearContents();
    setRowCount(0);
    std::vector<logger_message_t> new_messages, warnings, infos, debugs, fatals, errors;
    new_messages.reserve(logger.messages().size());

    fatals = logger.filter().type(logger_message_t::fatal).messages();
    errors = logger.filter().type(logger_message_t::error).messages();
    if (print_warning())
        warnings = logger.filter().type(logger_message_t::warning).messages();
    if (print_info())
        infos = logger.filter().type(logger_message_t::info).messages();
    if (print_debug())
        debugs = logger.filter().type(logger_message_t::debug).messages();
//    new_messages.reserve(fatals.size()+errors.size()+warnings.size()+infos.size()+debugs.size());
    new_messages.insert(new_messages.end(),fatals.begin(),fatals.end());
    new_messages.insert(new_messages.end(),errors.begin(),errors.end());
    new_messages.insert(new_messages.end(),warnings.begin(),warnings.end());
    new_messages.insert(new_messages.end(),infos.begin(),infos.end());
    new_messages.insert(new_messages.end(),debugs.begin(),debugs.end());
    std::sort(new_messages.begin(),new_messages.end());
//    std::reverse(new_messages.begin(),new_messages.end());
    append_messages(new_messages);
    resizeColumnsToContents();
}

void table_log_t::set_print_warning(bool state)
{
    print_warning_p = state;
}
void table_log_t::set_print_info(bool state)
{
    print_info_p = state;
}
void table_log_t::set_print_debug(bool state)
{
    print_debug_p = state;
}
bool table_log_t::print_warning()
{
    return print_warning_p;
}
bool table_log_t::print_info()
{
    return print_info_p;
}
bool table_log_t::print_debug()
{
    return print_debug_p;
}


