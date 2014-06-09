SOURCES += \
    ../src/datum/datum.cpp \
    ../src/datum/eval.cpp \
    ../src/datum/float.cpp \
    ../src/node/node.cpp \
    ../src/node/manager.cpp \
    ../src/node/proxy.cpp \
    ../src/datum/input.cpp \
    ../src/datum/link.cpp \
    ../src/datum/name.cpp \
    ../src/node/3d/point3d.cpp

HEADERS += \
    ../src/datum/datum.h \
    ../src/datum/eval.h \
    ../src/datum/float.h \
    ../src/node/node.h \
    ../src/node/manager.h \
    ../src/node/proxy.h \
    ../src/datum/input.h \
    ../src/datum/link.h \
    ../src/datum/name.h \
    ../src/node/3d/point3d.h


INCLUDEPATH += ../src

macx {
    QMAKE_CXXFLAGS += $$system(/usr/local/bin/python3-config --includes)
    QMAKE_LFLAGS   += $$system(/usr/local/bin/python3-config --ldflags)
    LIBS += -L/usr/local/lib -lboost_python3
    QMAKE_CXXFLAGS += -isystem/usr/local/include
}
