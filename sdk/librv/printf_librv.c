#include "printf.h"

void putchar_(char c) {
	asm volatile (
        "li a0, 11\n\t"
        "mv a1, %0\n\t"
        "ecall"
        :
        : "r" (c)
        : "a0", "a1"
    );
}
