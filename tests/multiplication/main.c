#include <stdio.h>
#include <stdint.h>

#define ABS(x) ((x) < 0 ? -(x) : (x))

static volatile int32_t mem[14];

int main(void) {
    int32_t a = 5, b = 13, c, d = 20, e = -2;

    c = a * b;
    mem[0] = c;
    printf("mem[0]  (5 * 13)         = %d\n", mem[0]);

    c = ABS(a) / ABS(b);
    mem[1] = c;
    printf("mem[1]  (|5| / |13|)     = %d\n", mem[1]);

    c = d / e;
    mem[2] = c;
    printf("mem[2]  (20 / -2)        = %d\n", mem[2]);

    c = e % d;
    mem[3] = c;
    printf("mem[3]  (-2 %% 20)        = %d\n", mem[3]);

    c = ABS(a) % ABS(b);
    mem[4] = c;
    printf("mem[4]  (|5| %% |13|)     = %d\n", mem[4]);

    e = 0;

    c = d / e;
    mem[5] = c;
    printf("mem[5]  (20 / 0)         = %d\n", mem[5]);

    c = ABS(d) / ABS(e);
    mem[6] = c;
    printf("mem[6]  (|20| / |0|)     = %d\n", mem[6]);

    c = d % e;
    mem[7] = c;
    printf("mem[7]  (20 %% 0)         = %d\n", mem[7]);

    c = ABS(d) % ABS(e);
    mem[8] = c;
    printf("mem[8]  (|20| %% |0|)     = %d\n", mem[8]);

    d = 0x80000000; // INT_MIN
    e = -1;

    c = d / e;
    mem[9] = c;
    printf("mem[9]  (INT_MIN / -1)   = %d (0x%X)\n", mem[9], mem[9]);

    c = d % e;
    mem[10] = c;
    printf("mem[10] (INT_MIN %% -1)   = %d\n", mem[10]);

    a = 500000;
    b = 600000;

    c = ((int64_t)a * b) >> 32;
    mem[11] = c;
    printf("mem[11] (MULH: a*b >> 32) = %d\n", mem[11]);

    uint32_t ua = 0xFFFFFFFF;
    uint32_t ub = 0xFFFFFFFF;

    c = ((uint64_t)ua * ub) >> 32;
    mem[12] = c;
    printf("mem[12] (MULHU: ua*ub >>32)= %u (0x%X)\n", (uint32_t)mem[12], (uint32_t)mem[12]);

    c = ((int64_t)a * (uint64_t)ub) >> 32;
    mem[13] = c;
    printf("mem[13] (MULHSU: a*ub >>32)= %d\n", mem[13]);

    return 0;
}
