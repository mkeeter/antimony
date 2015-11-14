LEMONSOURCE = src/tree/v2syntax.y

################################################################################

lemon_hpp.name = lemon ${QMAKE_FILE_IN} (header)
lemon_hpp.input = LEMONSOURCE
lemon_hpp.output = $${OUT_PWD}/${QMAKE_FILE_BASE}.lemon.hpp
lemon_hpp.commands = lemon -q -c -s ${QMAKE_FILE_IN} && \
                     mv ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.h \
                        $${OUT_PWD}/${QMAKE_FILE_BASE}.lemon.hpp && \
                     rm ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.c
lemon_hpp.variable_out = GENERATED_SOURCES
lemon_hpp.clean = $${OUT_PWD}/${QMAKE_FILE_BASE}.lemon.hpp
lemon_hpp.config += target_predeps

################################################################################

lemon_cpp.name = lemon ${QMAKE_FILE_IN} (source)
lemon_cpp.input = LEMONSOURCE
lemon_cpp.output = $${OUT_PWD}/${QMAKE_FILE_BASE}.lemon.cpp
lemon_cpp.commands = lemon -q -c -s ${QMAKE_FILE_IN} && \
                     mv ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.c \
                        $${OUT_PWD}/${QMAKE_FILE_BASE}.lemon.cpp && \
                     rm ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.h
lemon_cpp.variable_out = GENERATED_SOURCES
lemon_cpp.clean = $${OUT_PWD}/${QMAKE_FILE_BASE}.lemon.cpp
lemon_cpp.config += target_predeps

################################################################################

QMAKE_EXTRA_COMPILERS += lemon_hpp lemon_cpp

INCLUDEPATH += $${OUT_PWD}
