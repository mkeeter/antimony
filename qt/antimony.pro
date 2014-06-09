QT += core gui widgets

TARGET = antimony
TEMPLATE = app
CONFIG += c++11

include(fab.pri)
include(core.pri)

SOURCES += \
    ../src/main.cpp \
    ../src/mainwindow.cpp \

HEADERS += \
    ../src/mainwindow.h
