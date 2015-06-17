# Configuration that is common to Antimony and supporting libraries
CONFIG += c++11 object_parallel_to_source

# Bump optimization up to -O3 in release builds
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3
QMAKE_CFLAGS_RELEASE -= -O2
QMAKE_CFLAGS_RELEASE += -O3

QMAKE_CXXFLAGS += -Werror=switch
QMAKE_CFLAGS += -std=c11
