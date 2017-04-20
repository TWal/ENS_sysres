#ifndef __SUPOS_ASSERT_H
#define __SUPOS_ASSERT_H

#include "stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SUP_OS_KERNEL
    __attribute__((noreturn)) void bsod(const char* s, ...);

#define __S(x) #x
#define __S_(x) __S(x)
#define S__LINE__ __S_(__LINE__)
#define assert(cond) {                                                  \
        if(!(cond)) {                                                   \
            bsod("Assertion failed at " __FILE__ ":" S__LINE__ ": " #cond); \
        }                                                               \
    }
#else

    // TODO printf something.
#define assert(cond) {                                                  \
        if(!(cond)) {                                                   \
            _exit(-42);                                                  \
        }                                                               \
    }


#endif

#ifdef __cplusplus
}
#endif


#endif
