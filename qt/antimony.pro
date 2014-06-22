QT += core gui widgets

TARGET = antimony
TEMPLATE = app
CONFIG += c++11

include(fab.pri)
include(core.pri)

SOURCES += \
    ../src/main.cpp \
    ../src/ui/main_window.cpp \
    ../src/ui/view_selector.cpp \
    ../src/ui/canvas.cpp \
    ../src/app.cpp \
    ../src/control/control.cpp

HEADERS += \
    ../src/ui/main_window.h \
    ../src/ui/view_selector.h \
    ../src/ui/canvas.h \
    ../src/app.h \
    ../src/control/control.h

FORMS += \
    ../src/ui/forms/main_window.ui \
    ../src/ui/forms/view_selector.ui \
    ../src/ui/forms/viewer.ui \
