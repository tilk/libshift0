
#include <stdlib.h>
#include <stdio.h>
#include "unwind.h"

extern int marker;
extern void* stacks[];

extern "C"
_Unwind_Reason_Code
__reset_personality(int version, _Unwind_Action actions,
                    _Unwind_Exception_Class exception_class,
                    struct _Unwind_Exception *ue_header,
                    struct _Unwind_Context *context)
{
    if (actions & _UA_SEARCH_PHASE)
        return _URC_CONTINUE_UNWIND;
    if (actions & _UA_CLEANUP_PHASE) {
//        void** base = _Unwind_GetCFA(context);
        free(stacks[--marker]);
        return _URC_CONTINUE_UNWIND;
    }
    abort();
}

