#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "common.h"
#include "riscv.h"
#include "cpu.h"
#include "bus.h"
#include "arch.h"
#include "csr.h"

static void print_usage(const char *path) {
	printf("usage: %s <binary rom>\n", path);
}

static bool load_rom_binary(const char *filename) {
    FILE *f = fopen(filename, "rb");
	size_t bytes_read;
    if (!f) {
        printf("[err] failed to open %s.\n", filename);
        return false;
    }
    bytes_read = fread(cpu.rom, 1, ROM_SIZE, f);
    printf("[info] loaded %u bytes into ROM.\n", (unsigned) bytes_read);
    fclose(f);
    return true;
}

int main(int argc, const char *argv[]) {
	bool instr_loop = true;
	double elapsed;
	struct timespec start, end;

	if (argc != 2) {
		print_usage(argv[0]);
		return 1;
	}

	if (!load_rom_binary(argv[1]))
		return 1;

    clock_gettime(CLOCK_MONOTONIC, &start);
	while (instr_loop && mcycle < MAX_CYCLES) {
		reg_t bus_val = 0;
		uint32_t instr;
		bus_read(cpu.pc, MEM_W, &bus_val);
		instr = (uint32_t) bus_val;
		if (instr == 0)
			return 1;
		instr_loop = process_instr(instr);
	}
	clock_gettime(CLOCK_MONOTONIC, &end);
	elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;
	if (mcycle < MAX_CYCLES) {
		printf("[info] total cycles: %llu.\n", mcycle);
		printf("[info] frequency: %.2lfMHz.\n", mcycle/(elapsed*1000000));
	} else {
		printf("[info] program hit hard cycle limit without exiting.\n");
	}
	return 0;
}
