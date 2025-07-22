/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once


#include "../def.hpp"
#include "../lang.hpp"


//
// Copied and modified from
// https://github.com/nemequ/portable-snippets/tree/master/debug-trap
//

#if _DEBUG

#if defined(__APPLE__) || defined(__unix__)
#include <sys/types.h>
#include <unistd.h>
#endif

#if __ibmxl__ || __xlC__
#include <builtins.h>
#endif

//namespace {  // to hide all the functions

// It's more conventient with the define as the IDE ends up right at the source code instead of here

//#ifndef HaveDebugTrapBody
#if defined(__APPLE__) || defined(__unix__)

#if __aarch64__

#if 0
#define ADebugTrapBody {\
    __asm__ __volatile__(\
        "mov    x0, %x0;"     /* pid */\
        "mov    x1, #0x11;"   /* SIGSTOP */ \
        "mov    x16, #0x25;"  /* syscall 37 = kill */\
        "svc    #0x80;"       /* interrupt */ \
        "mov    x0, x0;"      /* nop */\
        ::  "r"(getpid())\
        :   "x0", "x1", "x16", "memory"\
    );}

#endif

AForceInline void ADebugTrapBody() noexcept
//void DebugTrap()
{
    __asm__ __volatile__(R"(
             mov    x0, %x0;     // pid
             mov    x1, #0x11;   // SIGSTOP
             mov    x16, #0x25;  // syscall 37 = kill
             svc    #0x80;       // software interrupt
             //mov    x0, x0;      // nop
         )"
         ::  "r"(getpid())
         :   "x0", "x1", "x16", "memory"
     );
}

#elif __arm__

AForceInline void ADebugTrapBody() noexcept
//void DebugTrap()
//mov    r0, r0;     // nop
{
    __asm__ __volatile__ (R"(
            mov    r0, %0;     // pid
            mov    r1, #0x11;  // SIGSTOP
            mov    r12, #0x25; // syscall 37 = kill
            svc    #0x80;      // software interrupt
            mov    r0, r0;     // nop
        )"
        ::  "r"(getpid())
        :   "r0", "r1", "r12", "memory"
    );
}

#elif __i386__ || __x86_64__
AForceInline void ADebugTrapBody() noexcept { __asm__ __volatile__("int $3;"); }
#endif  // architectures

//#define DebugTrap() ADebugTrapBody()
//#define DebugTrap() do { if (!std::is_constant_evaluated()) ADebugTrapBody() } while(0)

#define AHaveDebugTrapBody 1

#endif // if __APPLE__
//#endif // ifndef DebugTrap


#if _MSC_VER || __INTEL_COMPILER
    #define ADebugTrapBody()    __debugbreak()
    #define AHaveDebugTrapBody  1
#endif


#if !HaveDebugTrapBody && __has_builtin(__builtin_debugtrap)
    #define ADebugTrapBody()    __builtin_debugtrap()
    #define HaveDebugTrapBody   1
#endif

#if !HaveDebugTrapBody && __has_builtin(__builtin_trap)
    #define ADebugTrapBody()    __builtin_trap()
    #define HaveDebugTrapBody   1
#endif



#if !AHaveDebugTrapBody
    #if __ARMCC_VERSION
	    #define ADebugTrapBody() __breakpoint(42)
    #elif __ibmxl__ || __xlC__
	    #define ADebugTrapBody() __trap(42)
    #elif __i386__ || __x86_64__
	    AForceInline void ADebugTrapBody() noexcept { __asm__ __volatile__("int $03"); }
    #elif __thumb__
	    AForceInline void ADebugTrapBody() noexcept { __asm__ __volatile__(".inst 0xde01"); }
    #elif __alpha__
	    AForceInline void ADebugTrapBody() noexcept { __asm__ __volatile__("bpt"); }
    #endif
    #define AHaveDebugTrapBody 1
#endif  // if not defined DebugTrap


//}  // namespace noname


//#if !AHaveDebugTrapBody && __has_include(<signal.h>)
//    #include <signal.h>
//        #ifdef SIGTRAP
//    #define ADebugTrapBody() raise(SIGTRAP)
//        #else
//    #define ADebugTrapBody() raise(SIGABRT)
//        #endif
//    #define AHaveDebugTrapBody 1
//#endif  // HaveDebugTrapBody


#if !AHaveDebugTrapBody
    #error "Cannot come up with ADebugTrapBody()"
#endif
#undef AHaveDebugTrapBody

#define ADebugTrap() \
    do { if (!std::is_constant_evaluated()) {\
            ADebugTrapBody();\
    }} while(0)


#define ADebugTrapAssert(cond) \
    do { if (!std::is_constant_evaluated() && apl::isFalse(cond)) {\
            ADebugTrapBody();\
    }} while(0)


//
// Obsolete
//
//#define DebugTrap() \
//    do { \
//        if (!std::is_constant_evaluated())\
//            ADebugTrapBody(); \
//    } while(0)
//
//#define DebugTrapAssert(_Cond) \
//    do {\
//        if (!std::is_constant_evaluated() && !ALikely(_Cond)) \
//            ADebugTrapBody(); \
//    } while(0)


#else // Release

//#define DebugTrap() (void(0))
//#define DebugTrapAssert(__cond) ((void)(__cond))

#define ADebugTrap(...)         ::apl::noop(__VA_ARGS__)
#define ADebugTrapAssert(...)   ::apl::noop(__VA_ARGS__)


//#define DebugTrap(...)         ::apl::noop(__VA_ARGS__)
//#define DebugTrapAssert(...)   ::apl::noop(__VA_ARGS__)

#endif

//} // namespace apl

