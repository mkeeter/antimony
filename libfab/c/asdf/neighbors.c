#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "asdf/asdf.h"
#include "asdf/neighbors.h"

/*  face is the face relative to neighbor
    split and dir should not have the face normal axis bit set
*/
_STATIC_
const ASDF* get_subcell(
    const ASDF* const neighbor, const uint8_t face,
    const uint8_t split, const uint8_t dir)
{
    if (!neighbor)  return NULL;

    // Next, check to see if there are any bits left in split.
    // If not, then we've recursed all the way: return.
    if (!split) {
        return clone_virtual_asdf(neighbor);
    }

    if (neighbor->state != BRANCH && neighbor->state != VIRTUAL)
    {
        return NULL;
    }

    // First, check to see if the neighbor splits on the axis
    // parallel to the face normal.  If so, extract half
    // along that split as a virtual ASDF and recurse on it.
    const uint8_t face_axis = 1 << (face/2);
    if (neighbor->branches[face_axis]) {

        const uint8_t face_dir = (face % 2) ? face_axis : 0;
        ASDF* const virtual = calloc(1, sizeof(ASDF));
        virtual->state = VIRTUAL;
        for (int b=0; b < 8; ++b) {
            if (b & face_axis)  continue;
            virtual->branches[b] = neighbor->branches[b|face_dir];
        }
        const ASDF* const result = get_subcell(
            virtual, face, split, dir
        );
        free_virtual_asdf(virtual);
        return result;
    }

    // There will be a maximum of two bits set in nsplit
    // (because one of the axes is filtered out above)
    const uint8_t nsplit =
        (neighbor->branches[4] ? 4 : 0) |
        (neighbor->branches[2] ? 2 : 0) |
        (neighbor->branches[1] ? 1 : 0);

    // There will be either 1 or 2 bits set in split.
    // (there can't be 3, because we zeroed out the axis bit
    //  before invoking this function, and there can't be zero,
    //  because otherwise we would have returned already)

    // If the neighbor doesn't split at all, then recurse on it
    // (this occasionally occurs with VIRTUAL ASDFs with a single
    // branch).
    if (nsplit == 0) {
        return get_subcell(neighbor->branches[0], face, split, dir);
    }

    else {
        const uint8_t matched_axes = nsplit & split;
        ASDF* virtual = calloc(1, sizeof(ASDF));
        virtual->state = VIRTUAL;
        for (int b=0; b < 8; ++b) {
            if (b & matched_axes || b & face_axis ||
                !neighbor->branches[b|(matched_axes & dir)])
            {
                continue;
            }

            ASDF* v = (ASDF*)get_subcell(
                neighbor->branches[b|(matched_axes & dir)],
                face, split & ~matched_axes, dir
            );
            virtual->branches[b] = v;

            // If we failed, then clear everything and return.
            if (!v || (v->state == VIRTUAL && !v->branches[0])) {
                ASDF* error = NULL;

                // If this failure is indicative of a topologicaly mismatch,
                // then return a special error node (defined as a VIRTUAL
                // ASDF with no branches, and various data values stored
                // in the d array for debug).
                if (!matched_axes) {
                    error = calloc(1, sizeof(ASDF));
                    error->state = VIRTUAL;
                    error->d[0] = v ? v->d[0] : face_axis;
                    error->d[1] = v ? v->d[1] : nsplit;
                    error->d[2] = v ? v->d[2] : split;
                }
                free_virtual_asdf(virtual);
                return error;
            }
        }
        return virtual;
    }
}


/* Face is the face of asdf */
const ASDF* get_neighbor_v(
        const ASDF* const asdf, const uint8_t branch,
        const uint8_t face, const ASDF* const neighbor)
{
    if (!asdf->branches[branch]) return NULL;

    const uint8_t split =
        (asdf->branches[1] ? 1 : 0) |
        (asdf->branches[2] ? 2 : 0) |
        (asdf->branches[4] ? 4 : 0);

    const uint8_t face_axis = 1 << (face/2);
    const uint8_t face_dir  = (face % 2) ? face_axis : 0;

    // If this split face is inside the given ASDF cell,
    // then pick the appropriate branch and return it
    if ( (branch & face_axis) != face_dir &&
         asdf->branches[branch^face_axis])
    {
        return asdf->branches[branch^face_axis];
    }
    // Otherwise, use get_subcell to find the appropriate cell
    // (actual or virtual) in our neighbor.
    else {
        const uint8_t split_axes = split & ~face_axis;
        const uint8_t split_dir  = branch & ~face_axis;

        return get_subcell(
            neighbor, face^1, split_axes, split_dir
        );
    }
}


