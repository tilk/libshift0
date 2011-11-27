
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include "shift0.h"

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

inline int32_t tested_reset(int32_t (*f)())
{
    int32_t ret;
//    TEST_REGISTERS(ret = reset(f));
    BEGIN_TEST_REGISTERS
        asm volatile(SETREGS : : INREGS(inregs[0], inregs[1], inregs[2]) : );
        ret = reset(f);
        asm volatile(GETREGS : OUTREGS(outregs[0], outregs[1], outregs[2]) : : CLEANEDREGS);
    END_TEST_REGISTERS
    /*
    TEST_REGISTERS( 
    asm(SETREGS "\n\tpushl %[test]\n\tcall reset\n\taddl $4, %%esp\n\t" GETREGS 
        : OUTREGS(outregs[0], outregs[1], outregs[2]), "=a" (ret)
        : INREGS(inregs[0], inregs[1], inregs[2]), [test] "m" (f) 
        : CLEANEDREGS, CALLERSAVEDREGS)); 
    */
    return ret;
}

inline int32_t tested_shift0(int32_t (*f)(struct cont*))
{
    int32_t ret;
    BEGIN_TEST_REGISTERS
        asm volatile(SETREGS : : INREGS(inregs[0], inregs[1], inregs[2]) : );
        ret = shift0(f);
        asm volatile(GETREGS : OUTREGS(outregs[0], outregs[1], outregs[2]) : : CLEANEDREGS);
    END_TEST_REGISTERS
    /*
    TEST_REGISTERS(
    asm(SETREGS "\n\tpushl %[test]\n\tcall shift0\n\taddl $4, %%esp\n\t" GETREGS
        : OUTREGS(outregs[0], outregs[1], outregs[2]), "=a" (ret)
        : INREGS(inregs[0], inregs[1], inregs[2]), [test] "m" (f)
        : CLEANEDREGS, CALLERSAVEDREGS_NOEAX));
    */
    return ret;
}

inline int32_t tested_runcont(struct cont* cont, int32_t p)
{
    int32_t ret;
    BEGIN_TEST_REGISTERS
        asm volatile(SETREGS : : INREGS(inregs[0], inregs[1], inregs[2]) : CLEANEDREGS);
        ret = runcont(cont, p);
        asm volatile(GETREGS : OUTREGS(outregs[0], outregs[1], outregs[2]) : : );
    END_TEST_REGISTERS
    /*
    TEST_REGISTERS(
    asm(SETREGS "\n\tpushl %[arg]\n\tpushl %[p]\n\tcall runcont\n\taddl $8, %%esp\n\t" GETREGS
        : OUTREGS(outregs[0], outregs[1], outregs[2]), "=a" (ret)
        : INREGS(inregs[0], inregs[1], inregs[2]), [arg] "m" (p), [p] "m" (cont)
        : CLEANEDREGS, CALLERSAVEDREGS_NOEAX));
    */
    return ret;
}

inline int32_t tested_reset_catch(int32_t (*f)())
{
    int32_t ret;
    BEGIN_TEST_REGISTERS
        asm volatile(SETREGS : : INREGS(inregs[0], inregs[1], inregs[2]) : );
        try {
            ret = reset(f);
            abort();
        } catch (int32_t v) {
            asm volatile(GETREGS : OUTREGS(outregs[0], outregs[1], outregs[2]) : : CLEANEDREGS);
            END_TEST_REGISTERS
            return v;
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

#define RUN_TEST_EXC(c, t) RUN_TEST(c, { \
        try { \
            t; abort(); \
        } catch (int32_t v) { \
            assert(v == value1); \
        } \
    });

void throw_scramble(int32_t v)
{
    BEGIN_TEST_REGISTERS
        asm volatile(SETREGS : : INREGS(inregs[0], inregs[1], inregs[2]) : CLEANEDREGS);
        throw v;
    END_TEST_REGISTERS
}

int32_t test1()
{
    counter++;
    return value1;
}

int32_t test2x(struct cont *p)
{
    counter++;
    return value1;
}

int32_t test2()
{
    counter++;
    tested_shift0(test2x);
    counter++;
    return 0;
}

int32_t test3x(struct cont *p)
{
    counter++;
    assert(tested_runcont(p, 3) == 3+value2);
    assert(tested_runcont(p, 4) == 4+value2);
    assert(tested_runcont(p, 5) == 5+value2);
    counter++;
    return value1;
}

int32_t test3()
{
    counter++;
    int32_t v = tested_shift0(test3x);
    counter++;
    return v+value2;
}

int32_t test4()
{
    counter++;
    throw_scramble(value1);
}

int32_t test5x(struct cont *c)
{
    counter++;
    throw_scramble(value1);
}

int32_t test5()
{
    counter++;
    tested_shift0(test5x);
}

int32_t test6x(struct cont *c)
{
    counter++;
    try {
        tested_runcont(c, 0);
        tested_runcont(c, 1);
        abort();
    } catch (int32_t v) {
        assert(v == value1);
    }
    tested_runcont(c, 0);
    tested_runcont(c, 1);
}

int32_t test6()
{
    counter++;
    if (tested_shift0(test6x)); {
        counter++;
        throw_scramble(value1);
    }
}

int main()
{
    RUN_TEST(1, assert(tested_reset(test1) == value1));
    RUN_TEST(2, assert(tested_reset(test2) == value1));
    RUN_TEST(6, assert(tested_reset(test3) == value1));
    RUN_TEST(1, assert(tested_reset_catch(test4) == value1));
    RUN_TEST(2, assert(tested_reset_catch(test5) == value1));
    RUN_TEST_EXC(4, tested_reset(test6));
}

