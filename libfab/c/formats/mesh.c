#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "formats/mesh.h"

void free_mesh(Mesh* mesh)
{
    free(mesh->vdata);
    free(mesh->tdata);
    free(mesh);
}

////////////////////////////////////////////////////////////////////////////////

void mesh_reserve_t(Mesh* const mesh, const uint32_t tcount)
{
    while (mesh->talloc < tcount) {
        if (mesh->talloc)   mesh->talloc *= 2;
        else                mesh->talloc  = 2;
        mesh->tdata = realloc(
            mesh->tdata, sizeof(*mesh->tdata)*3*mesh->talloc
        );
    }
}

void mesh_reserve_v(Mesh* const mesh, const uint32_t vcount)
{
    while (mesh->valloc < vcount) {
        if (mesh->valloc)   mesh->valloc *= 2;
        else                mesh->valloc  = 2;
        mesh->vdata = realloc(
            mesh->vdata, sizeof(*mesh->tdata)*6*mesh->valloc
        );
    }
}


void increase_indices(Mesh* const mesh, uint32_t di)
{
    for (uint32_t t = 0; t < mesh->tcount; ++t) {
        mesh->tdata[t*3]   += di;
        mesh->tdata[t*3+1] += di;
        mesh->tdata[t*3+2] += di;
    }
}

////////////////////////////////////////////////////////////////////////////////

void save_mesh(const char* filename, const Mesh* const mesh)
{
    FILE* f = fopen(filename, "wb");

    float header_bounds[] = {mesh->X.lower, mesh->X.upper,
                             mesh->Y.lower, mesh->Y.upper,
                             mesh->Z.lower, mesh->Z.upper};

    // Write out the mesh's bounds
    for (int h=0; h < sizeof(header_bounds); ++h) {
        fputc(((char*)&header_bounds)[h], f);
    }

    // Write out the triangle count
    for (int t=0; t < sizeof(mesh->tcount); ++t) {
        fputc(((char*)&mesh->tcount)[t], f);
    }

    // Write out the vertex count
    for (int v=0; v < sizeof(mesh->vcount); ++v) {
        fputc(((char*)&mesh->vcount)[v], f);
    }

    // Write the buffers to file
    fwrite((void*)mesh->tdata, sizeof(mesh->tdata[0])*3, mesh->tcount, f);
    fwrite((void*)mesh->vdata, sizeof(mesh->vdata[0])*6, mesh->vcount, f);

    fclose(f);
}

////////////////////////////////////////////////////////////////////////////////

Mesh* load_mesh(const char* filename)
{
    FILE* f = fopen(filename, "rb");

    Mesh* const mesh = calloc(1, sizeof(Mesh));

    // Read in the header
    fscanf(f, "%4c", (char*)(&(mesh->X.lower)));
    fscanf(f, "%4c", (char*)(&(mesh->X.upper)));
    fscanf(f, "%4c", (char*)(&(mesh->Y.lower)));
    fscanf(f, "%4c", (char*)(&(mesh->Y.upper)));
    fscanf(f, "%4c", (char*)(&(mesh->Z.lower)));
    fscanf(f, "%4c", (char*)(&(mesh->Z.upper)));

    fscanf(f, "%4c", (char*)&(mesh->tcount));
    fscanf(f, "%4c", (char*)&(mesh->vcount));

    // Allocate space for the mesh data
    mesh_reserve_v(mesh, mesh->vcount);
    mesh_reserve_t(mesh, mesh->tcount);

    // Load all the data
    fread((void*)mesh->tdata, sizeof(*mesh->tdata)*3, mesh->tcount, f);
    fread((void*)mesh->vdata, sizeof(*mesh->vdata)*6, mesh->vcount, f);

    fclose(f);

    return mesh;
}

////////////////////////////////////////////////////////////////////////////////

Mesh* merge_meshes(const uint32_t count, const Mesh* const* const meshes)
{
    Mesh* const out = calloc(1, sizeof(Mesh));

    out->X = (Interval){INFINITY, -INFINITY};
    out->Y = (Interval){INFINITY, -INFINITY};
    out->Z = (Interval){INFINITY, -INFINITY};

    for (int m=0; m < count; ++m) {

        mesh_reserve_v(out, out->vcount + meshes[m]->vcount);
        memcpy(out->vdata + out->vcount*6,
                meshes[m]->vdata, meshes[m]->vcount*6*sizeof(*out->vdata)
        );

        mesh_reserve_t(out, out->tcount + meshes[m]->tcount);
        memcpy(out->tdata + out->tcount*3,
                meshes[m]->tdata, meshes[m]->tcount*3*sizeof(*out->tdata)
        );

        // Adjust indices of offset vertices within the larger list
        for (int i=0; i < meshes[m]->tcount; ++i) {
            out->tdata[(out->tcount+i)*3]     += out->vcount;
            out->tdata[(out->tcount+i)*3 + 1] += out->vcount;
            out->tdata[(out->tcount+i)*3 + 2] += out->vcount;
        }

        out->vcount += meshes[m]->vcount;
        out->tcount += meshes[m]->tcount;


        out->X.lower = fmin(out->X.lower, meshes[m]->X.lower);
        out->X.upper = fmax(out->X.upper, meshes[m]->X.upper);
        out->Y.lower = fmin(out->Y.lower, meshes[m]->Y.lower);
        out->Y.upper = fmax(out->Y.upper, meshes[m]->Y.upper);
        out->Z.lower = fmin(out->Z.lower, meshes[m]->Z.lower);
        out->Z.upper = fmax(out->Z.upper, meshes[m]->Z.upper);
    }
    return out;
}
