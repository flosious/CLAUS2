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

#ifndef CUSTOM_QSTANDARDITEMS_H
#define CUSTOM_QSTANDARDITEMS_H

#include <QStandardItem>
#include "../src/log.hpp"
#include "../src/mgroup.hpp"

/*declare types to store in userRole of QVariant of the item*/
Q_DECLARE_METATYPE(measurements_::dsims_t*);
Q_DECLARE_METATYPE(mgroups_::dsims_t*);
Q_DECLARE_METATYPE(measurements_::tofsims_t*);
Q_DECLARE_METATYPE(mgroups_::tofsims_t*);

Q_DECLARE_METATYPE(quantity::quantity_t*);
Q_DECLARE_METATYPE(cluster_t*);


class custom_QStandardItems
{
public:
    enum class columns{col_size,col_method,col_olcdb,col_group_id,col_settings,col_export,col_graph_line_style,
                      col_SR,col_RSF,col_matrix,col_sample,col_median};
    enum class roles{role_mgroup=2,role_measurement,role_cluster,role_quantity,role_expanded,role_graphId};
    static std::map<columns,std::string> column_names;

    class base_QStandardItem
    {
    protected:
        base_QStandardItem(QStandardItem* item);
        QStandardItem* item;
        std::set<columns> cols;
    public:
        void update();
        ///update just a specific col, if it exists
        void update(columns col);
        ///checks if the given col is already added
        bool is_col(columns col) const;
        void add_column(columns col);
        void del_column(columns col);
    };

    class quantity_t : base_QStandardItem
    {
    private:
        quantity::quantity_t* quantity;
    public:
        quantity_t(quantity::quantity_t* quantity, QStandardItem* item);
    };

    class cluster_t : base_QStandardItem
    {
    private:
        cluster_t* cluster;
    public:
        cluster_t(cluster_t* cluster, QStandardItem* item);
    };

    template<typename M_t>
    class measurement_t : base_QStandardItem
    {
    private:
        class_logger_t class_logger;
        M_t *mgroup;
    public:
        measurement_t(M_t* measurement, QStandardItem* item);
    };

    template<typename MG_t>
    class mgroup_QStandardItem
    {
    private:
        class_logger_t class_logger;
        MG_t* mgroup;
    public:
        mgroup_QStandardItem(MG_t* mgroup, QStandardItem* item);
    };

    template<typename tool_t>
    class tool_QStandardItem
    {
    private:
        class_logger_t class_logger;
        tool_t *tool;
    public:
        tool_QStandardItem(tool_t* tool, QStandardItem* item);
        void remove_group();
        void remove_measurement();
    };
};



#endif // CUSTOM_QSTANDARDITEMS_H
