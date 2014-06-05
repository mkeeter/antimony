QT += core gui widgets

TARGET = antimony
TEMPLATE = app

SOURCES += \
    ../src/main.cpp \
    ../src/mainwindow.cpp \
    ../src/datum/datum.cpp \
    ../src/datum/eval.cpp \
    ../src/datum/float.cpp \
    ../src/node/node.cpp \
    ../src/node/manager.cpp \
    ../src/node/proxy.cpp

HEADERS  += \
    ../src/mainwindow.h \
    ../src/datum/datum.h \
    ../src/datum/eval.h \
    ../src/datum/float.h \
    ../src/node/node.h \
    ../src/node/manager.h \
    ../src/node/proxy.h

INCLUDEPATH += ../src

CONFIG += c++11

macx {
    QMAKE_CFLAGS   += $$system(/usr/local/bin/python3-config --cflags)
    QMAKE_CXXFLAGS += $$system(/usr/local/bin/python3-config --includes)
    QMAKE_LFLAGS   += $$system(/usr/local/bin/python3-config --ldflags)
}
