cmake_minimum_required(VERSION 2.6)
project(Antimony)
set(CMAKE_BUILD_TYPE RELEASE)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -g -Werror=switch")
set(CMAKE_CXX_FLAGS_RELEASE  "-O3 -DRELEASE")
set(CMAKE_CXX_FLAGS_DEBUG    "-O0")

################################################################################

find_package(PythonLibs 3.3 REQUIRED)

if (APPLE)
    find_package(Boost REQUIRED COMPONENTS python3)
elseif (UNIX)
    foreach (PYTHON_NAME python3 python-py35 python-py34)
        find_package(Boost COMPONENTS ${PYTHON_NAME})
        if (${Boost_FOUND})
            break()
        endif()
    endforeach()
endif()

################################################################################

add_subdirectory(lib/graph)
add_subdirectory(lib/fab)

add_subdirectory(app)
