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

#include "mgroups_treeview_t.hpp"


/*********************************/
/*****     selection_t     *******/
/*********************************/


mgroups_treeview_t::selection_t::selection_t(int tool_section_row,
                                             int mgroup_row,
                                             int measurement_row) :
    tool_section_row_p(tool_section_row),
    mgroup_row_p(mgroup_row),
    measurement_row_p(measurement_row)
{
}

mgroups_treeview_t::selection_t::selection_t(const QModelIndex &idx)
{
    if (!idx.isValid())
        return;
    if (idx.column()!=0)
        return;
    if (idx.parent().parent().isValid()) // measurement item row selected
    {
        *this = selection_t(idx.parent().parent().row(),idx.parent().row(),idx.row());
    }
    else if (idx.parent().isValid()) // measurement_group / section group item row selected
    {
        *this = selection_t(idx.parent().row(),idx.row());
    }
    else if (idx.isValid()) // tool section item row selected
    {
        *this = selection_t(idx.row());
    }
}

int mgroups_treeview_t::selection_t::measurement_row() const
{
    return measurement_row_p;
}
int mgroups_treeview_t::selection_t::mgroup_row() const
{
    return mgroup_row_p;
}
int mgroups_treeview_t::selection_t::tool_section_row() const
{
    return tool_section_row_p;
}

bool mgroups_treeview_t::selection_t::is_ungrouped_measurement_section() const
{
    if (tool_section_row()>=0 && mgroup_row()==0 && measurement_row()<0)
        return true;
    return false;
}
bool mgroups_treeview_t::selection_t::is_mgroup() const
{
    if (tool_section_row()>=0 && mgroup_row()>0 && measurement_row()<0)
        return true;
    return false;
}
bool mgroups_treeview_t::selection_t::is_measurement() const
{
    if (tool_section_row()>=0 && mgroup_row()>=0 && measurement_row()>=0)
        return true;
    return false;
}

/*********************************/

std::map<mgroups_treeview_t::sections,std::string> mgroups_treeview_t::section_names
{
    {tofsims,"tofsims"},
    {dsims,"dsims"}
};

std::map<mgroups_treeview_t::columns,std::string> mgroups_treeview_t::column_names
{
    {col_method,"method groups"},
    {col_sizes,"<size>"},
    {col_olcdb,"olcdb"},
    {col_group_id,"group id"},
    {col_settings,"settings"}
};



void mgroups_treeview_t::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    logger.debug(__func__,"this").enter();
    if (topLeft == bottomRight)
    {
        selection_t selection(topLeft);
        if (!selection.is_mgroup())
            return;
        int row = selection.mgroup_row()-1;
        std::string name = model->itemFromIndex(topLeft)->text().toStdString();

        if (selection.tool_section_row()==tofsims)
        {
            auto& MG = claus->tofsims.mgroups.at(row);
            MG.name =name;
            logger.debug(__func__,"MG tofsims name").signal(MG.name);
        }
        else if (selection.tool_section_row()==dsims)
        {
            auto& MG = claus->dsims.mgroups.at(row);
            MG.name =name;
            logger.debug(__func__,"MG dsims name").signal(MG.name);
        }
    }
}

void mgroups_treeview_t::resizeColumns()
{
    for (int i=0;i<col_LAST;i++)
    {
        this->resizeColumnToContents(i);
    }
}

mgroups_treeview_t::mgroups_treeview_t(QWidget *parent ) :
    logger(global_logger,__FILE__,"mgroups_treeview_t"), parent(parent), model(new QStandardItemModel (sections::LAST, columns::col_LAST)),
    dsims_section(dsims,&claus->dsims, model), tofsims_section(tofsims,&claus->tofsims, model)
{
    set_actions();
    set_contextMenu();
    connect_signals_to_slots();
    set_model();
    set_header();
    update();
    expand(model->index(dsims,0));
    expand(model->index(tofsims,0));
    setTextElideMode(Qt::ElideLeft);
    resizeColumns();
    setAcceptDrops(true);
//    setDragEnabled(true);
    setDragDropMode(NoDragDrop);
    viewport()->setAcceptDrops(true);
    setDropIndicatorShown(true);
    //testing
//    QStandardItem *item = new QStandardItem(QString("test"));
//    model->setItem(2,0,item);
//    claus->tofsims // tool_with_files_measurements_groups<files_::tofsims_t,measurements_::tofsims_t,mgroups_::tofsims_t>
}

mgroups_treeview_t::selection_t mgroups_treeview_t::get_selection() const
{
    QModelIndexList indexes = this->selectionModel()->selectedIndexes();
    selection_t selection;
    for (auto& idx : indexes)
    {
        if (!idx.isValid())
            continue;
        if (idx.column()!=0)
            continue;
        if (idx.parent().parent().isValid()) // measurement item row selected
        {
            return selection_t(idx.parent().parent().row(),idx.parent().row(),idx.row());
        }
        else if (idx.parent().isValid()) // measurement_group / section group item row selected
        {
            return selection_t(idx.parent().row(),idx.row());
        }
        else if (idx.isValid()) // tool section item row selected
        {
            return selection_t(idx.row());
        }
    }
    return selection;
}

