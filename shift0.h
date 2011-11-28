/* Declarations for the shift0/reset0 control operators
 * shift0.h
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

#ifndef _SHIFT0_H
#define _SHIFT0_H

#include <features.h>
#include <sys/types.h>

__BEGIN_DECLS

struct cont {};

typedef void* (*resettable_t)();
typedef void* (*resettable_arg_t)(void*);
typedef void* (*shiftable_t)(struct cont*);
typedef void* (*shiftable_arg_t)(struct cont*, void*);

void* reset_arg(resettable_arg_t fun, void* arg);
void* reset(resettable_t fun);
#define reset(f) reset_arg((resettable_arg_t)f,0)
void* shift0_arg(shiftable_arg_t fun, void* a);
void* shift0(shiftable_t fun);
#define shift0(f) shift0_arg((shiftable_arg_t)f,0)
void* shift_arg(shiftable_arg_t fun, void *a);
void* shift(shiftable_t fun);
void* runcont(struct cont *fun, void* val);
void freecont(struct cont *fun);

__END_DECLS

#endif
