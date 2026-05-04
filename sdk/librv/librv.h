#ifndef LIBRV_H
#define LIBRV_H

#include <stdint.h>

// ============================================================================
// PERFORMANCE COUNTERS
// ============================================================================

inline uint64_t query_performance_counter(void) {
	uint32_t hi, lo, hi2;

    __asm__ volatile (
        "1:\n"
        "csrr %0, mcycleh\n"
        "csrr %1, mcycle\n"
        "csrr %2, mcycleh\n"
        "bne  %0, %2, 1b"
        : "=r" (hi), "=r" (lo), "=r" (hi2)
    );

    return ((uint64_t)hi << 32) | lo;
}

inline uint64_t query_instruction_counter(void) {
	uint32_t hi, lo, hi2;

    __asm__ volatile (
        "1:\n"
        "csrr %0, minstreth\n"
        "csrr %1, minstret\n"
        "csrr %2, minstreth\n"
        "bne  %0, %2, 1b"
        : "=r" (hi), "=r" (lo), "=r" (hi2)
    );

    return ((uint64_t)hi << 32) | lo;
}

// ============================================================================
// C LIBRARY
// ============================================================================

// ========================= MEMORY ===========================================
#define RAM_OFFSET 0x20000000

#endif /* LIBRV_H */
