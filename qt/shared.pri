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

cygwin {
    QMAKE_CFLAGS += -std=c11

    QMAKE_CXXFLAGS += $$system(python3-config --includes)
    QMAKE_CFLAGS += $$system(python3-config --includes)
    LIBS += $$system(python3-config --libs)
    LIBS += -lboost_python3
    LIBS += $$system(libpng-config --libs)
}

macx {
    QMAKE_CXXFLAGS += $$system(/usr/local/bin/python3-config --includes)
    QMAKE_LFLAGS   += $$system(/usr/local/bin/python3-config --ldflags)
    LIBS += -L/usr/local/lib -lboost_python3
    QMAKE_CXXFLAGS += -isystem/usr/local/include
}

linux {
    QMAKE_CXXFLAGS += $$system(/usr/bin/python3-config --includes)
    QMAKE_LFLAGS   += $$system(/usr/bin/python3-config --ldflags)

    # Even though this is in QMAKE_LFLAGS, the linker is picky about
    # library ordering (so it needs to be here too).
    LIBS += -lpython3.4m

    # ldconfig is being used to find libboost_python, but it's in a different
    # place in different distros (and is not in the default $PATH on Debian).
    # First, check to see if it's on the default $PATH.
    system(which ldconfig > /dev/null) {
        LDCONFIG_BIN = "ldconfig"
    }
    # If that failed, then search for it in its usual places.
    isEmpty(LDCONFIG_BIN) {
        for(p, $$list(/sbin/ldconfig /usr/bin/ldconfig)) {
            exists($$p): LDCONFIG_BIN = $$p
        }
    }
    # If that search failed too, then exit with an error.
    isEmpty(LDCONFIG_BIN) {
        error("Could not find ldconfig!")
    }

    # Check for different boost::python naming schemes
    LDCONFIG_OUT = $$system($$LDCONFIG_BIN -p|grep python)
    for (b, $$list(boost_python-py34 boost_python3)) {
        contains(LDCONFIG_OUT, "lib$${b}.so") {
            LIBS += "-l$$b"
            GOT_BOOST_PYTHON = True
        }
    }

    # If we couldn't find boost::python, exit with an error.
    isEmpty(GOT_BOOST_PYTHON) {
        error("Could not find boost::python3")
    }
}
