SOURCES += \
    ../fab/tree/eval_c.c \
    ../fab/tree/render.c \
    ../fab/tree/tree.c \
    ../fab/tree/parser.c \
    ../fab/tree/triangulate.c \
    ../fab/tree/math/math_f.c \
    ../fab/tree/math/math_i.c \
    ../fab/tree/math/math_r.c \
    ../fab/tree/node/node_c.c \
    ../fab/tree/node/opcodes.c \
    ../fab/tree/node/printers.c \
    ../fab/tree/node/results.c \
    ../fab/formats/png.c \
    ../fab/util/region.c \
    ../fab/shape.cpp \
    ../fab/fab.cpp \
    ../fab/bounds.cpp \
    ../fab/transform.cpp

HEADERS += \
    ../fab/tree/eval.h \
    ../fab/tree/render.h \
    ../fab/tree/tree.h \
    ../fab/tree/parser.h \
    ../fab/tree/triangulate.h \
    ../fab/tree/math/math_f.h \
    ../fab/tree/math/math_i.h \
    ../fab/tree/math/math_r.h \
    ../fab/tree/node/node.h \
    ../fab/tree/node/opcodes.h \
    ../fab/tree/node/printers.h \
    ../fab/tree/node/results.h \
    ../fab/formats/png.h \
    ../fab/util/region.h \
    ../fab/shape.h \
    ../fab/fab.h \
    ../fab/bounds.h \
    ../fab/transform.h


INCLUDEPATH += ../fab
DEFINES += '_STATIC_= '

macx {
    LIBS += -L/usr/local/lib -lpng
    INCLUDEPATH += /usr/local/include/libpng16/
}
