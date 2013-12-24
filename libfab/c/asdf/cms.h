#ifndef ASDF_CMS_H
#define ASDF_CMS_H

struct ASDF_;
struct Mesh_;

/** @brief Triangulates an ASDF using cubical marching squares. */
struct Mesh_* triangulate_cms(struct ASDF_* const asdf);

#endif
