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
# include(controls.pri)

SOURCES += \
    ../src/app/main.cpp \
    ../src/app/app.cpp \
    ../src/ui/main_window.cpp \
    ../src/ui/colors.cpp \
    ../src/ui/canvas/canvas.cpp \
    ../src/ui/canvas/scene.cpp \
    ../src/ui/canvas/inspector/inspector.cpp \
    ../src/ui/canvas/inspector/inspector_text.cpp \
    ../src/ui/canvas/inspector/inspector_button.cpp \
    ../src/ui/canvas/inspector/inspector_row.cpp \
    ../src/ui/canvas/connection.cpp \
    ../src/ui/canvas/port.cpp \
    ../src/ui/viewport/viewport.cpp \
    ../src/ui/viewport/view_selector.cpp \
    ../src/ui/viewport/depth_image.cpp \
    #../src/ui/script/syntax.cpp \
    #../src/ui/resolution_dialog.cpp \
    #../src/ui/exporting_dialog.cpp \
    #../src/ui/tooltip.cpp \
    #../src/ui/script/script_editor.cpp \
    #../src/ui/script/script_buttons.cpp \
    #../src/render/render_task.cpp \
    #../src/render/render_worker.cpp \
    #../src/render/render_image.cpp \
    #../src/render/export_mesh.cpp \
    #../src/render/export_bitmap.cpp \
    #../src/render/export_json.cpp \

HEADERS += \
    ../src/ui/main_window.h \
    ../src/app/app.h \
    ../src/ui/colors.h \
    ../src/ui/canvas/canvas.h \
    ../src/ui/canvas/scene.h \
    ../src/ui/canvas/inspector/inspector.h \
    ../src/ui/canvas/inspector/inspector_text.h \
    ../src/ui/canvas/inspector/inspector_button.h \
    ../src/ui/canvas/inspector/inspector_row.h \
    ../src/ui/canvas/port.h \
    ../src/ui/canvas/connection.h \
    ../src/ui/viewport/viewport.h \
    ../src/ui/viewport/view_selector.h \
    ../src/ui/viewport/depth_image.h \
    #../src/ui/view_selector.h \
    #../src/render/render_task.h \
    #../src/render/render_worker.h \
    #../src/render/render_image.h \
    #../src/render/export_mesh.h \
    #../src/render/export_bitmap.h \
    #../src/render/export_json.h \
    #../src/ui/depth_image.h \
    #../src/ui/script/syntax.h \
    #../src/ui/resolution_dialog.h \
    #../src/ui/exporting_dialog.h \
    #../src/ui/tooltip.h \
    #../src/ui/script/script_editor.h \
    #../src/ui/script/script_buttons.h \

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
