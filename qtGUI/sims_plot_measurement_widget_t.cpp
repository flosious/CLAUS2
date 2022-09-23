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

#include "sims_plot_measurement_widget_t.hpp"


sims_plot_measurement_widget_t::sims_plot_measurement_widget_t(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::sims_plot_measurement_widget_t)
{
    ui->setupUi(this);
    //give the cluster_tree something to plot, which is not NULL!
    ui->clusters_tree->set_sims_plot_window(ui->sims_plot_window);
}

sims_plot_measurement_widget_t::~sims_plot_measurement_widget_t()
{
    delete ui;
}
