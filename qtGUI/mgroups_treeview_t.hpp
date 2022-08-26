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
#include "../src/mgroup.hpp"
#include "../src/measurement.hpp"

template<typename G,typename M>
class section_t
{
public:
    section_t(G& group, const std::vector<M*>& measurements);
    std::map<QStandardItem*,G*> group_item;
    std::map<QStandardItem*, M*> measurement_items;
};

class mgroups_treeview : public QTreeView
{
    Q_OBJECT
private:
//    std::map<QStandardItem*,>
public:
    mgroups_treeview();
};

#endif // MGROUPS_TREEVIEW_T_H
