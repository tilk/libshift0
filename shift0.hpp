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
class cont {
public:
    ~cont() { freecont(c); };

    T1 &operator()(const T2 &e) const { return runme(e); }
    
private:
    cont(cont& pc) {};
    cont(::cont* pc): c(pc) {};
    cont &operator=(cont &pc) {};

    template<typename T1x, typename T2x>
    T1x &runme(const T2x &e) const;

    ::cont *c;
};
/*
template<typename T1, typename T2>
T1 &cont<T1,T2>::operator()(const T2 &e) const {
    T1 *ret = static_cast<T1*>(runcont(c, static_cast<int32_t>(*e)));
    return *ret;
};

template<typename T1>
T1 &cont<T1,int32_t>::operator()(const int32_t &e) const {
    T1 *ret = static_cast<T1*>(runcont(c, e));
    return *ret;
};*/


};

#endif
