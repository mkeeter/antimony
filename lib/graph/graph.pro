TARGET = SbGraph
TEMPLATE = lib
VERSION = 0.8
CONFIG -= core
CONFIG += staticlib

INCLUDEPATH += inc

include(../../qt/common.pri)
include(../../qt/python.pri)

SOURCES += \
    src/datum.cpp \
    src/graph.cpp \
    src/node.cpp \
    src/proxy.cpp \
    src/script.cpp \
    src/util.cpp \
    src/types/downstream.cpp \
    src/types/root.cpp \
