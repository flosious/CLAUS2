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

std::map<mgroups_treeview_t::sections,std::string> mgroups_treeview_t::section_names
{
    {tofsims,"tofsims"},
    {dsims,"dsims"}
};

std::map<mgroups_treeview_t::columns,std::string> mgroups_treeview_t::column_names
{
    {col_method,"groups/measurements"},
    {col_sizes,"<size>"},
    {col_olcdb,"olcdb"},
    {col_group_id,"group id"},
    {col_settings,"settings"},
    {col_export,"export location"}
};



void mgroups_treeview_t::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
//    QTreeView::dataChanged(topLeft,bottomRight,roles);
    if (updating)
        return;
    log_f;
    logger.debug("this").enter();
    if (topLeft == bottomRight)
    {
        std::string name = model->itemFromIndex(topLeft)->text().toStdString();
        if (topLeft.column()==col_method)
        {
            if (dsims_section.is_mgroup(topLeft))
            {
                auto* MG = dsims_section.get_mgroup_from_QIndex(topLeft);
                MG->set_name(name);
                MG->set_export_location(MG->export_location());
                logger.debug("MG dsims name").signal(MG->name());
            }
            else if (tofsims_section.is_mgroup(topLeft))
            {
                auto* MG = tofsims_section.get_mgroup_from_QIndex(topLeft);
                MG->set_name(name);
                MG->set_export_location(MG->export_location());
                logger.debug("MG tofsims name").signal(MG->name());
            }
            update();
        }
        else if (topLeft.column()==col_export)
        {
            std::string new_export_location = tools::file::check_directory_string(name);
            if (dsims_section.is_mgroup(topLeft.siblingAtColumn(0)))
            {
                dsims_section.get_mgroup_from_QIndex(topLeft.siblingAtColumn(0))->set_export_location(new_export_location);
            }
            else if (dsims_section.is_grouped_measurement(topLeft.siblingAtColumn(0)))
            {
                dsims_section.get_measurement_from_QIndex(topLeft.siblingAtColumn(0))->export_location = new_export_location;
            }
            else if (tofsims_section.is_mgroup(topLeft.siblingAtColumn(0)))
            {
                tofsims_section.get_mgroup_from_QIndex(topLeft.siblingAtColumn(0))->set_export_location(new_export_location);
            }
            else if (tofsims_section.is_grouped_measurement(topLeft.siblingAtColumn(0)))
            {
                tofsims_section.get_measurement_from_QIndex(topLeft.siblingAtColumn(0))->export_location = new_export_location;
            }
            logger.info("new_export_location").value(new_export_location);
            update();
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
    class_logger(__FILE__,"mgroups_treeview_t"), parent(parent), model(new QStandardItemModel (sections::LAST, columns::col_LAST)),
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


void mgroups_treeview_t::saveState()
{
    log_f;
    logger.debug("this").enter();
//    persistentModelIndexList.clear();
//    ///all data is carried in the 0th column item/index
//    for (int r=0;r<model->rowCount();r++)
//    {
//        QModelIndex index = model->index(r,0);
//        if (isExpanded(index))
//        {
//            index.data()=1;
//        }
//        //save childrens to this index
//        QPersistentModelIndex persistentIndex(index);
//        persistentModelIndexList << persistentIndex;
//    }
//    QModelIndex index = model->index(dsims,0);
//    if (isExpanded(index))
//        dsims_section.expanded=true;
//    else
//        dsims_section.expanded=false;
    dsims_section.saveStates_to_expanded_mgroups_position_list(this);
    tofsims_section.saveStates_to_expanded_mgroups_position_list(this);
}

void mgroups_treeview_t::restoreState()
{
    log_f;
    logger.debug("this").enter();
    dsims_section.restoreStates_from_expanded_mgroups_position_list(this);
    tofsims_section.restoreStates_from_expanded_mgroups_position_list(this);
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
    log_f;
//    model->setHorizontalHeaderLabels({"method","olcdb","group_id","settings"});
    QStringList list;
    list.reserve(col_LAST);
    for (int i=0;i<col_LAST;i++)
    {
        columns col = static_cast<columns>(i);
        if (column_names.find(col)==column_names.end())
        {
            logger.fatal("column_names").value(std::to_string(col),10,"col not found in column_names <"+std::to_string(col_LAST) +">");
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

//void mgroups_treeview_t::show_selection_in_log()
//{
//    log_f;
//    logger.debug("this").enter();
//    bool go_update = false;
//    QModelIndexList indexes = this->selectionModel()->selectedIndexes();

//    dsims_section.show_selection_in_log(indexes);
//    tofsims_section.show_selection_in_log(indexes);
//    logger.debug("this").exit();
//}

void mgroups_treeview_t::ungroup_selection()
{
    bool go_update = false;
    QModelIndexList indexes = this->selectionModel()->selectedIndexes();

    if (dsims_section.ungroup_selection(indexes))
    {
        go_update = true;
    }
    if (tofsims_section.ungroup_selection(indexes))
    {
        go_update = true;
    }
    if (go_update)
    {
        update();
        update_measurements_treeview();
    }
}

/////new, using QVariant custom class pointers
void mgroups_treeview_t::delete_selection()
{
//    saveState();
    bool go_update = false;
    QModelIndexList indexes = this->selectionModel()->selectedIndexes();

    if (dsims_section.delete_selection(indexes))
    {
        go_update = true;
    }
    if (tofsims_section.delete_selection(indexes))
    {
        go_update = true;
    }
    if (go_update)
    {
        update();
        update_measurements_treeview();
    }
}

/**********************************/
/*****         slots            ***/
/**********************************/

void mgroups_treeview_t::update()
{
    log_f;
    logger.debug("this").enter();
    updating=true;
    saveState();
//    model->removeRows(0,2);
    logger.debug("this").signal("dsims_section.tool_section_item(col_method)");
    model->setItem(dsims,col_method,dsims_section.tool_section_item(col_method));
    logger.debug("this").signal("dsims_section.tool_section_item(col_sizes)");
    model->setItem(dsims,col_sizes,dsims_section.tool_section_item(col_sizes));


    logger.debug("this").signal("tofsims_section.tool_section_item(col_method)");
    model->setItem(tofsims,col_method,tofsims_section.tool_section_item(col_method));
    logger.debug("this").signal("tofsims_section.tool_section_item(col_sizes)");
    model->setItem(tofsims,col_sizes,tofsims_section.tool_section_item(col_sizes));

    restoreState();
    updating=false;
//    logger.debug("this").signal("resizeColumns");
//    resizeColumns();
//    model->setItem(tofsims,0,tofsims_section.tool_section_item());
    logger.debug("this").exit();

}

