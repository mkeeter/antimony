#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "asdf/asdf.h"
#include "asdf/triangulate.h"
#include "asdf/neighbors.h"

#include "formats/mesh.h"

#include "util/region.h"

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

////////////////////////////////////////////////////////////////////////////////


/*  mesh_zero_crossing
 *
 *  Finds a zero crossing for a given leaf.  May store new vertices in the
 *  vdata buffer (if we haven't already solved for this crossing).
 *
 *  Returns an index into the vdata buffer.
 */
_STATIC_
uint32_t mesh_zero_crossing(ASDF* const leaf,
                            const ASDF* const neighbors[6],
                            const uint8_t v0, const uint8_t v1,
                            Mesh* const mesh)
{

    if (!leaf->data.tri) {
        leaf->data.tri = calloc(64, sizeof(uint32_t));
    }

    // If we've already solved for this zero crossing then return
    // the appropriate vertex id
    if (leaf->data.tri[(v0<<3)|v1]) {
        return leaf->data.tri[(v0<<3)|v1] - 1;
    }

    // First, look up this edge in our neighbors to see if we
    // find a match.  If we do, then save the vertex ID and set
    // found to 'true'
    _Bool found = false;
    uint32_t id;

    for (uint8_t axis=0; axis < 3 && !found; ++axis) {

        // If the edge doesn't vary along this axis, we can compare
        // with the appropriate neighbor.
        uint8_t mask = 1 << axis;
        if (!(mask & ~(v0 ^ v1))) continue;

        // Find the appropriate neighbor
        const ASDF* neighbor = neighbors[axis*2 + ((v0 & mask) ? 1 : 0)];
        // If we don't have this neighbor or this neighbor doesn't have
        // a vertex cache, then keep going.
        if (!neighbor || !neighbor->data.tri)  continue;

        // Look up this vertex in the neighbor's edge cache
        uint16_t index = ((v0 ^ mask)<<3) | (v1 ^ mask);
        // If we don't find it, keep going
        if (!neighbor->data.tri[index]) continue;

        // We found the vertex!  Save it in the id variable.
        found = true;
        id = neighbor->data.tri[index] - 1;
        leaf->data.tri[(v0 << 3)|v1] = id + 1;
        leaf->data.tri[(v1 << 3)|v0] = id + 1;
    }

    // If we didn't find a match among neighbors, solve for the
    // zero crossing via interpolation.
    if (!found) {

        Vec3f c = asdf_zero_crossing(leaf, v0, v1);

        id = mesh->vcount;
        leaf->data.tri[(v0<<3)|v1] = id+1;
        leaf->data.tri[(v1<<3)|v0] = id+1;

        // Allocate more space if we need it to store this vertex.
        mesh_reserve_v(mesh, mesh->vcount+1);

        mesh->vdata[mesh->vcount*6]   = c.x;
        mesh->vdata[mesh->vcount*6+1] = c.y;
        mesh->vdata[mesh->vcount*6+2] = c.z;
        mesh->vdata[mesh->vcount*6+3] = 0;
        mesh->vdata[mesh->vcount*6+4] = 0;
        mesh->vdata[mesh->vcount*6+5] = 0;
        mesh->vcount++;
    }

    // The gradient will be a sum from all of the cells
    // that share this vertex.
    Vec3f g = asdf_gradient(leaf, mesh->vdata[id*6], mesh->vdata[id*6+1], mesh->vdata[id*6+2]);
    mesh->vdata[id*6+3] += g.x;
    mesh->vdata[id*6+4] += g.y;
    mesh->vdata[id*6+5] += g.z;

    return id;
}
////////////////////////////////////////////////////////////////////////////////


/*  evaluate_voxel
 *
 *  Uses marching tetrahedrons to generate a set of triangles
 *  for a given leaf cell.  Triangle vertices in idata as indexes into
 *  the vdata list, and icount is updated appropriately.
 */
_STATIC_
void evaluate_voxel(ASDF* const leaf,
                    const ASDF* const neighbors[6],
                    Mesh* const mesh)
{

    // Loop over the six tetrahedra that make up a voxel cell
    for (int t = 0; t < 6; ++t) {

        // Find vertex positions for this tetrahedron
        uint8_t vertices[] = {0, 7, VERTEX_LOOP[t], VERTEX_LOOP[t+1]};

        // Figure out which of the sixteen possible combinations
        // we're currently experiencing.
        uint8_t lookup = 0;
        for (int vertex = 3; vertex >= 0; --vertex) {
            lookup = (lookup << 1) + (leaf->d[vertices[vertex]] < 0);
        }

        if (EDGE_MAP[lookup][0][0][0] == -1)    continue;


        // Store the first triangle in the vertex array
        mesh_reserve_t(mesh, mesh->tcount + 1);
        for (int v=0; v < 3; ++v) {
            mesh->tdata[mesh->tcount*3 + v] = mesh_zero_crossing(
                leaf, neighbors,
                vertices[EDGE_MAP[lookup][0][v][0]],
                vertices[EDGE_MAP[lookup][0][v][1]],
                mesh
            );
        }
        mesh->tcount++;

        // Move on to the second triangle, aborting if there isn't one.
        if (EDGE_MAP[lookup][1][0][0] == -1)    continue;

        // Store the second triangle in the vertex array
        mesh_reserve_t(mesh, mesh->tcount + 1);
        for (int v=0; v < 3; ++v) {
            mesh->tdata[mesh->tcount*3 + v] = mesh_zero_crossing(
                leaf, neighbors,
                vertices[EDGE_MAP[lookup][1][v][0]],
                vertices[EDGE_MAP[lookup][1][v][1]],
                mesh
            );
        }
        mesh->tcount++;
    }
}

////////////////////////////////////////////////////////////////////////////////

_STATIC_
void _triangulate(ASDF* const asdf, const ASDF* const neighbors[6],
                  Mesh* const mesh, volatile int* const halt)
{
    if (*halt || !asdf) return;

    if (asdf->state == LEAF) {
        evaluate_voxel(asdf, neighbors, mesh);
    } else if (asdf->state == BRANCH) {

        const ASDF* new_neighbors[6];

        for (int i=0; i < 8; ++i) {
            get_neighbors_3d(asdf, neighbors, new_neighbors, i);
            _triangulate(asdf->branches[i], new_neighbors, mesh, halt);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

Mesh* triangulate(ASDF* const asdf, volatile int* const halt)
{
    const ASDF* neighbors[6] = {NULL, NULL, NULL, NULL, NULL, NULL};
    Mesh* mesh = calloc(1, sizeof(Mesh));
    _triangulate(asdf, neighbors, mesh, halt);
    free_data(asdf);

    // Save mesh bounding box
    mesh->X = asdf->X;
    mesh->Y = asdf->Y;
    mesh->Z = asdf->Z;

    return mesh;
}


