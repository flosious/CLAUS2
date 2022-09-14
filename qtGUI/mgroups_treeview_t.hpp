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


Q_DECLARE_METATYPE(measurements_::dsims_t*);
Q_DECLARE_METATYPE(mgroups_::dsims_t*);
Q_DECLARE_METATYPE(measurements_::tofsims_t*);
Q_DECLARE_METATYPE(mgroups_::tofsims_t*);

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

    /*****************/
    class selection_t
    {
        int measurement_row_p=-1;
        int mgroup_row_p=-1;
        int tool_section_row_p=-1;
    public:
        selection_t(int tool_section_row=-1, int mgroup_row=-1, int measurement_row=-1);
        selection_t(const QModelIndex &topLeft);
        int measurement_row() const;
        int mgroup_row() const;
        int tool_section_row() const;
        bool is_ungrouped_measurement_section() const;
        //an actual mgroup section row
        bool is_mgroup() const;
        ///grouped or ungrouped measurement
        bool is_measurement() const;
    };


    template<typename mgroup_t, typename measurement_t, typename tool_with_files_measurements_groups_t>
    class tool_section_t
    {
    private:
        class_logger_t logger;
    protected:

        static QStandardItem* item(measurement_t& measurement, columns col)
        {
            class_logger_t logger(global_logger,__FILE__,"mgroups_treeview_t::tool_section_t::group_section_t::item");
            QStandardItem *item = new QStandardItem;
            stringstream ss;
            QVariant variant_data;
            switch (col)
            {
            case col_method:
//                item = new QStandardItem(QString(measurement.to_string_short().c_str()));
//                item = new QStandardItem(QString(measurement.filename_with_path.c_str()));
                item->setText(QString(tools::file::extract_filename(measurement.filename_with_path).c_str()));
                /* new */
                variant_data.setValue(&measurement);
                item->setData(variant_data);
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
        static QStandardItem* item(mgroup_t& mgroup, columns col)
        {
            class_logger_t logger(global_logger,__FILE__,"mgroups_treeview_t::tool_section_t::group_section_t::item");
            QStandardItem *item = new QStandardItem;
            stringstream ss;
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
                    item->appendRow(row(M));
                }
                /* new */
                variant_data.setValue(&mgroup);
                item->setData(variant_data);
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

        QStandardItem* unrouped_measurements_section_item(columns col)
        {
            logger.debug(__func__,"this").enter();
            QStandardItem *item = new QStandardItem(QString(""));
            stringstream ss;
            switch (col)
            {
            case col_method:
                logger.debug(__func__,"col_method").enter();
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
                logger.debug(__func__,"col_method").exit();
                break;
            case col_sizes:
                logger.debug(__func__,"col_sizes").enter();
                ss.str("");
                ss << "measurements: <" << this->tool->measurements.size() << ">";
                item->setText(ss.str().c_str());
                item->setEditable(false);
                logger.debug(__func__,"col_sizes").exit();
                break;
            }
            logger.debug(__func__,"this").exit();
            return item;
        }
        QList<QStandardItem*> unrouped_measurements_section_row()
        {
            QList<QStandardItem*> column_items;
            for (int c=0;c<col_LAST;c++)
            {
                columns col = static_cast<columns>(c);
                column_items << unrouped_measurements_section_item(col);
            }
            // this will lead to a SIGBUS ERROR
//            column_items << unrouped_measurements_section_item(col_method) << unrouped_measurements_section_item(col_sizes);
            return column_items;
        }
        QStandardItemModel* model;
    public:
        sections tool_section_id;
        tool_section_t(sections tool_section_id, tool_with_files_measurements_groups_t* tool, QStandardItemModel* model) :
            tool_section_id(tool_section_id),tool(tool), logger(global_logger,__FILE__,"mgroups_treeview_t::tool_section_t"), model(model)
        {
        }

        void ungroup_selection(const QModelIndexList& indexes)
        {
            bool go_update=false;
            for (auto& idx : indexes)
            {
                if (!idx.isValid())
                    continue;
                if (idx.column()!=0)
                    continue;
                auto variant_data = idx.data(Qt::UserRole+1);

                if ( (qMetaTypeId<measurement_t*>() == variant_data.userType()) ) // measurement
                {
                    if ((idx.parent().isValid() && idx.parent().data(Qt::UserRole+1).isValid())) // measurement within a mgroup (NOT ungrouped)
                    {
                        auto idx_pos = tools::vec::get_index_position_by_comparing_pointers(idx.parent().data(Qt::UserRole+1).value<mgroup_t*>()->measurements_p,variant_data.value<measurement_t*>());
//                        tools::vec::erase(,);
//                        tool->
                    }
                }
                else if (qMetaTypeId<measurement_t*>() == variant_data.userType()) // mgroup
                {

                }
            }
        }
        ///returns true, is something was deleted; false if nothing changed
        bool delete_selection(const QModelIndexList& indexes)
        {
            bool go_update=false;
            for (auto& idx : indexes)
            {
                if (!idx.isValid())
                    continue;
                if (idx.column()!=0)
                    continue;
                auto variant_data = idx.data(Qt::UserRole+1);

                if (!variant_data.isValid()  ) //ungrouped measurements or section
                {
                    if (!idx.data().isValid())
                        return false;

                    int rows = model->itemFromIndex(idx)->rowCount();
                    std::vector<unsigned int> erase_idx;

                    if (idx.child(0,0).data(Qt::UserRole+1).userType()==qMetaTypeId<measurement_t*>())
                    {
                        erase_idx.clear();
                        for (int r=0;r<rows;r++)
                        {
                            variant_data = idx.child(r,0).data(Qt::UserRole+1);
                            auto idx_pos = tools::vec::get_index_position_by_comparing_pointers(tool->measurements,variant_data.value<measurement_t*>());
                            if (idx_pos>=0)
                                erase_idx.push_back(idx_pos);
                        }
                        if (erase_idx.size()>0)
                        {
                            tools::vec::erase(tool->measurements,erase_idx);
                            go_update = true;
                        }
                    }
                }
                else if (qMetaTypeId<measurement_t*>() == variant_data.userType()) // measurement
                {
                    auto idx_pos = tools::vec::get_index_position_by_comparing_pointers(tool->measurements,variant_data.value<measurement_t*>());
                    if (idx_pos>=0)
                    {
                        tools::vec::erase(tool->measurements,{static_cast<unsigned int>(idx_pos)});
                        go_update = true;
                    }
                    else if (idx.parent().isValid() && idx.parent().data(Qt::UserRole+1).isValid()) // measurement within mgroup
                    {
                        idx_pos = tools::vec::get_index_position_by_comparing_pointers((idx.parent().data(Qt::UserRole+1).value<mgroup_t*>())->measurements_p,variant_data.value<measurement_t*>());
                        if (idx_pos>=0)
                        {
                            tools::vec::erase(idx.parent().data(Qt::UserRole+1).value<mgroup_t*>()->measurements_p,{static_cast<unsigned int>(idx_pos)});
                            go_update = true;
                        }
                    }
                }
                else if (qMetaTypeId<mgroup_t*>() == variant_data.userType()) // mgroup
                {
                    auto idx_pos = tools::vec::get_index_position_by_comparing_pointers(tool->mgroups,variant_data.value<mgroup_t*>());
                    if (idx_pos>=0)
                    {
                        tools::vec::erase(tool->mgroups,{static_cast<unsigned int>(idx_pos)});
                        go_update = true;
                    }
                }
            }
            return go_update;
        }
        QStandardItem* tool_section_item(columns col)
        {
            logger.debug(__func__,"this").enter();
            QStandardItem *item = new QStandardItem;
            stringstream ss;
            switch (col)
            {
            case col_method:
                logger.debug(__func__,"col_method").enter();
                ss.str("");
                ss << section_names.at(tool_section_id);
                item->setText(ss.str().c_str());
                item->appendRow(unrouped_measurements_section_row());
                for (auto& MG : this->tool->mgroups)
                {
                    item->appendRow(row(MG));
                }
                item->setEditable(false);
                logger.debug(__func__,"col_method").exit();
                break;
            case col_sizes:
                logger.debug(__func__,"col_sizes").enter();
                stringstream ss;
                ss << "mgroups: <" << this->tool->mgroups.size() << ">";
                auto s = ss.str().c_str();
                item->setText(s);
                logger.debug(__func__,"col_sizes").signal(s);
                item->setEditable(false);
                logger.debug(__func__,"col_sizes").exit();
                break;
            }
            logger.debug(__func__,"this").exit();
            return item;
        }

    }; // tool_section_t
    selection_t get_selection() const;
    class_logger_t logger;
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
    QMenu* contextMenu;
    QAction* delete_selection_action;
    QAction* ungroup_selection_action;
    mgroups_treeview_t(QWidget *parent = nullptr);

    void keyPressEvent(QKeyEvent *e) override;
    void delete_selection();
    void ungroup_selected_group(const selection_t& selection);
    void ungroup_selected_measurement(const selection_t& selection);
    void ungroup_selection();

    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>()) override;

    void resizeColumns();public slots:
    void update();
signals:
    void update_measurements_treeview();
};
extern processor *claus;
extern Logger global_logger;
#endif // MGROUPS_TREEVIEW_T_H
