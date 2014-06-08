QT += core gui widgets testlib

CONFIG += testcase

SOURCES += \
    ../src/datum/datum.cpp \
    ../src/datum/eval.cpp \
    ../src/datum/float.cpp \
    ../src/node/node.cpp \
    ../src/node/manager.cpp \
    ../src/node/proxy.cpp \
    ../src/node/3d/point3d.cpp \
    ../src/datum/input.cpp \
    ../src/datum/link.cpp \
    ../src/datum/name.cpp \
    ../test/test_datum.cpp \
    ../test/main.cpp \
    ../test/test_node.cpp \
    ../test/test_name.cpp \
    ../test/test_proxy.cpp \
    ../src/datum/script.cpp

HEADERS  += \
    ../src/datum/datum.h \
    ../src/datum/eval.h \
    ../src/datum/float.h \
    ../src/node/node.h \
    ../src/node/manager.h \
    ../src/node/proxy.h \
    ../src/node/3d/point3d.h \
    ../src/datum/input.h \
    ../src/datum/link.h \
    ../src/datum/name.h \
    ../test/test_datum.h \
    ../test/test_node.h \
    ../test/test_name.h \
    ../test/test_proxy.h \
    ../src/datum/script.h

INCLUDEPATH += ../src

CONFIG += c++11

macx {
    QMAKE_CXXFLAGS += $$system(/usr/local/bin/python3-config --includes)
    QMAKE_LFLAGS   += $$system(/usr/local/bin/python3-config --ldflags)
    LIBS += -L/usr/local/lib -lboost_python3
    QMAKE_CXXFLAGS += -isystem/usr/local/include
}
