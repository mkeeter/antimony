FLEXSOURCE = src/tree/v2syntax.l

################################################################################

flex_hpp.name = flex ${QMAKE_FILE_IN} (header)
flex_hpp.input = FLEXSOURCE
flex_hpp.output = $${OUT_PWD}/${QMAKE_FILE_BASE}.yy.hpp
flex_hpp.commands = flex --header-file=$${OUT_PWD}/${QMAKE_FILE_BASE}.yy.hpp \
                      ${QMAKE_FILE_IN}
flex_hpp.variable_out = GENERATED_SOURCES
flex_hpp.clean = $${OUT_PWD}/${QMAKE_FILE_BASE}.yy.hpp
flex_hpp.config += target_predeps

################################################################################

flex_cpp.name = flex ${QMAKE_FILE_IN} (source)
flex_cpp.input = FLEXSOURCE
flex_cpp.output = $${OUT_PWD}/${QMAKE_FILE_BASE}.yy.cpp
flex_cpp.commands = flex --outfile=$${OUT_PWD}/${QMAKE_FILE_BASE}.yy.cpp \
                      ${QMAKE_FILE_IN}
flex_cpp.variable_out = GENERATED_SOURCES
flex_cpp.clean = $${OUT_PWD}/${QMAKE_FILE_BASE}.yy.cpp
flex_cpp.config += target_predeps

# Force lemon to run before flex, because flex needs lemon's compiled header
flex_cpp.depends = $${OUT_PWD}/${QMAKE_FILE_BASE}.lemon.hpp

################################################################################

QMAKE_EXTRA_COMPILERS += flex_cpp flex_hpp

INCLUDEPATH += $${OUT_PWD}
