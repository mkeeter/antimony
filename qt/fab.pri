SOURCES += \
    ../fab/tree/eval.c \
    ../fab/tree/render.c \
    ../fab/tree/tree.c \
    ../fab/tree/parser.c \
    ../fab/tree/triangulate.c \
    ../fab/tree/math/math_f.c \
    ../fab/tree/math/math_i.c \
    ../fab/tree/math/math_r.c \
    ../fab/tree/node/node.c \
    ../fab/tree/node/opcodes.c \
    ../fab/tree/node/printers.c \
    ../fab/tree/node/results.c \
    ../fab/formats/png.c \
    ../fab/util/region.c \

INCLUDEPATH += ../fab
DEFINES += '_STATIC_= '
