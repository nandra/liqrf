# -------------------------------------------------
# Project created by QtCreator 2009-07-10T12:47:44
# -------------------------------------------------
QT += dbus
TARGET = iqrf_ide
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    programmer.cpp \
    hex_parser.cpp \
    setup_dialog.cpp \
    compile_dialog.cpp \
#    core_class/iqrf_dev.cpp \
#    core_class/lusb.cpp \
#    core_class/spi.cpp \
    iqrf_core/spi.cpp \
    iqrf_core/lusb.cpp \
    iqrf_core/iqrf_dev.cpp
HEADERS += mainwindow.h \
    programmer.h \
    lusb.h \
    hex_parser.h \
    setup_dialog.h \
    compile_dialog.h \
#    core_class/iqrf_dev.h \
#    core_class/lusb.h \
#    core_class/spi.h \
    iqrf_core/spi.h \
    iqrf_core/lusb.h \
    iqrf_core/iqrf_dev.h
FORMS += mainwindow.ui \
    setup_dialog.ui \
    compile_dialog.ui
