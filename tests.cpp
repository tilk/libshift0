/* Test suite.
 * tests.cpp
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

#include <cstdlib>
#include <cstdio>
#include <cassert>
#include "shift0.hpp"

#define SETREGS "movl %[ebxin], %%ebx\
\n\tmovl %[esiin], %%esi\
\n\tmovl %[ediin], %%edi"

#define GETREGS "movl %%ebx, %[ebxout]\
\n\tmovl %%esi, %[esiout]\
\n\tmovl %%edi, %[ediout]"

#define INREGS(ebx,esi,edi) [ebxin] "m" (ebx), [esiin] "m" (esi), [ediin] "m" (edi)
#define OUTREGS(ebx,esi,edi) [ebxout] "=m" (ebx), [esiout] "=m" (esi), [ediout] "=m" (edi)
#define CLEANEDREGS "ebx", "esi", "edi"
#define CALLERSAVEDREGS "eax", "ecx", "edx"
#define CALLERSAVEDREGS_NOEAX "ecx", "edx"

#define NUMREGS 3

#define BEGIN_TEST_REGISTERS \
    int inregs[NUMREGS], outregs[NUMREGS]; \
    for (int i = 0; i < NUMREGS; i++) inregs[i] = rand(); 

#define END_TEST_REGISTERS \
    for (int i = 0; i < NUMREGS; i++) \
        if (outregs[i] != inregs[i]) abort(); 

#define TEST_REGISTERS(instr) { \
    int inregs[NUMREGS], outregs[NUMREGS]; \
    for (int i = 0; i < NUMREGS; i++) inregs[i] = rand(); \
    instr; \
    for (int i = 0; i < NUMREGS; i++) \
        if (outregs[i] != inregs[i]) abort(); \
}

inline void* tested_reset_arg(void* (*f)(void*), void* arg)
{
    void* ret;
    BEGIN_TEST_REGISTERS
    asm volatile(SETREGS : : INREGS(inregs[0], inregs[1], inregs[2]) : );
    ret = reset_arg(f, arg);
    asm volatile(GETREGS : OUTREGS(outregs[0], outregs[1], outregs[2]) : : CLEANEDREGS);
    END_TEST_REGISTERS
    return ret;
}

inline void* tested_shift0(void* (*f)(struct cont*))
{
    void* ret;
    BEGIN_TEST_REGISTERS
    asm volatile(SETREGS : : INREGS(inregs[0], inregs[1], inregs[2]) : );
    ret = shift0(f);
    asm volatile(GETREGS : OUTREGS(outregs[0], outregs[1], outregs[2]) : : CLEANEDREGS);
    END_TEST_REGISTERS
    return ret;
}

inline void* tested_runcont(struct cont* cont, void* p)
{
    void* ret;
    BEGIN_TEST_REGISTERS
    asm volatile(SETREGS : : INREGS(inregs[0], inregs[1], inregs[2]) : CLEANEDREGS);
    ret = runcont(cont, p);
    asm volatile(GETREGS : OUTREGS(outregs[0], outregs[1], outregs[2]) : : );
    END_TEST_REGISTERS
    return ret;
}

inline void* tested_reset_arg_catch(void* (*f)(void*), void* arg)
{
    void* ret;
    BEGIN_TEST_REGISTERS
    asm volatile(SETREGS : : INREGS(inregs[0], inregs[1], inregs[2]) : );
    try {
        ret = reset_arg(f, arg);
        abort();
    } catch (int32_t v) {
        asm volatile(GETREGS : OUTREGS(outregs[0], outregs[1], outregs[2]) : : CLEANEDREGS);
        END_TEST_REGISTERS
        return (void*)v;
    }
}

int counter;
int32_t value1, value2;

#define RUN_TEST(c, t) { \
    value1 = rand(); value2 = rand(); \
    counter = 0; \
    t; \
    assert(counter==c); \
}

void throw_scramble(int32_t v)
{
    BEGIN_TEST_REGISTERS
    asm volatile(SETREGS : : INREGS(inregs[0], inregs[1], inregs[2]) : CLEANEDREGS);
    throw v;
    END_TEST_REGISTERS
}

void* test1(void* a)
{
    assert((int32_t)a == value1);
    counter++;
    return (void*)value1;
}

void* test2x(struct cont *p)
{
    counter++;
    freecont(p);
    return (void*)value1;
}

void* test2(void* a)
{
    assert((int32_t)a == value1);
    counter++;
    tested_shift0(test2x);
    counter++;
    return 0;
}

void* test3x(struct cont *p)
{
    counter++;
    assert((int32_t)tested_runcont(p, (void*)3) == 3+value2);
    assert((int32_t)tested_runcont(p, (void*)4) == 4+value2);
    assert((int32_t)tested_runcont(p, (void*)5) == 5+value2);
    counter++;
    freecont(p);
    return (void*)value1;
}

void* test3(void* a)
{
    assert((int32_t)a == value1);
    counter++;
    void* v = tested_shift0(test3x);
    counter++;
    return (void*)((int32_t)v+value2);
}

void* test4(void* a)
{
    assert((int32_t)a == value1);
    counter++;
    throw_scramble(value1);
}

void* test5x(struct cont *c)
{
    counter++;
    freecont(c);
    throw_scramble(value1);
}

void* test5(void* a)
{
    assert((int32_t)a == value1);
    counter++;
    tested_shift0(test5x);
}

void* test6x(struct cont *c)
{
    counter++;
    try {
        tested_runcont(c, (void*)0);
        tested_runcont(c, (void*)1);
        abort();
    } catch (int32_t v) {
        assert(v == value1);
    }
    tested_runcont(c, (void*)0);
    tested_runcont(c, (void*)1);
}

void* test6(void* a)
{
    assert((int32_t)a == value1);
    counter++;
    void* ret = tested_shift0(test6x);
    if (ret) {
        counter++;
        throw_scramble(value1);
    }
}

int main()
{
    RUN_TEST(1, assert((int32_t)tested_reset_arg(test1, (void*)value1) == value1));
    RUN_TEST(2, assert((int32_t)tested_reset_arg(test2, (void*)value1) == value1));
    RUN_TEST(6, assert((int32_t)tested_reset_arg(test3, (void*)value1) == value1));
    RUN_TEST(1, assert((int32_t)tested_reset_arg_catch(test4, (void*)value1) == value1));
    RUN_TEST(2, assert((int32_t)tested_reset_arg_catch(test5, (void*)value1) == value1));
    RUN_TEST(4, assert((int32_t)tested_reset_arg_catch(test6, (void*)value1) == value1));
}

