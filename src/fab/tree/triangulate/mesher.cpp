#include "tree/triangulate/mesher.h"

#include "tree/tree.h"
#include "tree/eval.h"
#include "util/switches.h"

#if MIN_VOLUME < 60
#error "MIN_VOLUME is below minimum for meshing implementation."
#endif

// Edge and triangle tables are from Corey Bloyd (corysama@yahoo.com)
// and are releated into the public domain.
// (http://paulbourke.net/geometry/polygonise/marchingsource.cpp)
static const uint8_t VERTEX_OFFSET[8][3] =
{
    {0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0},
    {0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1}
};

// This array stores the vertices (as indexes in VERTEX_OFFSET) that
// make up the 12 edges of a single cube.
static const uint8_t EDGE_CONNECTION[12][2] =
{
    {0,1}, {1,2}, {2,3}, {3,0},
    {4,5}, {5,6}, {6,7}, {7,4},
    {0,4}, {1,5}, {2,6}, {3,7}
};

// For a given voxel occupancy mask, returns a list of vertices
// (given as edges in EDGE_CONNECTION to intersect) with which
// to build triangles.
static const int8_t TRIANGLE_CONNECTION_TABLE[256][16] =
{
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 8, 3, 0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 9, 0, 1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 8, 3, 1, 8, 1, 9,-1,-1,-1,-1,-1,-1,-1},
    {10, 1, 2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 8, 3, 0, 1, 2,10,-1,-1,-1,-1,-1,-1,-1},
    { 9, 0, 2, 9, 2,10,-1,-1,-1,-1,-1,-1,-1},
    { 3, 2, 8, 2,10, 8, 8,10, 9,-1,-1,-1,-1},
    {11, 2, 3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {11, 2, 0,11, 0, 8,-1,-1,-1,-1,-1,-1,-1},
    {11, 2, 3, 0, 1, 9,-1,-1,-1,-1,-1,-1,-1},
    { 2, 1,11, 1, 9,11,11, 9, 8,-1,-1,-1,-1},
    {10, 1, 3,10, 3,11,-1,-1,-1,-1,-1,-1,-1},
    { 1, 0,10, 0, 8,10,10, 8,11,-1,-1,-1,-1},
    { 0, 3, 9, 3,11, 9, 9,11,10,-1,-1,-1,-1},
    { 8,10, 9, 8,11,10,-1,-1,-1,-1,-1,-1,-1},
    { 8, 4, 7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 3, 0, 4, 3, 4, 7,-1,-1,-1,-1,-1,-1,-1},
    { 1, 9, 0, 8, 4, 7,-1,-1,-1,-1,-1,-1,-1},
    { 9, 4, 1, 4, 7, 1, 1, 7, 3,-1,-1,-1,-1},
    {10, 1, 2, 8, 4, 7,-1,-1,-1,-1,-1,-1,-1},
    { 2,10, 1, 0, 4, 7, 0, 7, 3,-1,-1,-1,-1},
    { 4, 7, 8, 0, 2,10, 0,10, 9,-1,-1,-1,-1},
    { 2, 7, 3, 2, 9, 7, 7, 9, 4, 2,10, 9,-1},
    { 2, 3,11, 7, 8, 4,-1,-1,-1,-1,-1,-1,-1},
    { 7,11, 4,11, 2, 4, 4, 2, 0,-1,-1,-1,-1},
    { 3,11, 2, 4, 7, 8, 9, 0, 1,-1,-1,-1,-1},
    { 2, 7,11, 2, 1, 7, 1, 4, 7, 1, 9, 4,-1},
    { 8, 4, 7,11,10, 1,11, 1, 3,-1,-1,-1,-1},
    {11, 4, 7, 1, 4,11, 1,11,10, 1, 0, 4,-1},
    { 3, 8, 0, 7,11, 4,11, 9, 4,11,10, 9,-1},
    { 7,11, 4, 4,11, 9,11,10, 9,-1,-1,-1,-1},
    { 9, 5, 4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 3, 0, 8, 4, 9, 5,-1,-1,-1,-1,-1,-1,-1},
    { 5, 4, 0, 5, 0, 1,-1,-1,-1,-1,-1,-1,-1},
    { 4, 8, 5, 8, 3, 5, 5, 3, 1,-1,-1,-1,-1},
    { 2,10, 1, 9, 5, 4,-1,-1,-1,-1,-1,-1,-1},
    { 0, 8, 3, 5, 4, 9,10, 1, 2,-1,-1,-1,-1},
    {10, 5, 2, 5, 4, 2, 2, 4, 0,-1,-1,-1,-1},
    { 3, 4, 8, 3, 2, 4, 2, 5, 4, 2,10, 5,-1},
    {11, 2, 3, 9, 5, 4,-1,-1,-1,-1,-1,-1,-1},
    { 9, 5, 4, 8,11, 2, 8, 2, 0,-1,-1,-1,-1},
    { 3,11, 2, 1, 5, 4, 1, 4, 0,-1,-1,-1,-1},
    { 8, 5, 4, 2, 5, 8, 2, 8,11, 2, 1, 5,-1},
    { 5, 4, 9, 1, 3,11, 1,11,10,-1,-1,-1,-1},
    { 0, 9, 1, 4, 8, 5, 8,10, 5, 8,11,10,-1},
    { 3, 4, 0, 3,10, 4, 4,10, 5, 3,11,10,-1},
    { 4, 8, 5, 5, 8,10, 8,11,10,-1,-1,-1,-1},
    { 9, 5, 7, 9, 7, 8,-1,-1,-1,-1,-1,-1,-1},
    { 0, 9, 3, 9, 5, 3, 3, 5, 7,-1,-1,-1,-1},
    { 8, 0, 7, 0, 1, 7, 7, 1, 5,-1,-1,-1,-1},
    { 1, 7, 3, 1, 5, 7,-1,-1,-1,-1,-1,-1,-1},
    { 1, 2,10, 5, 7, 8, 5, 8, 9,-1,-1,-1,-1},
    { 9, 1, 0,10, 5, 2, 5, 3, 2, 5, 7, 3,-1},
    { 5, 2,10, 8, 2, 5, 8, 5, 7, 8, 0, 2,-1},
    {10, 5, 2, 2, 5, 3, 5, 7, 3,-1,-1,-1,-1},
    {11, 2, 3, 8, 9, 5, 8, 5, 7,-1,-1,-1,-1},
    { 9, 2, 0, 9, 7, 2, 2, 7,11, 9, 5, 7,-1},
    { 0, 3, 8, 2, 1,11, 1, 7,11, 1, 5, 7,-1},
    { 2, 1,11,11, 1, 7, 1, 5, 7,-1,-1,-1,-1},
    { 3, 9, 1, 3, 8, 9, 7,11,10, 7,10, 5,-1},
    { 9, 1, 0,10, 7,11,10, 5, 7,-1,-1,-1,-1},
    { 3, 8, 0, 7,10, 5, 7,11,10,-1,-1,-1,-1},
    {11, 5, 7,11,10, 5,-1,-1,-1,-1,-1,-1,-1},
    {10, 6, 5,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 8, 3, 0,10, 6, 5,-1,-1,-1,-1,-1,-1,-1},
    { 0, 1, 9, 5,10, 6,-1,-1,-1,-1,-1,-1,-1},
    {10, 6, 5, 9, 8, 3, 9, 3, 1,-1,-1,-1,-1},
    { 1, 2, 6, 1, 6, 5,-1,-1,-1,-1,-1,-1,-1},
    { 0, 8, 3, 2, 6, 5, 2, 5, 1,-1,-1,-1,-1},
    { 5, 9, 6, 9, 0, 6, 6, 0, 2,-1,-1,-1,-1},
    { 9, 6, 5, 3, 6, 9, 3, 9, 8, 3, 2, 6,-1},
    { 3,11, 2,10, 6, 5,-1,-1,-1,-1,-1,-1,-1},
    { 6, 5,10, 2, 0, 8, 2, 8,11,-1,-1,-1,-1},
    { 1, 9, 0, 6, 5,10,11, 2, 3,-1,-1,-1,-1},
    { 1,10, 2, 5, 9, 6, 9,11, 6, 9, 8,11,-1},
    {11, 6, 3, 6, 5, 3, 3, 5, 1,-1,-1,-1,-1},
    { 0, 5, 1, 0,11, 5, 5,11, 6, 0, 8,11,-1},
    { 0, 5, 9, 0, 3, 5, 3, 6, 5, 3,11, 6,-1},
    { 5, 9, 6, 6, 9,11, 9, 8,11,-1,-1,-1,-1},
    {10, 6, 5, 4, 7, 8,-1,-1,-1,-1,-1,-1,-1},
    { 5,10, 6, 7, 3, 0, 7, 0, 4,-1,-1,-1,-1},
    { 5,10, 6, 0, 1, 9, 8, 4, 7,-1,-1,-1,-1},
    { 4, 5, 9, 6, 7,10, 7, 1,10, 7, 3, 1,-1},
    { 7, 8, 4, 5, 1, 2, 5, 2, 6,-1,-1,-1,-1},
    { 4, 1, 0, 4, 5, 1, 6, 7, 3, 6, 3, 2,-1},
    { 9, 4, 5, 8, 0, 7, 0, 6, 7, 0, 2, 6,-1},
    { 4, 5, 9, 6, 3, 2, 6, 7, 3,-1,-1,-1,-1},
    { 7, 8, 4, 2, 3,11,10, 6, 5,-1,-1,-1,-1},
    {11, 6, 7,10, 2, 5, 2, 4, 5, 2, 0, 4,-1},
    {11, 6, 7, 8, 0, 3, 1,10, 2, 9, 4, 5,-1},
    { 6, 7,11, 1,10, 2, 9, 4, 5,-1,-1,-1,-1},
    { 6, 7,11, 4, 5, 8, 5, 3, 8, 5, 1, 3,-1},
    { 6, 7,11, 4, 1, 0, 4, 5, 1,-1,-1,-1,-1},
    { 4, 5, 9, 3, 8, 0,11, 6, 7,-1,-1,-1,-1},
    { 9, 4, 5, 7,11, 6,-1,-1,-1,-1,-1,-1,-1},
    {10, 6, 4,10, 4, 9,-1,-1,-1,-1,-1,-1,-1},
    { 8, 3, 0, 9,10, 6, 9, 6, 4,-1,-1,-1,-1},
    { 1,10, 0,10, 6, 0, 0, 6, 4,-1,-1,-1,-1},
    { 8, 6, 4, 8, 1, 6, 6, 1,10, 8, 3, 1,-1},
    { 9, 1, 4, 1, 2, 4, 4, 2, 6,-1,-1,-1,-1},
    { 1, 0, 9, 3, 2, 8, 2, 4, 8, 2, 6, 4,-1},
    { 2, 4, 0, 2, 6, 4,-1,-1,-1,-1,-1,-1,-1},
    { 3, 2, 8, 8, 2, 4, 2, 6, 4,-1,-1,-1,-1},
    { 2, 3,11, 6, 4, 9, 6, 9,10,-1,-1,-1,-1},
    { 0,10, 2, 0, 9,10, 4, 8,11, 4,11, 6,-1},
    {10, 2, 1,11, 6, 3, 6, 0, 3, 6, 4, 0,-1},
    {10, 2, 1,11, 4, 8,11, 6, 4,-1,-1,-1,-1},
    { 1, 4, 9,11, 4, 1,11, 1, 3,11, 6, 4,-1},
    { 0, 9, 1, 4,11, 6, 4, 8,11,-1,-1,-1,-1},
    {11, 6, 3, 3, 6, 0, 6, 4, 0,-1,-1,-1,-1},
    { 8, 6, 4, 8,11, 6,-1,-1,-1,-1,-1,-1,-1},
    { 6, 7,10, 7, 8,10,10, 8, 9,-1,-1,-1,-1},
    { 9, 3, 0, 6, 3, 9, 6, 9,10, 6, 7, 3,-1},
    { 6, 1,10, 6, 7, 1, 7, 0, 1, 7, 8, 0,-1},
    { 6, 7,10,10, 7, 1, 7, 3, 1,-1,-1,-1,-1},
    { 7, 2, 6, 7, 9, 2, 2, 9, 1, 7, 8, 9,-1},
    { 1, 0, 9, 3, 6, 7, 3, 2, 6,-1,-1,-1,-1},
    { 8, 0, 7, 7, 0, 6, 0, 2, 6,-1,-1,-1,-1},
    { 2, 7, 3, 2, 6, 7,-1,-1,-1,-1,-1,-1,-1},
    { 7,11, 6, 3, 8, 2, 8,10, 2, 8, 9,10,-1},
    {11, 6, 7,10, 0, 9,10, 2, 0,-1,-1,-1,-1},
    { 2, 1,10, 7,11, 6, 8, 0, 3,-1,-1,-1,-1},
    { 1,10, 2, 6, 7,11,-1,-1,-1,-1,-1,-1,-1},
    { 7,11, 6, 3, 9, 1, 3, 8, 9,-1,-1,-1,-1},
    { 9, 1, 0,11, 6, 7,-1,-1,-1,-1,-1,-1,-1},
    { 0, 3, 8,11, 6, 7,-1,-1,-1,-1,-1,-1,-1},
    {11, 6, 7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {11, 7, 6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 0, 8, 3,11, 7, 6,-1,-1,-1,-1,-1,-1,-1},
    { 9, 0, 1,11, 7, 6,-1,-1,-1,-1,-1,-1,-1},
    { 7, 6,11, 3, 1, 9, 3, 9, 8,-1,-1,-1,-1},
    { 1, 2,10, 6,11, 7,-1,-1,-1,-1,-1,-1,-1},
    { 2,10, 1, 7, 6,11, 8, 3, 0,-1,-1,-1,-1},
    {11, 7, 6,10, 9, 0,10, 0, 2,-1,-1,-1,-1},
    { 7, 6,11, 3, 2, 8, 8, 2,10, 8,10, 9,-1},
    { 2, 3, 7, 2, 7, 6,-1,-1,-1,-1,-1,-1,-1},
    { 8, 7, 0, 7, 6, 0, 0, 6, 2,-1,-1,-1,-1},
    { 1, 9, 0, 3, 7, 6, 3, 6, 2,-1,-1,-1,-1},
    { 7, 6, 2, 7, 2, 9, 2, 1, 9, 7, 9, 8,-1},
    { 6,10, 7,10, 1, 7, 7, 1, 3,-1,-1,-1,-1},
    { 6,10, 1, 6, 1, 7, 7, 1, 0, 7, 0, 8,-1},
    { 9, 0, 3, 6, 9, 3, 6,10, 9, 6, 3, 7,-1},
    { 6,10, 7, 7,10, 8,10, 9, 8,-1,-1,-1,-1},
    { 8, 4, 6, 8, 6,11,-1,-1,-1,-1,-1,-1,-1},
    {11, 3, 6, 3, 0, 6, 6, 0, 4,-1,-1,-1,-1},
    { 0, 1, 9, 4, 6,11, 4,11, 8,-1,-1,-1,-1},
    { 1, 9, 4,11, 1, 4,11, 3, 1,11, 4, 6,-1},
    {10, 1, 2,11, 8, 4,11, 4, 6,-1,-1,-1,-1},
    {10, 1, 2,11, 3, 6, 6, 3, 0, 6, 0, 4,-1},
    { 0, 2,10, 0,10, 9, 4,11, 8, 4, 6,11,-1},
    { 2,11, 3, 6, 9, 4, 6,10, 9,-1,-1,-1,-1},
    { 3, 8, 2, 8, 4, 2, 2, 4, 6,-1,-1,-1,-1},
    { 2, 0, 4, 2, 4, 6,-1,-1,-1,-1,-1,-1,-1},
    { 1, 9, 0, 3, 8, 2, 2, 8, 4, 2, 4, 6,-1},
    { 9, 4, 1, 1, 4, 2, 4, 6, 2,-1,-1,-1,-1},
    { 8, 4, 6, 8, 6, 1, 6,10, 1, 8, 1, 3,-1},
    { 1, 0,10,10, 0, 6, 0, 4, 6,-1,-1,-1,-1},
    { 8, 0, 3, 9, 6,10, 9, 4, 6,-1,-1,-1,-1},
    {10, 4, 6,10, 9, 4,-1,-1,-1,-1,-1,-1,-1},
    { 9, 5, 4, 7, 6,11,-1,-1,-1,-1,-1,-1,-1},
    { 4, 9, 5, 3, 0, 8,11, 7, 6,-1,-1,-1,-1},
    { 6,11, 7, 4, 0, 1, 4, 1, 5,-1,-1,-1,-1},
    { 6,11, 7, 4, 8, 5, 5, 8, 3, 5, 3, 1,-1},
    { 6,11, 7, 1, 2,10, 9, 5, 4,-1,-1,-1,-1},
    {11, 7, 6, 8, 3, 0, 1, 2,10, 9, 5, 4,-1},
    {11, 7, 6,10, 5, 2, 2, 5, 4, 2, 4, 0,-1},
    { 7, 4, 8, 2,11, 3,10, 5, 6,-1,-1,-1,-1},
    { 4, 9, 5, 6, 2, 3, 6, 3, 7,-1,-1,-1,-1},
    { 9, 5, 4, 8, 7, 0, 0, 7, 6, 0, 6, 2,-1},
    { 4, 0, 1, 4, 1, 5, 6, 3, 7, 6, 2, 3,-1},
    { 7, 4, 8, 5, 2, 1, 5, 6, 2,-1,-1,-1,-1},
    { 4, 9, 5, 6,10, 7, 7,10, 1, 7, 1, 3,-1},
    { 5, 6,10, 0, 9, 1, 8, 7, 4,-1,-1,-1,-1},
    { 5, 6,10, 7, 0, 3, 7, 4, 0,-1,-1,-1,-1},
    {10, 5, 6, 4, 8, 7,-1,-1,-1,-1,-1,-1,-1},
    { 5, 6, 9, 6,11, 9, 9,11, 8,-1,-1,-1,-1},
    { 0, 9, 5, 0, 5, 3, 3, 5, 6, 3, 6,11,-1},
    { 0, 1, 5, 0, 5,11, 5, 6,11, 0,11, 8,-1},
    {11, 3, 6, 6, 3, 5, 3, 1, 5,-1,-1,-1,-1},
    { 1, 2,10, 5, 6, 9, 9, 6,11, 9,11, 8,-1},
    { 1, 0, 9, 6,10, 5,11, 3, 2,-1,-1,-1,-1},
    { 6,10, 5, 2, 8, 0, 2,11, 8,-1,-1,-1,-1},
    { 3, 2,11,10, 5, 6,-1,-1,-1,-1,-1,-1,-1},
    { 9, 5, 6, 3, 9, 6, 3, 8, 9, 3, 6, 2,-1},
    { 5, 6, 9, 9, 6, 0, 6, 2, 0,-1,-1,-1,-1},
    { 0, 3, 8, 2, 5, 6, 2, 1, 5,-1,-1,-1,-1},
    { 1, 6, 2, 1, 5, 6,-1,-1,-1,-1,-1,-1,-1},
    {10, 5, 6, 9, 3, 8, 9, 1, 3,-1,-1,-1,-1},
    { 0, 9, 1, 5, 6,10,-1,-1,-1,-1,-1,-1,-1},
    { 8, 0, 3,10, 5, 6,-1,-1,-1,-1,-1,-1,-1},
    {10, 5, 6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {11, 7, 5,11, 5,10,-1,-1,-1,-1,-1,-1,-1},
    { 3, 0, 8, 7, 5,10, 7,10,11,-1,-1,-1,-1},
    { 9, 0, 1,10,11, 7,10, 7, 5,-1,-1,-1,-1},
    { 3, 1, 9, 3, 9, 8, 7,10,11, 7, 5,10,-1},
    { 2,11, 1,11, 7, 1, 1, 7, 5,-1,-1,-1,-1},
    { 0, 8, 3, 2,11, 1, 1,11, 7, 1, 7, 5,-1},
    { 9, 0, 2, 9, 2, 7, 2,11, 7, 9, 7, 5,-1},
    {11, 3, 2, 8, 5, 9, 8, 7, 5,-1,-1,-1,-1},
    {10, 2, 5, 2, 3, 5, 5, 3, 7,-1,-1,-1,-1},
    { 5,10, 2, 8, 5, 2, 8, 7, 5, 8, 2, 0,-1},
    { 9, 0, 1,10, 2, 5, 5, 2, 3, 5, 3, 7,-1},
    { 1,10, 2, 5, 8, 7, 5, 9, 8,-1,-1,-1,-1},
    { 1, 3, 7, 1, 7, 5,-1,-1,-1,-1,-1,-1,-1},
    { 8, 7, 0, 0, 7, 1, 7, 5, 1,-1,-1,-1,-1},
    { 0, 3, 9, 9, 3, 5, 3, 7, 5,-1,-1,-1,-1},
    { 9, 7, 5, 9, 8, 7,-1,-1,-1,-1,-1,-1,-1},
    { 4, 5, 8, 5,10, 8, 8,10,11,-1,-1,-1,-1},
    { 3, 0, 4, 3, 4,10, 4, 5,10, 3,10,11,-1},
    { 0, 1, 9, 4, 5, 8, 8, 5,10, 8,10,11,-1},
    { 5, 9, 4, 1,11, 3, 1,10,11,-1,-1,-1,-1},
    { 8, 4, 5, 2, 8, 5, 2,11, 8, 2, 5, 1,-1},
    { 3, 2,11, 1, 4, 5, 1, 0, 4,-1,-1,-1,-1},
    { 9, 4, 5, 8, 2,11, 8, 0, 2,-1,-1,-1,-1},
    {11, 3, 2, 9, 4, 5,-1,-1,-1,-1,-1,-1,-1},
    { 3, 8, 4, 3, 4, 2, 2, 4, 5, 2, 5,10,-1},
    {10, 2, 5, 5, 2, 4, 2, 0, 4,-1,-1,-1,-1},
    { 0, 3, 8, 5, 9, 4,10, 2, 1,-1,-1,-1,-1},
    { 2, 1,10, 9, 4, 5,-1,-1,-1,-1,-1,-1,-1},
    { 4, 5, 8, 8, 5, 3, 5, 1, 3,-1,-1,-1,-1},
    { 5, 0, 4, 5, 1, 0,-1,-1,-1,-1,-1,-1,-1},
    { 3, 8, 0, 4, 5, 9,-1,-1,-1,-1,-1,-1,-1},
    { 9, 4, 5,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 7, 4,11, 4, 9,11,11, 9,10,-1,-1,-1,-1},
    { 3, 0, 8, 7, 4,11,11, 4, 9,11, 9,10,-1},
    {11, 7, 4, 1,11, 4, 1,10,11, 1, 4, 0,-1},
    { 8, 7, 4,11, 1,10,11, 3, 1,-1,-1,-1,-1},
    { 2,11, 7, 2, 7, 1, 1, 7, 4, 1, 4, 9,-1},
    { 3, 2,11, 4, 8, 7, 9, 1, 0,-1,-1,-1,-1},
    { 7, 4,11,11, 4, 2, 4, 0, 2,-1,-1,-1,-1},
    { 2,11, 3, 7, 4, 8,-1,-1,-1,-1,-1,-1,-1},
    { 2, 3, 7, 2, 7, 9, 7, 4, 9, 2, 9,10,-1},
    { 4, 8, 7, 0,10, 2, 0, 9,10,-1,-1,-1,-1},
    { 2, 1,10, 0, 7, 4, 0, 3, 7,-1,-1,-1,-1},
    {10, 2, 1, 8, 7, 4,-1,-1,-1,-1,-1,-1,-1},
    { 9, 1, 4, 4, 1, 7, 1, 3, 7,-1,-1,-1,-1},
    { 1, 0, 9, 8, 7, 4,-1,-1,-1,-1,-1,-1,-1},
    { 3, 4, 0, 3, 7, 4,-1,-1,-1,-1,-1,-1,-1},
    { 8, 7, 4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 8, 9,10, 8,10,11,-1,-1,-1,-1,-1,-1,-1},
    { 0, 9, 3, 3, 9,11, 9,10,11,-1,-1,-1,-1},
    { 1,10, 0, 0,10, 8,10,11, 8,-1,-1,-1,-1},
    {10, 3, 1,10,11, 3,-1,-1,-1,-1,-1,-1,-1},
    { 2,11, 1, 1,11, 9,11, 8, 9,-1,-1,-1,-1},
    {11, 3, 2, 0, 9, 1,-1,-1,-1,-1,-1,-1,-1},
    {11, 0, 2,11, 8, 0,-1,-1,-1,-1,-1,-1,-1},
    {11, 3, 2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 3, 8, 2, 2, 8,10, 8, 9,10,-1,-1,-1,-1},
    { 9, 2, 0, 9,10, 2,-1,-1,-1,-1,-1,-1,-1},
    { 8, 0, 3, 1,10, 2,-1,-1,-1,-1,-1,-1,-1},
    {10, 2, 1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 8, 1, 3, 8, 9, 1,-1,-1,-1,-1,-1,-1,-1},
    { 9, 1, 0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    { 8, 0, 3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}
};


Mesher::Mesher(MathTree* tree, bool detect_edges)
    : tree(tree), detect_edges(detect_edges),
      data(new float[MIN_VOLUME]), has_data(false),
      X(new float[MIN_VOLUME]),
      Y(new float[MIN_VOLUME]),
      Z(new float[MIN_VOLUME]),
      ex(new float[MIN_VOLUME]),
      ey(new float[MIN_VOLUME]),
      ez(new float[MIN_VOLUME]),
      nx(new float[MIN_VOLUME]),
      ny(new float[MIN_VOLUME]),
      nz(new float[MIN_VOLUME])
{
    // Nothing to do here
}

Mesher::~Mesher()
{
    for (auto ptr : {data, X, Y, Z, ex, ey, ez, nx, ny, nz})
        delete [] ptr;
}


// Returns the normals of the provided triangle.
Triangle Mesher::get_normals(const Triangle& t)
{
    // Get a small value for epsilon by taking 1/10th the smallest edge length
    // (this will be our sampling step for normal estimation).
    const float len_a = (t.a - t.b).norm();
    const float len_b = (t.b - t.c).norm();
    const float len_c = (t.c - t.a).norm();
    const float epsilon = fmin(len_a, fmin(len_b, len_c)) / 10.0f;

    // We'll be evaluating a dummy region to numerically estimate gradients
    Region dummy = (Region){
        .X = nx, .Y = ny, .Z = nz,
        .voxels = 12};

    // Load position data into the dummy region
    int i=0;
    for (auto v : {t.a, t.b, t.c})
    {
        dummy.X[i]   = v[0];
        dummy.X[i+1] = v[0] + epsilon;
        dummy.X[i+2] = v[0];
        dummy.X[i+3] = v[0];

        dummy.Y[i]   = v[1];
        dummy.Y[i+1] = v[1];
        dummy.Y[i+2] = v[1] + epsilon;
        dummy.Y[i+3] = v[1];

        dummy.Z[i]   = v[2];
        dummy.Z[i+1] = v[2];
        dummy.Z[i+2] = v[2];
        dummy.Z[i+3] = v[2] + epsilon;
        i += 4;
    }

    float* out = eval_r(tree, dummy);

    // Extract normals from the evaluated data.
    Triangle normals;
    i = 0;
    for (auto n : {&normals.a, &normals.b, &normals.c})
    {
        const float dx = out[i+1] - out[i];
        const float dy = out[i+2] - out[i];
        const float dz = out[i+3] - out[i];
        *n = Vec3f(dx, dy, dz).normalized();
        i += 4;
    }

    return normals;
}

// Mark that the first edge of the most recent triangle is swappable
// (as part of feature detection / extraction).
void Mesher::mark_swappable()
{
    /*
    const unsigned c = verts.size();
    std::array<float, 6> key = {{
        verts[c-9], verts[c-8], verts[c-7],
        verts[c-6], verts[c-5], verts[c-4]}};

    auto found = swappable.find(key);
    if (found != swappable.end())
    {
        const unsigned a = found->second;
        const unsigned b = c - 9;

        // If the new triangle is overlapping with its neighbor, then
        // merge the two instead of letting them overlap.
        const Vec3f e = (Vec3f){verts[a+3] - verts[a],
                                verts[a+4] - verts[a+1],
                                verts[a+5] - verts[a+2]};
        const float dot = vec3f_dot(e,
                vec3f_cross(get_triangle_normal(a),
                            get_triangle_normal(b)));

        if (dot >= 0)
        {
            Vec3f swap = (Vec3f){verts[a+6], verts[a+7], verts[a+8]};
            // Loop through and find other instances of this vertex,
            // then replace them with the new vertex.
            for (int i=a-18; i > a+27; i += 3)
            {
                if (i < 0 || i + 2 >= verts.size())
                    continue;

                if (verts[i] == verts[a+6] &&
                    verts[i+1] == verts[a+7] &&
                    verts[i+2] == verts[a+8])
                {
                    verts[i] = verts[b+6];
                    verts[i+1] = verts[b+7];
                    verts[i+2] = verts[b+8];
                }
            }
            removed.push_back(a);
        }
        else
        {
            for (int i=0; i < 3; ++i)
            {
                verts[a + 3 + i] = verts[b + 6 + i];
                verts[b + 3 + i] = verts[a + 6 + i];
            }
        }
        swappable.erase(found);
    }
    else
    {
        std::array<float, 6> reversed = {{
            verts[c-6], verts[c-5], verts[c-4],
            verts[c-9], verts[c-8], verts[c-7]}};
        swappable[reversed] = c - 9;
    }
    */
}

Vec3f Mesher::plane_plane_point(const Vec3f& a, const Vec3f& na,
                                const Vec3f& b, const Vec3f& nb,
                                const Vec3f& c)
{
    /*
    // Convert into Hessian normal form
    const float pa = -vec3f_dot(na, a);
    const float pb = -vec3f_dot(nb, b);

    // Finding the edge direction is easy:
    const Vec3f edge = vec3f_cross(na, nb);

    // Finding a point on the edge is a bit tricker.
    // We collapse the undetermined system by setting x, y, or z=0
    // (depending on the edge's normal).
    float eq1[2];
    float eq2[2];
    char zeroed;
    if (fabs(edge.x) > 0.01)
    {
        eq1[0] = na.y; eq1[1] = na.z;
        eq2[0] = nb.y; eq2[1] = nb.z;
        zeroed = 'x';
    }
    else if (fabs(edge.y) > 0.01)
    {
        eq1[0] = na.x; eq1[1] = na.z;
        eq2[0] = nb.x; eq2[1] = nb.z;
        zeroed = 'y';
    } else {
        eq1[0] = na.x; eq1[1] = na.y;
        eq2[0] = nb.x; eq2[1] = nb.y;
        zeroed = 'z';
    }

    // Then, solve the pair of simultaneous equations
    const float z = (-pa + eq1[0]/eq2[0]*pb) /
                    ((eq1[1] - eq1[0]/eq2[0]) * eq2[1]);
    const float y = (-pa - eq1[1]*z) / eq1[0];

    // Lastly, create the point at the right place, depending on which
    // term had been zeroed to collapse the underdetermined system.
    Vec3f edge_pt;
    if (zeroed == 'x')
        edge_pt = (Vec3f){0, y, z};
    else if (zeroed == 'y')
        edge_pt = (Vec3f){y, 0, z};
    else
        edge_pt = (Vec3f){y, z, 0};

    // Lastly, work out the point on the edge that is the closest to the
    // provided point c.
    const float t = vec3f_dot(edge, (Vec3f){c.x - edge_pt.x,
                                            c.y - edge_pt.y,
                                            c.z - edge_pt.z});
    return (Vec3f){edge_pt.x + t * edge.x,
                   edge_pt.y + t * edge.y,
                   edge_pt.z + t * edge.z};
                   */
}

Vec3f Mesher::edge_feature_point(const Vec3f& a, const Vec3f& na,
                                 const Vec3f& b, const Vec3f& nb,
                                 const Vec3f& c, const Vec3f& nc,
                                 const Vec3f& fallback)
{
    // Find two combined edge normals
    Vec3f n_ab = edge_normal(a, b, c);
    Vec3f n_bc = edge_normal(b, c, a);

    Vec3f center = (a + b + c) / 3;

    Vec3f p0 = plane_intersection(a, na, b, nb, center, n_ab, fallback);
    Vec3f p1 = plane_intersection(c, nc, b, nb, center, n_bc, fallback);

    Vec3f d0 = p0 - center;
    Vec3f d1 = p1 - center;

    return d0.norm() < d1.norm() ? p0 : p1;
}

bool Mesher::point_in_triangle(const Vec3f& a, const Vec3f& b,
                               const Vec3f& c, const Vec3f& p)
{
    // Triangle's normal
    Eigen::Vector3d n = (b - a).cross(c - a).normalized();

    // Projected point
    Eigen::Vector3d proj = p - n*(p - a).dot(n);

    // Check that the point is on the correct side of the three
    // lines in the triangle; returning false otherwise.
    if ((proj - a).cross(c - a).dot(n) <= 0)
        return false;
    if ((proj - b).cross(a - b).dot(n) <= 0)
        return false;
    if ((proj - c).cross(b - c).dot(n) <= 0)
        return false;
    return true;
}

void Mesher::process_feature(Mesher::FeatureType type, const Triangle& tri,
                             const Triangle& normals)
{
    // Pick out a new center point
    Vec3f intersection = (tri.a + tri.b + tri.c) / 3;

    if (type == FEATURE_CORNER)
    {
        intersection = plane_intersection(tri.a, normals.a,
                                          tri.b, normals.b,
                                          tri.c, normals.c, intersection);
    }
    else if (type == FEATURE_EDGE_AB_C)
    {
        intersection = edge_feature_point(tri.b, normals.b,
                                          tri.c, normals.c,
                                          tri.a, normals.a, intersection);
    }
    else if (type == FEATURE_EDGE_BC_A)
    {
        intersection = edge_feature_point(tri.c, normals.c,
                                          tri.a, normals.a,
                                          tri.b, normals.b, intersection);
    }
    else if (type == FEATURE_EDGE_CA_B)
    {
        intersection = edge_feature_point(tri.a, normals.a,
                                          tri.b, normals.b,
                                          tri.c, normals.c, intersection);
    }

    // Add the new (split) triangles
    triangles.push_back(Triangle(tri.a, tri.b, intersection));
    if (type != FEATURE_EDGE_AB_C)
        mark_swappable();

    triangles.push_back(Triangle(tri.b, tri.c, intersection));
    if (type != FEATURE_EDGE_BC_A)
        mark_swappable();

    triangles.push_back(Triangle(tri.c, tri.a, intersection));
    if (type != FEATURE_EDGE_CA_B)
        mark_swappable();
}

void Mesher::add_triangle(const Triangle& t)
{
    // Skip edge detection if it is turned off.
    if (!detect_edges)
    {
        triangles.push_back(t);
        return;
    }

    // If any of the normals could not be estimated, return immediately.
    Triangle normals = get_normals(t);
    for (auto v : {normals.a, normals.b, normals.c})
        if (v[0] == 0 && v[1] == 0 && v[2] == 0)
        {
            triangles.push_back(t);
            return;
        }

    const float ab = fabs(normals.a.dot(normals.b));
    const float bc = fabs(normals.b.dot(normals.c));
    const float ca = fabs(normals.c.dot(normals.a));

    const float theta = fmin(fmin(ab, bc), ca);

    // First cutoff -- if there isn't a feature, then return immediately
    if (theta > 0.9)
        return;

    Vec3f nstar;
    if (ab < bc && ab < ca)
        nstar = normals.a.cross(normals.b);
    else if (bc < ab && bc < ca)
        nstar = normals.b.cross(normals.c);
    else
        nstar = normals.c.cross(normals.a);


    // This is threshold for deciding that two normals are the same
    // (when they are dot-producted together)
    const float same = 0.95;

    if (ab < same && bc < same && ca < same)
        process_feature(FEATURE_CORNER, t, normals);
    else if (ab >= same && bc < same && ca < same)
        process_feature(FEATURE_EDGE_AB_C, t, normals);
    else if (ab < same && bc >= same && ca < same)
        process_feature(FEATURE_EDGE_BC_A, t, normals);
    else if (ab < same && bc < same && ca >= same)
        process_feature(FEATURE_EDGE_CA_B, t, normals);
    else
        triangles.push_back(t);
}

// Loads a vertex into the vertex list.
// If this vertex completes a triangle, check for features.
void Mesher::push_vert(const float x, const float y, const float z)
{
    triangle.push_back(Vec3f(x, y, z));
    if (triangle.size() == 3)
    {
        triangles.push_back(Triangle(triangle[0], triangle[1], triangle[2]));
        triangle.clear();

        // If this is the first triangle being constructed (or fan_start has
        // just been cleared), store an iterator to this triangle so that we
        // know where the next triangle fan begins.
        if (fan_start == triangles.end())
            fan_start--;
    }
}


// Evaluates a region voxel-by-voxel, storing the output in the data
// member of the tristate struct.
bool Mesher::load_packed(const Region& r)
{
    // Only load the packed matrix if we have few enough voxels.
    const unsigned voxels = (r.ni+1) * (r.nj+1) * (r.nk+1);
    if (voxels >= MIN_VOLUME)
        return false;

    // Do a round of interval evaluation for tree pruning
    eval_i(tree, (Interval){r.X[0], r.X[r.ni]},
                 (Interval){r.Y[0], r.Y[r.nj]},
                 (Interval){r.Z[0], r.Z[r.nk]});
    disable_nodes(tree);

    // Flatten a 3D region into a 1D list of points that
    // touches every point in the region, one by one.
    int q = 0;
    for (unsigned k=0; k <= r.nk; ++k) {
        for (unsigned j=0; j <= r.nj; ++j) {
            for (unsigned i=0; i <= r.ni; ++i) {
                X[q] = r.X[i];
                Y[q] = r.Y[j];
                Z[q] = r.Z[k];
                q++;
            }
        }
    }

    // Make a dummy region that has the newly-flattened point arrays as the
    // X, Y, Z coordinate data arrays (so that we can run eval_r on it).
    packed = (Region) {
        .imin=r.imin, .jmin=r.jmin, .kmin=r.kmin,
        .ni=r.ni, .nj=r.nj, .nk=r.nk,
        .X=X, .Y=Y, .Z=Z, .voxels=voxels};

    // Run eval_r and copy the data out
    memcpy(data, eval_r(tree, packed), voxels * sizeof(float));
    has_data = true;

    return true;
}

void Mesher::unload_packed()
{
    enable_nodes(tree);
    has_data = false;
}

void Mesher::get_corner_data(const Region& r, float d[8])
{
    // Populates an 8-element array with the function evaluation
    // results from the corner of a single-voxel region.
    for (int i=0; i < 8; ++i)
    {
        // Figure out where this bit of data lives in the larger eval_r array.
        const unsigned index =
            (r.imin - packed.imin + VERTEX_OFFSET[i][0]) +
            (r.jmin - packed.jmin + VERTEX_OFFSET[i][1])
                * (packed.ni+1) +
            (r.kmin - packed.kmin + VERTEX_OFFSET[i][2])
                * (packed.ni+1) * (packed.nj+1);

        d[i] = data[index];
    }
}

void Mesher::eval_zero_crossings(Vec3f* v0, Vec3f* v1, unsigned count)
{
    float p[count];
    for (unsigned i=0; i < count; ++i)
        p[i] = 0.5;

    float step = 0.25;

    Region dummy = (Region){
        .X = ex,
        .Y = ey,
        .Z = ez,
        .voxels = count};

    for (int iteration=0; iteration < 8; ++iteration)
    {
        // Load new data into the x, y, z arrays.
        for (unsigned i=0; i < count; i++)
        {
            dummy.X[i] = v0[i][0] * (1 - p[i]) + v1[i][0] * p[i];
            dummy.Y[i] = v0[i][1] * (1 - p[i]) + v1[i][1] * p[i];
            dummy.Z[i] = v0[i][2] * (1 - p[i]) + v1[i][2] * p[i];
        }
        float* out = eval_r(tree, dummy);

        for (unsigned i=0; i < count; i++)
            if      (out[i] < 0)    p[i] += step;
            else if (out[i] > 0)    p[i] -= step;

        step /= 2;
    }

}

// Flushes out a queue of interpolation commands
void Mesher::flush_queue()
{
    Vec3f low[MIN_VOLUME];
    Vec3f high[MIN_VOLUME];

    // Go through the list, saving a list of vertex pairs on which
    // interpolation should be run into low and high.
    unsigned count=0;
    for (auto c : queue)
    {
        if (c.cmd == InterpolateCommand::INTERPOLATE)
        {
            low[count] = c.v0;
            high[count] = c.v1;
            count++;
        }
    }

    if (count)
        eval_zero_crossings(low, high, count);

    // Next, go through and actually load vertices
    // (either directly or from the cache)
    count = 0;
    for (auto c : queue)
    {
        if (c.cmd == InterpolateCommand::INTERPOLATE)
        {
            push_vert(ex[count], ey[count], ez[count]);
            count++;
        }
        else if (c.cmd == InterpolateCommand::CACHED)
        {
            unsigned i = c.cached;
            push_vert(ex[i], ey[i], ez[i]);
        }
        else if (c.cmd == InterpolateCommand::END_OF_FAN)
        {
            // Check for features here

            fan_start = triangles.end();
        }
    }
    queue.clear();
}

// Push an END_OF_VOXEL command to the command queue.
void Mesher::end_voxel()
{
    queue.push_back((InterpolateCommand){
            .cmd=InterpolateCommand::END_OF_VOXEL});
}

// Schedule an interpolate calculation in the queue.
void Mesher::interpolate_between(const Vec3f& v0, const Vec3f& v1)
{
    InterpolateCommand next = (InterpolateCommand){
        .cmd=InterpolateCommand::INTERPOLATE, .v0=v0, .v1=v1};

    // Walk through the list, looking for duplicates.
    // If we find the same operation, then switch to a CACHED lookup instead.
    unsigned count = 0;
    for (auto c : queue)
    {
        if (c.cmd == InterpolateCommand::INTERPOLATE)
        {
            if ((v0 == c.v0 && v1 == c.v1) || (v0 == c.v1 && v1 == c.v0))
            {
                next.cmd = InterpolateCommand::CACHED;
                next.cached = count;
            }
            count++;
        }
    }

    queue.push_back(next);
    if (next.cmd == InterpolateCommand::INTERPOLATE && count + 1 == MIN_VOLUME)
        flush_queue();
}



void Mesher::triangulate_voxel(const Region& r, const float* const d)
{
    // Figure out which of the 256 possible combinations
    // we're currently experiencing.
    uint8_t lookup = 0;
    for (int i=7; i>=0; --i)
        lookup = (lookup << 1) + (d[i] >= 0);

    int i=0;
    for (i=0; TRIANGLE_CONNECTION_TABLE[lookup][i] != -1; i++)
    {
        const uint8_t edge = TRIANGLE_CONNECTION_TABLE[lookup][i];
        const uint8_t v0 = EDGE_CONNECTION[edge][0];
        const uint8_t v1 = EDGE_CONNECTION[edge][1];

        const Vec3f p0 = Vec3f(r.X[VERTEX_OFFSET[v0][0]],
                               r.Y[VERTEX_OFFSET[v0][1]],
                               r.Z[VERTEX_OFFSET[v0][2]]);
        const Vec3f p1 = Vec3f(r.X[VERTEX_OFFSET[v1][0]],
                               r.Y[VERTEX_OFFSET[v1][1]],
                               r.Z[VERTEX_OFFSET[v1][2]]);

        if (d[v0] < 0)
            interpolate_between(p0, p1);
        else
            interpolate_between(p1, p0);

        // At the end of a triangle, check to see if the next triangle
        // is disconnected.  If so, push an END_FAN message to the queue
        // (which tells edge detection to run on the previous set of
        // triangles)
        if (i % 3 == 2)
        {
            bool end_fan = true;
            // Loop over the next three vertices.  If any of them agree with
            // the previous three vertices, then mark that the fan continues
            // (otherwise, leave end_fan = true)
            for (int j=1; j <= 3; ++j)
            {
                if (TRIANGLE_CONNECTION_TABLE[lookup][i+j] == -1)
                    break;

                for (int k=0; k < 3; ++k)
                    if (TRIANGLE_CONNECTION_TABLE[lookup][i+j] ==
                        TRIANGLE_CONNECTION_TABLE[lookup][i-k])
                    {
                        end_fan = false;
                    }
            }

            if (end_fan)
                queue.push_back((InterpolateCommand){
                        .cmd=InterpolateCommand::END_OF_FAN});
        }
    }
}

Vec3f Mesher::plane_intersection(const Vec3f& pa, const Vec3f& na,
                                 const Vec3f& pb, const Vec3f& nb,
                                 const Vec3f& pc, const Vec3f& nc,
                                 const Vec3f& fallback)
{
    Eigen::Matrix3d n;
    n.col(0) << na;
    n.col(1) << nb;
    n.col(2) << nc;

    const double det = n.determinant();
    if (fabs(det) <= 0.01)
        return fallback;

    // Graphics Gems 1
    // Intersection of Three Planes
    // Ronald Goldman
    Eigen::Vector3d out =
        (pa.dot(na)*nb.cross(nc) +
         pb.dot(nb)*nc.cross(na) +
         pc.dot(nc)*na.cross(nb)) / det;

    return (Vec3f){static_cast<float>(out[0]),
                   static_cast<float>(out[1]),
                   static_cast<float>(out[2])};
}

Vec3f Mesher::edge_normal(const Vec3f& a, const Vec3f& b, const Vec3f& c)
{
    Eigen::Vector3d out = (b-a).cross(c-a).cross(b-a);

    return (Vec3f){static_cast<float>(out[0] / out.norm()),
                   static_cast<float>(out[1] / out.norm()),
                   static_cast<float>(out[2] / out.norm())};
}

void Mesher::triangulate_region(const Region& r)
{
    // If we can calculate all of the points in this region with a single
    // eval_r call, then do so.  This large chunk will be used in future
    // recursive calls to make things more efficient.
    bool loaded_data = !has_data;
    if (loaded_data)
        loaded_data = load_packed(r);

    // If we have greater than one voxel, subdivide and recurse.
    if (r.voxels > 1)
    {
        Region octants[8];
        const uint8_t split = octsect(r, octants);
        for (int i=0; i < 8; ++i)
            if (split & (1 << i))
                triangulate_region(octants[i]);
    }
    else
    {
        // Load corner values from this voxel
        // (from the packed data array)
        float d[8];
        get_corner_data(r, d);

        // Loop over the six tetrahedra that make up a voxel cell
        triangulate_voxel(r, d);
    }

    // Mark that a voxel has ended
    // (which will eventually trigger decimation)
    end_voxel();

    // If this stage of the recursion loaded data into the buffer,
    // clear the has_data flag (so that future stages will re-run
    // eval_r on their portion of the space) and re-enable disabled
    // nodes.
    if (loaded_data)
    {
        flush_queue();
        unload_packed();
    }
}

float* Mesher::get_verts(unsigned* count)
{
    // There are 9 floats in each triangle
    *count = triangles.size() * 9;

    float* out = (float*)malloc(sizeof(float) * (*count));

    unsigned i = 0;
    for (auto t : triangles)
        for (auto v : {t.a, t.b, t.c})
            for (int j=0; j < 3; ++j)
                out[i++] = v[j];

    return out;
}
