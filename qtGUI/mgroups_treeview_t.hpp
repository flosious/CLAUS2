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

#ifndef MGROUPS_TREEVIEW_T_H
#define MGROUPS_TREEVIEW_T_H

#include <map>
#include <QTreeView>
#include <QStandardItem>
#include <QMenu>
#include <QAction>
#include <QKeyEvent>
#include "../src/mgroup.hpp"
#include "../src/measurement.hpp"
#include "../src/processor.hpp"


Q_DECLARE_METATYPE(measurements_::dsims_t*); //saved in Qt::UserRole+1
Q_DECLARE_METATYPE(mgroups_::dsims_t*); //saved in Qt::UserRole+2
Q_DECLARE_METATYPE(measurements_::tofsims_t*); //saved in Qt::UserRole+1
Q_DECLARE_METATYPE(mgroups_::tofsims_t*); //saved in Qt::UserRole+2

class mgroups_treeview_t : public QTreeView
{
    Q_OBJECT
private:
    ///tool_sections ; don t forget to adjust column_names; leave LAST as last
    enum sections {tofsims, dsims, LAST};
    static std::map<sections,std::string> section_names;
    ///don t forget to adjust column_names; leave LAST as last
    enum columns {col_method, col_sizes, col_olcdb, col_group_id, col_settings, col_LAST};
    static std::map<columns,std::string> column_names;
    class_logger_t class_logger;
    template<typename mgroup_t, typename measurement_t, typename tool_with_files_measurements_groups_t>
    class tool_section_t
    {
    private:
        class_logger_t class_logger;
    protected:
        ///save measurement* pointer data in item role (Qt::UserRole+ !1! )
        static QStandardItem* item(measurement_t& measurement, columns col)
        {
            class_logger_t logger(__FILE__,"mgroups_treeview_t::tool_section_t::group_section_t::item");
            QStandardItem *item = new QStandardItem;
            std::stringstream ss;
            QVariant variant_data;
            switch (col)
            {
            case col_method:
//                item = new QStandardItem(QString(measurement.to_string_short().c_str()));
//                item = new QStandardItem(QString(measurement.filename_with_path.c_str()));
                item->setText(QString(tools::file::extract_filename(measurement.filename_with_path).c_str()));
                /* new */
                variant_data.setValue(&measurement);
                item->setData(variant_data,Qt::UserRole+1);
                item->setCheckable(true);
                item->setCheckState(Qt::Unchecked);
                /******/
                break;
            case col_sizes:
                ss.str();
                ss << "clusters: <" << measurement.clusters.size() <<">";
                item->setText(QString(ss.str().c_str()));
                break;
            case col_olcdb:
                item->setText(QString(std::to_string(measurement.olcdb).c_str()));
                break;
            case col_group_id:
                item->setText(QString(measurement.group_id.c_str()));
                break;
            case col_settings:
                item->setText(QString(measurement.settings.to_string_short().c_str()));
                break;
            default:
                logger.error(__func__,"col").value(std::to_string(col) + "unknown");
            }
            item->setEditable(false);

            return item;
        }
        ///save mgroup* pointer data in item role (Qt::UserRole+ !2! )
        static QStandardItem* item(mgroup_t& mgroup, columns col)
        {
            class_logger_t logger(__FILE__,"mgroups_treeview_t::tool_section_t::group_section_t::item");
            QStandardItem *item = new QStandardItem;
            std::stringstream ss;
            QVariant variant_data;
            switch (col)
            {
            case col_method:
                ss.str("");
                if (mgroup.name!="")
                    ss << mgroup.name ;
                else
                    ss << mgroup.to_string_short();
                item->setText(QString(ss.str().c_str()));
                item->setEditable(true);
                //adding all measurements in this group here
                for (auto& M : mgroup.measurements_p)
                {
                    QList<QStandardItem*> _row = row(M);
                    //add mgroup information to measurement item
                    QVariant variant;
                    variant.setValue(&mgroup);
                    _row.at(0)->setData(variant,Qt::UserRole+2);
                    /*******************************************/
                    item->appendRow(_row);
                }
                /* new */
                variant_data.setValue(&mgroup);
                item->setData(variant_data,Qt::UserRole+2);
                /******/
                break;
            case col_sizes:
                ss.str();
                ss << "measurements: <" << mgroup.measurements_p.size() <<">";
                item->setText(QString(ss.str().c_str()));
                item->setEditable(false);
                break;
            case col_olcdb:
                item->setText(QString(std::to_string(mgroup.olcdb).c_str()));
                item->setEditable(false);
                break;
            case col_group_id:
                item->setText(QString(mgroup.group_id.c_str()));
                item->setEditable(false);
                break;
            case col_settings:
                item->setText(QString(mgroup.settings_p.to_string_short().c_str()));
                item->setEditable(false);
                break;
            default:
                logger.error(__func__,"col").value(std::to_string(col) + "unknown");
            }

            return item;
        }

