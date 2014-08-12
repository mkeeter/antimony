QT += core gui widgets testlib

CONFIG += testcase
CONFIG += c++11

DEFINES += ANTIMONY_TESTS

include(fab.pri)
include(shared.pri)

SOURCES += \
    ../test/main.cpp \
    ../test/test_datum.cpp \
    ../test/test_node.cpp \
    ../test/test_name.cpp \
    ../test/test_proxy.cpp \
    ../test/test_fab.cpp \
    ../test/test_script.cpp \
    ../test/test_shape.cpp \
    ../test/test_function.cpp \
    ../test/test_serialize.cpp \

HEADERS += \
    ../test/test_datum.h \
    ../test/test_node.h \
    ../test/test_name.h \
    ../test/test_proxy.h \
    ../test/test_fab.h \
    ../test/test_script.h \
    ../test/test_shape.h \
    ../test/test_function.h \
    ../test/test_serialize.h \



