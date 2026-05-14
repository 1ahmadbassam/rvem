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
	clock_t start, end;

	if (argc != 2) {
		print_usage(argv[0]);
		return 1;
	}

	if (!load_rom_binary(argv[1]))
		return 1;

    start = clock();
	while (instr_loop && mcycle < MAX_CYCLES) {
		reg_t bus_val = 0;
		uint32_t instr;
		bus_read(cpu.pc, MEM_W, &bus_val);
		instr = (uint32_t) bus_val;
		if (instr == 0)
			return 1;
		instr_loop = process_instr(instr);
	}
	end = clock();
	elapsed = (double)(end - start) / CLOCKS_PER_SEC;
	if (mcycle < MAX_CYCLES) {
		printf("[info] total cycles: "FMT_U64".\n", mcycle);
		if (elapsed > 0)
			printf("[info] frequency: %.2lf MHz.\n", UINT64_DOUBLE_DIV(mcycle, elapsed) / 1000000.0);
		else
			printf("[info] frequency: inf MHz.\n");
	} else {
		printf("[info] program hit hard cycle limit without exiting.\n");
	}
	return 0;
}
