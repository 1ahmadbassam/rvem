volatile unsigned int global_result = 0;

int main() {
    unsigned int state = 12345;
    const unsigned int ITERATIONS = 10000000;

    for (unsigned int i = 0; i < ITERATIONS; i++) {
        state = (state * 1103515245) + 12345;
    }

    global_result = state;

    return 0;
}
