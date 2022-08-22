#include "files_treeview_t.hpp"

/*****************************************/
/**            parent_entry_t           **/
/*****************************************/

files_treeview_t::parent_entry_t::parent_entry_t(methods method_id, files_treeview_t *files_treeview) :
        method_id(method_id), files_treeview(files_treeview)
{

}

std::vector<unsigned int> files_treeview_t::parent_entry_t::get_selected_rows() const
{
    std::vector<unsigned int> selected_idxs;
    QModelIndexList indexes = files_treeview->selectionModel()->selectedIndexes();
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

void files_treeview_t::parent_entry_t::selections_to_measurements()
{

}

std::vector<QStandardItem*> files_treeview_t::parent_entry_t::itemCols(const std::vector<std::vector<std::string>>& lines_and_cols )
{
//    if (lines_and_cols.size()==0)
//        return {};
    std::vector<QStandardItem*> items;
    items.reserve(2);
    QStandardItem *item, *child;
    std::stringstream label;
    label.str("");
    label << method_names[method_id] << " <" << lines_and_cols.size() << ">";
    item = new QStandardItem( QString(label.str().c_str())  );
    item->setEditable(false);
    item->setSelectable(false);

    for (const auto& cols : lines_and_cols )
    {
        QList<QStandardItem*> childs;
        for (const auto& col : cols)
        {
            child = new QStandardItem (QString(col.c_str()));
            childs << child;
        }
        item->appendRow(childs);
    }
    items.push_back(item);
    item = new QStandardItem( QString("")  );
    item->setEditable(false);
    item->setSelectable(false);
    items.push_back(item);
    return items;
}


/*****************************************/
/**     parent_entry_t::tofsims_t       **/
/*****************************************/

files_treeview_t::tofsims_t::tofsims_t(files_treeview_t *files_treeview) :
        parent_entry_t(tofsims,files_treeview)
{
}

std::vector<QStandardItem*> files_treeview_t::tofsims_t::itemCols()
{
    auto& files = claus->tofsims.files();
    std::vector<std::vector<std::string>> lines_and_cols(files.size());
    for (size_t i=0; i < files.size();i++) // tofsims.files() is temporary file
    {
        auto& F = files.at(i);
        std::vector<std::string> cols_in_line(2);
        cols_in_line.at(0) = F.name.filename_with_path.c_str();
        cols_in_line.at(1) = F.contents.to_string();
        lines_and_cols.at(i) = cols_in_line;
    }
    return parent_entry_t::itemCols(lines_and_cols);
}


/*****************************************/
/**     parent_entry_t::dsims_t       **/
/*****************************************/

files_treeview_t::dsims_t::dsims_t(files_treeview_t *files_treeview) :
        parent_entry_t(dsims,files_treeview)
{
}

std::vector<QStandardItem*> files_treeview_t::dsims_t::itemCols()
{
    auto& files = claus->dsims.files;
    std::vector<std::vector<std::string>> lines_and_cols(files.size());
    for (size_t i=0; i < files.size();i++)
    {
        auto& F = files.at(i);
        std::vector<std::string> cols_in_line(2);
        cols_in_line.at(0) = F.name.filename_with_path.c_str();
        cols_in_line.at(1) = F.contents.to_string();
        lines_and_cols.at(i) = cols_in_line;
    }
    return parent_entry_t::itemCols(lines_and_cols);
}

/*****************************************/
/**     parent_entry_t::aishu       **/
/*****************************************/

files_treeview_t::aishu_t::aishu_t(files_treeview_t *files_treeview) :
        parent_entry_t(aishu,files_treeview)
{
}

std::vector<QStandardItem*> files_treeview_t::aishu_t::itemCols()
{
    auto& files = claus->aishu.files;
    std::vector<std::vector<std::string>> lines_and_cols(files.size());
    for (size_t i=0; i < files.size();i++) // tofsims.files() is temporary file
    {
        auto& F = files.at(i);
        std::vector<std::string> cols_in_line(2);
        cols_in_line.at(0) = F.name.filename_with_path.c_str();
        cols_in_line.at(1) = F.contents.to_string();
        lines_and_cols.at(i) = cols_in_line;
    }
    return parent_entry_t::itemCols(lines_and_cols);
}

/*****************************************/
/**     parent_entry_t::unknown_filenames_t       **/
/*****************************************/

files_treeview_t::unknown_filenames_t::unknown_filenames_t(files_treeview_t *files_treeview) :
        parent_entry_t(unknown_filenames,files_treeview)
{
}

std::vector<QStandardItem*> files_treeview_t::unknown_filenames_t::itemCols()
{
    const auto& files = claus->unknown_filenames;

    std::vector<std::vector<std::string>> lines_and_cols(files.size());

    for (size_t i=0; i < files.size();i++) // tofsims.files() is temporary file
    {
        auto& F = files.at(i);
        std::vector<std::string> cols_in_line(2);
        cols_in_line.at(0) = F;
        cols_in_line.at(1) = "not recognized";
        lines_and_cols.at(i) = cols_in_line;
    }

    return parent_entry_t::itemCols(lines_and_cols);
}

/*****************************************/

std::map<files_treeview_t::methods,std::string> files_treeview_t::method_names
{
    {unknown_filenames,"unknown filenames"},
    {tofsims,"tofsims"},
    {aishu,"aishu"},
    {dsims,"dsims"},
    {dektak6m, "dektak6m"},
    {p17, "p17"},
    {xps, "xps"}
};

files_treeview_t::files_treeview_t(QWidget *parent) : QTreeView(parent), logger(global_logger,__FILE__,"files_treeview_t")
{
    //sets the model
    createModel();
//    update();
}

void files_treeview_t::createModel()
{
//    int total_lines_count = LAST;
    int total_columns_count = 2;
    QStandardItem *item = nullptr;
    model = new QStandardItemModel (total_lines_count(), total_columns_count);
    model->setHorizontalHeaderLabels({"method","details"});
    std::stringstream label;

    //unknown_filenames
	label.str("");
    label  << method_names[unknown_filenames];
    item = new QStandardItem( QString( label.str().c_str() ) );
	item->setEditable(false);
	item->setSelectable(false);
	model->setItem(unknown_filenames,0,item);
	item = new QStandardItem( QString("") );
	item->setEditable(false);
    item->setSelectable(false);
	model->setItem(unknown_filenames,1,item);

    //tofsims
    if (claus->tofsims.files().size()>0)
    {
        label.str("");
        label  << method_names[tofsims];
        item = new QStandardItem( QString( label.str().c_str() ) );
        item->setEditable(false);
        item->setSelectable(false);
        model->setItem(tofsims,0,item);
        item = new QStandardItem( QString("") );
        item->setEditable(false);
        item->setSelectable(false);
        model->setItem(tofsims,1,item);
    }

    //dsims
    if (claus->dsims.files.size()>0)
    {
        label.str("");
        label  << method_names[dsims];
        item = new QStandardItem( QString( label.str().c_str() ) );
        item->setEditable(false);
        item->setSelectable(false);
        model->setItem(dsims,0,item);
        item = new QStandardItem( QString("") );
        item->setEditable(false);
        item->setSelectable(false);
        model->setItem(dsims,1,item);
    }

    //aishu
    if (claus->aishu.files.size()>0)
    {
        label.str("");
        label  << method_names[aishu];
        item = new QStandardItem( QString( label.str().c_str() ) );
        item->setEditable(false);
        item->setSelectable(false);
        model->setItem(aishu,0,item);
        item = new QStandardItem( QString("") );
        item->setEditable(false);
        item->setSelectable(false);
        model->setItem(aishu,1,item);
    }
    setSelectionMode(QTreeView::MultiSelection);
    setModel(model);
}

files_treeview_t::unknown_filenames_t files_treeview_t::unknown_filenames_entries()
{
    return unknown_filenames_t(this);
}

files_treeview_t::tofsims_t files_treeview_t::tofsims_entries()
{
    return tofsims_t(this);
}
files_treeview_t::dsims_t files_treeview_t::dsims_entries()
{
    return dsims_t(this);
}
files_treeview_t::aishu_t files_treeview_t::aishu_entries()
{
    return aishu_t(this);
}

void files_treeview_t::update(methods method_id, const std::vector<QStandardItem*>& itemCols)
{
    for (int col=0; col<itemCols.size(); col++)
       model->setItem(method_id,col,itemCols.at(col));
}

void files_treeview_t::update()
{
    logger.debug(__func__,"this").enter();
    //unknown_filenames
    update(unknown_filenames, unknown_filenames_entries().itemCols());
    logger.debug(__func__,"this").signal("unknown_filenames updated");
    //tofsims
    update(tofsims, tofsims_entries().itemCols());
    logger.debug(__func__,"this").signal("tofsims_entries updated");
    //dsims
    update(dsims, dsims_entries().itemCols());
    logger.debug(__func__,"this").signal("dsims_entries updated");
    //aishu
    update(aishu, aishu_entries().itemCols());
    logger.debug(__func__,"this").signal("aishu_entries updated");

    setSelectionMode(QTreeView::MultiSelection);
    setModel(model);
    logger.debug(__func__,"this").exit();
    return;
}

int files_treeview_t::total_lines_count() const
{
    return LAST;
}

void files_treeview_t::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Delete)
    {
        if (selectionModel()==nullptr)
            return;
		bool erased = false;
        QModelIndexList indexes = selectionModel()->selectedIndexes();
        std::vector<unsigned int> deletions_idxs;


        deletions_idxs = tofsims_entries().get_selected_rows();
        if (deletions_idxs.size()>0)
        {
            tools::vec::erase<files_::tofsims_t>(claus->tofsims.files(),deletions_idxs);
            erased=true;
        }

        deletions_idxs = dsims_entries().get_selected_rows();
        if (deletions_idxs.size()>0)
        {
            tools::vec::erase<files_::dsims_t>(claus->dsims.files,deletions_idxs);
            erased=true;
        }

        deletions_idxs = aishu_entries().get_selected_rows();
        if (deletions_idxs.size()>0)
        {
            tools::vec::erase<files_::aishu_t>(claus->aishu.files,deletions_idxs);
//            auto T = claus->tofsims.files();
            erased=true;
        }
        deletions_idxs = unknown_filenames_entries().get_selected_rows();
        if (deletions_idxs.size()>0)
        {
            tools::vec::erase(claus->unknown_filenames,deletions_idxs);
			erased=true;
		}
        if (erased)
			update();
    }
    else
		QTreeView::keyPressEvent(e);
}
