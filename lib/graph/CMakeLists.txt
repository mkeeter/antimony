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

target_include_directories(SbGraph SYSTEM PRIVATE
    ${Python_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)
target_link_libraries(SbGraph
    ${Boost_LIBRARIES}
    ${Python_LIBRARY_RELEASE}
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
    ${Python_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ../../vendor
)

################################################################################

set_property(TARGET SbGraph PROPERTY CXX_STANDARD 11)
set_property(TARGET SbGraph PROPERTY C_STANDARD 99)
set_property(TARGET SbGraphTest PROPERTY CXX_STANDARD 11)
set_property(TARGET SbGraphTest PROPERTY C_STANDARD 99)