        template<typename MG_or_M_t>
        static QList<QStandardItem*> row(MG_or_M_t& MG_or_M)
        {
            QList<QStandardItem*> itemList;
            itemList.reserve(col_LAST);
            for (int c=0; c<col_LAST;c++)
            {
                columns col = static_cast<columns>(c);
                itemList << item(MG_or_M,col);
            }
            return itemList;
        }

        tool_with_files_measurements_groups_t* tool;

        QStandardItem* group_section_item(mgroup_t MG)
        {
            QStandardItem *group_item = item(MG,static_cast<columns>(0));
            for (auto& M : MG.measurements_p)
            {
                group_item->appendRow( row(M) );
            }

            return group_item;
        }

        QStandardItem* ungrouped_measurements_section_item(columns col)
        {
            log_f;
            logger.debug("this").enter();
            QStandardItem *item = new QStandardItem(QString(""));
            std::stringstream ss;
            switch (col)
            {
            case col_method:
                logger.debug("col_method").enter();
                ss.str("");
                ss << "ungrouped";
                item->setText(ss.str().c_str());
                item->setEditable(false);
                if (this->tool->measurements.size()>0)
                {
                    for (auto& M : this->tool->measurements)
                    {
                        item->appendRow( row(M) );
                    }
                }
                logger.debug("col_method").exit();
                break;
            case col_sizes:
                logger.debug("col_sizes").enter();
                ss.str("");
                ss << "measurements: <" << this->tool->measurements.size() << ">";
                item->setText(ss.str().c_str());
                item->setEditable(false);
                logger.debug("col_sizes").exit();
                break;
            }
            logger.debug("this").exit();
            return item;
        }
        QList<QStandardItem*> ungrouped_measurements_section_row()
        {
            QList<QStandardItem*> column_items;
            for (int c=0;c<col_LAST;c++)
            {
                columns col = static_cast<columns>(c);
                column_items << ungrouped_measurements_section_item(col);
            }
            // this will lead to a SIGBUS ERROR
//            column_items << ungrouped_measurements_section_item(col_method) << ungrouped_measurements_section_item(col_sizes);
            return column_items;
        }
        QStandardItemModel* model;
        ///mgroup pointers in here indicate expanded items in treeview
        std::vector<int> expanded_mgroups_position_list;
    public:
        static bool index_has_measurement(const QModelIndex& idx)
        {
            //idx contains data at the correct position (UserRole+1) ?
            if (!idx.data(Qt::UserRole+1).isValid())
                    return false;
            //idx data is correct type?
            if (idx.data(Qt::UserRole+1).userType() != qMetaTypeId<measurement_t*>())
                return false;
            return true;
        }
        static bool index_has_mgroup(const QModelIndex& idx)
        {
            if (!idx.data(Qt::UserRole+2).isValid())
                    return false;
            //idx data is correct type?
            if (idx.data(Qt::UserRole+2).userType() != qMetaTypeId<mgroup_t*>())
                return false;
            return true;
        }
        static bool is_grouped_measurement(const QModelIndex& idx)
        {
            if (index_has_measurement(idx) && index_has_mgroup(idx))
                    return true;
            return false;
        }
        static bool is_UNgrouped_measurement(const QModelIndex& idx)
        {
            if (index_has_measurement(idx) && !index_has_mgroup(idx))
                    return true;
            return false;
        }
        static bool is_mgroup(const QModelIndex& idx)
        {
            if (!index_has_measurement(idx) && index_has_mgroup(idx))
                return true;
            return false;
        }


        static measurement_t* get_measurement_from_QIndex(const QModelIndex& idx)
        {
            if (!index_has_measurement(idx))
                return nullptr;
            return idx.data(Qt::UserRole+1).value<measurement_t*>();
        }
        static mgroup_t* get_mgroup_from_QIndex(const QModelIndex& idx)
        {
            if (!index_has_mgroup(idx))
                return nullptr;
            return idx.data(Qt::UserRole+2).value<mgroup_t*>();
        }
        sections tool_section_id;
        tool_section_t(sections tool_section_id, tool_with_files_measurements_groups_t* tool, QStandardItemModel* model) :
            tool_section_id(tool_section_id),tool(tool), class_logger(__FILE__,"mgroups_treeview_t::tool_section_t"), model(model)
        {
        }
        bool expanded=true;


