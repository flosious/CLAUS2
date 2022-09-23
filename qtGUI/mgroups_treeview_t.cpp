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
        std::string name = model->itemFromIndex(topLeft)->text().toStdString();

        if (dsims_section.is_mgroup(topLeft))
        {
            auto* MG = dsims_section.get_mgroup_from_QIndex(topLeft);
            MG->name =name;
            logger.debug(__func__,"MG tofsims name").signal(MG->name);
        }
        else if (tofsims_section.is_mgroup(topLeft))
        {
            auto* MG = tofsims_section.get_mgroup_from_QIndex(topLeft);
            MG->name =name;
            logger.debug(__func__,"MG tofsims name").signal(MG->name);
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

