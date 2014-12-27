QT += core gui widgets opengl

TARGET = antimony
TEMPLATE = app
CONFIG += c++11 object_parallel_to_source

DEFINES += ANTIMONY

# Bump optimization up to -O3 in release builds
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

QMAKE_CXXFLAGS += -Werror=switch

GITREV = $$system(git log --pretty=format:'%h' -n 1)
GITDIFF = $$system(git diff --quiet --exit-code || echo "+")

QMAKE_CXXFLAGS += "-D'GITREV=\"$${GITREV}$${GITDIFF}\"'"

include(fab.pri)
include(shared.pri)
include(controls.pri)

SOURCES += \
    ../src/app/main.cpp \
    ../src/app/app.cpp \
    ../src/app/undo/stack.cpp \
    ../src/app/undo/undo_move.cpp \
    ../src/ui/main_window.cpp \
    ../src/ui/canvas/canvas.cpp \
    ../src/ui/canvas/scene.cpp \
    ../src/ui/canvas/inspector/inspector.cpp \
    ../src/ui/canvas/inspector/inspector_text.cpp \
    ../src/ui/canvas/inspector/inspector_row.cpp \
    ../src/ui/canvas/inspector/inspector_menu.cpp \
    ../src/ui/canvas/connection.cpp \
    ../src/ui/canvas/port.cpp \
    ../src/ui/viewport/viewport.cpp \
    ../src/ui/viewport/scene.cpp \
    ../src/ui/viewport/view_selector.cpp \
    ../src/ui/viewport/depth_image.cpp \
    ../src/ui/script/syntax.cpp \
    ../src/ui/script/editor.cpp \
    ../src/ui/util/button.cpp \
    ../src/ui/util/colors.cpp \
    ../src/ui/dialogs/resolution_dialog.cpp \
    ../src/ui/dialogs/exporting_dialog.cpp \
    ../src/render/render_task.cpp \
    ../src/render/render_worker.cpp \
    ../src/render/render_image.cpp \
    ../src/render/export_mesh.cpp \
    ../src/render/export_bitmap.cpp \
    ../src/render/export_json.cpp \

HEADERS += \
    ../src/ui/main_window.h \
    ../src/app/app.h \
    ../src/app/undo/stack.h \
    ../src/app/undo/undo_move.h \
    ../src/ui/canvas/canvas.h \
    ../src/ui/canvas/scene.h \
    ../src/ui/canvas/inspector/inspector.h \
    ../src/ui/canvas/inspector/inspector_text.h \
    ../src/ui/canvas/inspector/inspector_row.h \
    ../src/ui/canvas/inspector/inspector_menu.h \
    ../src/ui/canvas/port.h \
    ../src/ui/canvas/connection.h \
    ../src/ui/viewport/viewport.h \
    ../src/ui/viewport/scene.h \
    ../src/ui/viewport/view_selector.h \
    ../src/ui/viewport/depth_image.h \
    ../src/ui/script/syntax.h \
    ../src/ui/script/editor.h \
    ../src/ui/util/button.h \
    ../src/ui/util/colors.h \
    ../src/ui/dialogs/resolution_dialog.h \
    ../src/ui/dialogs/exporting_dialog.h \
    ../src/render/render_task.h \
    ../src/render/render_worker.h \
    ../src/render/render_image.h \
    ../src/render/export_mesh.h \
    ../src/render/export_bitmap.h \
    ../src/render/export_json.h \

FORMS += \
    ../src/ui/forms/main_window.ui \
    ../src/ui/forms/resolution_dialog.ui \
    ../src/ui/forms/exporting_dialog.ui \

RESOURCES += \
    ../gl/gl.qrc \

macx {
    QMAKE_INFO_PLIST = ../app/Info.plist
    ICON = ../app/sb.icns
}
