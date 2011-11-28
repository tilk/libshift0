/* Personality functions for proper stack unwinding.
 * shift0personality.c
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
#include <stdio.h>
#include "unwind.h"
#include "shift0.h"

extern int marker;
extern void* stacks[];

int32_t runcont_dirty(struct cont *fun, int32_t val);

_Unwind_Reason_Code
__reset_personality(int version, _Unwind_Action actions,
                    _Unwind_Exception_Class exception_class,
                    struct _Unwind_Exception *ue_header,
                    struct _Unwind_Context *context)
{
    if (actions & _UA_SEARCH_PHASE)
        return _URC_CONTINUE_UNWIND;
    if (actions & _UA_CLEANUP_PHASE) {
        free(stacks[--marker]);
        return _URC_CONTINUE_UNWIND;
    }
    abort();
}

_Unwind_Reason_Code
__runcont_personality(int version, _Unwind_Action actions,
                    _Unwind_Exception_Class exception_class,
                    struct _Unwind_Exception *ue_header,
                    struct _Unwind_Context *context)
{
    if (actions & _UA_SEARCH_PHASE)
        return _URC_CONTINUE_UNWIND;
    if (actions & _UA_CLEANUP_PHASE) {
        void** base = (void**)_Unwind_GetGR(context,5);
        struct cont* c = (struct cont*)*(base+2);
        *((int32_t (**)(struct cont*, int32_t)) c) = runcont_dirty;
        return _URC_CONTINUE_UNWIND;
    }
    abort();
}

