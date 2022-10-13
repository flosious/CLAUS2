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


#ifndef FILES_TREEVIEW_T_HPP
#define FILES_TREEVIEW_T_HPP

#include <iostream>
#include <QTreeView>
#include <QKeyEvent>
#include <QStandardItemModel>
#include "../src/processor.hpp"
#include <QMenu>

class files_treeview_t : public QTreeView
{
    Q_OBJECT
private:
    class_logger_t logger;
    ///this is the order of parent points in the treeview; leave "LAST" at last position
    enum methods {unknown_filenames, tofsims, dsims, dektak6m, jpg, aishu, LAST};
    static std::map<methods,std::string> method_names;

    class parent_entry_t
    {
    public:
        ///returns a vector of row indices
        std::vector<unsigned int> get_selected_rows() const;
        ///will transform selected files to measurements and essentially delete them from processor::files_p
    protected:
        parent_entry_t(methods method_id, files_treeview_t *files_treeview);
        virtual void selections_to_measurements();
        ///transform a string matrix to a proper items matrix useable by QtModels
        std::vector<QStandardItem*> itemCols(const std::vector<std::vector<std::string>>& lines_and_cols);
        methods method_id;
        ///the current model used in treeview
        files_treeview_t *files_treeview=nullptr;
    };
    class jpg_t : public parent_entry_t
    {
    public:
        jpg_t(files_treeview_t *files_treeview);
        std::vector<QStandardItem*> itemCols();
    };
    class dektak6m_t : public parent_entry_t
    {
    public:
        dektak6m_t(files_treeview_t *files_treeview);
        std::vector<QStandardItem*> itemCols();
    };
    class tofsims_t : public parent_entry_t
    {
    public:
        tofsims_t(files_treeview_t *files_treeview);
        std::vector<QStandardItem*> itemCols();
    };
    class dsims_t : public parent_entry_t
    {
    public:
        dsims_t(files_treeview_t *files_treeview);
        std::vector<QStandardItem*> itemCols();
    };
    class aishu_t : public parent_entry_t
    {
    public:
        aishu_t(files_treeview_t *files_treeview);
        std::vector<QStandardItem*> itemCols();
    };
    class unknown_filenames_t : public parent_entry_t
    {
    public:
        unknown_filenames_t(files_treeview_t *files_treeview);
        std::vector<QStandardItem*> itemCols();
    };
    QStandardItemModel* model;
protected:
    int total_lines_count() const;
    void keyPressEvent(QKeyEvent *e) override;
    ///sets the model
    void createModel();
    void update(methods method_id, const std::vector<QStandardItem*>& itemCols);
    void set_actions();
    void set_contextMenu();
    void connect_signals_to_slots();
public:
    files_treeview_t(QWidget *parent = nullptr);
//    void contextMenuEvent(QContextMenuEvent *event) override;
    QMenu* contextMenu;
    QAction* test_action;
    QAction* selections_to_measurements_action;
    QAction* delete_selection_action;

    unknown_filenames_t unknown_filenames_entries();
    tofsims_t tofsims_entries();
    dsims_t dsims_entries();
    aishu_t aishu_entries();
    dektak6m_t dektak6m_entries();
    jpg_t jpg_entries();
//    parent_entry_t dsims_entries();
//    parent_entry_t xps_entries();
//    parent_entry_t dektak6m_entries();
//    parent_entry_t p17_entries();
//    parent_entry_t camera_images_entries();

public slots:
    ///will reload from processor: klaus()->"method"->files
    void update();
    void test();
    void delete_selection();
    void selections_to_measurements();
signals:
    void update_measurements_treeview();
};
extern processor *claus;

#endif // FILES_TREEVIEW_T_HPP
