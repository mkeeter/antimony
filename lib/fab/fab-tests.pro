TARGET = SbFabTests
TEMPLATE = app
VERSION = 0.8
CONFIG -= core

INCLUDEPATH += ../../vendor

include(fab.pri)
SOURCES += tests/main.cpp tests/parser.cpp tests/shape.cpp
