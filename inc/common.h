#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

typedef int bool;
#define true 1
#define false 0

#ifndef inline
#define inline __inline
#endif

#define UNUSED_VARIABLE(x) ((void)(x))

#if defined(_MSC_VER) && _MSC_VER > 1200
    #define MULTI_LINE_MACRO_BEGIN          \
        __pragma(warning(push))             \
        __pragma(warning(disable:4127))     \
        do {
    #define MULTI_LINE_MACRO_END            \
        } while(0)                          \
        __pragma(warning(pop))
#else /* defined(_MSC_VER) && _MSC_VER > 1200 */
    #define MULTI_LINE_MACRO_BEGIN do {  
    #define MULTI_LINE_MACRO_END } while(0)
#endif /* defined(_MSC_VER) && _MSC_VER > 1200 */

#if defined(WIN32) && !(defined(__CYGWIN__) || defined(__MINGW32__))
    #define FMT_I64 "%I64"
    #define FMT_U64 "%I64u"
    #define FMT_H64 "%I64X"
    #define SUFFIX_I64(num) num##i64
    #define SUFFIX_U64(num) num##ui64 
#else /* defined(WIN32) && !(defined(__CYGWIN__) || defined(__MINGW32__)) */
    #define FMT_I64 "%lld"
    #define FMT_U64 "%llu"
    #define FMT_H64 "%llX"
    #define SUFFIX_I64(num) num##LL
    #define SUFFIX_U64(num) num##LLU
#endif /* defined(WIN32) && !(defined(__CYGWIN__) || defined(__MINGW32__)) */

#if defined(_MSC_VER) && (_MSC_VER <= 1200)
#define UINT64_DOUBLE_DIV(val, div) (((int64_t) (val)) / (div))
#else
#define UINT64_DOUBLE_DIV(val, div) ((val) / (div))
#endif /* defined(_MSC_VER) && (_MSC_VER <= 1200) */

/* portable SRA */
#define SRAW(val, sh) ((sh) == 0 ? (int32_t)(val) : (int32_t)(((uint32_t)(val) >> (sh)) | (((uint32_t)(val) & SIGN_BIT_WORD) ? (~0U << (32 - (sh))) : 0U)))
#define SRA(val, sh) ((sh) == 0 ? (sreg_t)(val) : (sreg_t)(((reg_t)(val) >> (sh)) | (((reg_t)(val) & SIGN_BIT) ? (~((reg_t)0) << (XLEN - (sh))) : (reg_t)0)))
/* portable sign-extension */
#define SIGN_EXTENDWU(uval, amt) (-(((uval) & (1U << ((amt) - 1)))) | (uval))
#define SIGN_EXTENDW(val, amt) ((int32_t) SIGN_EXTENDWU((uint32_t) (val), (amt)))
#define SIGN_EXTENDU(uval, amt) (-(((uval) & (BASE_SHIFT << ((amt) - 1)))) | (uval))
#define SIGN_EXTEND(val, amt) ((sreg_t) SIGN_EXTENDU((reg_t) (val), (amt)))

#endif /* COMMON_H */
