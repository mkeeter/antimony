include(datums.pri)
include(nodes.pri)

SOURCES += \
    ../src/graph/node/manager.cpp \
    ../src/graph/node/proxy.cpp \
    ../src/graph/datum/input.cpp \
    ../src/graph/datum/link.cpp \
    ../src/graph/datum/wrapper.cpp \
    ../src/graph/node/serializer.cpp \
    ../src/graph/node/deserializer.cpp \

HEADERS += \
    ../src/graph/node/manager.h \
    ../src/graph/node/proxy.h \
    ../src/graph/datum/input.h \
    ../src/graph/datum/link.h \
    ../src/graph/datum/wrapper.h \
    ../src/graph/node/serializer.h \
    ../src/graph/node/deserializer.h \

INCLUDEPATH += ../src

macx {
    QMAKE_CXXFLAGS += $$system(/usr/local/bin/python3-config --includes)
    QMAKE_LFLAGS   += $$system(/usr/local/bin/python3-config --ldflags)
    LIBS += -L/usr/local/lib -lboost_python3
    QMAKE_CXXFLAGS += -isystem/usr/local/include
}

linux {
    QMAKE_CXXFLAGS += $$system(/usr/bin/python3-config --includes)
    QMAKE_LFLAGS   += $$system(/usr/bin/python3-config --ldflags)
    LIBS += -lboost_python-py34 -lpython3.4m
}
