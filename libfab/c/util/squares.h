#include <stdint.h>

/*
    Data tables for marching squares


    Vertices have the following IDs
   1        3
    --------
    |      |
    |      |       ^ Y
    |      |       |
    --------       --> X
   0        2

    (divided by two from d[i] indices, since we're
     looking at a flat ASDF and we don't care about z)


    Edges are numbered as follows:
       1
    --------
    |      |
  2 |      | 3     ^ Y
    |      |       |
    --------       --> X
        0


*/

// For a given set of filled corners, this array defines
// the cell edges from which we draw interior edges
static const int8_t EDGE_MAP[16][2][2] = {
    {{-1, -1}, {-1, -1}}, // ----
    {{0, 2}, {-1, -1}},  // ---0
    {{2, 1}, {-1, -1}},  // --1-
    {{0, 1}, {-1, -1}},  // --10
    {{3, 0}, {-1, -1}},  // -2--
    {{3, 2}, {-1, -1}},  // -2-0
    {{3, 0}, { 2,  1}},  // -21-
    {{3, 1}, {-1, -1}},  // -210

    {{1, 3}, {-1, -1}},  // 3---
    {{1, 3}, { 0,  2}},  // 3--0
    {{2, 3}, {-1, -1}},  // 3-1-
    {{0, 3}, {-1, -1}},  // 3-10
    {{1, 0}, {-1, -1}},  // 32--
    {{1, 2}, {-1, -1}},  // 32-0
    {{2, 0}, {-1, -1}},  // 321-
    {{-1,-1}, {-1, -1}}  // 3210
};


// Indexed by edge number, returns vertex index
static const int8_t VERTEX_MAP[4][2] = {
    {0, 2},
    {3, 1},
    {1, 0},
    {2, 3}
};
