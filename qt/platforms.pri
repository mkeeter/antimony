cygwin {
    QMAKE_CXXFLAGS += $$system(python3-config --includes)
    QMAKE_CFLAGS += $$system(python3-config --includes)
    LIBS += $$system(python3-config --libs)
    LIBS += -lboost_python3
}

macx {
    QMAKE_INFO_PLIST = ../deploy/mac/Info.plist
    ICON = ../deploy/mac/sb.icns

    QMAKE_CXXFLAGS += $$system(/usr/local/bin/python3-config --includes)
    QMAKE_LFLAGS   += $$system(/usr/local/bin/python3-config --ldflags)
    LIBS += -L/usr/local/lib -lboost_python3
    QMAKE_CXXFLAGS += -isystem/usr/local/include
}

linux {
    executable.path = /usr/local/bin
    executable.files = antimony
    nodes_folder.path = /usr/local/bin/sb/nodes
    nodes_folder.files = ../py/nodes/*
    fab_folder.path = /usr/local/bin/sb/fab
    fab_folder.files = ../py/fab/*
    INSTALLS += executable nodes_folder fab_folder

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
