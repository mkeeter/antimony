cmake_minimum_required(VERSION 2.6)
project(libfab)

#set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_BUILD_TYPE RELEASE)

set(CMAKE_C_FLAGS "-Wall -g -fPIC -pedantic -std=gnu99")
set(CMAKE_C_FLAGS_RELEASE  "-O3 -D '_STATIC_=static'")
set(CMAKE_C_FLAGS_DEBUG    "-O0 -D '_STATIC_= '")

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})
find_library(M_LIB m)

include_directories(.)

add_library(fab SHARED
    asdf/asdf.c asdf/render.c asdf/file_io.c
    asdf/triangulate.c  asdf/import.c asdf/cache.c
    asdf/neighbors.c asdf/contour.c asdf/distance.c
    asdf/cms.c

    tree/eval.c tree/render.c
    tree/tree.c tree/packed.c
    tree/parser.c

    tree/math/math_f.c tree/math/math_i.c tree/math/math_r.c

    tree/node/node.c tree/node/opcodes.c
    tree/node/printers.c tree/node/results.c

    cam/toolpath.c cam/distance.c cam/slices.c

    formats/png.c formats/stl.c formats/mesh.c

    util/region.c util/vec3f.c util/path.c
)

target_link_libraries(fab ${PNG_LIBRARY} ${M_LIB})
install(TARGETS fab DESTINATION ${PROJECT_SOURCE_DIR})
