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


#include "measurements_treeview_t.hpp"


/*****************************************/
/**            parent_entry_t           **/
/*****************************************/

measurements_treeview_t::parent_entry_t::parent_entry_t(methods method_id, measurements_treeview_t *measurements_treeview) :
        method_id(method_id), measurements_treeview(measurements_treeview)
{

}

std::vector<unsigned int> measurements_treeview_t::parent_entry_t::get_selected_rows() const
{
    std::vector<unsigned int> selected_idxs;
    QModelIndexList indexes = measurements_treeview->selectionModel()->selectedIndexes();
    for (auto& idx : indexes)
    {
        if (!idx.isValid())
            continue;
        if (idx.parent().row()!=method_id)
            continue;
        if (idx.column()!=0)
            continue;
        selected_idxs.push_back(idx.row());
    }
    return selected_idxs;
}

std::vector<QStandardItem*> measurements_treeview_t::parent_entry_t::itemCols(const std::vector<std::vector<std::string>>& lines_and_cols )
{
    std::vector<QStandardItem*> items;
    items.reserve(2);
    QStandardItem *item, *child;
    std::stringstream label;

    label.str("");
    label << method_names[method_id] << " <" << lines_and_cols.size() << ">";
    item = new QStandardItem( QString(label.str().c_str())  );
    item->setEditable(false);
    item->setSelectable(false);
    int max_cols = 1;
    for (const auto& cols : lines_and_cols )
    {
        QList<QStandardItem*> childs;
        for (const auto& col : cols)
        {
            child = new QStandardItem (QString(col.c_str()));
            child->setEditable(false);
            childs << child;
        }
        item->appendRow(childs);
        if (cols.size()>max_cols)
            max_cols = cols.size();
    }
    items.push_back(item);

    if (max_cols<2)
        return items;

    for (int c=0;c<max_cols;c++)
    {
        item = new QStandardItem( QString("")  );
        item->setEditable(false);
        item->setSelectable(false);
        items.push_back(item);
    }
    return items;
}


/*****************************************/
/**     parent_entry_t::tofsims_t       **/
/*****************************************/

measurements_treeview_t::tofsims_t::tofsims_t(measurements_treeview_t *measurements_treeview) :
        parent_entry_t(tofsims,measurements_treeview), logger(global_logger,__FILE__,"measurements_treeview_t::tofsims_t")
{
}

std::vector<QStandardItem*> measurements_treeview_t::tofsims_t::itemCols()
{
    logger.debug(__func__,"this").enter();
    int s = claus->tofsims.measurements.size();
    std::vector<std::vector<std::string>> lines_and_cols(s);
    logger.debug(__func__,"claus->tofsims.measurements.size()").value(std::to_string(s));
    int c=0;

    for (auto& M : claus->tofsims.measurements)
    {
        logger.debug(__func__,"measurement.at("+std::to_string(c)+")").value(M.to_string_short());
        std::vector<std::string> cols_in_line(1);

        cols_in_line.at(0) = M.to_string_short();
        logger.debug(__func__,"measurement.at("+std::to_string(c)+")").signal("cols_in_line.at(0)");
        lines_and_cols.at(c) = cols_in_line;
        logger.debug(__func__,"measurement.at("+std::to_string(c)+")").signal("lines_and_cols.at(" + std::to_string(c)+")");
        c++;
    }

    logger.debug(__func__,"this").exit();
    return parent_entry_t::itemCols(lines_and_cols);
}

/*****************************************/
/**     parent_entry_t::dektak6m_t      **/
/*****************************************/

measurements_treeview_t::dektak6m_t::dektak6m_t(measurements_treeview_t *measurements_treeview) :
        parent_entry_t(dektak6m,measurements_treeview), logger(global_logger,__FILE__,"measurements_treeview_t::dektak6m_t")
{
}

std::vector<QStandardItem*> measurements_treeview_t::dektak6m_t::itemCols()
{
    logger.debug(__func__,"this").enter();
    int s = claus->dektak6m.measurements.size();
    std::vector<std::vector<std::string>> lines_and_cols(s);
    logger.debug(__func__,"claus->dektak6m.measurements.size()").value(std::to_string(s));
    int c=0;

    for (auto& M : claus->dektak6m.measurements)
    {
        logger.debug(__func__,"measurement.at("+std::to_string(c)+")").value(M.to_string_short());
        std::vector<std::string> cols_in_line(1);
        cols_in_line.at(0) = M.to_string_short();
        lines_and_cols.at(c) = cols_in_line;
        c++;
    }

    logger.debug(__func__,"this").exit();
    return parent_entry_t::itemCols(lines_and_cols);
}

/*****************************************/
/**     parent_entry_t::dsims_t       **/
/*****************************************/

measurements_treeview_t::dsims_t::dsims_t(measurements_treeview_t *measurements_treeview) :
        parent_entry_t(dsims,measurements_treeview), logger(global_logger,__FILE__,"measurements_treeview_t::dsims_t")
{
}

