flex.name = Flex ${QMAKE_FILE_IN}
flex.input = FLEXSOURCES
flex.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.yy.cpp
flex.commands = flex --outfile=${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.yy.cpp --header-file=${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.yy.hpp  ${QMAKE_FILE_IN}
flex.CONFIG += target_predeps
flex.variable_out = GENERATED_SOURCES
silent:flex.commands = @echo Lex ${QMAKE_FILE_IN} && $$flex.commands
QMAKE_EXTRA_COMPILERS += flex