
#include <stdlib.h>
#include "shift0.h"

int32_t runcont_reentry(struct cont *fun, int32_t val);
int32_t runcont_dirty(struct cont *fun, int32_t val);

void freecont(struct cont* c)
{
    int32_t (*f)(struct cont*, int32_t) = 
        *((int32_t (**)(struct cont*, int32_t))c);
    if (f == runcont_reentry) abort();
    if (f == runcont_dirty) {
        void *p = ((void**)c)[7];
        free(p);
    }
    free(c);
}

