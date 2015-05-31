QT += core gui widgets opengl network

TARGET = antimony
TEMPLATE = app
CONFIG += c++11 object_parallel_to_source

DEFINES += ANTIMONY

# Bump optimization up to -O3 in release builds
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

QMAKE_CXXFLAGS += -Werror=switch
QMAKE_CFLAGS += -std=c11

QMAKE_CXXFLAGS += $$system(python3-config --includes)
QMAKE_CFLAGS += $$system(python3-config --includes)
LIBS += $$system(python3-config --libs)
LIBS += -lboost_python3
LIBS += $$system(libpng-config --libs)

GITREV = $$system(git log --pretty=format:'%h' -n 1)
GITDIFF = $$system(git diff --quiet --exit-code || echo "+")
GITTAG = $$system(git describe --exact-match --tags 2> /dev/null)
GITBRANCH = $$system(git rev-parse --abbrev-ref HEAD)

QMAKE_CXXFLAGS += "-D'GITREV=\"$${GITREV}$${GITDIFF}\"'"
QMAKE_CXXFLAGS += "-D'GITTAG=\"$${GITTAG}\"'"
QMAKE_CXXFLAGS += "-D'GITBRANCH=\"$${GITBRANCH}\"'"

OLD_GL_SET = $$(OLD_GL)
equals(OLD_GL_SET, "true") {
  message("Using QGLWidget instead of QOpenGLWidget")
  QMAKE_CXXFLAGS += "-DOLD_GL"
}

include(fab.pri)
include(shared.pri)
include(controls.pri)

SOURCES += \
    ../src/app/main.cpp \
    ../src/app/app.cpp \
    ../src/app/undo/stack.cpp \
    ../src/app/undo/undo_command.cpp \
    ../src/app/undo/undo_move.cpp \
    ../src/app/undo/undo_add_link.cpp \
    ../src/app/undo/undo_delete_link.cpp \
    ../src/app/undo/undo_change_expr.cpp \
    ../src/app/undo/undo_delete_node.cpp \
    ../src/app/undo/undo_delete_multi.cpp \
    ../src/app/undo/undo_add_multi.cpp \
    ../src/app/undo/undo_add_node.cpp \
    ../src/ui/main_window.cpp \
    ../src/ui/canvas/canvas.cpp \
    ../src/ui/canvas/graph_scene.cpp \
    ../src/ui/canvas/inspector/inspector.cpp \
    ../src/ui/canvas/inspector/inspector_title.cpp \
    ../src/ui/canvas/inspector/inspector_text.cpp \
    ../src/ui/canvas/inspector/inspector_row.cpp \
    ../src/ui/canvas/inspector/inspector_buttons.cpp \
    ../src/ui/canvas/connection.cpp \
    ../src/ui/canvas/port.cpp \
    ../src/ui/viewport/viewport.cpp \
    ../src/ui/viewport/viewport_scene.cpp \
    ../src/ui/viewport/view_selector.cpp \
    ../src/ui/viewport/depth_image.cpp \
    ../src/ui/script/syntax.cpp \
    ../src/ui/script/editor.cpp \
    ../src/ui/script/script_pane.cpp \
    ../src/ui/util/button.cpp \
    ../src/ui/util/colors.cpp \
    ../src/ui/dialogs/resolution_dialog.cpp \
    ../src/ui/dialogs/exporting_dialog.cpp \
    ../src/render/render_task.cpp \
    ../src/render/render_worker.cpp \
    ../src/render/render_image.cpp \
    ../src/export/export_mesh.cpp \
    ../src/export/export_heightmap.cpp \

HEADERS += \
    ../src/ui/main_window.h \
    ../src/app/app.h \
    ../src/app/undo/stack.h \
    ../src/app/undo/undo_move.h \
    ../src/app/undo/undo_add_link.h \
    ../src/app/undo/undo_delete_link.h \
    ../src/app/undo/undo_change_expr.h \
    ../src/app/undo/undo_delete_node.h \
    ../src/app/undo/undo_delete_multi.h \
    ../src/app/undo/undo_add_multi.h \
    ../src/app/undo/undo_add_node.h \
    ../src/ui/canvas/canvas.h \
    ../src/ui/canvas/graph_scene.h \
    ../src/ui/canvas/inspector/inspector.h \
    ../src/ui/canvas/inspector/inspector_title.h \
    ../src/ui/canvas/inspector/inspector_text.h \
    ../src/ui/canvas/inspector/inspector_row.h \
    ../src/ui/canvas/inspector/inspector_buttons.h \
    ../src/ui/canvas/port.h \
    ../src/ui/canvas/connection.h \
    ../src/ui/viewport/viewport.h \
    ../src/ui/viewport/viewport_scene.h \
    ../src/ui/viewport/view_selector.h \
    ../src/ui/viewport/depth_image.h \
    ../src/ui/script/syntax.h \
    ../src/ui/script/editor.h \
    ../src/ui/script/script_pane.h \
    ../src/ui/util/button.h \
    ../src/ui/util/colors.h \
    ../src/ui/dialogs/resolution_dialog.h \
    ../src/ui/dialogs/exporting_dialog.h \
    ../src/render/render_task.h \
    ../src/render/render_worker.h \
    ../src/render/render_image.h \
    ../src/export/export_mesh.h \
    ../src/export/export_worker.h \
    ../src/export/export_heightmap.h \
    ../src/util/hash.h \

FORMS += \
    ../src/ui/forms/main_window.ui \
    ../src/ui/forms/resolution_dialog.ui \
    ../src/ui/forms/exporting_dialog.ui \

RESOURCES += \
    ../gl/gl.qrc \

macx {
    QMAKE_INFO_PLIST = ../deploy/mac/Info.plist
    ICON = ../deploy/mac/sb.icns
}

linux {
    executable.path = /usr/local/bin
    executable.files = antimony
    nodes_folder.path = /usr/local/bin/sb/nodes
    nodes_folder.files = ../py/nodes/*
    fab_folder.path = /usr/local/bin/sb/fab
    fab_folder.files = ../py/fab/*
    INSTALLS += executable nodes_folder fab_folder
}
