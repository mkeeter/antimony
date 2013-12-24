/*
CUBICAL MARCHING SQUARES!
http://graphics.csie.ntu.edu.tw/CMS/
*/
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include <assert.h>

#include "asdf/asdf.h"
#include "asdf/neighbors.h"
#include "asdf/cms.h"

#include "formats/mesh.h"

#include "util/squares.h"
#include "util/vec3f.h"

/*

Faces are always in the order -z, +z, -y, +y, -x, +x
and cubes are unrolled in the following order, looking
directly towards the -y face (labelled '2').

        _____
        | 1 |
    -----------------
    | 4 | 2 | 5 | 3 |
    -----------------
        | 0 |
        -----

        ^ z
        |
        ---> x

Edges are numbered as follows:

        1
    ---------       ^ y
    |       |       |
  2 |       | 3     ---> x
    |       |
    ---------
        0


*/

/* UNROLLED_VERTS[i] gives the four vertices (as 3-bit corner indices)
that define face i on an ASDF cell
*/
static const uint8_t UNROLLED_VERTS[6][4] = {
    {2, 0, 6, 4}, // face 0
    {1, 3, 5, 7}, // face 1
    {0, 1, 4, 5}, // face 2
    {6, 7, 2, 3}, // face 3
    {2, 3, 0, 1}, // face 4
    {4, 5, 6, 7}, // face 5
};

/* NEXT_POSITION[face][edge] gives the connected [face, edge]
*/
static const int8_t NEXT_POSITION[6][4][2] = {
    {{3, 0}, {2, 0}, {4, 0}, {5, 0}}, // face 0
    {{2, 1}, {3, 1}, {4, 1}, {5, 1}}, // face 1
    {{0, 1}, {1, 0}, {4, 3}, {5, 2}}, // face 2
    {{0, 0}, {1, 1}, {5, 3}, {4, 2}}, // face 3
    {{0, 2}, {1, 2}, {3, 3}, {2, 2}}, // face 4
    {{0, 3}, {1, 3}, {2, 3}, {3, 2}}, // face 5
};

/*
   ---------
   | 1 | 3 |
   ---------
   | 0 | 2 |
   ---------

  If you want to divide face f of an ASDF into four subfaces,
  SUBCELL_FACE[f][b] gives the subface that branch b maps into
  (or -1 if it doesn't map to a subface)
*/
static const int8_t SUBCELL_FACE[6][8] = {
    { 1, -1,  0, -1,  3, -1,  2, -1},
    {-1,  0, -1,  1, -1,  2, -1,  3},
    { 0,  1, -1, -1,  2,  3, -1, -1},
    {-1, -1,  2,  3, -1, -1,  0,  1},
    { 2,  3,  0,  1, -1, -1, -1, -1},
    {-1, -1, -1, -1,  0,  1,  2,  3}
};

/*
For a given face, FACE_AXES[6][{0,1}] describes the direction pointed
to by that face's {y,x} axes respectively.
This direction is a 4-bit field, with the top bit as 1 for positive
pointing and 0 for negative pointing.  The bottom 3 bits represent
x, y, and z axes respectively (so 8|4 is pointing in the +x direction)
*/
static const uint8_t FACE_AXES[6][2] = {
    {0|2, 8|4},
    {8|2, 8|4},
    {8|1, 8|4},
    {8|1, 0|4},
    {8|1, 0|2},
    {8|1, 8|2}
};

////////////////////////////////////////////////////////////////////////////////

typedef struct CMSvert_ {
    Vec3f pos;
    Vec3f normal;
    struct CMSvert_*** ptrs; // array of places that point to this CMSvert
    int ptr_count; // Number of ptrs
    uint32_t index;
} CMSvert;

typedef struct CMSpath_ {
    int edge;
    struct CMSpath_* next;
    struct CMSpath_* prev;
    CMSvert* vertex;
} CMSpath;

