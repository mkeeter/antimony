SOURCES += \
    ../src/fab/tree/eval_c.c \
    ../src/fab/tree/render.c \
    ../src/fab/tree/tree.c \
    ../src/fab/tree/parser.c \
    ../src/fab/tree/triangulate.c \
    ../src/fab/tree/math/math_f.c \
    ../src/fab/tree/math/math_i.c \
    ../src/fab/tree/math/math_r.c \
    ../src/fab/tree/node/node_c.c \
    ../src/fab/tree/node/opcodes.c \
    ../src/fab/tree/node/printers.c \
    ../src/fab/tree/node/printers_ss.cpp \
    ../src/fab/tree/node/results.c \
    ../src/fab/formats/png.c \
    ../src/fab/formats/stl.c \
    ../src/fab/util/region.c \
    ../src/fab/types/shape.cpp \
    ../src/fab/fab.cpp \
    ../src/fab/types/bounds.cpp \
    ../src/fab/types/transform.cpp

HEADERS += \
    ../src/fab/tree/eval.h \
    ../src/fab/tree/render.h \
    ../src/fab/tree/tree.h \
    ../src/fab/tree/parser.h \
    ../src/fab/tree/triangulate.h \
    ../src/fab/tree/math/math_f.h \
    ../src/fab/tree/math/math_i.h \
    ../src/fab/tree/math/math_r.h \
    ../src/fab/tree/node/node.h \
    ../src/fab/tree/node/opcodes.h \
    ../src/fab/tree/node/printers.h \
    ../src/fab/tree/node/printers_ss.h \
    ../src/fab/tree/node/results.h \
    ../src/fab/formats/png.h \
    ../src/fab/formats/stl.h \
    ../src/fab/util/region.h \
    ../src/fab/types/shape.h \
    ../src/fab/fab.h \
    ../src/fab/types/bounds.h \
    ../src/fab/types/transform.h

INCLUDEPATH += ../src/fab
DEFINES += '_STATIC_= '

linux {
    QMAKE_CFLAGS += -std=gnu99
    QMAKE_CXXFLAGS += $$system(/usr/bin/python3-config --includes)
    LIBS += -lpng
}

macx {
    LIBS += -L/usr/local/lib -lpng
    INCLUDEPATH += /usr/local/include/libpng15/
    INCLUDEPATH += /usr/local/include/libpng16/
}
