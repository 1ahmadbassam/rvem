#ifndef BUS_H
#define BUS_H

#include <stdio.h>

#include "common.h"
#include "riscv.h"
#include "cpu.h"
#include "arch.h"

static inline void bus_read(reg_t addr, unsigned char op, reg_t *val) {
	const unsigned char range = ((op == MEM_H || op == MEM_HU) ? 1
							   : (op == MEM_W) ? 3
#ifdef RV64
							   : (op == MEM_WU) ? 3
							   : (op == MEM_D) ? 7
#endif
							   : 0);

	if (addr >= RAM_START && addr < (reg_t) (RAM_START + RAM_SIZE - range))
		ram_read(addr, op, val);
	else if (addr < (reg_t) (ROM_SIZE - range))
		rom_read(addr, op, val);
	else
		printf("[warn] unmapped bus read 0x"HEX_FMT"; ignored.", addr);
}

static inline void bus_write(reg_t addr, unsigned char op, reg_t val) {
	const unsigned char range = ((op == MEM_H || op == MEM_HU) ? 1
							   : (op == MEM_W) ? 3
#ifdef RV64
							   : (op == MEM_WU) ? 3
							   : (op == MEM_D) ? 7
#endif
							   : 0);

	if (addr >= RAM_START && addr < (reg_t) (RAM_START + RAM_SIZE - range))
		ram_write(addr, op, val);
	else if (addr < (reg_t) (ROM_SIZE - range))
		printf("[warn] rom write at 0x"HEX_FMT"; ignored.", addr);
	else
		printf("[warn] unmapped bus write 0x"HEX_FMT"; ignored.", addr);
}

#endif /* BUS_H */
