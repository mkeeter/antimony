TARGET = SbGraphTests
TEMPLATE = app
VERSION = 0.8
CONFIG -= core

INCLUDEPATH += ../../vendor

include(graph.pri)
SOURCES += tests/main.cpp
