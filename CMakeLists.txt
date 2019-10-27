cmake_minimum_required(VERSION 3.1)
project(Antimony)
set(CMAKE_BUILD_TYPE RELEASE)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -g -Werror=switch")
set(CMAKE_CXX_FLAGS_RELEASE  "-O3 -DRELEASE")
set(CMAKE_CXX_FLAGS_DEBUG    "-O0")

################################################################################

find_package(Python 3.3 REQUIRED COMPONENTS Interpreter Development)

if (WIN32)
    add_definitions("-DBOOST_PYTHON_STATIC_LIB")
endif()

find_package(Boost REQUIRED COMPONENTS python${Python_VERSION_MAJOR}${Python_VERSION_MINOR})

if (NOT ${Boost_FOUND})
    message(FATAL_ERROR "Could not find boost::python3")
endif()

################################################################################

add_subdirectory(lib/graph)
add_subdirectory(lib/fab)

add_subdirectory(app)
