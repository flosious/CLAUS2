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
//    auto& measurements = claus->tofsims.measurements();
    int s = claus->tofsims.measurements().size();
    std::vector<std::vector<std::string>> lines_and_cols(s);
    logger.debug(__func__,"claus->tofsims.measurements().size()").value(std::to_string(s));
//    for (size_t i=0; i < s;i++)
    int c=0;
//    auto Ms_copy = claus->tofsims.measurements();
//    for (auto& M : Ms_copy)
//        cout << M.to_string() << endl;

    for (auto& M : claus->tofsims.measurements())
    {
//        auto& M = claus->tofsims.measurements().at(i);
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
    {dektak6m, "dektak6m"},
    {p17, "p17"},
    {xps, "xps"}
};

measurements_treeview_t::measurements_treeview_t(QWidget *parent)
    : QTreeView(parent), parent(parent), logger(global_logger,__FILE__,"measurements_treeview_t")
{
    //sets the model
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

    setModel(model);
    logger.debug(__func__,"this").exit();
    return;
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
            tools::vec::erase<measurements_::tofsims_t>(claus->tofsims.measurements(),deletions_idxs);
            erased=true;
        }

        //dsims
        deletions_idxs = dsims_entries().get_selected_rows();
        if (deletions_idxs.size()>0)
        {
            tools::vec::erase<measurements_::dsims_t>(claus->dsims.measurements,deletions_idxs);
            erased=true;
        }

        if (erased)
            update();
    }
    else
        QTreeView::keyPressEvent(e);
}
