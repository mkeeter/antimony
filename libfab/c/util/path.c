#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "util/path.h"

Path* backtrace_path(Path* const p, Path* const orig)
{
    if (p == NULL)  return orig;
    if (p == orig || !p->prev)     return p;
    return backtrace_path(p->prev, orig);
}

////////////////////////////////////////////////////////////////////////////////

_STATIC_
void disconnect_node(Path* const p)
{
    for (int i=0; i < p->ptr_count; ++i) {
        *(p->ptrs[i]) = NULL;
    }
    free(p->ptrs);
    p->ptrs = NULL;
    p->ptr_count = 0;
}

void disconnect_path(Path* const start)
{
    Path* current = start;
    do {
        disconnect_node(current);
        current = current->next;
    } while (current != NULL && current != start);
}

////////////////////////////////////////////////////////////////////////////////

Path* decimate_path(Path* start, float error)
{
    Path* current = start;
    _Bool at_start = true;

    while (current && (current != start || at_start)) {
        // Store the next step in the linked list
        Path* next = current->next;
        at_start = false;

        // If we have two segments in a row, check to see if we can
        // delete the middle node (in the variable 'current')
        if (current->prev && current->next && current->prev != current->next)
        {
            const float A = sqrt(pow(current->x - current->prev->x,2) +
                                 pow(current->y - current->prev->y, 2));
            const float B = sqrt(pow(current->x - current->next->x,2) +
                                 pow(current->y - current->next->y, 2));
            const float C = sqrt(pow(current->next->x - current->prev->x,2) +
                                 pow(current->next->y - current->prev->y, 2));

            // Sort so that a >= b >= c
            const float a = fmax(A, fmax(B, C));
            const float c = fmin(A, fmin(B, C));
            const float b = A + B + C - a - c;

            // Heron's formula (numerically stable version)
            const float area = sqrt((a+(b+c))*(c-(a-b))*
                                    (c+(a-b))*(a+(b-c)))/4;

            if (area < error) {
                // Stitch the ends together
                current->prev->next = current->next;
                current->next->prev = current->prev;

                // Disconnect this node from the array
                disconnect_node(current);

                // Adjust the start point so that we don't do loop
                // detection by searching for a deleted node
                if (current == start) {
                    start = current->next;
                    at_start = true;
                }
                free(current);
            }
        }

        // Continue to travel through the list
        current = next;
    }

    return start;
}


void free_paths(Path** const paths, int count)
{
    for (int p=0; p < count; ++p)   free_path(paths[p]);
    free(paths);
}


void free_path(Path* const start)
{
    Path* pt = start;
    do {
        disconnect_node(pt);
        Path* const prev = pt;
        pt = pt->next;
        free(prev);
    } while (pt != NULL && pt != start);
}
