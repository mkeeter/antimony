lemon.name = lemon ${QMAKE_FILE_IN}
lemon.input = LEMONSOURCE
lemon.output = $${OUT_PWD}/${QMAKE_FILE_BASE}.lemon.cpp \
               $${OUT_PWD}/${QMAKE_FILE_BASE}.lemon.hpp
lemon.commands = lemon -q -c -s ${QMAKE_FILE_IN} && \
                 mv ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.c \
                    $${OUT_PWD}/${QMAKE_FILE_BASE}.lemon.cpp && \
                 mv ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.h \
                    $${OUT_PWD}/${QMAKE_FILE_BASE}.lemon.hpp
lemon.variable_out = GENERATED_SOURCES
lemon.clean = $${OUT_PWD}/${QMAKE_FILE_BASE}.lemon.hpp \
              $${OUT_PWD}/${QMAKE_FILE_BASE}.lemon.cpp \

lemon.CONFIG += target_predeps
QMAKE_EXTRA_COMPILERS += lemon

INCLUDEPATH += $${OUT_PWD}