// Path4p is a pointer to an array of four CMSpaths
typedef CMSpath* (*Path4p)[4];

////////////////////////////////////////////////////////////////////////////////

/* Modifies all pointers to v1 so that they point to v0, then frees v1.
*/
_STATIC_
void merge_vertices(CMSvert* v0, CMSvert* v1)
{
    if (v0 == v1){
        return;
    }

    // Allocate more space so that pointers to v1 can be
    // transfered over to v0
    v0->ptrs = realloc(
        v0->ptrs,
        (v0->ptr_count + v1->ptr_count)*sizeof(CMSvert*)
    );

    // Modify the pointers and move them over them to v0
    for (int v=0; v < v1->ptr_count; ++v) {
        *(v1->ptrs[v]) = v0;
        v0->ptrs[v0->ptr_count++] = v1->ptrs[v];
    }

    free(v1->ptrs);
    free(v1);
}

////////////////////////////////////////////////////////////////////////////////

/*  Attaches vertex v to the provided path point, adding
 *  a backlink in v->ptrs to path->vertex so that the vertex
 *  can disconnect itself as needed.
 */
_STATIC_
void bind_vertex(CMSpath* path, CMSvert* v)
{
    path->vertex = v;
    v->ptrs = realloc(v->ptrs, sizeof(CMSvert**)*(v->ptr_count+1));
    v->ptrs[v->ptr_count++] = &(path->vertex);
}


/*  Disconnects vertex from the provided path point.
 *  Removes the backlink in v->ptrs to path->vertex,
 *  and frees the vertex if there are no references.
 */
_STATIC_
void unbind_vertex(CMSpath* path)
{
    CMSvert* const v = path->vertex;
    path->vertex = NULL;

    // Swap the saved pointer to the end of the list, then
    // delete it by reallocating less space.
    for (int p=0; p < v->ptr_count; ++p) {
        if (v->ptrs[p] == &(path->vertex)) {
            v->ptrs[p] = v->ptrs[v->ptr_count-1];
            break;
        }
    }
    v->ptrs = realloc(v->ptrs, (--v->ptr_count)*sizeof(CMSvert**));

    if (v->ptr_count == 0) {
        free(v->ptrs);
        free(v);
    }
}

////////////////////////////////////////////////////////////////////////////////

/*
 * Links end to start.  End and start should be duplicate points
 * (on different edges); end will be removed from the combined
 * path and freed.
 */
_STATIC_
void link_paths(CMSpath* const end, CMSpath* const start)
{
    if (!end || !start) return;
    merge_vertices(start->vertex, end->vertex);

    CMSpath* const prev = end->prev;
    unbind_vertex(end);
    free(end);

    prev->next = start;
    start->prev = prev;
    start->edge = -1;
}


/* Reverses a CMSpath object without modifying edge or vertex
 * values.  Returns a pointer to the start of the reversed path
 * (which is previously the end of the input path).
 */
_STATIC_
CMSpath* reverse_path(CMSpath* p)
{
    while (p->next) p = p->next;
    CMSpath* const start = p;

    while (p) {
        CMSpath* const next = p->prev;
        p->prev = p->next;
        p->next = next;
        p = p->next;
    }
    return start;
}


/* Frees a CMSpath object, unbinding its vertices.
 */
_STATIC_
void free_cmspath(CMSpath* path) {
    if (path == NULL)   return;
    unbind_vertex(path);

    free_cmspath(path->next);
    free(path);
}

////////////////////////////////////////////////////////////////////////////////

/*  Solve for a zero crossing point along a particular edge,
 *  using interpolation on corner values.
 */
