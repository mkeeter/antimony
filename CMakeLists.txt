cmake_minimum_required(VERSION 2.6)
project(Antimony)
set(CMAKE_BUILD_TYPE RELEASE)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -g -Werror=switch -pedantic")
set(CMAKE_CXX_FLAGS_RELEASE  "-O3 -DRELEASE")
set(CMAKE_CXX_FLAGS_DEBUG    "-O0")

################################################################################

find_package(PythonLibs 3.3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS python3)

################################################################################

add_subdirectory(lib/graph)
add_subdirectory(lib/fab)

add_subdirectory(app)
