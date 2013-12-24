#ifndef PATH_H
#define PATH_H

/*  struct Path_
 *
 *  Stores x and y coordinates, pointers to neighbors, and
 *  pointers to the places where cells are pointing to it
 *  (so that it can disconnect itself from the cells when
 *  being deleted).
 */
typedef struct Path_ {
    struct Path_* prev;
    struct Path_* next;
    float x, y, z;
    struct Path_*** ptrs; // parent pointers to this path
    int ptr_count;
} Path;


/*  backtrace
 *
 *  Travels backwards along a path until reaching a start node
 *  or the orig node.
 */
Path* backtrace_path(Path* const p, Path* const orig);


/*  disconnect_path
 *
 *  Removes a path from the ptrs array, using the self
 *  ptrs stored in each path node.
 */
void disconnect_path(Path* const p);


/*  decimate_path
 *
 *  Removes nodes in the path that can be removed without significant error.
 *  Returns a new start node, which will be different if the original start
 *  node is removed during the decimation.
 */
Path* decimate_path(Path* start, float error);


/*  free_paths
 *
 *  Frees each path in an array and the array itself.
 */
void free_paths(Path** const paths, int count);


/*  free_path
 *
 *  Frees a single path, disconnecting nodes from their referents.
 */
void free_path(Path* const start);

#endif