_STATIC_
CMSpath* zero_crossing(
    const int8_t edge, const Vec3f corners[4], const float d[4])
{
    const uint8_t v0 = VERTEX_MAP[edge][0];
    const uint8_t v1 = VERTEX_MAP[edge][1];

    // Interpolation from d0 to d1
    const float d0 = d[v0];
    const float d1 = d[v1];
    const float interp = (d0)/(d0-d1);

    CMSpath* const p = calloc(1, sizeof(CMSpath));

    // Find interpolated coordinates and store them in a vertex
    CMSvert* v = malloc(sizeof(CMSvert));
    *v = (CMSvert){
        .pos = (Vec3f){
            corners[v0].x*(1-interp) + corners[v1].x*interp,
            corners[v0].y*(1-interp) + corners[v1].y*interp,
            corners[v0].z*(1-interp) + corners[v1].z*interp
        },
        .normal = (Vec3f){0, 0, 0},
        .ptrs = NULL,
        .ptr_count = 0,
        .index = 0
    };
    bind_vertex(p, v);

    return p;
}

////////////////////////////////////////////////////////////////////////////////

/* Generates a single face using the marching squares algorithm
*/
_STATIC_
void gen_face(const Vec3f corners[4], const float d[4], CMSpath* paths[4])
{
    const uint8_t edges =
        (d[0] < 0 ? 1 : 0) |
        (d[1] < 0 ? 2 : 0) |
        (d[2] < 0 ? 4 : 0) |
        (d[3] < 0 ? 8 : 0);

    const int8_t e0a = EDGE_MAP[edges][0][0];
    const int8_t e0b = EDGE_MAP[edges][0][1];

    if (e0a != -1) {
        paths[e0a] = zero_crossing(e0a, corners, d);
        CMSpath* const end = zero_crossing(e0b, corners, d);

        paths[e0a]->next = end;
        end->prev = paths[e0a];

        paths[e0a]->edge = e0a;
        end->edge = e0b;
    }

    const int8_t e1a = EDGE_MAP[edges][1][0];
    const int8_t e1b = EDGE_MAP[edges][1][1];

    if (e1a != -1) {
        paths[e1a] = zero_crossing(e1a, corners, d);
        CMSpath* const end = zero_crossing(e1b, corners, d);

        paths[e1a]->next = end;
        end->prev = paths[e1a];

        paths[e1a]->edge = e1a;
        end->edge = e1b;
    }
}

////////////////////////////////////////////////////////////////////////////////

typedef struct ASDFstack_ {
    const ASDF* asdf;
    uint8_t b;
    struct ASDFstack_* next;
} ASDFstack;

_STATIC_
ASDFstack* pop(ASDFstack* s)
{
    ASDFstack* const tmp = s->next;
    free(s);
    return tmp;
}

////////////////////////////////////////////////////////////////////////////////

_STATIC_
ASDFstack* find_edge(const ASDF* const asdf,
                     const uint8_t face, const uint8_t edge)
{
    if (!asdf)  return NULL;

    // Base case: if we get a leaf, then check to see if it has
    // a path starting on the desired face and edge.  If so,
    // construct a stack with the leaf as the top node and
    // return.
    if (asdf->state == LEAF) {
        if (asdf->data.cms[face][edge]) {
            ASDFstack* stack = calloc(1, sizeof(ASDFstack));
            stack->asdf = asdf;
            return stack;
        } else {
            return NULL;
        }
    }

    // If we can't do a recursive test, then return.
    if (asdf->state != BRANCH && asdf->state != VIRTUAL) {
        return NULL;
    }

    const uint8_t edge_axis = 1 << (edge / 2);
    const uint8_t edge_dir  = (edge % 2) ? edge_axis : 0;
    const uint8_t mask =
        (asdf->branches[4] ? 4 : 0) |
        (asdf->branches[2] ? 2 : 0) |
        (asdf->branches[1] ? 1 : 0);

    // Figure out whether there's an subface edge that
    // we can use on one of the ASDF's branches
    for (int b=0; b < 8; ++b) {
        const int8_t scf = SUBCELL_FACE[face][b];

        // If this branch doesn't map to a valid subface
        // or it maps to a subface that doesn't have the
        // desired edge, skip it.
        if (scf == -1 || (scf & edge_axis) != edge_dir) {
            continue;
        }

        // Recurse down this branch of the ASDF.
        ASDFstack* stack = find_edge(
            asdf->branches[b&mask], face, edge
        );
        // If we find a hit, then append the ASDF to the
        // end of the stack and return the stack.
        if (stack != NULL) {
            ASDFstack* end = stack;
            while (end->next) end = end->next;
            end->next = calloc(1, sizeof(ASDFstack));
            end->next->b = b & mask;
            end->next->asdf = asdf;
            return stack;
        }
    }

    return NULL;
}


