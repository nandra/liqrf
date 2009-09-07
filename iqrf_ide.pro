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
    core/iqrf_dev.cpp \
    core/lusb.cpp \
    core/spi.cpp
HEADERS += mainwindow.h \
    programmer.h \
    lusb.h \
    hex_parser.h \
    setup_dialog.h \
    compile_dialog.h \
    core/iqrf_dev.h \
    core/lusb.h \
    core/spi.h
FORMS += mainwindow.ui \
    setup_dialog.ui \
    compile_dialog.ui
