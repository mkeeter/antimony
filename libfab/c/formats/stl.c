#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <formats/stl.h>
#include <formats/mesh.h>

Mesh* load_stl(const char* filename)
{
    FILE* input = fopen(filename, "rb");

    Mesh* const mesh = calloc(1, sizeof(Mesh));
    mesh->X = (Interval){INFINITY, -INFINITY};
    mesh->Y = (Interval){INFINITY, -INFINITY};
    mesh->Z = (Interval){INFINITY, -INFINITY};

    // Skip the STL file header
    fseek(input, 80, SEEK_SET);
    // Read in the triangle count
    fscanf(input, "%4c", (char*)&mesh->tcount);

    // Allocate space for the incoming triangles and vertices
    mesh_reserve_t(mesh, mesh->tcount);
    mesh_reserve_v(mesh, mesh->vcount*3);

    for (int t=0; t < mesh->tcount; ++t) {

        // Current position in the vertex buffer
        // (each triangle is 3 vertices with 6 floats each)
        const unsigned v = t * 18;

        // Ignore the normal vector
        for (int c=0; c < 12; ++c)   getc(input);

        // Read 3 sets of 3 floats (each 4 bytes)
        fscanf(input, "%12c", (char*)&(mesh->vdata[v]));
        fscanf(input, "%12c", (char*)&(mesh->vdata[v+6]));
        fscanf(input, "%12c", (char*)&(mesh->vdata[v+12]));

        // Ignore attribute byte count
        for (int c=0; c < 2; ++c)   getc(input);

        // Find triangle plane vectors
        const float a1 = mesh->vdata[v+6] - mesh->vdata[v],
                    b1 = mesh->vdata[v+12] - mesh->vdata[v],
                    a2 = mesh->vdata[v+7] - mesh->vdata[v+1],
                    b2 = mesh->vdata[v+13] - mesh->vdata[v+1],
                    a3 = mesh->vdata[v+8] - mesh->vdata[v+2],
                    b3 = mesh->vdata[v+14] - mesh->vdata[v+2];

        // Get normal with cross product
        const float nx = a2*b3 - a3*b2,
                    ny = a3*b1 - a1*b3,
                    nz = a1*b2 - a2*b1;

        // And save the normal in the vertex buffer
        for (int i=0; i < 3; ++i) {
            mesh->vdata[v+3+i*6] = nx;
            mesh->vdata[v+4+i*6] = ny;
            mesh->vdata[v+5+i*6] = nz;
        }


        mesh->tdata[t*3]     = t*3;
        mesh->tdata[t*3 + 1] = t*3 + 1;
        mesh->tdata[t*3 + 2] = t*3 + 2;
    }

    fclose(input);

    return mesh;
}

////////////////////////////////////////////////////////////////////////////////

void save_stl(Mesh* mesh, const char* filename)
{
    FILE* stl = fopen(filename, "wb");

    // 80-character header
    fprintf(stl, "This is a binary STL file made in kokopelli    \n(github.com/mkeeter/kokopelli)\n\n");

    for (int i=0; i<4; ++i) {
        fputc(((char*)&mesh->tcount)[i], stl);
    }

    for (int t=0; t < mesh->tcount; ++t) {

        // Write the face normal (which we'll keep empty)
        for (int j=0; j < 12; ++j) fputc(0, stl);

        // Write out all of the vertices.
        for (int v=0; v < 3; ++v) {
            float xyz[3] = {
                mesh->vdata[6*mesh->tdata[t*3+v]],
                mesh->vdata[6*mesh->tdata[t*3+v]+1],
                mesh->vdata[6*mesh->tdata[t*3+v]+2]
            };
            for (int j=0; j < 12; ++j) {
                fputc(((char*)&xyz)[j], stl);
            }
        }

        fputc(0, stl);
        fputc(0, stl);
    }

    fclose(stl);
}

////////////////////////////////////////////////////////////////////////////////

/*
void draw_triangle(Triangle tri, Region r, uint16_t*const*const img)
{
    int imin = ni*(fmin(fmin(tri.x0, tri.x1), tri.x2) - xmin) / (xmax - xmin);
    if (imin < 0)   imin = 0;

    int imax = ni*(fmax(fmax(tri.x0, tri.x1), tri.x2) - xmin) / (xmax - xmin);
    if (imax >= ni)   imax = ni-1;

    int jmin = nj*(fmin(fmin(tri.y0, tri.y1), tri.y2) - ymin) / (ymax - ymin);
    if (jmin < 0)   jmin = 0;

    int jmax = nj*(fmax(fmax(tri.y0, tri.y1), tri.y2) - ymin) / (ymax - ymin);
    if (jmax >= nj)   jmax = 0;
}
*/