void get_neighbors_v(const ASDF* const asdf, const ASDF* const old[6],
        const ASDF* new[6], const uint8_t branch)
{
    for (int f=0; f < 6; ++f) {
        const ASDF* const n = get_neighbor_v(asdf, branch, f, old[f]);
        if (n && n->state == VIRTUAL && n->branches[0] == NULL) {
            printf("Neighbor split error; mesh may not be closed\n");
            get_neighbor_v(asdf, branch, f, old[f]);
            free_virtual_asdf((ASDF*)n);
            new[f] = NULL;
        } else {
            new[f] = n;
        }
    }
}


void get_neighbors_3d(const ASDF* const asdf, const ASDF* const old[6],
                      const ASDF* new[6], const uint8_t b)
{
    for (int i=0; i < 6; ++i) new[i] = NULL;

    if (!asdf->branches[b]) return;

    for (uint8_t axis=0; axis < 6; ++axis) {

        uint8_t mask = 1 << (axis/2);
        uint8_t dir  = mask * (axis % 2);

        // If we're pointing to within our own cell, then
        // pick the interior neighbor.
        if ( (b&mask)^dir && asdf->branches[b^mask]) {
            new[axis] = asdf->branches[b^mask];
        }

        // Correctly handle situations where a non-branch is
        // next to a branch, but the branch only splits on
        // the parallel axis.  Ascii art:
        //
        //   |---|---:---|
        //   | N | A : B |
        //   |---|---:---|
        //
        // N is the neighbor, the larger cell is splitting into
        // A and B on the axis along which it touches N (and no
        // others axes) so N remains a valid neighbor.
        else if (old[axis] && old[axis]->state == LEAF) {

            _Bool crack = false;
            for (int split=0; split < 3; ++split) {
                if ((1 << split) & mask)    continue;
                if (asdf->branches[1<<split] != NULL) crack = true;
            }

            new[axis] = crack ? NULL : old[axis];
        }

        // Otherwise, check to see that the neighbor splits on
        // the same axes (other than the one along which we're
        // joining).
        else if (old[axis] && old[axis]->state == BRANCH) {


            uint8_t split_mask = (old[axis]->branches[1] ? 1 : 0) |
                                 (old[axis]->branches[2] ? 2 : 0) |
                                 (old[axis]->branches[4] ? 4 : 0);

            _Bool crack = false;
            for (int split=0; split < 3; ++split) {

                // If we split differently than our neighbor and
                // this split isn't along the relevant axis, record
                // that there's a crack here.
                _Bool split_A = asdf->branches[1<<split] != NULL;
                _Bool split_B = old[axis]->branches[1<<split] != NULL;

                if ((split_A ^ split_B) && !((1 << split) & mask))
                {
                    crack = true;
                }
            }

            // Pick the appropriate cell in the neighbor
            if (crack) {
                new[axis] = NULL;
            } else if (dir) {
                new[axis] = old[axis]->branches[(b &~mask) & split_mask];
            } else {
                new[axis] = old[axis]->branches[(b | mask) & split_mask];
            }

        }
    }

}



void get_neighbors_2d(const ASDF* const asdf, const ASDF* const old[4],
                      const ASDF* new[4], uint8_t b)
{
    // Zero out the array elements
    for (int i=0; i < 4; ++i)   new[i] = NULL;

    if (!asdf->branches[b]) return;

    for (uint8_t axis=0; axis < 4; ++axis) {

        uint8_t mask = 1 << (axis/2 + 1);
        uint8_t dir  = mask * (axis % 2);

        // If we're pointing to within our own cell, then
        // pick the interior neighbor.
        if ( (b&mask)^dir && asdf->branches[b^mask]) {
            new[axis] = asdf->branches[b^mask];
        }

        else if (old[axis] && old[axis]->state != BRANCH) {
            new[axis] = old[axis];
        }

        // Otherwise, check to see that the neighbor splits on
        // the same axes (other than the one along which we're
        // joining).
        else if (old[axis] && old[axis]->state == BRANCH) {

            uint8_t split_mask = (old[axis]->branches[2] ? 2 : 0) |
                                 (old[axis]->branches[4] ? 4 : 0);

            _Bool crack = false;
            for (int split=0; split < 3; ++split) {

                // If we split differently than our neighbor and
                // this split isn't along the relevant axis, record
                // that there's a crack here.
                _Bool split_A = asdf->branches[1<<split] != NULL;
                _Bool split_B = old[axis]->branches[1<<split] != NULL;

                if ((split_A ^ split_B) && !((1 << split) & mask))
                {
                    crack = true;
                }
            }

            if (crack) { // Multi-scale stitching is okay in 2D
                new[axis] = old[axis];
            } else if (dir) {
                new[axis] = old[axis]->branches[(b &~mask) & split_mask];
            } else {
                new[axis] = old[axis]->branches[(b | mask) & split_mask];
            }

        }
    }

}
