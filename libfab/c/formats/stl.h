#ifndef FORMATS_STL_H
#define FORMATS_STL_H

#include <stdint.h>

struct Mesh_;

/** @brief Loads an STL file.
*/
struct Mesh_* load_stl(const char* filename);

/** @brief Writes a mesh to an STL file */
void save_stl(struct Mesh_* mesh, const char* filename);

#endif