_STATIC_
CMSpath* clone_path(CMSpath* src)
{
    CMSpath*  dst = NULL;

    CMSpath*  prev = NULL;
    CMSpath** curr = &dst;

    while (src) {
        *curr = malloc(sizeof(CMSpath));
        **curr = (CMSpath){
            .edge=src->edge,
            .next=NULL, .prev=prev,
            .vertex=NULL
        };
        bind_vertex(*curr, src->vertex);

        prev = *curr;
        curr = &((**curr).next);
        src = src->next;
    }
    return dst;
}


/*  Copies the paths from one face to another.
 *  Paths are unique to each cube, but pointers to vertex
 *  objects are shared.
 *
 *  Face is the face on this ASDF.
*/
_STATIC_
CMSpath* clone_merged_path(const ASDF* const asdf,
                    const uint8_t face, const uint8_t edge)
{
    // Find the starting edge for this path.
    ASDFstack* stack = find_edge(asdf, face, edge);

    CMSpath* path = NULL;
    CMSpath* end = NULL;

    // Current edge (used when grabbing an edge from a face)
    uint8_t ce = edge;

    while (stack) {

        CMSpath* segment = clone_path(
            stack->asdf->data.cms[face][ce]
        );

        if (!path) {
            path = segment;
            end = path;
        }
        else {
            link_paths(end, segment);
            end = segment;
        }
        while (end->next)   end = end->next;
        ce = end->edge ^ 1;

        const uint8_t fa = FACE_AXES[face][end->edge/2];
        const uint8_t edge_axis = fa & 7;
        const uint8_t edge_dir =
            (((fa & 8) != 0) == (end->edge & 1)) ? edge_axis : 0;

        // Back up through the stack until we can continue
        // tracing this path on an adjoining cell.
        // If we back all of the way up out of the stack, then
        // we're done with the path tracing.
        stack = pop(stack);
        while (stack) {
            // If we can move into an adjacent cell, then do so.
            if ((stack->b & edge_axis) != edge_dir &&
                stack->asdf->branches[stack->b^edge_axis])
            {
                // Recurse down the asdf to find the lowest-level cell
                ASDFstack* const new_stack = find_edge(
                    stack->asdf->branches[stack->b^edge_axis],
                    face, end->edge^1
                );

                {   // Attach the old stack to the end of the new one
                    ASDFstack* tmp = new_stack;
                    while (tmp->next)   tmp = tmp->next;
                    tmp->next = stack;
                }

                // Mark that we recursed down a different branch this time.
                stack->b ^= edge_axis;

                // And over-write the current stack.
                stack = new_stack;
                break;
            }
            else {
                stack = pop(stack);
            }
        }

    }
    return path;
}

////////////////////////////////////////////////////////////////////////////////

/*  Populates the data array of a single ASDF cell.
*/
_STATIC_
void gen_cube(ASDF* const asdf)
{
    asdf->data.cms = calloc(6, sizeof(CMSpath*)*4);

    // Iterate over each face in this model, finding contours.
    for (int f=0; f < 6; ++f) {

        CMSpath** my_face = asdf->data.cms[f];

        // Get corner positions and distance samples
        float d[4];
        Vec3f corners[4];
        for (int v=0; v < 4; ++v) {
            const uint8_t c = UNROLLED_VERTS[f][v];
            d[v] = asdf->d[c];
            corners[v] = (Vec3f){
                (c & 4) ? asdf->X.upper : asdf->X.lower,
                (c & 2) ? asdf->Y.upper : asdf->Y.lower,
                (c & 1) ? asdf->Z.upper : asdf->Z.lower
            };
        }

        // Generate the contours of this face
        gen_face(corners, d, my_face);
    }
}


