cmake_minimum_required(VERSION 2.6)
project(libcfab)

#set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_BUILD_TYPE RELEASE)

set(CMAKE_C_FLAGS "-Wall -g -fPIC -pedantic -std=gnu99")
set(CMAKE_C_FLAGS_RELEASE  "-O3 -D '_STATIC_=static'")
set(CMAKE_C_FLAGS_DEBUG    "-O0 -D '_STATIC_= '")

# libpng is required
find_package(PNG REQUIRED)

# libm is required
find_library(M_LIB m)

# glfw and openGL are required
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
find_library(OPENGL_LIBRARY OpenGL)

include_directories(${PNG_INCLUDE_DIR} ${GLFW_INCLUDE_DIRS} .)

add_library(cfab SHARED

    tree/eval.c tree/render.c
    tree/tree.c tree/parser.c
    tree/triangulate.c

    tree/math/math_f.c tree/math/math_i.c tree/math/math_r.c

    tree/node/node.c tree/node/opcodes.c
    tree/node/printers.c tree/node/results.c

    formats/png.c

    util/region.c

    render/render.c render/tape.c render/command.c
    render/shader.c
)

# Link against libpng, libm, glfw, and opengl
target_link_libraries(cfab
    ${PNG_LIBRARY} ${M_LIB} ${GLFW_LIBRARIES} ${OPENGL_LIBRARY})

# To install, copy the cfab library up one level
install(TARGETS cfab DESTINATION ${PROJECT_SOURCE_DIR}/..)
