#ifndef STL_H
#define STL_H

#ifdef __cplusplus
extern "C" {
#endif

/** Exports to a .stl file.
 *
 *  verts is an array of xyz verts packed into triangles.
 *  count is the number of floats in the array.
 */
void save_stl(float* verts, unsigned count, const char* filename);

#ifdef __cplusplus
}
#endif

#endif
