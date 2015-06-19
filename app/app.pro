QT += core gui widgets opengl network

TARGET = Antimony
TEMPLATE = app

INCLUDEPATH += src

LIBS += -L../lib/fab -lSbFab
INCLUDEPATH += ../lib/fab/inc

include(../qt/common.pri)
include(../qt/python.pri)
include(../qt/libpng.pri)
include(src/graph/graph.pri)
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
    src/app/main.cpp \
    src/app/app.cpp \
    src/app/undo/stack.cpp \
    src/app/undo/undo_command.cpp \
    src/app/undo/undo_move.cpp \
    src/app/undo/undo_add_link.cpp \
    src/app/undo/undo_delete_link.cpp \
    src/app/undo/undo_change_expr.cpp \
    src/app/undo/undo_delete_node.cpp \
    src/app/undo/undo_delete_multi.cpp \
    src/app/undo/undo_add_multi.cpp \
    src/app/undo/undo_add_node.cpp \
    src/ui/main_window.cpp \
    src/ui/canvas/canvas.cpp \
    src/ui/canvas/graph_scene.cpp \
    src/ui/canvas/inspector/inspector.cpp \
    src/ui/canvas/inspector/inspector_title.cpp \
    src/ui/canvas/inspector/inspector_text.cpp \
    src/ui/canvas/inspector/inspector_row.cpp \
    src/ui/canvas/inspector/inspector_buttons.cpp \
    src/ui/canvas/connection.cpp \
    src/ui/canvas/port.cpp \
    src/ui/viewport/viewport.cpp \
    src/ui/viewport/viewport_scene.cpp \
    src/ui/viewport/view_selector.cpp \
    src/ui/viewport/depth_image.cpp \
    src/ui/script/syntax.cpp \
    src/ui/script/editor.cpp \
    src/ui/script/script_pane.cpp \
    src/ui/util/button.cpp \
    src/ui/util/colors.cpp \
    src/ui/dialogs/resolution_dialog.cpp \
    src/ui/dialogs/exporting_dialog.cpp \
    src/render/render_task.cpp \
    src/render/render_worker.cpp \
    src/render/render_image.cpp \
    src/export/export_mesh.cpp \
    src/export/export_heightmap.cpp \
    src/control/control.cpp \
    src/control/control_root.cpp \
    src/control/proxy.cpp \
    src/control/point.cpp \
    src/control/wireframe.cpp \

HEADERS += \
    src/ui/main_window.h \
    src/app/app.h \
    src/app/undo/stack.h \
    src/app/undo/undo_move.h \
    src/app/undo/undo_add_link.h \
    src/app/undo/undo_delete_link.h \
    src/app/undo/undo_change_expr.h \
    src/app/undo/undo_delete_node.h \
    src/app/undo/undo_delete_multi.h \
    src/app/undo/undo_add_multi.h \
    src/app/undo/undo_add_node.h \
    src/ui/canvas/canvas.h \
    src/ui/canvas/graph_scene.h \
    src/ui/canvas/inspector/inspector.h \
    src/ui/canvas/inspector/inspector_title.h \
    src/ui/canvas/inspector/inspector_text.h \
    src/ui/canvas/inspector/inspector_row.h \
    src/ui/canvas/inspector/inspector_buttons.h \
    src/ui/canvas/port.h \
    src/ui/canvas/connection.h \
    src/ui/viewport/viewport.h \
    src/ui/viewport/viewport_scene.h \
    src/ui/viewport/view_selector.h \
    src/ui/viewport/depth_image.h \
    src/ui/script/syntax.h \
    src/ui/script/editor.h \
    src/ui/script/script_pane.h \
    src/ui/util/button.h \
    src/ui/util/colors.h \
    src/ui/dialogs/resolution_dialog.h \
    src/ui/dialogs/exporting_dialog.h \
    src/render/render_task.h \
    src/render/render_worker.h \
    src/render/render_image.h \
    src/export/export_mesh.h \
    src/export/export_worker.h \
    src/export/export_heightmap.h \
    src/util/hash.h \
    src/control/control.h \
    src/control/control_root.h \
    src/control/proxy.h \
    src/control/point.h \
    src/control/wireframe.h \

FORMS += \
    src/ui/forms/main_window.ui \
    src/ui/forms/resolution_dialog.ui \
    src/ui/forms/exporting_dialog.ui \

RESOURCES += \
    gl/gl.qrc \
