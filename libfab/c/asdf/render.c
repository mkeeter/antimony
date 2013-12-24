#include <stdio.h>
#include <math.h>

#include "asdf/asdf.h"
#include "asdf/render.h"

#include "util/switches.h"
#include "util/macros.h"
#include "util/vec3f.h"


////////////////////////////////////////////////////////////////////////////////
// Forward declarations of _STATIC_ functions

_STATIC_
void asdf_leaf_shaded(
    const ASDF* const leaf, const Region r,
    const float M[4], uint16_t*const*const depth,
    uint16_t*const*const shaded, uint8_t (**normals)[3]
);


/*  asdf_fill_shaded
 *
 *  Fills in all pixels in a region that (when coordinate-transformed)
 *  lie within the asdf's bounds.
 */
_STATIC_
void asdf_fill_shaded(
    const ASDF* const asdf, const Region r,
    const float M[4], uint16_t*const*const depth
);


/*  asdf_fill
 *
 *  Fills in all pixels in a region that (when coordinate-transformed)
 *  lie within the asdf's bounds.
 */
_STATIC_
void asdf_fill(
    const ASDF* const asdf, const Region r,
    const float M[4], uint16_t*const*const depth
);


/*  asdf_leaf
 *
 *  Fills in all pixels in a region that (when coordinate-transformed)
 *  lie within the asdf's bounds and have an interpolated d-value
 *  that is less than zero.
 */
_STATIC_
void asdf_leaf(
    const ASDF* const asdf, const Region r,
    const float M[4], uint16_t*const*const depth
);

// End of forward declarations
////////////////////////////////////////////////////////////////////////////////

