QT += core gui widgets

TARGET = antimony
TEMPLATE = app
CONFIG += c++11

DEFINES += ANTIMONY

# Bump optimization up to -O3 in release builds
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

QMAKE_CXXFLAGS += -Werror=switch

include(fab.pri)
include(shared.pri)
include(controls.pri)

SOURCES += \
    ../src/main.cpp \
    ../src/app.cpp \
    ../src/ui/main_window.cpp \
    ../src/ui/canvas.cpp \
    ../src/ui/colors.cpp \
    ../src/ui/connection.cpp \
    ../src/ui/port.cpp \
    ../src/ui/view_selector.cpp \
    ../src/render/render_task.cpp \
    ../src/render/render_worker.cpp \
    ../src/render/render_image.cpp \
    ../src/render/export_mesh.cpp \
    ../src/ui/depth_image.cpp \
    ../src/ui/syntax.cpp \
    ../src/ui/script.cpp \
    ../src/ui/inspector/inspector.cpp \
    ../src/ui/inspector/inspector_text.cpp \
    ../src/ui/inspector/inspector_button.cpp \
    ../src/ui/inspector/inspector_row.cpp \
    ../src/ui/resolution_dialog.cpp \

HEADERS += \
    ../src/ui/main_window.h \
    ../src/ui/view_selector.h \
    ../src/ui/canvas.h \
    ../src/app.h \
    ../src/ui/colors.h \
    ../src/ui/port.h \
    ../src/ui/connection.h \
    ../src/render/render_task.h \
    ../src/render/render_worker.h \
    ../src/render/render_image.h \
    ../src/render/export_mesh.h \
    ../src/ui/depth_image.h \
    ../src/ui/syntax.h \
    ../src/ui/script.h \
    ../src/ui/inspector/inspector.h \
    ../src/ui/inspector/inspector_text.h \
    ../src/ui/inspector/inspector_button.h \
    ../src/ui/inspector/inspector_row.h \
    ../src/ui/resolution_dialog.h \

FORMS += \
    ../src/ui/forms/main_window.ui \
    ../src/ui/forms/view_selector.ui \
    ../src/ui/forms/resolution_dialog.ui \

macx {
    QMAKE_INFO_PLIST = ../app/Info.plist
    ICON = ../app/sb.icns
}
