#include "fab/util/ustack.h"

void ustack_push(ustack* u, unsigned i)
{
    if (u->allocated == 0)
    {
        u->allocated = 2;
        u->data = malloc(u->allocated * sizeof(unsigned));
    }
    else if (u->ptr >= u->allocated)
    {
        u->allocated *= 2;
        u->data = realloc(u->data, u->allocated * sizeof(unsigned));
    }

    u->data[u->ptr++] = i;
}

unsigned ustack_pop(ustack* u)
{
    return u->data[--u->ptr];
}

void ustack_free(ustack* u)
{
    free(u->data);
    free(u);
}

void ustack_increment(ustack* u)
{
    u->data[u->ptr - 1]++;
}
