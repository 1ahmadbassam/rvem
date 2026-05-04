#include <stdio.h>

#define FIB_NUM 50
#if FIB_NUM < 2
#error("Must compute at least 1 fibonacci number")
#endif

static unsigned long long fib_array[FIB_NUM+1];

unsigned long long fib_mem(unsigned long long a) {
  if (a <= 1) {
    fib_array[a] = a;
    return fib_array[a];
  }

  if (fib_array[a] == 0)
    fib_array[a] = fib_mem(a - 1) + fib_mem(a - 2);

  return fib_array[a];
}

int main(void) {
  int i;
  fib_array[0] = (unsigned long long) 0;
  fib_array[1] = (unsigned long long) 1;
  fib_mem((unsigned long long) FIB_NUM);

  printf("Fibonacci Sequence up to %d:\n", FIB_NUM);
  printf("-------------------------------\n");

  for (i = 0; i <= FIB_NUM; i++) {
    printf("Fib(%2d) = %llu\n", i, fib_array[i]);
  }

  return 0;
}
