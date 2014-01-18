#include <stdlib.h>

#include "triangulate.h"

#include "tree/tree.h"
#include "util/constants.h"

static const uint8_t VERTEX_LOOP[] = {6, 4, 5, 1, 3, 2, 6};

// Based on which vertices are filled, this map tells you which
// edges to interpolate between when forming zero, one, or two
// triangles.
// (filled vertex is first in the pair)
static const int EDGE_MAP[16][2][3][2] = {
    {{{-1,-1}, {-1,-1}, {-1,-1}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // ----
    {{{ 0, 2}, { 0, 1}, { 0, 3}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // ---0
    {{{ 1, 0}, { 1, 2}, { 1, 3}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // --1-
    {{{ 1, 2}, { 1, 3}, { 0, 3}}, {{ 0, 3}, { 0, 2}, { 1, 2}}}, // --10
    {{{ 2, 0}, { 2, 3}, { 2, 1}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // -2--
    {{{ 0, 3}, { 2, 3}, { 2, 1}}, {{ 2, 1}, { 0, 1}, { 0, 3}}}, // -2-0
    {{{ 1, 0}, { 2, 0}, { 2, 3}}, {{ 2, 3}, { 1, 3}, { 1, 0}}}, // -21-
    {{{ 2, 3}, { 1, 3}, { 0, 3}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // -210

    {{{ 3, 0}, { 3, 1}, { 3, 2}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // 3---
    {{{ 3, 2}, { 0, 2}, { 0, 1}}, {{ 0, 1}, { 3, 1}, { 3, 2}}}, // 3--0
    {{{ 1, 2}, { 3, 2}, { 3, 0}}, {{ 3, 0}, { 1, 0}, { 1, 2}}}, // 3-1-
    {{{ 1, 2}, { 3, 2}, { 0, 2}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // 3-10
    {{{ 3, 0}, { 3, 1}, { 2, 1}}, {{ 2, 1}, { 2, 0}, { 3, 0}}}, // 32--
    {{{ 3, 1}, { 2, 1}, { 0, 1}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // 32-0
    {{{ 3, 0}, { 1, 0}, { 2, 0}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // 321-
    {{{-1,-1}, {-1,-1}, {-1,-1}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // 3210
};


// Performs trilinear interpolation, where d is an array of corner values
// on a unit cube (d[0] at (0,0,0), d[1] at (0,0,1), d[2] at (0,1,0), etc.
_STATIC_
float interpolate(const float d[8],
                  const float x, const float y, const float z)
{
    const float c00 = d[0]*(1-x) + d[4]*x;
    const float c01 = d[1]*(1-x) + d[5]*x;
    const float c10 = d[2]*(1-x) + d[6]*x;
    const float c11 = d[3]*(1-x) + d[7]*x;

    const float  c0 =  c00*(1-y) + c10*y;
    const float  c1 =  c01*(1-y) + c11*y;

    const float   c =   c0*(1-z) + c1*z;

    return c;
}


// Finds the zero crossing on the line between two vertices of a unit cube.
// d is sample values at the cube's corners.
_STATIC_
Vec3f zero_crossing(const float d[8],
                    const uint8_t v0, const uint8_t v1)
{
    const float x0 =  (v0 & 4) ? 1 : 0,
                y0 =  (v0 & 2) ? 1 : 0,
                z0 =  (v0 & 1) ? 1 : 0;

    const float dx = ((v1 & 4) ? 1 : 0) - x0,
                dy = ((v1 & 2) ? 1 : 0) - y0,
                dz = ((v1 & 1) ? 1 : 0) - z0;

    float p = 0.5;
    float step = 0.25;

    // Binary search along the edge to find the zero crossing
    for (int iteration=0; iteration < 16; ++iteration)
    {
        const float r = interpolate(d, x0 + p*dx, y0 + p*dy, z0 + p*dz);

        if      (r < -EPSILON)  p += step;
        else if (r > EPSILON)   p += step;
        else                    break;

        step /= 2;
    }

    return (Vec3f){x0 + p*dx, y0 + p*dy, z0 + p*dz};
}


_STATIC_
unsigned triangulate_voxel(MathTree* tree, const Region r, float** mesh)
{
    return 0;
}

float* triangulate(MathTree* tree, const Region r, unsigned* const out)
{

    *out = 12;
    return NULL;
}
