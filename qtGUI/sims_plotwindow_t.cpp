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

#include "sims_plotwindow_t.hpp"

/**************************/
/******   axIs_t     ******/
/**************************/
void sims_plotwindow_t::axis_t::set_label()
{
    std::stringstream label;
    if (quantity_p.is_set())
        label << quantity_p.name() << " [" << quantity_p.unit().to_string() <<  "]";
    else
        label << "unknown";
    qcpAxis->setLabel(label.str().c_str());
}

sims_plotwindow_t::axis_t::axis_t(QCPAxis *qcpAxis_s, const quantity::quantity_t& quantity_s) :
    quantity_p(quantity_s,{1}), qcpAxis(qcpAxis_s), log_c
{
    set_label();
}

bool sims_plotwindow_t::axis_t::change_unit(unit_t new_unit)
{
    auto new_q = quantity_p.change_unit(new_unit);
    if (new_q.is_set())
    {
        quantity_p = new_q;
        set_label();
        return true;
    }
    return false;
}


void sims_plotwindow_t::axis_t::set_quantity(const quantity::quantity_t& quantity_s)
{
    quantity_p = quantity_s;
    set_label();
}

void sims_plotwindow_t::axis_t::unset()
{
    quantity_p.clear();
    set_label();
//    qcpAxis->setVisible(false);
}

void sims_plotwindow_t::axis_t::set_scale_linear()
{
    qcpAxis->setScaleType(QCPAxis::stLinear);
    QSharedPointer<QCPAxisTicker> linTicker(new QCPAxisTicker);
    qcpAxis->setTicker(linTicker);
}
void sims_plotwindow_t::axis_t::set_scale_logarithmic()
{
    qcpAxis->setScaleType(QCPAxis::stLogarithmic);
    QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
    qcpAxis->setTicker(logTicker);
}

void sims_plotwindow_t::axis_t::set_range(quantity::quantity_t start, quantity::quantity_t stop)
{
    start = start.change_unit(quantity_p.unit());
    stop = stop.change_unit(quantity_p.unit());
    if (!start.is_set())
        return;
    if (!stop.is_set())
        return;
    qcpAxis->setRange(start.data().front(), stop.data().front());
}

void sims_plotwindow_t::axis_t::set_visible(bool visibility)
{
    qcpAxis->setVisible(visibility);
}

void sims_plotwindow_t::axis_t::rescale_to_full_multiple_of(double multiple)
{
    log_f;
    qcpAxis->rescale();
    double start, stop;

    if (qcpAxis->scaleType()==QCPAxis::stLogarithmic)
    {
        stop = pow(10,ceil(log10(qcpAxis->range().upper)));
        start = stop/1E6;
    }
    else
    {
        stop = ceil(qcpAxis->range().upper / multiple) * multiple;
        start = floor(qcpAxis->range().lower / multiple) * multiple;
    }
    qcpAxis->setRange(start,stop);
}

const quantity::quantity_t& sims_plotwindow_t::axis_t::quantity() const
{
    return quantity_p;
}

/*****************************/
/********  x_axis_t  *********/
/*****************************/

void sims_plotwindow_t::x_axis_t::update_quantity()
{
    switch (option)
    {
    case options::sputter_depth:
    {
        set_quantity(quantity::sputter_depth_t({1},{"nm"}));
        return;
    }
    case options::sputter_time:
    {
        set_quantity(quantity::sputter_time_t({1},{"s"}));
        return;
    }
    case options::sputter_points:
    {
        quantity::quantity_t Q("sputter_points",{1},units::SI::one);
        set_quantity(Q);
        return;
    }
    }

}

sims_plotwindow_t::x_axis_t::x_axis_t(sims_plotwindow_t* sims_plotwindow, QCPAxis *qcpAxis_s, options option) :
    axis_t(qcpAxis_s,{}), sims_plotwindow(sims_plotwindow)
{
    update_quantity();
}

quantity::quantity_t sims_plotwindow_t::x_axis_t::quantity_from_crater(const crater_t& crater) const
{
    switch (option)
    {
    case options::sputter_depth:
    {
        return crater.sputter_depth;
    }
    case options::sputter_time:
    {
        return crater.sputter_time;
    }
    case options::sputter_points:
    {
        return crater.sputter_points();
    }
    }
    return {};
}

quantity::quantity_t sims_plotwindow_t::x_axis_t::quantity_from_measurement(const measurements_::sims_t &measurement) const
{
    return quantity_from_crater(measurement.crater);
}

