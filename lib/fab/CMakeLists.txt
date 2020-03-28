find_package(PNG REQUIRED)
find_package(FLEX REQUIRED)

################################################################################

# Custom command to generate the parser sources with lemon
if(WIN32)
    find_program(LEMON_EXECUTABLE lemon.exe DOC "path to the lemon executable")
else()
    find_program(LEMON_EXECUTABLE lemon DOC "path to the lemon executable")
endif()

mark_as_advanced(LEMON_EXECUTABLE)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/v2syntax.lemon.hpp
           ${CMAKE_CURRENT_BINARY_DIR}/v2syntax.lemon.cpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/tree/v2syntax.y
    COMMAND ${LEMON_EXECUTABLE} -q -c -s ${CMAKE_CURRENT_SOURCE_DIR}/src/tree/v2syntax.y
    COMMAND mv ${CMAKE_CURRENT_SOURCE_DIR}/src/tree/v2syntax.c ${CMAKE_CURRENT_BINARY_DIR}/v2syntax.lemon.cpp
    COMMAND mv ${CMAKE_CURRENT_SOURCE_DIR}/src/tree/v2syntax.h ${CMAKE_CURRENT_BINARY_DIR}/v2syntax.lemon.hpp)

# Custom command to generate the lexer sources with flex
if(WIN32)
    find_program(FLEX_EXECUTABLE flex.exe DOC "path to the flex executable")
else()
    find_program(FLEX_EXECUTABLE flex DOC "path to the flex executable")
endif()
mark_as_advanced(FLEX_EXECUTABLE)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/v2syntax.yy.hpp
           ${CMAKE_CURRENT_BINARY_DIR}/v2syntax.yy.cpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/tree/v2syntax.l
    COMMAND ${FLEX_EXECUTABLE} --outfile=${CMAKE_CURRENT_BINARY_DIR}/v2syntax.yy.cpp
                               --header-file=${CMAKE_CURRENT_BINARY_DIR}/v2syntax.yy.hpp
                               ${CMAKE_CURRENT_SOURCE_DIR}/src/tree/v2syntax.l)

add_library(SbFab STATIC
    src/fab.cpp
    src/formats/png.c
    src/formats/stl.c
    src/tree/eval.c
    src/tree/math/math_f.c
    src/tree/math/math_g.c
    src/tree/math/math_i.c
    src/tree/math/math_r.c
    src/tree/node/node_c.c
    src/tree/node/opcodes.c
    src/tree/node/printers.c
    src/tree/node/printers_ss.cpp
    src/tree/node/results.c
    src/tree/parser.c
    src/tree/render.c
    src/tree/tree.c
    src/tree/v2parser.cpp
    src/tree/triangulate/mesher.cpp
    src/tree/triangulate/triangle.cpp
    src/tree/triangulate.cpp
    src/types/bounds.cpp
    src/types/shape.cpp
    src/types/transform.cpp
    src/util/region.c
    src/util/ustack.c

    # Generated files
    ${CMAKE_CURRENT_BINARY_DIR}/v2syntax.lemon.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/v2syntax.yy.cpp
)

target_include_directories(SbFab SYSTEM PRIVATE
    ${Python_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${PNG_INCLUDE_DIR}
)

# Add the binary dir to include generated files
target_include_directories(SbFab PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(SbFab
    ${Boost_LIBRARIES}
    ${Python_LIBRARY_RELEASE}
    ${PNG_LIBRARIES}
)
target_include_directories(SbFab PUBLIC inc)
target_include_directories(SbFab SYSTEM PRIVATE
    vendor)

################################################################################

add_executable(SbFabTest
    tests/main.cpp
    tests/parser.cpp
    tests/shape.cpp
)
target_link_libraries(SbFabTest SbFab)

target_include_directories(SbFabTest SYSTEM PRIVATE
    ../../vendor
    ${Boost_INCLUDE_DIRS}
    ${Python_INCLUDE_DIRS}
)

################################################################################

set_property(TARGET SbFab PROPERTY CXX_STANDARD 11)
set_property(TARGET SbFab PROPERTY C_STANDARD 99)
set_property(TARGET SbFabTest PROPERTY CXX_STANDARD 11)
set_property(TARGET SbFabTest PROPERTY C_STANDARD 99)
