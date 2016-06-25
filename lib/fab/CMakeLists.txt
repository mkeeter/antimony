cmake_minimum_required(VERSION 2.6)
set(CMAKE_BUILD_TYPE RELEASE)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -g -Werror=switch -pedantic -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE  "-O3 -DRELEASE")
set(CMAKE_CXX_FLAGS_DEBUG    "-O0")

################################################################################

find_package(PythonLibs 3.3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS python3)
find_package(PNG REQUIRED)
find_package(FLEX REQUIRED)

################################################################################

# Custom command to generate the parser sources with lemon
find_program(LEMON_EXECUTABLE lemon DOC "path to the lemon executable")
mark_as_advanced(LEMON_EXECUTABLE)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/v2syntax.lemon.hpp
           ${CMAKE_CURRENT_BINARY_DIR}/v2syntax.lemon.cpp
    COMMAND ${LEMON_EXECUTABLE} -q -c -s ${CMAKE_CURRENT_SOURCE_DIR}/src/tree/v2syntax.y
    COMMAND mv ${CMAKE_CURRENT_SOURCE_DIR}/src/tree/v2syntax.c ${CMAKE_CURRENT_BINARY_DIR}/v2syntax.lemon.cpp
    COMMAND mv ${CMAKE_CURRENT_SOURCE_DIR}/src/tree/v2syntax.h ${CMAKE_CURRENT_BINARY_DIR}/v2syntax.lemon.hpp)

# Custom command to generate the lexer sources with flex
find_program(FLEX_EXECUTABLE flex DOC "path to the flex executable")
mark_as_advanced(FLEX_EXECUTABLE)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/v2syntax.yy.hpp
           ${CMAKE_CURRENT_BINARY_DIR}/v2syntax.yy.cpp
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

target_include_directories(SbFab SYSTEM PUBLIC
    ${PYTHON_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${PNG_INCLUDE_DIR}
)

# Add the binary dir to include generated files
target_include_directories(SbFab PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(SbFab
    ${PYTHON_LIBRARY}
    ${Boost_LIBRARIES}
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
)

################################################################################