void sims_plotwindow_t::x_axis_t::change(options axis_option)
{
    option = axis_option;
    update_quantity();
}

/*****************************/

/**************************/
/******   axEs_t     ******/
/**************************/
sims_plotwindow_t::axes_t::axes_t(QCPAxisRect *axisRect_s, sims_plotwindow_t *sims_plotwindow) :
    intensity(axisRect_s->axis(QCPAxis::atLeft),quantity::intensity_t({1},{"c/s"})),
    concentration_abs(axisRect_s->addAxis(QCPAxis::atRight),quantity::concentration_t({1},{"at/ccm"})),
    concentration_rel(axisRect_s->addAxis(QCPAxis::atRight),quantity::concentration_rel_t({1},units::derived::atom_percent,quantity::dimensions::SI::relative)),
//    sputter_time(quantity::sputter_time_t({1},{"s"}),axisRect_s->axis(QCPAxis::atBottom)),
//    sputter_depth(quantity::sputter_depth_t({1},{"nm"}),axisRect_s->axis(QCPAxis::atBottom))
    x(sims_plotwindow,axisRect_s->axis(QCPAxis::atBottom))
{
    set_x_axis_as_sputter_time();
    set_intensity_axis();
    set_concentration_abs_axis();
    set_concentration_rel_axis();
}

void sims_plotwindow_t::axes_t::set_x_axis_as_sputter_time()
{
    x.set_scale_linear();
    x.set_range(quantity::sputter_time_t({0}),quantity::sputter_time_t({15},units::derived::min));
    x.set_visible(true);
}

void sims_plotwindow_t::axes_t::set_intensity_axis()
{
    intensity.set_scale_logarithmic();
    intensity.set_range(quantity::intensity_t({1}), quantity::intensity_t({1E6}));
    intensity.set_visible(true);
}

void sims_plotwindow_t::axes_t::set_concentration_abs_axis()
{
    concentration_abs.set_scale_logarithmic();
    concentration_abs.set_range(quantity::concentration_t({1E15}), quantity::concentration_t({1E21}));
    concentration_abs.set_visible(false);
}

void sims_plotwindow_t::axes_t::set_concentration_rel_axis()
{
    concentration_rel.set_scale_linear();
    concentration_rel.set_range(quantity::concentration_rel_t({0}), quantity::concentration_rel_t({100}));
    concentration_rel.set_visible(false);
}


/**************************/

const std::map<int, QColor> sims_plotwindow_t::iterate_colors
{
    { 0,QColor(0,        0,      0)}, //black

    { 1,QColor(255,      0,      0)}, //red
    { 2,QColor(0,        255,    0)}, //green
    { 3,QColor(0,        0,      255)}, //blue

    { 4,QColor(255,      255,    0)},
    { 5,QColor(0,        255,    255)},
    { 6,QColor(255,        0,    255)},

    { 7,QColor(255/2,    255/1,  255/2)},
    { 8,QColor(255/2,    255/2,  255/1)},
    { 9,QColor(255/1,    255/2,  255/2)}
};

//void sims_plotwindow_t::make_window()
//{
//    plotLayout()->clear();
//    QCPAxisRect *wideAxisRect = new QCPAxisRect(this);
////    wideAxisRect->setupFullAxesBox(true);
//    axes.set_axes(wideAxisRect);
//}

sims_plotwindow_t::sims_plotwindow_t(QWidget *parent) : QCustomPlot(parent), class_logger(__FILE__,"sims_plotwindow_t"), axes(axisRect(),this)
{
//    axisRect()->axis(QCPAxis::atLeft, 1);
//    make_window();
    setInteraction(QCP::iRangeDrag,true);
    setInteraction(QCP::iRangeZoom,true);
    setInteraction(QCP::iSelectPlottables);
//    setSelectionRectMode(QCP::srmZoom);
    axisRect()->setRangeZoomAxes(QList{axes.x.qcpAxis,axes.intensity.qcpAxis,axes.concentration_abs.qcpAxis,axes.concentration_rel.qcpAxis});
    axisRect()->setRangeDragAxes(QList{axes.x.qcpAxis,axes.intensity.qcpAxis,axes.concentration_abs.qcpAxis,axes.concentration_rel.qcpAxis});
}

QColor sims_plotwindow_t::get_color_from_graph_id(int graph_id)
{
    return iterate_colors.at(graph_id % iterate_colors.size());
}


