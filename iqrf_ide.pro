# -------------------------------------------------
# Project created by QtCreator 2009-07-10T12:47:44
# -------------------------------------------------
QT += dbus
TARGET = iqrf_ide
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    spi.cpp \
    programmer.cpp \
    lusb.cpp \
    iqrf_dev.cpp \
    hex_parser.cpp \
    setup_dialog.cpp \
    compile_dialog.cpp
HEADERS += mainwindow.h \
    spi.h \
    programmer.h \
    lusb.h \
    iqrf_dev.h \
    hex_parser.h \
    setup_dialog.h \
    compile_dialog.h
FORMS += mainwindow.ui \
    setup_dialog.ui \
    compile_dialog.ui
