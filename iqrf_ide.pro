# -------------------------------------------------
# Project created by QtCreator 2009-07-10T12:47:44
# -------------------------------------------------
QT += dbus

LIBS += -L/usr/lib -lusb -L/usr/local/lib -liqrf

TARGET = iqrf_ide

TEMPLATE = app

SOURCES += main.cpp \
    mainwindow.cpp \
    programmer.cpp \
    hex_parser.cpp \
    setup_dialog.cpp \
    compile_dialog.cpp

HEADERS += mainwindow.h \
    programmer.h \
    hex_parser.h \
    setup_dialog.h \
    compile_dialog.h

FORMS += mainwindow.ui \
    setup_dialog.ui \
    compile_dialog.ui

RC_FILE +=