void render_asdf(const ASDF* const asdf, const Region r_,
                 const float M[4], uint16_t*const*const depth)
{
    if (!asdf || asdf->state == EMPTY)  return;

    // Shrink the region based on the bounds of this ASDF
    // (taking the rotation matrix into account)
    const Region r = rot_bound_region(asdf, r_, M);

    if (r.voxels == 0)  return;

    else if (asdf->state == FILLED) {
        asdf_fill(asdf, r, M, depth);
    }

    else if (asdf->state == LEAF) {
        asdf_leaf(asdf, r, M, depth);
    }

    // From a branching cell, recurse down all branches
    else if (asdf->state == BRANCH) {
        for (int i=0; i < 8; ++i) {
            render_asdf(asdf->branches[i], r, M, depth);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

_STATIC_
void asdf_fill(
    const ASDF* const asdf, const Region r,
    const float M[4], uint16_t*const*const depth)
{

    const Vec3f dx = deproject((Vec3f){r.X[1] - r.X[0], 0, 0}, M),
                dy = deproject((Vec3f){0, r.Y[1] - r.Y[0], 0}, M),
                dz = deproject((Vec3f){0, 0, r.Z[0] - r.Z[1]}, M);

    const Vec3f start = deproject(
        (Vec3f){
            (r.X[0]+r.X[1])/2,
            (r.Y[0]+r.Y[1])/2,
            r.Z[r.nk]
        }, M
    );


    for (int j=0; j < r.nj; ++j) {
        for (int i=0; i < r.ni; ++i) {
            for (int k=r.nk; k >= 0; --k) {

                const uint16_t L = r.L[k];
                if (L < depth[r.jmin+j][r.imin+i])  break;

                const Vec3f pos = (Vec3f){
                    start.x + dx.x*i + dy.x*j + dz.x*k,
                    start.y + dx.y*i + dy.y*j + dz.y*k,
                    start.z + dx.z*i + dy.z*j + dz.z*k
                };

                // If there's no overlap between the voxel and
                // the pixel (in the voxel's coordinate frame),
                // then skip this pixel
                if (pos.x < asdf->X.lower || asdf->X.upper < pos.x ||
                    pos.y < asdf->Y.lower || asdf->Y.upper < pos.y ||
                    pos.z < asdf->Z.lower || asdf->Z.upper < pos.z)
                {
                    continue;
                }

                depth[r.jmin+j][r.imin+i] = L;
                break;
            } // End of z loop
        } // End of x loop
    } // End of y loop
}


_STATIC_
void asdf_leaf(
    const ASDF* const asdf, const Region r,
    const float M[4], uint16_t*const*const depth)
{

    const Vec3f dx = deproject((Vec3f){r.X[1] - r.X[0], 0, 0}, M),
                dy = deproject((Vec3f){0, r.Y[1] - r.Y[0], 0}, M),
                dz = deproject((Vec3f){0, 0, r.Z[0] - r.Z[1]}, M);

    const Vec3f start = deproject(
        (Vec3f){
            (r.X[0]+r.X[1])/2,
            (r.Y[0]+r.Y[1])/2,
            r.Z[r.nk]
        }, M
    );

    for (int j=0; j < r.nj; ++j) {
        for (int i=0; i < r.ni; ++i) {
            for (int k=r.nk; k >= 0; --k) {
                const uint16_t L = r.L[k];

                if (L < depth[r.jmin+j][r.imin+i])  break;

                const Vec3f pos = (Vec3f){
                    start.x + dx.x*i + dy.x*j + dz.x*k,
                    start.y + dx.y*i + dy.y*j + dz.y*k,
                    start.z + dx.z*i + dy.z*j + dz.z*k
                };

                // If there's no overlap between the voxel and
                // the pixel (in the voxel's coordinate frame),
                // then skip this pixel
                if (pos.x < asdf->X.lower || asdf->X.upper < pos.x ||
                    pos.y < asdf->Y.lower || asdf->Y.upper < pos.y ||
                    pos.z < asdf->Z.lower || asdf->Z.upper < pos.z)
                {
                    continue;
                }

                if (asdf_interpolate(asdf, pos.x, pos.y, pos.z) < 0)
                {
                    depth[r.jmin+j][r.imin+i] = L;
                    break;
                }
            } // End of z loop
        } // End of x loop
    } // End of y loop
}

////////////////////////////////////////////////////////////////////////////////

void render_asdf_shaded(const ASDF* const asdf, const Region r_,
                 const float M[4], uint16_t*const*const depth,
                 uint16_t*const*const shaded, uint8_t (**normals)[3])
{
    if (!asdf || asdf->state == EMPTY)  return;

    // Shrink the region based on the bounds of this ASDF
    // (taking the rotation matrix into account)
    const Region r = rot_bound_region(asdf, r_, M);

    if (r.voxels == 0)  return;

    // If this is a filled in cell, then fill it in in the depth field
    else if (asdf->state == FILLED) {
        asdf_fill_shaded(asdf, r, M, depth);
    }

    // For a leaf cell or pixel-wide cell, render as a leaf
    else if (asdf->state == LEAF) {
        asdf_leaf_shaded(asdf, r, M, depth, shaded, normals);
    }

    // From a branching cell, recurse down all branches
    else if (asdf->state == BRANCH) {
        for (int i=0; i < 8; ++i) {
            render_asdf_shaded(
                asdf->branches[i], r, M, depth, shaded, normals
            );
        }
    }

}

////////////////////////////////////////////////////////////////////////////////

_STATIC_
void asdf_fill_shaded(
    const ASDF* const asdf, const Region r,
    const float M[4], uint16_t*const*const depth)
{
    Interval pX, pY, pZ;

    // Loop over X, Y, and Z points
    for (int j=0; j < r.nj; ++j) {

        for (int i=0; i < r.ni; ++i) {

            // Loop downwards over Z values

            for (int k=r.nk-1; k >= 0; --k) {

                // If we can't brighten the point, return early
                const uint16_t L = r.L[k];
                if (L < depth[r.jmin+j][r.imin+i])  break;

                deproject_cube( (Interval){r.X[i], r.X[i+1]},
                    (Interval){r.Y[j], r.Y[j+1]},
                    (Interval){r.Z[k], r.Z[k+1]},
                    &pX, &pY, &pZ, M);

                // If there's no overlap, then skip this pixel
                if (pZ.upper < asdf->Z.lower || asdf->Z.upper < pZ.lower ||
                    pX.upper < asdf->X.lower || asdf->X.upper < pX.lower ||
                    pY.upper < asdf->Y.lower || asdf->Y.upper < pY.lower)
                {
                    continue;
                }

                // Now, sample at the center of the pixel to see if we should
                // color it in.
                Vec3f p;
                float c=0;
                for (int q=0; q < 8; ++q) {
                    p = (Vec3f) {
                        q & 4 ? pX.upper : pX.lower,
                        q & 2 ? pY.upper : pY.lower,
                        q & 1 ? pZ.upper : pZ.lower };

                    c = asdf_interpolate(asdf, p.x, p.y, p.z);
                    if (c < 0)  break;
                }

                // If we've found an intersection, then mark it.
                if (c < 0) {
                    depth[r.jmin+j][r.imin+i] = L;
                }

            } // End of Z loop

        }   // End of X loop

    }   // End of Y loop
}


_STATIC_
void asdf_leaf_shaded(const ASDF* const leaf, const Region r,
                      const float M[4], uint16_t*const*const depth,
                      uint16_t*const*const shaded, uint8_t (**normals)[3])
{
    Interval pX, pY, pZ;

    // Loop over X, Y, and Z points
    for (int j=0; j < r.nj; ++j) {

        for (int i=0; i < r.ni; ++i) {

            // Loop downwards over Z values

            for (int k=r.nk-1; k >= 0; --k) {

                // If we can't brighten the point, return early
                const uint16_t L = r.L[k];
                if (L < depth[r.jmin+j][r.imin+i])  break;

                deproject_cube( (Interval){r.X[i], r.X[i+1]},
                    (Interval){r.Y[j], r.Y[j+1]},
                    (Interval){r.Z[k], r.Z[k+1]},
                    &pX, &pY, &pZ, M);

                // If there's no overlap, then skip this pixel
                if (pZ.upper < leaf->Z.lower || leaf->Z.upper < pZ.lower ||
                    pX.upper < leaf->X.lower || leaf->X.upper < pX.lower ||
                    pY.upper < leaf->Y.lower || leaf->Y.upper < pY.lower)
                {
                    continue;
                }

                // Now, sample at the center of the pixel to see if we should
                // color it in.
                Vec3f p;
                float c=0;
                for (int q=0; q < 8; ++q) {
                    p = (Vec3f) {
                        q & 4 ? pX.upper : pX.lower,
                        q & 2 ? pY.upper : pY.lower,
                        q & 1 ? pZ.upper : pZ.lower };

                    c = asdf_interpolate(leaf, p.x, p.y, p.z);
                    if (c < 0)  break;
                }

                // If we've found an intersection, then color it in!
                if (c < 0) {

                    // Save the depth
                    depth[r.jmin+j][r.imin+i] = L;

                    // Find the gradient
                    Vec3f gradient = asdf_gradient(leaf, p.x, p.y, p.z);
                    Vec3f normal = normalize(project(gradient, M));

                    // If this point has a front-facing normal, shade it.
                    if (normal.z >= 0) {
                        float xy = sqrt(pow(normal.x, 2) + pow(normal.y, 2));
                        float alpha = fabs(atan2(normal.z, xy) - M_PI_2);

                        float shade = 65535 * sqrt(1 - alpha / M_PI_2);
                        shaded[r.jmin+j][r.imin+i] = shade;
                    }

                    // And draw colors to show the normal
                    normals[r.jmin+j][r.imin+i][0] = 255*(normal.x/2+0.5);
                    normals[r.jmin+j][r.imin+i][1] = 255*(normal.y/2+0.5);
                    normals[r.jmin+j][r.imin+i][2] = 255*(normal.z/2+0.5);
                }

            } // End of Z loop

        }   // End of X loop

    }   // End of Y loop
}

////////////////////////////////////////////////////////////////////////////////

void draw_asdf_cells(ASDF* a, Region r, uint8_t (**img)[3])
{
    if (!a)  return;

    uint8_t R = 0, G = 0, B = 0;
    if (a->state == BRANCH) {
        for (int i=0; i < 8; i++)
            draw_asdf_cells(a->branches[i], r, img);
        return;
    } else if (a->state == EMPTY) {
        B = 255;
        R = 50;
        G = 100;
    } else if (a-> state == FILLED)
        R = 255;
    else if (a->state == LEAF) {
        G = 255;
    }

    int imin, imax, jmin, jmax;

    if (a->X.upper <= r.X[0] || a->X.lower >= r.X[r.ni] ||
        a->Y.upper <= r.Y[0] || a->Y.lower >= r.Y[r.nj])
        return;

    float dx = r.X[r.ni] - r.X[0];
    imin = BOUND(round(r.ni*(a->X.lower - r.X[0]) / dx), 0, r.ni);
    imax = BOUND(round(r.ni*(a->X.upper - r.X[0]) / dx), 1, r.ni);

    float dy = r.Y[r.nj] - r.Y[0];
    jmin = BOUND(round(r.nj*(a->Y.lower - r.Y[0]) / dy), 0, r.nj);
    jmax = BOUND(round(r.nj*(a->Y.upper - r.Y[0]) / dy), 1, r.nj);

    for (int i=imin; i < imax; ++i) {
        img[jmin][i][0] = R;
        img[jmin][i][1] = G;
        img[jmin][i][2] = B;
        img[jmax-1][i][0] = R;
        img[jmax-1][i][1] = G;
        img[jmax-1][i][2] = B;
    }
    for (int j=jmin; j < jmax; ++j) {
        img[j][imin][0] = R;
        img[j][imin][1] = G;
        img[j][imin][2] = B;
        img[j][imax-1][0] = R;
        img[j][imax-1][1] = G;
        img[j][imax-1][2] = B;
    }

}

////////////////////////////////////////////////////////////////////////////////

void draw_asdf_distance(
    ASDF* const asdf, const Region r_,
    const float minimum, const float maximum,
    uint16_t*const*const depth)
{
    if (!asdf)  return;

    const Region r = bound_region(asdf, r_);

    if (asdf->state == LEAF) {
        for (int j=0; j < r.nj; ++j) {
            for (int i=0; i < r.ni; ++i) {
                const float f = asdf_sample(
                    asdf, (Vec3f){r.X[i], r.Y[j], r.Z[0]});

                uint16_t d = 65535*(f-minimum)/(maximum-minimum);
                if (d > depth[r.jmin+j][r.imin+i]) {
                    depth[r.jmin+j][r.imin+i] = d;
                }
            }
        }
    } else if (asdf->state == BRANCH) {
        for (int i=0; i < 8; i += 2) {
            draw_asdf_distance(asdf->branches[i], r, minimum, maximum, depth);
        }
    }
}