////////////////////////////////////////////////////////////////////////////////

_STATIC_
void populate_faces(ASDF* const asdf)
{
    if (asdf == NULL) {
        return;
    } else if (asdf->state == LEAF) {
        gen_cube(asdf);
    } else if (asdf->state == BRANCH) {
        for (int i=0; i < 8; ++i) {
            populate_faces(asdf->branches[i]);
        }
    }

}

////////////////////////////////////////////////////////////////////////////////

_STATIC_
void link_loop(ASDF* const asdf, const uint8_t f, const uint8_t e)
{
    CMSpath* prev = NULL;
    uint8_t cf = f;
    uint8_t ce = e;

    // Only trace a valid loop.
    CMSpath* const loop_start = asdf->data.cms[cf][ce];
    if (loop_start == NULL) return;

    do {
        // Get the path on this particular face
        CMSpath* p = asdf->data.cms[cf][ce];

        // If we're continuing around the loop, then disconnect
        // this path and merge it with the last point in the previous
        // path (welding the vertex and modifying the pointers)
        if (prev != NULL) {
            // Disconnect from asdf data
            asdf->data.cms[cf][ce] = NULL;
            link_paths(prev, p);
        }

        // Walk to the end of this path
        while (p->next) p = p->next;

        // Figure out where we go from here (next face and edge)
        // to continue the loop.
        const uint8_t new_f = NEXT_POSITION[cf][p->edge][0];
        const uint8_t new_e = NEXT_POSITION[cf][p->edge][1];

        cf = new_f;
        ce = new_e;
        prev = p;
    } while (cf != f || ce != e);

    // Remove the last link of the path (since it's a closed
    // loop, the last link is a duplicate of the first)
    merge_vertices(loop_start->vertex, prev->vertex);
    unbind_vertex(prev);
    prev->prev->next = NULL;
    free(prev);
}

