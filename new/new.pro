QT += core gui widgets opengl network

TARGET = Antimony
TEMPLATE = app

INCLUDEPATH += src

LIBS += -L../lib/fab -lSbFab -L../lib/graph -lSbGraph
INCLUDEPATH += ../lib/fab/inc
INCLUDEPATH += ../lib/graph/inc

PRE_TARGETDEPS += ../lib/graph/libSbGraph.a
PRE_TARGETDEPS += ../lib/fab/libSbFab.a

include(../qt/common.pri)
include(../qt/python.pri)
include(../qt/libpng.pri)
include(git.pri)

# Copy the py/fab and py/nodes directory when building the application
make_sb.commands = $(MKDIR) $$OUT_PWD/sb
copy_nodes.commands = $(COPY_DIR) $$PWD/../py/nodes $$OUT_PWD/sb
copy_fab.commands = $(COPY_DIR) $$PWD/../py/fab $$OUT_PWD/sb
first.depends = $(first) make_sb copy_nodes copy_fab
QMAKE_EXTRA_TARGETS += first make_sb copy_nodes copy_fab

# Details for Mac applications
macx {
    # Normally you would deploy an icon and a .plist file by setting
    # QMAKE_INFO_PLIST and ICON, but qmake doesn't work properly when
    # the build directory and .pro file are at different levels (QTBUG-46133).
    QMAKE_POST_LINK += $(COPY) $$PWD/../deploy/mac/Info.plist $$OUT_PWD/$${TARGET}.app/Contents;
    QMAKE_POST_LINK += $(COPY) $$PWD/../deploy/mac/sb.icns $$OUT_PWD/$${TARGET}.app/Contents/Resources;
}

# Installation details for Linux systems
linux {
    # Rename file from "Antimony" to "antimony"
    QMAKE_POST_LINK += $(MOVE) $$OUT_PWD/$${TARGET} $$OUT_PWD/$$lower($${TARGET})

    executable.path = /usr/local/bin
    executable.files = antimony
    nodes_folder.path = /usr/local/bin/sb/nodes
    nodes_folder.files = ../py/nodes/*
    fab_folder.path = /usr/local/bin/sb/fab
    fab_folder.files = ../py/fab/*
    INSTALLS += executable nodes_folder fab_folder
}

SOURCES += \
    src/app/main.cpp                        \
    src/app/app.cpp                         \
    src/canvas/scene.cpp                    \
    src/canvas/view.cpp                     \
    src/graph/constructor/populate.cpp      \
    src/graph/proxy/graph.cpp               \
    src/graph/proxy/node.cpp                \
    src/graph/proxy/script.cpp              \
    src/graph/proxy/datum.cpp               \
    src/graph/proxy/superdatum.cpp          \

HEADERS += \
    src/app/app.h                           \
    src/canvas/scene.h                      \
    src/canvas/view.h                       \
    src/graph/proxy/graph.h                 \
    src/graph/constructor/populate.h        \
    src/graph/proxy/node.h                  \
    src/graph/proxy/script.h                \
    src/graph/proxy/datum.h                 \
    src/graph/proxy/superdatum.h            \
