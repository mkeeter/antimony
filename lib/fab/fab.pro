TARGET = SbFab
TEMPLATE = lib
VERSION = 0.8
CONFIG -= core
CONFIG += staticlib

INCLUDEPATH += inc vendor

include(../../qt/common.pri)
include(../../qt/python.pri)
include(../../qt/libpng.pri)

include(flex.pri)
include(lemon.pri)
FLEXSOURCE = src/tree/v2syntax.l
LEMONSOURCE = src/tree/v2syntax.y

SOURCES += \
    src/tree/v2parser.cpp \
    src/fab.cpp \
    src/formats/png.c \
    src/formats/stl.c \
    src/tree/eval.c \
    src/tree/math/math_f.c \
    src/tree/math/math_g.c \
    src/tree/math/math_i.c \
    src/tree/math/math_r.c \
    src/tree/node/node_c.c \
    src/tree/node/opcodes.c \
    src/tree/node/printers.c \
    src/tree/node/printers_ss.cpp \
    src/tree/node/results.c \
    src/tree/parser.c \
    src/tree/render.c \
    src/tree/tree.c \
    src/tree/triangulate/mesher.cpp \
    src/tree/triangulate/triangle.cpp \
    src/tree/triangulate.cpp \
    src/types/bounds.cpp \
    src/types/shape.cpp \
    src/types/transform.cpp \
    src/util/region.c \
    src/util/ustack.c \