std::vector<QStandardItem*> measurements_treeview_t::dsims_t::itemCols()
{
    logger.debug(__func__,"this").enter();
    int s = claus->dsims.measurements.size();
    std::vector<std::vector<std::string>> lines_and_cols(s);
    logger.debug(__func__,"claus->dsims.measurements.size()").value(std::to_string(s));
    int c=0;
    for (auto& M : claus->dsims.measurements)
    {
        logger.debug(__func__,"measurement.at("+std::to_string(c)+")").value(M.to_string_short());
        std::vector<std::string> cols_in_line(1);
        cols_in_line.at(0) = M.to_string_short();
        logger.debug(__func__,"measurement.at("+std::to_string(c)+")").signal("cols_in_line.at(0)");
        lines_and_cols.at(c) = cols_in_line;
        logger.debug(__func__,"measurement.at("+std::to_string(c)+")").signal("lines_and_cols.at(" + std::to_string(c)+")");
        c++;
    }

    logger.debug(__func__,"this").exit();
    return parent_entry_t::itemCols(lines_and_cols);
}

/*****************************************/

std::map<measurements_treeview_t::methods,std::string> measurements_treeview_t::method_names
{
    {tofsims,"tofsims"},
    {dsims,"dsims"},
    {dektak6m, "dektak6m"}
};

measurements_treeview_t::measurements_treeview_t(QWidget *parent)
    : QTreeView(parent), parent(parent), logger(global_logger,__FILE__,"measurements_treeview_t")
{
    set_actions();
    set_contextMenu();
    connect_signals_to_slots();
    createModel();
//    setDragDropMode(DragDrop);
//    setDragEnabled(true);
    setAcceptDrops(true);
    viewport()->setAcceptDrops(true);
    setDropIndicatorShown(true);


//    update();
}

void measurements_treeview_t::createModel()
{
//    int total_lines_count = LAST;
    int total_columns_count = 1;
    QStandardItem *item = nullptr;
    model = new QStandardItemModel (total_lines_count(), total_columns_count);
    model->setHorizontalHeaderLabels({"method"});

    std::stringstream label;

    //tofsims
    label.str("");
    label  << method_names[tofsims];
    item = new QStandardItem( QString( label.str().c_str() ) );
    item->setEditable(false);
    item->setSelectable(false);
    model->setItem(tofsims,0,item);


    //dsims
    label.str("");
    label  << method_names[dsims];
    item = new QStandardItem( QString( label.str().c_str() ) );
    item->setEditable(false);
    item->setSelectable(false);
    model->setItem(dsims,0,item);

    //dektak6m
    label.str("");
    label  << method_names[dektak6m];
    item = new QStandardItem( QString( label.str().c_str() ) );
    item->setEditable(false);
    item->setSelectable(false);
    model->setItem(dektak6m,0,item);

    setSelectionMode(QTreeView::MultiSelection);
    setModel(model);
}

measurements_treeview_t::tofsims_t measurements_treeview_t::tofsims_entries()
{
    return tofsims_t(this);
}
measurements_treeview_t::dsims_t measurements_treeview_t::dsims_entries()
{
    return dsims_t(this);
}

measurements_treeview_t::dektak6m_t measurements_treeview_t::dektak6m_entries()
{
    return dektak6m_t(this);
}


void measurements_treeview_t::set_plot_window(QCustomPlot *measurement_plot_window_s)
{
    measurement_plot_window = measurement_plot_window_s;
}

void measurements_treeview_t::update(methods method_id, const std::vector<QStandardItem*>& itemCols)
{
    logger.debug(__func__,"this").enter(15, "method_id= " + method_names[method_id] + ", itemCols");
    logger.debug(__func__,"itemCols").signal("size = " + std::to_string(itemCols.size()));
    for (size_t col=0; col<itemCols.size(); col++)
    {
       logger.debug(__func__,"itemCols").signal("col = " + std::to_string(col));
       model->setItem(method_id,col,itemCols.at(col));
    }
    logger.debug(__func__,"this").exit();
}

void measurements_treeview_t::set_actions()
{
    group_selection_action = new QAction("group selected",this);
    delete_selection_action = new QAction("delete selected",this);
    auto_group_action = new QAction("auto group",this);

}
void measurements_treeview_t::set_contextMenu()
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    contextMenu = new QMenu(this);

    contextMenu->addAction(group_selection_action);
    contextMenu->addAction(delete_selection_action);
    contextMenu->addAction(auto_group_action);
}
void measurements_treeview_t::connect_signals_to_slots()
{
    connect(group_selection_action,&QAction::triggered, this, &measurements_treeview_t::group_selection);
    connect(delete_selection_action,&QAction::triggered, this, &measurements_treeview_t::delete_selection);
    connect(auto_group_action,&QAction::triggered, this, &measurements_treeview_t::auto_group);
}

int measurements_treeview_t::total_lines_count() const
{
    return LAST;
}

