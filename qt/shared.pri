include(datums.pri)
include(nodes.pri)

SOURCES += \
    ../src/node/manager.cpp \
    ../src/node/proxy.cpp \
    ../src/datum/input.cpp \
    ../src/datum/link.cpp \
    ../src/datum/wrapper.cpp \
    ../src/node/serializer.cpp \
    ../src/node/deserializer.cpp \

HEADERS += \
    ../src/node/manager.h \
    ../src/node/proxy.h \
    ../src/datum/input.h \
    ../src/datum/link.h \
    ../src/datum/wrapper.h \
    ../src/node/serializer.h \
    ../src/node/deserializer.h \

INCLUDEPATH += ../src

macx {
    QMAKE_CXXFLAGS += $$system(/usr/local/bin/python3-config --includes)
    QMAKE_LFLAGS   += $$system(/usr/local/bin/python3-config --ldflags)
    LIBS += -L/usr/local/lib -lboost_python3
    QMAKE_CXXFLAGS += -isystem/usr/local/include
}
