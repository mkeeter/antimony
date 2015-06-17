SOURCES += \
    ../src/fab/tree/v2parser.cpp \
    ../src/fab/tree/eval.c \
    ../src/fab/tree/render.c \
    ../src/fab/tree/tree.c \
    ../src/fab/tree/parser.c \
    ../src/fab/tree/triangulate.cpp \
    ../src/fab/tree/triangulate/mesher.cpp \
    ../src/fab/tree/triangulate/triangle.cpp \
    ../src/fab/tree/math/math_f.c \
    ../src/fab/tree/math/math_i.c \
    ../src/fab/tree/math/math_r.c \
    ../src/fab/tree/math/math_g.c \
    ../src/fab/tree/node/node_c.c \
    ../src/fab/tree/node/opcodes.c \
    ../src/fab/tree/node/printers.c \
    ../src/fab/tree/node/printers_ss.cpp \
    ../src/fab/tree/node/results.c \
    ../src/fab/formats/png.c \
    ../src/fab/formats/stl.c \
    ../src/fab/util/region.c \
    ../src/fab/util/ustack.c \
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
    ../src/fab/tree/triangulate/mesher.h \
    ../src/fab/tree/triangulate/triangle.h \
    ../src/fab/tree/math/math_f.h \
    ../src/fab/tree/math/math_i.h \
    ../src/fab/tree/math/math_r.h \
    ../src/fab/tree/math/math_g.h \
    ../src/fab/tree/node/node.h \
    ../src/fab/tree/node/opcodes.h \
    ../src/fab/tree/node/printers.h \
    ../src/fab/tree/node/printers_ss.h \
    ../src/fab/tree/node/results.h \
    ../src/fab/formats/png.h \
    ../src/fab/formats/stl.h \
    ../src/fab/util/region.h \
    ../src/fab/util/ustack.h \
    ../src/fab/types/shape.h \
    ../src/fab/fab.h \
    ../src/fab/types/bounds.h \
    ../src/fab/types/transform.h

include(flex.pri)
include(lemon.pri)
FLEXSOURCES = ../src/fab/tree/v2syntax.l
LEMONSOURCES = ../src/fab/tree/v2syntax.y

INCLUDEPATH += ../src/fab
INCLUDEPATH += ../src
INCLUDEPATH += ../vendor
DEFINES += '_STATIC_= '

# Copy the py/fab and py/nodes directory when building the application
make_sb.commands = $(MKDIR) $$OUT_PWD/sb
copy_nodes.commands = $(COPY_DIR) $$PWD/../py/nodes $$OUT_PWD/sb
copy_fab.commands = $(COPY_DIR) $$PWD/../py/fab $$OUT_PWD/sb
first.depends = $(first) make_sb copy_nodes copy_fab
export(first.depends)
export(make_sb.commands)
export(copy_nodes.commands)
export(copy_fab.commands)
QMAKE_EXTRA_TARGETS += first make_sb copy_nodes copy_fab
