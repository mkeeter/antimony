include(datums.pri)
include(nodes.pri)

SOURCES += \
    ../src/graph/node/root.cpp \
    ../src/graph/node/proxy.cpp \
    ../src/graph/datum/input.cpp \
    ../src/graph/datum/link.cpp \
    ../src/graph/node/serializer.cpp \
    ../src/graph/node/deserializer.cpp \
    ../src/graph/hooks/hooks.cpp \
    ../src/graph/hooks/input.cpp \
    ../src/graph/hooks/meta.cpp \
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
    ../src/graph/hooks/hooks.h \
    ../src/graph/hooks/input.h \
    ../src/graph/hooks/meta.h \
    ../src/graph/hooks/output.h \
    ../src/graph/hooks/title.h \
    ../src/graph/hooks/ui.h \

INCLUDEPATH += ../src

macx {
    !contains(DEFINES, BREW_HOME){
        BREW_HOME=/usr/local
    }
    QMAKE_CXXFLAGS += $$system($$(BREW_HOME)/bin/python3-config --includes)
    QMAKE_LFLAGS   += $$system($$(BREW_HOME)/bin/python3-config --ldflags)
    LIBS += -L$$(BREW_HOME)/lib -lboost_python3
    QMAKE_CXXFLAGS += -isystem$$(BREW_HOME)/include
}

linux {
    QMAKE_CXXFLAGS += $$system(/usr/bin/python3-config --includes)
    QMAKE_LFLAGS   += $$system(/usr/bin/python3-config --ldflags)

    # Even though this is in QMAKE_LFLAGS, the linker is picky about
    # library ordering (so it needs to be here too).
    LIBS += -lpython3.4m

    # Check for different boost::python naming schemes
    LDCONFIG = $$system(ldconfig -p|grep python)
    contains(LDCONFIG, libboost_python-py34.so): LIBS += -lboost_python-py34
    contains(LDCONFIG, libboost_python3.so):     LIBS += -lboost_python3
}
