/* C++ wrappers for the shift0/reset0 control operators.
 * shift0.hpp
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

#ifndef _SHIFT0_HPP
#define _SHIFT0_HPP

#include "shift0.h"

#undef shift0
#undef reset

namespace s0 {

template<typename T1, typename T2>
T1 reset(T2 &f);

template<typename T1, typename T2, typename FO>
T1 shift0(FO &f);

template<typename T1, typename T2>
class cont {
public:
    cont(::cont* pc): c(pc) {};
    ~cont() { freecont(c); };

    T1 operator()(const T2 e) const {
        return reinterpret_cast<T1>(runcont(c, reinterpret_cast<void*>(e)));
    }
    
private:
    cont(cont& pc) {};
    cont &operator=(cont &pc) {};

    ::cont *c;
};

template<typename T1, typename FO>
class reset_helper {
private:
    friend T1 s0::reset<T1, FO>(FO &);
    reset_helper(FO &f): fobj(f) {};
    FO &fobj;
    static void *reset(void *p) {
        reset_helper<T1, FO> *helper = (reset_helper<T1, FO>*) p;
        return reinterpret_cast<void*>(helper->fobj());
    }
};

template<typename T1, typename T2, typename FO>
class shift0_helper {
private:
    friend T1 s0::shift0<T1, T2, FO>(FO &);
    shift0_helper(FO &f): fobj(f) {};
    FO &fobj;
    static void *shift0(::cont *c, void *p) {
        shift0_helper<T1, T2, FO> *helper = (shift0_helper<T1, T2, FO>*) p;
        return reinterpret_cast<void*>(helper->fobj(new cont<T1, T2>(c)));
    }
};

template<typename T1, typename FO>
T1 reset(FO &f)
{
    reset_helper<T1, FO> helper(f);
    reset_arg(reset_helper<T1, FO>::reset, (void*) &helper);
}

template<typename T1, typename T2, typename FO>
T1 shift0(FO &f)
{
    shift0_helper<T1, T2, FO> helper(f);
    shift0_arg(shift0_helper<T1, T2, FO>::shift0, (void*) &helper);
}

};

#endif