void measurements_treeview_t::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Delete)
    {
        if (selectionModel()==nullptr)
            return;
        bool erased = false;
        QModelIndexList indexes = selectionModel()->selectedIndexes();
        std::vector<unsigned int> deletions_idxs;

        //tofsims
        deletions_idxs = tofsims_entries().get_selected_rows();
        if (deletions_idxs.size()>0)
        {
            tools::vec::erase<measurements_::tofsims_t>(claus->tofsims.measurements,deletions_idxs);
            erased=true;
        }

        //dsims
        deletions_idxs = dsims_entries().get_selected_rows();
        if (deletions_idxs.size()>0)
        {
            tools::vec::erase<measurements_::dsims_t>(claus->dsims.measurements,deletions_idxs);
            erased=true;
        }

        //dektak6m
        deletions_idxs = dektak6m_entries().get_selected_rows();
        if (deletions_idxs.size()>0)
        {
            tools::vec::erase<measurements_::profilers_t::dektak6m_t>(claus->dektak6m.measurements,deletions_idxs);
            erased=true;
        }

        if (erased)
            update();
    }
    else
        QTreeView::keyPressEvent(e);
}


/**********************************/
/*****         slots            ***/
/**********************************/

void measurements_treeview_t::update()
{
    logger.debug(__func__,"this").enter();

    //tofsims
    update(tofsims, tofsims_entries().itemCols());
    logger.debug(__func__,"this").signal("tofsims_entries updated");
    setSelectionMode(QTreeView::MultiSelection);

    //dsims
    update(dsims, dsims_entries().itemCols());
    logger.debug(__func__,"this").signal("dsims_entries updated");
    setSelectionMode(QTreeView::MultiSelection);

    //dektak6m
    update(dektak6m, dektak6m_entries().itemCols());
    logger.debug(__func__,"this").signal("dektak6m_entries updated");
    setSelectionMode(QTreeView::MultiSelection);

    setModel(model);
    resizeColumnToContents(0);
    logger.debug(__func__,"this").exit();
    return;
}

void measurements_treeview_t::delete_selection()
{
    if (selectionModel()==nullptr)
        return;
    bool erased = false;
    QModelIndexList indexes = selectionModel()->selectedIndexes();
    std::vector<unsigned int> deletions_idxs;


    deletions_idxs = tofsims_entries().get_selected_rows();
    if (deletions_idxs.size()>0)
    {
        tools::vec::erase<measurements_::tofsims_t>(claus->tofsims.measurements,deletions_idxs);
        erased=true;
    }

    deletions_idxs = dsims_entries().get_selected_rows();
    if (deletions_idxs.size()>0)
    {
        tools::vec::erase<measurements_::dsims_t>(claus->dsims.measurements,deletions_idxs);
        erased=true;
    }

    deletions_idxs = dektak6m_entries().get_selected_rows();
    if (deletions_idxs.size()>0)
    {
        tools::vec::erase<measurements_::profilers_t::dektak6m_t>(claus->dektak6m.measurements,deletions_idxs);
        erased=true;
    }

    if (erased)
        update();
    return;
}

void measurements_treeview_t::group_selection()
{
    logger.debug(__func__,"this").enter();
    if (selectionModel()==nullptr)
        return;
    bool go_update=false;
    std::vector<unsigned int> selected_idxs;

    selected_idxs = tofsims_entries().get_selected_rows();
    if (selected_idxs.size()>0)
    {
        logger.debug(__func__,"tofsims selected_idxs.size()").value(selected_idxs.size());
        go_update = true;
        std::vector<measurements_::tofsims_t*> Ms;
        Ms.reserve(selected_idxs.size());
        for (auto idx : selected_idxs)
        {
            if (idx<claus->tofsims.measurements.size())
            {
                Ms.push_back(&claus->tofsims.measurements.at(idx));
            }
        }
        mgroups_::tofsims_t MG(Ms);
        claus->tofsims.mgroups.push_back(MG);
        tools::vec::erase<measurements_::tofsims_t>(claus->tofsims.measurements,selected_idxs);
    }

    selected_idxs = dsims_entries().get_selected_rows();
    if (selected_idxs.size()>0)
    {
        logger.debug(__func__,"dsims selected_idxs.size()").value(selected_idxs.size());
        go_update = true;
        std::vector<measurements_::dsims_t*> Ms;
        Ms.reserve(selected_idxs.size());
        for (auto idx : selected_idxs)
        {
            if (idx<claus->dsims.measurements.size())
            {
                Ms.push_back(&claus->dsims.measurements.at(idx));
            }
        }
        mgroups_::dsims_t MG(Ms);
        claus->dsims.mgroups.push_back(MG);
        tools::vec::erase<measurements_::dsims_t>(claus->dsims.measurements,selected_idxs);
    }

    if (go_update)
    {
        update();
        emit update_mgroups_treeview();
    }
    return;
}

void measurements_treeview_t::auto_group()
{
    bool go_update = false;
    if (claus->dsims.measurements.size()+claus->tofsims.measurements.size() > 0)
        go_update = true;
    claus->dsims.auto_group_ungrouped_measurements();
    claus->tofsims.auto_group_ungrouped_measurements();
    if (go_update)
    {
        update();
        emit update_mgroups_treeview();
    }
}