        void saveStates_to_expanded_mgroups_position_list(QTreeView *treeview)
        {
            log_f;
            expanded_mgroups_position_list.clear();
            QModelIndex index = model->index(tool_section_id,0);
            if (treeview->isExpanded(index))
            {
                expanded=true;
                for (int r=0;r<model->rowCount(index);r++)
                {
                    QModelIndex child_index = model->index(r,0,index);
                    if (treeview->isExpanded(child_index))
                    {
                        expanded_mgroups_position_list.push_back(r);
                        logger.debug("child_index").value("is expanded",10,"row: " + std::to_string(child_index.row()) + " column: " + std::to_string(child_index.column()) + " mgroup: " + get_mgroup_from_QIndex(child_index)->to_string_short() );
                    }
                }
            }
            else
                expanded=false;
        }
//        void removeIndexes_from_expanded_mgroups_position_list(QList<QModelIndex> Indexes)
//        {
//            for (auto& index : Indexes)
//            {
//                int position = index.row();
//                expanded_mgroups_position_list.erase(std::find(expanded_mgroups_position_list.begin(),expanded_mgroups_position_list.end(),position));
//                //don t forget to reduce all entries > position by 1!
//                for (auto& pos : expanded_mgroups_position_list)
//                {
//                    if (position>pos)
//                        pos--;
//                }
//            }
//        }
        void restoreStates_from_expanded_mgroups_position_list(QTreeView *treeview)
        {
            log_f;
            if (!expanded)
                return;
            QModelIndex index = model->index(tool_section_id,0);

            treeview->expand(index);
            for (int r : expanded_mgroups_position_list)
            {
                QModelIndex child_index = model->index(r,0,index);
                if (!child_index.isValid())
                    continue;
                treeview->expand(child_index);
                logger.debug("child_index").value("expanded",10,"row: " + std::to_string(child_index.row())+ " col: " + std::to_string(child_index.column()) + " mgroup: " + get_mgroup_from_QIndex(child_index)->to_string_short() );
            }
        }
        void show_selection_in_log(const QModelIndexList& indexes)
        {
            log_f;
            logger.debug("indexes.size()").value(indexes.size());
            for (auto& idx : indexes)
            {
                if (!idx.isValid())
                    continue;
                if (idx.column()!=0)
                    continue;
//                logger.debug("index").value("isValid",10,"col: " + std::to_string(idx.column()) +" row:" + std::to_string(idx.row()));
                if ( is_UNgrouped_measurement(idx) )
                {
                    logger.info("index").value("is_UNgrouped_measurement",10,"measurement: " + get_measurement_from_QIndex(idx)->to_string_short());
                }
                else if ( is_grouped_measurement(idx) )
                {
                    logger.debug("index").value("is_grouped_measurement",10,"measurement: " + get_measurement_from_QIndex(idx)->to_string_short() + " mgroup: "+ get_mgroup_from_QIndex(idx)->to_string_short());
                }
                else if (is_mgroup(idx))
                {
                    logger.debug("index").value("is_mgroup",10,get_mgroup_from_QIndex(idx)->to_string_short());
                }
            }
        }
        ///return true if something was changed
        bool ungroup_selection(const QModelIndexList& indexes)
        {
            bool go_update=false;
            for (auto& idx : indexes)
            {
                if (!idx.isValid())
                    continue;
                if (idx.column()!=0)
                    continue;

                if ( is_grouped_measurement(idx) ) // measurement
                {
                    int i = tool->ungroup_measurements_from_group({get_measurement_from_QIndex(idx)},get_mgroup_from_QIndex(idx));
                    if (i>0)
                        go_update=true;
                }
                else if (is_mgroup(idx)) // mgroup
                {
                    if (tool->ungroup(get_mgroup_from_QIndex(idx)))
                        go_update=true;
                }
            }
            return go_update;
        }
        ///returns true, is something was deleted; false if nothing changed
        bool delete_selection(const QModelIndexList& indexes)
        {
            log_f;
            bool go_update=false;
            for (auto& idx : indexes)
            {
                if (!idx.isValid())
                    continue;
                logger.debug("index").value("isValid",10,"col: " + std::to_string(idx.column()) +" row:" + std::to_string(idx.row()));
                if (idx.column()!=0)
                    continue;
                logger.debug("index").value("is in column: 0");
                if ( is_UNgrouped_measurement(idx) ) //ungrouped measurements or section
                {
                    logger.debug("index").value("is_UNgrouped_measurement",10,"measurement: " + get_measurement_from_QIndex(idx)->to_string_short());
                    std::vector<unsigned int> erase_idx;
                    auto idx_pos = tools::vec::get_index_position_by_comparing_pointers(tool->measurements, get_measurement_from_QIndex(idx));
                    if (idx_pos>=0)
                    {
//                        logger.debug("index").value("is_UNgrouped_measurement and idx_pos: "+std::to_string(idx_pos),10,"removed measurement: " + get_measurement_from_QIndex(idx)->to_string_short());
                        tools::vec::erase(tool->measurements,erase_idx);
                        go_update = true;
                    }
                }
                else if (is_grouped_measurement(idx)) // measurement
                {
                    logger.debug("index").value("is_grouped_measurement",10,"measurement: " + get_measurement_from_QIndex(idx)->to_string_short() + " mgroup: "+ get_mgroup_from_QIndex(idx)->to_string_short());
                    if (get_mgroup_from_QIndex(idx)->remove_measurement(get_measurement_from_QIndex(idx)))
                    {
//                        logger.debug("mgroup").value( get_mgroup_from_QIndex(idx)->to_string_short(),10, "removed measurement: " + get_measurement_from_QIndex(idx)->to_string_short() );
                        go_update = true;
                    }
                }
                else if (is_mgroup(idx)) // mgroup
                {
                    logger.debug("index").value("is_mgroup",10,get_mgroup_from_QIndex(idx)->to_string_short());
                    if (tool->remove_mgroup(get_mgroup_from_QIndex(idx)))
                    {
//                        logger.debug("mgroup").value("removed: " +  get_mgroup_from_QIndex(idx)->to_string_short() );
                        go_update = true;
                    }
                }
                if (go_update)
                {
                    logger.debug("removeRow").value("row: " + std::to_string( idx.row()) + " ; column: " + std::to_string(idx.column()) );
                    model->removeRow(idx.row(),idx.parent());
                }
            }
            return go_update;
        }
        QStandardItem* tool_section_item(columns col)
        {
            log_f;
            logger.debug("this").enter();
            QStandardItem *item = new QStandardItem;
            std::stringstream ss;
            switch (col)
            {
            case col_method:
                logger.debug("col_method").enter();
                ss.str("");
                ss << section_names.at(tool_section_id);
                item->setText(ss.str().c_str());
                item->appendRow(ungrouped_measurements_section_row());
                for (auto& MG : this->tool->mgroups)
                {
                    item->appendRow(row(MG));
                }
                item->setEditable(false);
                logger.debug("col_method").exit();
                break;
            case col_sizes:
                logger.debug("col_sizes").enter();
                std::stringstream ss;
                ss << "mgroups: <" << this->tool->mgroups.size() << ">";
                auto s = ss.str().c_str();
                item->setText(s);
                logger.debug("col_sizes").signal(s);
                item->setEditable(false);
                logger.debug("col_sizes").exit();
                break;
            }
            logger.debug("this").exit();
            return item;
        }
    }; // tool_section_t
    ///restores the states of the items
    void restoreState();
    QWidget *parent;
    QStandardItemModel* model;
    void set_model();
    void set_header();
    void set_actions();
    void set_contextMenu();
    void connect_signals_to_slots();
public:
    tool_section_t<mgroups_::dsims_t, measurements_::dsims_t,processor::tool_with_files_measurements_groups<files_::dsims_t,measurements_::dsims_t,mgroups_::dsims_t> > dsims_section;
    tool_section_t<mgroups_::tofsims_t, measurements_::tofsims_t, processor::tool_with_files_measurements_groups<files_::tofsims_t,measurements_::tofsims_t,mgroups_::tofsims_t>> tofsims_section;
//    bool use_pbp_calc=true;
    QMenu* contextMenu;
    QAction* delete_selection_action;
    QAction* ungroup_selection_action;
    mgroups_treeview_t(QWidget *parent = nullptr);

    void keyPressEvent(QKeyEvent *e) override;
    void delete_selection();
//    void show_selection_in_log();
    void ungroup_selection();

    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>()) override;

    void resizeColumns();public slots:
    void saveState();
    void update();
signals:
    void update_measurements_treeview();
};
extern processor *claus;

#endif // MGROUPS_TREEVIEW_T_H
