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

INCLUDEPATH += ../fab
DEFINES += '_STATIC_= '

macx {
    LIBS += -L/usr/local/lib -lpng
    INCLUDEPATH += /usr/local/include/libpng16/
}
