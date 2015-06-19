#include <stdlib.h>
#include <stdio.h>

#include "fab/formats/stl.h"

void save_stl(float* verts, unsigned count, const char* filename)
{
    FILE* stl = fopen(filename, "wb");

    // 80-character header
    fprintf(stl, "This is a binary STL file made in kokopelli    \n(github.com/mkeeter/kokopelli)\n\n");

    int tris = count / 9;
    for (unsigned i=0; i < sizeof(float); ++i) {
        fputc(((char*)&tris)[i], stl);
    }

    for (int t=0; t < tris; ++t) {

        // Write the face normal (which we'll keep empty)
        for (unsigned j=0; j < 3*sizeof(float); ++j) fputc(0, stl);

        // Write out all of the vertices.
        for (unsigned v=0; v < 9 * sizeof(float); ++v) {
            fputc(((char*)&verts[t*9])[v], stl);
        }

        fputc(0, stl);
        fputc(0, stl);
    }

    fclose(stl);
}
