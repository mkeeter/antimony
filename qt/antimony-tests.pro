QT += core gui widgets testlib

CONFIG += testcase
CONFIG += c++11

include(fab.pri)
include(core.pri)

SOURCES += \
    ../test/main.cpp \
    ../test/test_datum.cpp \
    ../test/test_node.cpp \
    ../test/test_name.cpp \
    ../test/test_proxy.cpp \
    ../test/test_fab.cpp

HEADERS += \
    ../test/test_datum.h \
    ../test/test_node.h \
    ../test/test_name.h \
    ../test/test_proxy.h \
    ../test/test_fab.h



