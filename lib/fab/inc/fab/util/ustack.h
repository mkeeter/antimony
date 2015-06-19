#ifndef USTACK_H
#define USTACK_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* A stack of unsigned integers.
 */
typedef struct ustack_ {
    unsigned* data;

    size_t ptr;
    size_t allocated;
} ustack;

// Pushes a new value to the stack
void ustack_push(ustack* u, unsigned i);

// Pops the top value from the stack
unsigned ustack_pop(ustack* u);

// Frees a ustack struct
void ustack_free(ustack* u);

// Increments the top value on the stack
void ustack_increment(ustack* u);

#ifdef __cplusplus
}
#endif

#endif
