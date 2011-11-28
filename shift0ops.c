/* Definition of freecont and derived control operators.
 * shift0ops.c
 *
 * Copyright (c) 2011, Marek Materzok <tilk@tilk.eu>
 *
 *  1. Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright 
 *     notice, this list of conditions and the following disclaimer in the 
 *     documentation and/or other materials provided with the distribution.
 *
 * The software is provided 'as is', without any warranty.
 */

#include <stdlib.h>
#include "shift0.h"

void *runcont_reentry(struct cont *fun, void *val);
void *runcont_dirty(struct cont *fun, void *val);

void freecont(struct cont* c)
{
    void *(*f)(struct cont*, void*) = 
        *((void *(**)(struct cont*, void*))c);
    if (f == runcont_reentry) abort();
    if (f == runcont_dirty) {
        void *p = ((void**)c)[7];
        free(p);
    }
    free(c);
}

#undef shift0
#undef reset

void *shift0(shiftable_t f)
{
    return shift0_arg((shiftable_arg_t) f, 0);
}

void *reset(resettable_t f)
{
    return reset_arg((resettable_arg_t) f, 0);
}

struct shift_str {
    shiftable_arg_t f;
    void *arg;
    struct cont *c;
};

void *_shift_arg_internal2(struct shift_str *s)
{
    return s->f(s->c, s->arg);
}

void *_shift_arg_internal(struct cont *c, struct shift_str *s)
{
    s->c = c;
    return reset_arg((resettable_arg_t) _shift_arg_internal2, (void*) s);
}

void *shift_arg(shiftable_arg_t f, void *arg)
{
    struct shift_str s;
    s.f = f;
    s.arg = arg;
    return shift0_arg((shiftable_arg_t) _shift_arg_internal, (void*) &s);
}

void *_shift_internal(struct cont *c, shiftable_t f)
{
    return reset_arg((resettable_arg_t)f, (void*)c);
}

void *shift(void *(*f)(struct cont *c))
{
    return shift0_arg((shiftable_arg_t) _shift_internal, (void*) f);
}

