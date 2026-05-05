#include <errno.h>

#undef errno
static int errno;

int *__errno(void) {
    return &errno;
}
