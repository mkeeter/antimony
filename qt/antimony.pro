QT += core gui widgets

TARGET = antimony
TEMPLATE = app
CONFIG += c++11

include(fab.pri)
include(core.pri)

SOURCES += \
    ../src/main.cpp \
    ../src/mainwindow.cpp \

macx {
    QMAKE_CFLAGS   += $$system(/usr/local/bin/python3-config --cflags)
    QMAKE_CXXFLAGS += $$system(/usr/local/bin/python3-config --includes)
    QMAKE_LFLAGS   += $$system(/usr/local/bin/python3-config --ldflags)
}
