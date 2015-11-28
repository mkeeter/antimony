QT += core gui widgets opengl network concurrent

CONFIG += object_parallel_to_source

TARGET = Antimony
TEMPLATE = app

LIBS += -L../lib/fab -lSbFab -L../lib/graph -lSbGraph
INCLUDEPATH += ../lib/fab/inc
INCLUDEPATH += ../lib/graph/inc

PRE_TARGETDEPS += ../lib/graph/libSbGraph.a
PRE_TARGETDEPS += ../lib/fab/libSbFab.a

include(../qt/common.pri)
include(../qt/python.pri)
include(../qt/libpng.pri)

################################################################################

# Copy the py/fab and py/nodes directory when building the application
make_sb.commands = $(MKDIR) $$OUT_PWD/sb
copy_nodes.commands = $(COPY_DIR) $$PWD/../py/nodes $$OUT_PWD/sb
copy_fab.commands = $(COPY_DIR) $$PWD/../py/fab $$OUT_PWD/sb
first.depends = $(first) make_sb copy_nodes copy_fab
QMAKE_EXTRA_TARGETS += first make_sb copy_nodes copy_fab

################################################################################

# Extract the git revision, tag, and branch, then populate them
# into various preprocessor macros to populate the About box.
GITREV = $$system(git log --pretty=format:'%h' -n 1)
GITDIFF = $$system(git diff --quiet --exit-code || echo "+")
GITTAG = $$system(git describe --exact-match --tags 2> /dev/null)
GITBRANCH = $$system(git rev-parse --abbrev-ref HEAD)

QMAKE_CXXFLAGS += "-D'GITREV=\"$${GITREV}$${GITDIFF}\"'"
QMAKE_CXXFLAGS += "-D'GITTAG=\"$${GITTAG}\"'"
QMAKE_CXXFLAGS += "-D'GITBRANCH=\"$${GITBRANCH}\"'"

################################################################################

# Details for Mac applications
macx {
    # Normally you would deploy an icon and a .plist file by setting
    # QMAKE_INFO_PLIST and ICON, but qmake doesn't work properly when
    # the build directory and .pro file are at different levels (QTBUG-46133).
    QMAKE_POST_LINK += $(COPY) $$PWD/../deploy/mac/Info.plist $$OUT_PWD/$${TARGET}.app/Contents;
    QMAKE_POST_LINK += $(COPY) $$PWD/../deploy/mac/sb.icns $$OUT_PWD/$${TARGET}.app/Contents/Resources;
}

################################################################################

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

################################################################################

SOURCES += \
    app/main.cpp                        \
    app/app.cpp                         \
    app/update.cpp                      \
    app/colors.cpp                      \
    canvas/scene.cpp                    \
    canvas/view.cpp                     \
    canvas/inspector/frame.cpp          \
    canvas/inspector/export.cpp         \
    canvas/inspector/title.cpp          \
    canvas/inspector/buttons.cpp        \
    canvas/inspector/util.cpp           \
    canvas/inspector/row.cpp            \
    canvas/datum_editor.cpp             \
    window/base.cpp                     \
    window/canvas.cpp                   \
    window/script.cpp                   \
    graph/constructor/populate.cpp      \
    graph/proxy/graph.cpp               \
    graph/proxy/node.cpp                \
    graph/proxy/script.cpp              \
    graph/proxy/datum.cpp               \
    graph/proxy/superdatum.cpp          \
    graph/hooks/hooks.cpp               \
    graph/hooks/export.cpp              \
    graph/hooks/title.cpp               \
    graph/serialize/serializer.cpp      \
    graph/serialize/deserializer.cpp    \
    script/syntax.cpp                   \
    script/editor.cpp                   \
    script/frame.cpp                    \
    undo/undo_command.cpp               \
    undo/undo_move_node.cpp             \
    undo/undo_change_script.cpp         \
    undo/undo_change_expr.cpp           \
    undo/undo_stack.cpp                 \
    dialog/exporting.cpp                \
    dialog/resolution.cpp               \
    export/export_mesh.cpp              \
    export/export_heightmap.cpp         \
    export/export_worker.cpp            \

HEADERS += \
    app/app.h                           \
    app/update.h                        \
    app/colors.h                        \
    canvas/scene.h                      \
    canvas/view.h                       \
    window/base.h                       \
    window/canvas.h                     \
    window/script.h                     \
    canvas/inspector/frame.h            \
    canvas/inspector/export.h           \
    canvas/inspector/title.h            \
    canvas/inspector/buttons.h          \
    canvas/inspector/util.h             \
    canvas/inspector/row.h              \
    canvas/datum_editor.h               \
    graph/constructor/populate.h        \
    graph/proxy/graph.h                 \
    graph/proxy/node.h                  \
    graph/proxy/script.h                \
    graph/proxy/datum.h                 \
    graph/proxy/superdatum.h            \
    graph/hooks/hooks.h                 \
    graph/hooks/export.h                \
    graph/hooks/title.h                 \
    graph/serialize/serializer.h        \
    graph/serialize/deserializer.h      \
    script/syntax.h                     \
    script/editor.h                     \
    script/frame.h                      \
    undo/undo_command.h                 \
    undo/undo_move_node.h               \
    undo/undo_change_script.h           \
    undo/undo_change_expr.h             \
    undo/undo_stack.h                   \
    dialog/exporting.h                  \
    dialog/resolution.h                 \
    export/export_mesh.h                \
    export/export_heightmap.h           \
    export/export_worker.h              \

FORMS += \
    forms/base_window.ui \
    forms/resolution_dialog.ui \
    forms/exporting_dialog.ui \

RESOURCES += \
    gl/gl.qrc \
