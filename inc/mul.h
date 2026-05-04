#ifndef MUL_H
#define MUL_H

#include <stdint.h>

static inline uint32_t rv_mulw(const uint32_t a, const uint32_t b) {
	return a * b;
}

static inline reg_t rv_mul(const reg_t a, const reg_t b) {
	return a * b;
}

#ifdef RV64M
static inline uint64_t mulhudw(const uint64_t a, const uint64_t b) {
	const uint64_t a_lo = (uint32_t)a, a_hi = a >> 32;
    const uint64_t b_lo = (uint32_t)b, b_hi = b >> 32;

    const uint64_t p00 = a_lo * b_lo;
    const uint64_t p10 = a_hi * b_lo;
    const uint64_t p01 = a_lo * b_hi;
    const uint64_t p11 = a_hi * b_hi;

    const uint64_t cross = p10 + (p00 >> 32);

    return p11 + (cross >> 32) + ((p01 + (uint32_t)cross) >> 32);
}
/* locate an appropriate multiplication function for 128-bit integers */
#if defined(_MSC_VER) && (defined(_M_X64) || defined(__amd64__) || defined(__x86_64__))
#include <intrin.h>
#pragma intrinsic(__mulh)
#pragma intrinsic(__umulh)

static inline sreg_t rv_mulh(const sreg_t a, const sreg_t b) {
	return (sreg_t) __mulh((__int64) a, (__int64) b);
}

static inline sreg_t rv_mulhsu(const sreg_t a, const reg_t b) {
	sreg_t res = (sreg_t) __umulh((unsigned __int64) a, (unsigned __int64) b);
	if (a < 0) res -= b;
    return res;
}

static inline reg_t rv_mulhu(const reg_t a, const reg_t b) {
	return (reg_t) __umulh((unsigned __int64) a, (unsigned __int64) b);
}

#else /* _MSC_VER */
#if defined(__GNUC__) || defined(__clang__)
static inline sreg_t rv_mulh(const sreg_t a, const sreg_t b) {
	return (sreg_t) ((__uint128) ((__int128) a * (__int128) b) >> 64);
}

static inline sreg_t rv_mulhsu(const sreg_t a, const reg_t b) {
	return (sreg_t) ((__uint128) ((__int128) a * (__int128) b) >> 64);
}

static inline reg_t rv_mulhu(const reg_t a, const reg_t b) {
	return (reg_t) (((__uint128) a * (__uint128) b) >> 64);
}
#else /* defined(__GNUC__) || defined(__clang__) */
/* fall back to software computation */
static inline sreg_t rv_mulh(const sreg_t a, const sreg_t b) {
	sreg_t res = (sreg_t) mulhudw((uint64_t) a, (uint64_t) b);
	if (a < 0) res -= b;
    if (b < 0) res -= a;
    return res;
}

static inline sreg_t rv_mulhsu(const sreg_t a, const reg_t b) {
	sreg_t res = (sreg_t) mulhudw((uint64_t) a, (uint64_t) b);
	if (a < 0) res -= b;
    return res;
}

static inline reg_t rv_mulhu(const reg_t a, const reg_t b) {
	return (reg_t) mulhudw((uint64_t) a, (uint64_t) b);
}
#endif /* defined(__GNUC__) || defined(__clang__) */
#endif /* _MSC_VER */
#else /* RV64M */
/* to use the same codebase with RV64 */
static inline sreg_t rv_mulh(const sreg_t a, const sreg_t b) {
	return (sreg_t) ((uint64_t) ((int64_t) a * (int64_t) b) >> 32);
}

static inline sreg_t rv_mulhsu(const sreg_t a, const uint32_t b) {
	return (sreg_t) ((uint64_t) ((int64_t) a * (int64_t) b) >> 32);
}

static inline reg_t rv_mulhu(const reg_t a, const reg_t b) {
	return (reg_t) (((uint64_t) a * (uint64_t) b) >> 32);
}
#endif /* RV64M */

#endif /* MUL_H */