//void sims_plotwindow_t::set_xAxis_quantity(const quantity::quantity_t& X)
//{
//    if (X.is_set())
//    {
//        xAxis_quantity = X;
//        xAxis->setLabel((X.name() +"[" + X.unit().to_string()+"]").c_str());
//        xAxis->setRange(0,X.data().back());
//    }
//}


//int sims_plotwindow_t::add_Y_quantity(const quantity::quantity_t& Y)
//{
//    log_f;

//    int graph_id = -1;
//    if (!Y.is_set() )
//    {
//        return -1;
//    }
//    if (!xAxis_quantity.is_set())
//    {
//        xAxis_quantity = quantity::quantity_t("#",Y.data_X_1D(),units::SI::one); // get the index positions, that means a vector of 0,1,2,3,4,5,6,...,size()-1
//    }
//    if (xAxis_quantity.data().size() != Y.data().size())
//    {
//        return -1;
//    }
//    ///intensity?
////    if (Y.dimension() == quantity::intensity_t().dimension())
//    if (Y.change_unit("c/s").is_set())
//    {
//        addGraph(axes.x.qcpAxis, axes.intensity.qcpAxis); // left + bottom
//        graph_id = graphCount()-1;
//        logger.debug(Y.to_string()).signal("added to graph",15,"graph_id="+ std::to_string(graph_id) + " xAxis1; yAxis1");

//    }
//    ///concentration?
//    else if (Y.dimension() == quantity::concentration_t().dimension())
//    {

//    }
//    else
//    {
//        logger.error(Y.to_string()).signal("could not add; unknown Axis");
//    }
//    if (graph_id>=0)
//    {
//        graph(graph_id)->setData(QVector<double>::fromStdVector(xAxis_quantity.data()), QVector<double>::fromStdVector(Y.data()) );
//        graph(graph_id)->setPen(get_color_from_graph_id(graph_id));
//        graph(graph_id)->setName(Y.name().c_str());
//        axes.intensity.qcpAxis->rescale();


//        replot();
//    }

//    return graph_id;
//}

sims_plotwindow_t::axis_t* sims_plotwindow_t::get_y_axis(const quantity::quantity_t& Y)
{
    sims_plotwindow_t::axis_t *y_axis_s = nullptr;
    if (Y.dimension() == axes.intensity.quantity().dimension())
    {
        y_axis_s = &axes.intensity;
    }
    else if (Y.dimension() == axes.concentration_abs.quantity().dimension())
    {
        y_axis_s =  &axes.concentration_abs;
    }
    else if (Y.dimension() == axes.concentration_rel.quantity().dimension())
    {
        y_axis_s = &axes.concentration_rel;

    }
    else
        y_axis_s = nullptr;

    return y_axis_s;
}

int sims_plotwindow_t::add_graph(const crater_t &crater, quantity::quantity_t Y)
{
    return add_graph(axes.x.quantity_from_crater(crater),Y);
}

int sims_plotwindow_t::add_graph(quantity::quantity_t X,  quantity::quantity_t Y)
{
    int graph_id = -1;
    log_f;
    X = X.change_unit(axes.x.quantity().unit());
    if (!X.is_set())
        return -1;

    auto y_axis = get_y_axis(Y);
    if (y_axis == nullptr)
    {
        logger.error(Y.to_string()).signal("unknown axis (dimension)");
        return -1;
    }
    Y = Y.change_unit(y_axis->quantity().unit());
    if (!Y.is_set())
    {
        logger.error(Y.to_string()).signal("could not transform unit to " + y_axis->quantity().unit().to_string());
        return -1;
    }
    addGraph(axes.x.qcpAxis, y_axis->qcpAxis);
    graph_id = graphCount()-1;
    logger.debug(Y.to_string()).signal("added graph_id: "+ std::to_string(graph_id),15,Y.to_string_detailed());
    graph(graph_id)->setData(QVector<double>(X.data().begin(),X.data().end()), QVector<double>(Y.data().begin(),Y.data().end()) );
    graph(graph_id)->setPen(get_color_from_graph_id(graph_id));
    graph(graph_id)->setName(Y.name().c_str());
    y_axis->set_visible(true);
    y_axis->rescale_to_full_multiple_of(10);
    axes.x.rescale_to_full_multiple_of(10);
//    rescaleAxes();

    return graph_id;
}

int sims_plotwindow_t::add_graph(const quantity::map_t &XY)
{
    return add_graph(XY.X(),XY.Y());
}

