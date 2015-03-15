include(datums.pri)
include(nodes.pri)

SOURCES += \
    ../src/graph/node/root.cpp \
    ../src/graph/node/proxy.cpp \
    ../src/graph/datum/input.cpp \
    ../src/graph/datum/link.cpp \
    ../src/graph/node/serializer.cpp \
    ../src/graph/node/deserializer.cpp \
    ../src/graph/node/deserializer_old.cpp \
    ../src/graph/hooks/hooks.cpp \
    ../src/graph/hooks/input.cpp \
    ../src/graph/hooks/output.cpp \
    ../src/graph/hooks/title.cpp \
    ../src/graph/hooks/ui.cpp \

HEADERS += \
    ../src/graph/node/root.h \
    ../src/graph/node/proxy.h \
    ../src/graph/datum/input.h \
    ../src/graph/datum/link.h \
    ../src/graph/node/serializer.h \
    ../src/graph/node/deserializer.h \
    ../src/graph/node/deserializer_old.h \
    ../src/graph/hooks/hooks.h \
    ../src/graph/hooks/input.h \
    ../src/graph/hooks/output.h \
    ../src/graph/hooks/title.h \
    ../src/graph/hooks/ui.h \

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