void mgroups_treeview_t::set_model()
{
    model->clear();
//    QStandardItem *item = nullptr;
//    model = new QStandardItemModel (total_lines_count(), total_columns_count);
//    model = new QStandardItemModel (sections::LAST, columns::col_LAST);
    setSelectionMode(QTreeView::SingleSelection);
    setModel(model);

//    tofsims_section.update();
//    dsims_section.update();
}

void mgroups_treeview_t::set_header()
{
//    model->setHorizontalHeaderLabels({"method","olcdb","group_id","settings"});
    QStringList list;
    list.reserve(col_LAST);
    for (int i=0;i<col_LAST;i++)
    {
        columns col = static_cast<columns>(i);
        if (column_names.find(col)==column_names.end())
        {
            logger.fatal(__func__,"column_names").value(std::to_string(col),10,"col not found in column_names <"+std::to_string(col_LAST) +">");
            return;
        }
        list.push_back(QString(column_names[col].c_str()));
    }
    model->setHorizontalHeaderLabels(list);
}

void mgroups_treeview_t::set_actions()
{
    delete_selection_action = new QAction("delete selected",this);
    ungroup_selection_action = new QAction("ungroup selected",this);
}

void mgroups_treeview_t::set_contextMenu()
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    contextMenu = new QMenu(this);
    contextMenu->addAction(delete_selection_action);
    contextMenu->addAction(ungroup_selection_action);
}
void mgroups_treeview_t::connect_signals_to_slots()
{
    connect(delete_selection_action,&QAction::triggered, this, &mgroups_treeview_t::delete_selection);
    connect(ungroup_selection_action,&QAction::triggered, this, &mgroups_treeview_t::ungroup_selection);
}

void mgroups_treeview_t::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Delete)
       delete_selection();
    else
        QTreeView::keyPressEvent(e);
}


void mgroups_treeview_t::ungroup_selected_group(const selection_t& selection)
{
//    const auto selection = get_selection();
    if (!selection.is_mgroup())
        return;

    if (selection.tool_section_row()==tofsims)
    {
        auto& MG = claus->tofsims.mgroups.at(selection.mgroup_row()-1);
        if (MG.measurements_p.size()>0)
            claus->tofsims.measurements.insert(claus->tofsims.measurements.end(),MG.measurements_p.begin(),MG.measurements_p.end());
        claus->tofsims.mgroups.erase(claus->tofsims.mgroups.begin()+selection.mgroup_row()-1);
    }

    if (selection.tool_section_row()==dsims)
    {
        auto& MG = claus->dsims.mgroups.at(selection.mgroup_row()-1);
        if (MG.measurements_p.size()>0)
            claus->dsims.measurements.insert(claus->dsims.measurements.end(),MG.measurements_p.begin(),MG.measurements_p.end());
        claus->dsims.mgroups.erase(claus->dsims.mgroups.begin()+selection.mgroup_row()-1);
    }
//    delete_selection();
    update();
    update_measurements_treeview();
}

void mgroups_treeview_t::ungroup_selected_measurement(const selection_t& selection)
{
    if (!selection.is_measurement())
        return;
    if (selection.mgroup_row()==0) // already in "ungrouped measurements"
        return;

    if (selection.tool_section_row()==tofsims)
    {
        auto& MG = claus->tofsims.mgroups.at(selection.mgroup_row()-1);
        auto& M = MG.measurements_p.at(selection.measurement_row());
        claus->tofsims.measurements.push_back(M);
        MG.measurements_p.erase(MG.measurements_p.begin()+selection.measurement_row());
    }

    if (selection.tool_section_row()==dsims)
    {
        auto& MG = claus->dsims.mgroups.at(selection.mgroup_row()-1);
        auto& M = MG.measurements_p.at(selection.measurement_row());
        claus->dsims.measurements.push_back(M);
        MG.measurements_p.erase(MG.measurements_p.begin()+selection.measurement_row());
    }
//    delete_selection();
    update();
    update_measurements_treeview();
}

void mgroups_treeview_t::ungroup_selection()
{
    const auto selection = get_selection();

    if (selection.is_mgroup())
        ungroup_selected_group(selection);

    if (selection.is_measurement())
        ungroup_selected_measurement(selection);
}

///new, using QVariant custom class pointers
void mgroups_treeview_t::delete_selection()
{
    bool go_update = false;
    QModelIndexList indexes = this->selectionModel()->selectedIndexes();

    if (dsims_section.delete_selection(indexes))
    {
        update();
        update_measurements_treeview();
        return;
    }
    if (tofsims_section.delete_selection(indexes))
    {
        update();
        update_measurements_treeview();
        return;
    }

}

//void mgroups_treeview_t::delete_selection()
//{
////    update();
////    return;
//    QModelIndexList indexes = this->selectionModel()->selectedIndexes();
//    bool go_update=false;

//    for (auto& idx : indexes)
//    {
//        if (!idx.isValid())
//            continue;
//        if (idx.column()!=0)
//            continue;
//        selection_t selection(idx);

