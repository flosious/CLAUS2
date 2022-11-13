QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../src/calc.cpp \
    ../src/calc.sims.cpp \
    ../src/calc.sims.matrix.cpp \
    ../src/cluster.cpp \
    ../src/config.cpp \
    ../src/crater.cpp \
    ../src/data_collector.cpp \
    ../src/database_t.cpp \
    ../src/definitions.cpp \
    ../src/element.cpp \
    ../src/filenames_collector_t.cpp \
    ../src/files.aishu.cpp \
    ../src/files.dsims.contents.cpp \
    ../src/files.dsims.cpp \
    ../src/files.dsims.name.cpp \
    ../src/files.file.contents.cpp \
    ../src/files.file.name.cpp \
    ../src/files.jpg.cpp \
    ../src/files.profilers.dektak6m.cpp \
    ../src/files.profilers.profiler.cpp \
    ../src/files.sims.contents.cpp \
    ../src/files.sims.cpp \
    ../src/files.sims.name.cpp \
    ../src/files.tofsims.contents.cpp \
    ../src/files.tofsims.cpp \
    ../src/files.tofsims.name.cpp \
    ../src/fit_functions.cpp \
    ../src/histogram.cpp \
    ../src/ion.cpp \
    ../src/isotope.cpp \
    ../src/log.cpp \
    ../src/measurement.cpp \
    ../src/measurement.dsims.cpp \
    ../src/measurement.profilers.cpp \
    ../src/measurement.sims.calc.cpp \
    ../src/measurement.sims.calc.implant.cpp \
    ../src/measurement.sims.cpp \
    ../src/measurement.tofsims.cpp \
    ../src/mgroup.cpp \
    ../src/mgroup.dsims.cpp \
    ../src/mgroup.sims.calc.cpp \
    ../src/mgroup.sims.calc.matrix.cpp \
    ../src/mgroup.sims.cpp \
    ../src/mgroup.sims.pbp.cpp \
    ../src/mgroup.tofsims.cpp \
    ../src/msettings.cpp \
    ../src/msettings.dsims.cpp \
    ../src/msettings.sims.cpp \
    ../src/msettings.tofsims.cpp \
    ../src/origin.cpp \
    ../src/print.cpp \
    ../src/processor.cpp \
    ../src/pse.cpp \
    ../src/quantity.cpp \
    ../src/quantity.dimension.cpp \
    ../src/quantity.map.cpp \
    ../src/quantity.table.cpp \
    ../src/sample.cpp \
    ../src/sample.matrix.cpp \
    ../src/statistics.cpp \
    ../src/tools.cpp \
    ../src/tools.file.cpp \
    ../src/tools.math.cpp \
    ../src/tools.matrix.cpp \
    ../src/tools.string.cpp \
    ../src/tools.time.cpp \
    ../src/tools.vec.cpp \
    ../src/tools.web.cpp \
    ../src/unit.cpp \
    custom_model_items_t.cpp \
    files_treeview_t.cpp \
    main.cpp \
    mainwindow.cpp \
    measurements_treeview_t.cpp \
    mgroups_treeview_t.cpp \
    plotwindow_t.cpp \
    qcustomplot.cpp \
    sims_plot_cluster_tree_t.cpp \
    sims_plot_measurement_widget_t.cpp \
    sims_plotwindow_t.cpp \
    table_log_t.cpp

HEADERS += \
    ../src/calc.hpp \
    ../src/cluster.hpp \
    ../src/config.hpp \
    ../src/crater.hpp \
    ../src/data_collector.hpp \
    ../src/database_t.hpp \
    ../src/definitions.hpp \
    ../src/element.hpp \
    ../src/filenames_collector_t.hpp \
    ../src/files.hpp \
    ../src/fit_functions.hpp \
    ../src/histogram.hpp \
    ../src/ion.hpp \
    ../src/isotope.hpp \
    ../src/log.hpp \
    ../src/measurement.hpp \
    ../src/mgroup.hpp \
    ../src/model.hpp \
    ../src/msettings.hpp \
    ../src/origin.hpp \
    ../src/persistence1d.hpp \
    ../src/print.hpp \
    ../src/processor.hpp \
    ../src/pse.hpp \
    ../src/quantity.hpp \
    ../src/sample.hpp \
    ../src/statistics.hpp \
    ../src/tools.hpp \
    ../src/unit.hpp \
    custom_model_items_t.hpp \
    files_treeview_t.hpp \
    mainwindow.h \
    measurements_treeview_t.hpp \
    mgroups_treeview_t.hpp \
    plotwindow_t.hpp \
    qcustomplot.h \
    sims_plot_cluster_tree_t.hpp \
    sims_plot_measurement_widget_t.hpp \
    sims_plotwindow_t.hpp \
    table_log_t.hpp

FORMS += \
    mainwindow.ui \
    sims_plot_measurement_widget_t.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#linux 64bit
LIBS += `PKG_CONFIG_PATH=/usr/lib/x86_64-linux-gnu/pkgconfig pkg-config --cflags --libs gsl sqlite3`
