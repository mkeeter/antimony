QT += core gui widgets testlib

CONFIG += testcase
CONFIG += c++11

include(fab.pri)
include(core.pri)

SOURCES += \
    ../test/test_datum.cpp \
    ../test/main.cpp \
    ../test/test_node.cpp \
    ../test/test_name.cpp \
    ../test/test_proxy.cpp \

