cmake_minimum_required(VERSION 2.6)
project(ao)
set(CMAKE_BUILD_TYPE RELEASE)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -g -Werror=switch -pedantic -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE  "-O3 -DRELEASE")
set(CMAKE_CXX_FLAGS_DEBUG    "-O0")

################################################################################

find_package(PythonLibs 3.3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS python3)

################################################################################

add_library(SbGraph STATIC
    src/datum.cpp
    src/graph.cpp
    src/node.cpp
    src/script_node.cpp
    src/graph_node.cpp
    src/proxy.cpp
    src/script.cpp
    src/util.cpp
    src/types/downstream.cpp
    src/types/root.cpp
    src/hooks/hooks.cpp
    src/hooks/input.cpp
    src/hooks/output.cpp
)

target_include_directories(SbGraph SYSTEM PUBLIC
    ${PYTHON_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)
target_link_libraries(SbGraph
    ${PYTHON_LIBRARY}
    ${Boost_LIBRARIES}
)
target_include_directories(SbGraph PUBLIC inc)

################################################################################

add_executable(SbGraphTest
    tests/datum.cpp
    tests/graph.cpp
    tests/link.cpp
    tests/main.cpp
    tests/node.cpp
    tests/script.cpp
    tests/subgraph.cpp
)
target_link_libraries(SbGraphTest SbGraph)

target_include_directories(SbGraphTest SYSTEM PRIVATE
    ../../vendor
)

################################################################################