//        if (selection.tool_section_row()==tofsims)
//        {
//            if (selection.is_ungrouped_measurement_section())
//            {
//                logger.debug(__func__,"tofsims").signal("selection.is_ungrouped_measurement_section()");
//                if (claus->tofsims.measurements.size()>0)
//                {
//                    claus->tofsims.measurements.clear();
//                    go_update=true; // this leads to segmentation fault
//                }
////                if (model->itemFromIndex(idx)->hasChildren())
////                    model->itemFromIndex(idx)->removeRows(0,model->itemFromIndex(idx)->rowCount());
//            }
//            else if (selection.is_mgroup())
//            {
//                logger.debug(__func__,"tofsims").signal("selection.is_mgroup()");
//                tools::vec::erase(claus->tofsims.mgroups,{(unsigned int)selection.mgroup_row()-1});
////                model->itemFromIndex(idx.parent())->removeRow(idx.row());
//                go_update=true;
//            }
//            else if (selection.is_measurement() && selection.mgroup_row()==0)
//            {
//                logger.debug(__func__,"tofsims").signal("selection.is_measurement() && selection.mgroup_row()==0");
//                tools::vec::erase(claus->tofsims.measurements,{(unsigned int)selection.measurement_row()});
////                model->itemFromIndex(idx.parent())->removeRow(idx.row());
//                go_update=true;
//            }
//            else if (selection.is_measurement() && selection.mgroup_row()>0)
//            {
//                logger.debug(__func__,"tofsims").signal("selection.is_measurement() && selection.mgroup_row()>0");
//                auto& MG = claus->tofsims.mgroups.at(selection.mgroup_row()-1);
//                tools::vec::erase(MG.measurements_p,{(unsigned int)selection.measurement_row()});
////                model->itemFromIndex(idx.parent())->removeRow(idx.row());
//                go_update=true;
//            }
//        }
//        else if (selection.tool_section_row()==dsims)
//        {
//            if (selection.is_ungrouped_measurement_section())
//            {
//                logger.debug(__func__,"dsims").signal("selection.is_ungrouped_measurement_section()");
//                if (claus->dsims.measurements.size()>0)
//                {
//                    claus->dsims.measurements.clear();
//                    go_update=true;
//                }
////                if (model->itemFromIndex(idx)->hasChildren())
////                    model->itemFromIndex(idx)->removeRows(0,model->itemFromIndex(idx)->rowCount());
//            }
//            else if (selection.is_mgroup())
//            {
//                logger.debug(__func__,"dsims").signal("selection.is_mgroup()");
//                tools::vec::erase(claus->dsims.mgroups,{(unsigned int)selection.mgroup_row()-1});
////                model->itemFromIndex(idx.parent())->removeRow(idx.row());
//                go_update=true;
//            }
//            else if (selection.is_measurement() && selection.mgroup_row()==0)
//            {
//                logger.debug(__func__,"dsims").signal("selection.is_measurement() && selection.mgroup_row()==0");
//                tools::vec::erase(claus->dsims.measurements,{(unsigned int)selection.measurement_row()});
////                model->itemFromIndex(idx.parent())->removeRow(idx.row());
//                go_update=true;
//            }
//            else if (selection.is_measurement() && selection.mgroup_row()>0)
//            {
//                logger.debug(__func__,"dsims").signal("selection.is_measurement() && selection.mgroup_row()>0");
//                auto& MG = claus->dsims.mgroups.at(selection.mgroup_row()-1);
//                tools::vec::erase(MG.measurements_p,{(unsigned int)selection.measurement_row()});
////                model->itemFromIndex(idx.parent())->removeRow(idx.row());
//                go_update=true;
//            }
//        }
//    }
//    if (go_update)
//    {
//        logger.debug(__func__,"go_update").signal("before update");
//        update();
//        logger.debug(__func__,"go_update").signal("updated");
//        update_measurements_treeview();
//        logger.debug(__func__,"go_update").signal("update_measurements_treeview");
//    }
//}

/**********************************/
/*****         slots            ***/
/**********************************/

void mgroups_treeview_t::update()
{
    logger.debug(__func__,"this").enter();


    logger.debug(__func__,"this").signal("dsims_section.tool_section_item(col_method)");
    model->setItem(dsims,col_method,dsims_section.tool_section_item(col_method));
    logger.debug(__func__,"this").signal("dsims_section.tool_section_item(col_sizes)");
    model->setItem(dsims,col_sizes,dsims_section.tool_section_item(col_sizes));


    logger.debug(__func__,"this").signal("tofsims_section.tool_section_item(col_method)");
    model->setItem(tofsims,col_method,tofsims_section.tool_section_item(col_method));
    logger.debug(__func__,"this").signal("tofsims_section.tool_section_item(col_sizes)");
    model->setItem(tofsims,col_sizes,tofsims_section.tool_section_item(col_sizes));

    logger.debug(__func__,"this").signal("resizeColumns");
//    resizeColumns();
//    model->setItem(tofsims,0,tofsims_section.tool_section_item());
    logger.debug(__func__,"this").exit();

}

