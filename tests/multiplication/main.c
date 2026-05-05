#include <stdio.h>
#include <stdint.h>

#define ABS(x) ((x) < 0 ? -(x) : (x))

static volatile int32_t mem[14];

int main(void) {
    int32_t a = 5, b = 13, c, d = 20, e = -2;

    asm volatile ("mul %0, %1, %2" : "=r"(c) : "r"(a), "r"(b));
    mem[0] = c;
    printf("mem[0]  (5 * 13)         = %d\n", mem[0]);

    asm volatile ("div %0, %1, %2" : "=r"(c) : "r"(ABS(a)), "r"(ABS(b)));
    mem[1] = c;
    printf("mem[1]  (|5| / |13|)     = %d\n", mem[1]);

    asm volatile ("div %0, %1, %2" : "=r"(c) : "r"(d), "r"(e));
    mem[2] = c;
    printf("mem[2]  (20 / -2)        = %d\n", mem[2]);

    asm volatile ("rem %0, %1, %2" : "=r"(c) : "r"(e), "r"(d));
    mem[3] = c;
    printf("mem[3]  (-2 %% 20)        = %d\n", mem[3]);

    asm volatile ("rem %0, %1, %2" : "=r"(c) : "r"(ABS(a)), "r"(ABS(b)));
    mem[4] = c;
    printf("mem[4]  (|5| %% |13|)     = %d\n", mem[4]);

    e = 0;

    asm volatile ("div %0, %1, %2" : "=r"(c) : "r"(d), "r"(e));
    mem[5] = c;
    printf("mem[5]  (20 / 0)         = %d\n", mem[5]);

    asm volatile ("div %0, %1, %2" : "=r"(c) : "r"(ABS(d)), "r"(ABS(e)));
    mem[6] = c;
    printf("mem[6]  (|20| / |0|)     = %d\n", mem[6]);

    asm volatile ("rem %0, %1, %2" : "=r"(c) : "r"(d), "r"(e));
    mem[7] = c;
    printf("mem[7]  (20 %% 0)         = %d\n", mem[7]);

    asm volatile ("rem %0, %1, %2" : "=r"(c) : "r"(ABS(d)), "r"(ABS(e)));
    mem[8] = c;
    printf("mem[8]  (|20| %% |0|)     = %d\n", mem[8]);

    d = 0x80000000; // INT_MIN
    e = -1;

    asm volatile ("div %0, %1, %2" : "=r"(c) : "r"(d), "r"(e));
    mem[9] = c;
    printf("mem[9]  (INT_MIN / -1)   = %d (0x%X)\n", mem[9], mem[9]);

    asm volatile ("rem %0, %1, %2" : "=r"(c) : "r"(d), "r"(e));
    mem[10] = c;
    printf("mem[10] (INT_MIN %% -1)   = %d\n", mem[10]);

    a = 500000;
    b = 600000;

    asm volatile ("mulh %0, %1, %2" : "=r"(c) : "r"(a), "r"(b));
    mem[11] = c;
    printf("mem[11] (MULH: a*b >> 32) = %d\n", mem[11]);

    uint32_t ua = 0xFFFFFFFF;
    uint32_t ub = 0xFFFFFFFF;

    asm volatile ("mulhu %0, %1, %2" : "=r"(c) : "r"(ua), "r"(ub));
    mem[12] = c;
    printf("mem[12] (MULHU: ua*ub >>32)= %u (0x%X)\n", (uint32_t)mem[12], (uint32_t)mem[12]);

    asm volatile ("mulhsu %0, %1, %2" : "=r"(c) : "r"(a), "r"(ub));
    mem[13] = c;
    printf("mem[13] (MULHSU: a*ub >>32)= %d\n", mem[13]);

    return 0;
}
