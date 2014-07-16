QT += core gui widgets

TARGET = antimony
TEMPLATE = app
CONFIG += c++11

include(fab.pri)
include(core.pri)

SOURCES += \
    ../src/main.cpp \
    ../src/ui/inspector.cpp \
    ../src/ui/main_window.cpp \
    ../src/ui/view_selector.cpp \
    ../src/ui/canvas.cpp \
    ../src/app.cpp \
    ../src/control/control.cpp \
    ../src/control/multiline.cpp \
    ../src/control/axes_control.cpp \
    ../src/control/3d/point3d_control.cpp \
    ../src/control/2d/point2d_control.cpp \
    ../src/control/2d/circle_control.cpp \
    ../src/ui/colors.cpp \
    ../src/ui/port.cpp \
    ../src/ui/connection.cpp \
    ../src/control/3d/cube_control.cpp \
    ../src/render/render_task.cpp \
    ../src/render/render_worker.cpp \
    ../src/render/render_image.cpp \
    ../src/ui/depth_image.cpp

HEADERS += \
    ../src/ui/main_window.h \
    ../src/ui/inspector.h \
    ../src/ui/view_selector.h \
    ../src/ui/canvas.h \
    ../src/app.h \
    ../src/control/control.h \
    ../src/control/multiline.h \
    ../src/control/axes_control.h \
    ../src/control/3d/point3d_control.h \
    ../src/control/2d/point2d_control.h \
    ../src/control/2d/circle_control.h \
    ../src/ui/colors.h \
    ../src/ui/port.h \
    ../src/ui/connection.h \
    ../src/control/3d/cube_control.h \
    ../src/render/render_task.h \
    ../src/render/render_worker.h \
    ../src/render/render_image.h \
    ../src/ui/depth_image.h

FORMS += \
    ../src/ui/forms/main_window.ui \
    ../src/ui/forms/view_selector.ui
