TARGET = SbGraphTests
TEMPLATE = app
VERSION = 0.8
CONFIG -= core

INCLUDEPATH += ../../vendor

include(graph.pri)
SOURCES += tests/main.cpp tests/datum.cpp tests/script.cpp tests/node.cpp \
           tests/link.cpp tests/graph.cpp tests/subgraph.cpp
