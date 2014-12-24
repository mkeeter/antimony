QT += core gui widgets testlib

CONFIG += testcase
CONFIG += c++11

DEFINES += ANTIMONY_TESTS

include(fab.pri)
include(shared.pri)

SOURCES += \
    ../src/test/main.cpp \
    ../src/test/test_datum.cpp \
    ../src/test/test_node.cpp \
    ../src/test/test_name.cpp \
    ../src/test/test_proxy.cpp \
    ../src/test/test_fab.cpp \
    ../src/test/test_script.cpp \
    ../src/test/test_shape.cpp \
    ../src/test/test_function.cpp \
    ../src/test/test_serialize.cpp \

HEADERS += \
    ../src/test/test_datum.h \
    ../src/test/test_node.h \
    ../src/test/test_name.h \
    ../src/test/test_proxy.h \
    ../src/test/test_fab.h \
    ../src/test/test_script.h \
    ../src/test/test_shape.h \
    ../src/test/test_function.h \
    ../src/test/test_serialize.h \



