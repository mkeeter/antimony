cmake_minimum_required(VERSION 2.6)
project(libcfab)

#set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_BUILD_TYPE RELEASE)

set(CMAKE_C_FLAGS "-Wall -g -fPIC -pedantic -std=gnu99")
set(CMAKE_C_FLAGS_RELEASE  "-O3 -D '_STATIC_=static'")
set(CMAKE_C_FLAGS_DEBUG    "-O0 -D '_STATIC_= '")

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})
find_library(M_LIB m)

include_directories(.)

add_library(cfab SHARED

    tree/eval.c tree/render.c
    tree/tree.c tree/parser.c

    tree/math/math_f.c tree/math/math_i.c tree/math/math_r.c

    tree/node/node.c tree/node/opcodes.c
    tree/node/printers.c tree/node/results.c

    formats/png.c

    util/region.c
)

target_link_libraries(cfab ${PNG_LIBRARY} ${M_LIB})
install(TARGETS cfab DESTINATION ${PROJECT_SOURCE_DIR}/..)