/* Walks all paths in the cube, making single-face paths into loops
 * that travel all of the way around the cube (with welded vertices)
*/
_STATIC_
void link_loops(ASDF* const asdf)
{
    if (asdf == NULL) {
        return;
    } else if (asdf->state == LEAF) {
        for (int f=0; f < 6; ++f) {
            for (int e=0; e < 4; ++e) {
                link_loop(asdf, f, e);
            }
        }
    } else if (asdf->state == BRANCH) {
        for (int i=0; i < 8; ++i) {
            link_loops(asdf->branches[i]);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

_STATIC_
uint32_t insert_vertex(Mesh* m, CMSvert* v)
{
    if (v->index)   return v->index-1;

    mesh_reserve_v(m, m->vcount + 1);

    m->vdata[m->vcount*6]   = v->pos.x;
    m->vdata[m->vcount*6+1] = v->pos.y;
    m->vdata[m->vcount*6+2] = v->pos.z;
    m->vdata[m->vcount*6+3] = v->normal.x;
    m->vdata[m->vcount*6+4] = v->normal.y;
    m->vdata[m->vcount*6+5] = v->normal.z;

    v->index = ++m->vcount;
    return v->index-1;
}


_STATIC_
void insert_triangle(Mesh* m, CMSvert* v1, CMSvert* v2, CMSvert* v3)
{
    mesh_reserve_t(m, m->tcount + 1);

    m->tdata[m->tcount*3]   = insert_vertex(m, v1);
    m->tdata[m->tcount*3+1] = insert_vertex(m, v2);
    m->tdata[m->tcount*3+2] = insert_vertex(m, v3);

    m->tcount++;
}


_STATIC_
void triangulate_loop(ASDF* const asdf,
        const uint8_t f, const uint8_t e, Mesh* const mesh)
{
    // Extract the loop
    CMSpath* const path_start = asdf->data.cms[f][e];
    asdf->data.cms[f][e] = NULL;

    // Trace the loop, keeping track of the average vertex position
    CMSpath* p = path_start;
    int count = 0;
    CMSvert* center = calloc(1, sizeof(CMSvert));

    while (p) {
        CMSvert* v = p->vertex;
        center->pos.x += v->pos.x;
        center->pos.y += v->pos.y;
        center->pos.z += v->pos.z;
        center->normal.x += v->normal.x;
        center->normal.y += v->normal.y;
        center->normal.z += v->normal.z;
        count++;
        p = p->next;
    }
    center->pos.x /= count;
    center->pos.y /= count;
    center->pos.z /= count;
    center->normal.x /= count;
    center->normal.y /= count;
    center->normal.z /= count;
    if (count == 3) {
        insert_triangle(mesh, path_start->next->next->vertex,
                path_start->next->vertex, path_start->vertex);
    } else if (count > 3) {
        // Make a triangle fan about the center vertex.
        p = path_start;
        while (p->next) {
            insert_triangle(mesh, p->next->vertex, p->vertex, center);
            p = p->next;
        }
        insert_triangle(mesh, path_start->vertex, p->vertex, center);
    }
    // Free the temporary vertex.
    free(center);

    // Disconnect this loop.
    free_cmspath(path_start);
}


_STATIC_
void triangulate_loops(ASDF* const asdf, Mesh* const mesh)
{
    if (asdf == NULL) {
        return;
    } else if (asdf->state == LEAF) {
        for (int f=0; f < 6; ++f) {
            for (int e=0; e < 4; ++e) {
                triangulate_loop(asdf, f, e, mesh);
            }
        }
    } else if (asdf->state == BRANCH) {
        for (int i=0; i < 8; ++i) {
            triangulate_loops(asdf->branches[i], mesh);
        }
    }
}


_STATIC_
void merge_faces(ASDF* const asdf, const ASDF* const neighbors[6])
{
    if (asdf == NULL) {
        return;
    } else if (asdf->state == LEAF) {
        // For every face and edge, attempt to clone it from
        // our neighbours.  Since we're recursing down the tree,
        // we'll automatically get multi-scale paths in clone_merged_path
        for (int f=0; f < 6; ++f) {
            for (int e=0; e < 4 && neighbors[f]; ++e) {
                CMSpath* p = clone_merged_path(neighbors[f], f^1, e);
                if (p == NULL)  continue;

                // Swap the edge (since the neighboring path is backwards)
                if ((p->edge <= 1) == (f <= 1))     p->edge ^= 1;

                // Save the ending edge for an assert check later
                const uint8_t end = p->edge;

                // Now reverse the path and swap the front edge.
                p = reverse_path(p);
                if ((p->edge <= 1) == (f <= 1))     p->edge ^= 1;

                // Assert that this is a simple (one-segment) path.
                assert(asdf->data.cms[f][p->edge]->next != NULL &&
                       asdf->data.cms[f][p->edge]->next->next == NULL);

                // A bit tautological, but worth a check
                assert(asdf->data.cms[f][p->edge]->edge == p->edge);

                // This is the important check: we need to make sure that we
                // ended up on the same edge as before.
                assert(asdf->data.cms[f][p->edge]->next->edge == end);

                free_cmspath(asdf->data.cms[f][p->edge]);
                asdf->data.cms[f][p->edge] = p;
            }
        }
    } else if (asdf->state == BRANCH) {
        const ASDF* new_neighbors[6];
        for (int b=0; b < 8; ++b) {
            get_neighbors_v(asdf, neighbors, new_neighbors, b);
            merge_faces(asdf->branches[b], new_neighbors);

            // Free any virtual neighbors.
            for (int n=0; n < 6; ++n) {
                free_virtual_asdf((ASDF*)new_neighbors[n]);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

_STATIC_
_Bool make_consistent(ASDF* const asdf, const ASDF* const neighbors[6])
{
    if (!asdf || asdf->state != BRANCH) {
        return true;
    }

    _Bool consistent = true;

    for (int b=0; b < 8; ++b) {
        const ASDF* new_neighbors[6];

        // Check each face, splitting this ASDF if there is a
        // topological inconsistancy.  For example, if these two
        // faces are touching each other
        //
        //   ---------      ---------
        //   |   |   |      |       |
        //   |   |   |      ---------
        //   |   |   |      |       |
        //   ---------      ---------
        //
        //  then we'll split one of them to ensure that grabbing edges
        //  from adjacent faces is always topologically possible.
        //
        for (int f=0; f < 6; ++f) {
            const ASDF* n = get_neighbor_v(asdf, b, f, neighbors[f]);

            if (n && n->state == VIRTUAL && !n->branches[0]) {
                consistent = false;
                const uint8_t new_split  = n->d[1];
                for (int c=0; c < 8; ++c) {
                    if (c & new_split)  continue;
                    asdf->branches[c|new_split] =
                        split_cell(
                            asdf->branches[c], neighbors[f], new_split
                        );
                }
                free_virtual_asdf((ASDF*)n);
                n = get_neighbor_v(asdf, b, f, neighbors[f]);
            }
            new_neighbors[f] = n;
        }

        // Recurse on the ASDF's branches
        if (!make_consistent(asdf->branches[b], new_neighbors)) {
            consistent = false;
        }

        for (int n=0; n < 6; ++n) {
            free_virtual_asdf((ASDF*)new_neighbors[n]);
        }
    }

    return consistent;
}

////////////////////////////////////////////////////////////////////////////////

_STATIC_
void record_normals(ASDF* const asdf)
{
    if (!asdf)  return;
    if (asdf->state == LEAF) {
        for (int f=0; f < 6; ++f) {
            for (int e=0; e < 4; ++e) {
                CMSpath* p = asdf->data.cms[f][e];
                if (!p) continue;
                while (p) {
                    Vec3f g = asdf_gradient(
                        asdf, p->vertex->pos.x,
                        p->vertex->pos.y, p->vertex->pos.z
                    );
                    p->vertex->normal.x += g.x;
                    p->vertex->normal.y += g.y;
                    p->vertex->normal.z += g.z;
                    p = p->next;
                }
            }
        }
    } else if (asdf->state == BRANCH) {
        for (int b=0; b < 8; ++b) {
            record_normals(asdf->branches[b]);
        }
    }
}


Mesh* triangulate_cms(ASDF* const asdf)
{
    // Modify the ASDF to resolve topological inconsistancies
    const ASDF* const neighbors[6] = {NULL, NULL, NULL, NULL, NULL, NULL};
    _Bool consistent = false;
    while (!consistent) {
        consistent = make_consistent(asdf, neighbors);
    }

    // Fill each ASDF leaf cell face with paths
    populate_faces(asdf);

    // Copy paths from neighboring cells to join vertices
    // and resolve multi-scale adjancencies
    merge_faces(asdf, neighbors);

    // Link face paths into closed loops
    link_loops(asdf);

    // Save vertex normals (found from ASDF gradient)
    record_normals(asdf);

    // Triangulate the loops, disconnecting and freeing them
    // as we travel through the tree.
    Mesh* mesh = calloc(1, sizeof(Mesh));
    triangulate_loops(asdf, mesh);

    free_data(asdf);

    mesh->X = asdf->X;
    mesh->Y = asdf->Y;
    mesh->Z = asdf->Z;

    return mesh;
}
