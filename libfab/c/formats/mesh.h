#ifndef FORMATS_MESH_H
#define FORMATS_MESH_H

#include <stdint.h>

#include "util/interval.h"

typedef struct Mesh_ {
    float* vdata;
    uint32_t vcount;
    uint32_t valloc;

    uint32_t* tdata;
    uint32_t  tcount;
    uint32_t  talloc;

    Interval X, Y, Z;
} Mesh;

/** @brief Frees a mesh object */
void free_mesh(Mesh* mesh);

/** @brief Ensures that the mesh can store the requested number of triangles
    @details Expands the tdata buffer if needed.
*/
void mesh_reserve_t(Mesh* mesh, const uint32_t tcount);

/** @brief Ensures that the mesh can store the requested number of vertices
    @details Expands the vdata buffer if needed.
*/
void mesh_reserve_v(Mesh* mesh, const uint32_t vcount);

/** @brief Increases a set of indices in a list
    @details Used when combining mesh vertex and index lists
    @param idata List of indices
    @param icount Number of indices in idata
    @param di Index increment
*/
void increase_indices(Mesh*, uint32_t di);

/** @brief Writes a mesh to a binary file full of indexed geometry
*/
void save_mesh(const char* filename, const Mesh* const mesh);

/** @brief Reads in a mesh structure from a file.
*/
Mesh* load_mesh(const char* filename);


/** @brief Merges a set of meshes into a single model
    @details Does not do anything fancy like vertex deduplication.
    @param count Number of meshes to merge
    @param meshes Array of pointers to meshes
    @returns A single merged Mesh object
*/
Mesh* merge_meshes(const uint32_t count, const Mesh* const* const meshes);

#endif
