#ifndef CPU_H
#define CPU_H

#include <stdint.h>

#include "common.h"
#include "arch.h"
#include "riscv.h"
#include "cpu.h"
#include "perf.h"

typedef struct cpu_struct {
	uint8_t rom[ROM_SIZE];
	uint8_t ram[RAM_SIZE];
	reg_t regfile[XRF_SIZE];
	reg_t pc;
} cpu_t;

typedef struct instr_struct {
	unsigned char rs1;
	unsigned char rs2;
	unsigned char rd;
	union {
		sreg_t imm;
		reg_t uimm;
	} imms;
	bool is_reg_write;
	bool is_mem_load;
} instr_t;

extern cpu_t cpu;

static inline void rom_read(reg_t addr, unsigned char op, reg_t *val) {
	const reg_t paddr = addr - (reg_t) ROM_START;
	switch(op) {
		case(MEM_B):
			*val = (sreg_t) (int8_t) cpu.rom[paddr];
			break;
		case(MEM_BU):
			*val = cpu.rom[paddr];
			break;
		case(MEM_H):
			PERF_MODEL_MEMORY(2, addr);
			*val = (sreg_t) (int16_t) (cpu.rom[paddr + 1] << 8 
											| cpu.rom[paddr]);
			break;
		case(MEM_HU):
			PERF_MODEL_MEMORY(2, addr);
			*val = cpu.rom[paddr + 1] << 8 | cpu.rom[paddr];
			break;
		case(MEM_W):
			PERF_MODEL_MEMORY(4, addr);
			*val = (sreg_t) (int32_t) ((uint32_t) cpu.rom[paddr + 3] << 24 
											| (uint32_t) cpu.rom[paddr + 2] << 16 
											| (uint32_t) cpu.rom[paddr + 1] << 8 
											| (uint32_t) cpu.rom[paddr]);
			break;
#ifdef RV64
		case(MEM_WU):
			PERF_MODEL_MEMORY(4, addr);
			*val = (reg_t) ((uint32_t) cpu.rom[paddr + 3] << 24 
											| (uint32_t) cpu.rom[paddr + 2] << 16 
											| (uint32_t) cpu.rom[paddr + 1] << 8 
											| (uint32_t) cpu.rom[paddr]);
			break;
		case(MEM_D):
			PERF_MODEL_MEMORY(8, addr);
			*val = (sreg_t) (int64_t) ((uint64_t) cpu.rom[paddr + 7] << 56 
											| (uint64_t) cpu.rom[paddr + 6] << 48 
											| (uint64_t) cpu.rom[paddr + 5] << 40 
											| (uint64_t) cpu.rom[paddr + 4] << 32 
											| (uint64_t) cpu.rom[paddr + 3] << 24 
											| (uint64_t) cpu.rom[paddr + 2] << 16 
											| (uint64_t) cpu.rom[paddr + 1] << 8 
											| (uint64_t) cpu.rom[paddr]);
			break;
#endif
		default:
			break;
	}
}

static inline void ram_read(reg_t addr, unsigned char op, reg_t *val) {
	const reg_t paddr = addr - (reg_t) RAM_START;
	switch(op) {
		case(MEM_B):
			*val = (sreg_t) (int8_t) cpu.ram[paddr];
			break;
		case(MEM_BU):
			*val = cpu.ram[paddr];
			break;
		case(MEM_H):
			PERF_MODEL_MEMORY(2, addr);
			*val = (sreg_t) (int16_t) (cpu.ram[paddr + 1] << 8 
											| cpu.ram[paddr]);
			break;
		case(MEM_HU):
			PERF_MODEL_MEMORY(2, addr);
			*val = cpu.ram[paddr + 1] << 8 | cpu.ram[paddr];
			break;
		case(MEM_W):
			PERF_MODEL_MEMORY(4, addr);
			*val = (sreg_t) (int32_t) ((uint32_t) cpu.ram[paddr + 3] << 24 
											| (uint32_t) cpu.ram[paddr + 2] << 16 
											| (uint32_t) cpu.ram[paddr + 1] << 8 
											| (uint32_t) cpu.ram[paddr]);
			break;
#ifdef RV64
		case(MEM_WU):
			PERF_MODEL_MEMORY(4, addr);
			*val = (reg_t) ((uint32_t) cpu.ram[paddr + 3] << 24 
											| (uint32_t) cpu.ram[paddr + 2] << 16 
											| (uint32_t) cpu.ram[paddr + 1] << 8 
											| (uint32_t) cpu.ram[paddr]);
			break;
		case(MEM_D):
			PERF_MODEL_MEMORY(8, addr);
			*val = (sreg_t) (int64_t) ((uint64_t) cpu.ram[paddr + 7] << 56 
											| (uint64_t) cpu.ram[paddr + 6] << 48 
											| (uint64_t) cpu.ram[paddr + 5] << 40 
											| (uint64_t) cpu.ram[paddr + 4] << 32 
											| (uint64_t) cpu.ram[paddr + 3] << 24 
											| (uint64_t) cpu.ram[paddr + 2] << 16 
											| (uint64_t) cpu.ram[paddr + 1] << 8 
											| (uint64_t) cpu.ram[paddr]);
			break;
#endif
		default:
			break;
	}
}

static inline void ram_write(reg_t addr, unsigned char op, reg_t val) {
	const reg_t paddr = addr - (reg_t) RAM_START;
	switch(op) {
		case(MEM_B):
			cpu.ram[paddr] = (uint8_t) val;
			break;
		case(MEM_H):
			PERF_MODEL_MEMORY(2, addr);
			cpu.ram[paddr] = (uint8_t) val;
			cpu.ram[paddr + 1] = (uint8_t) (val >> 8);
			break;
		case(MEM_W):
			PERF_MODEL_MEMORY(4, addr);
			cpu.ram[paddr] = (uint8_t) val;
			cpu.ram[paddr + 1] = (uint8_t) (val >> 8);
			cpu.ram[paddr + 2] = (uint8_t) (val >> 16);
			cpu.ram[paddr + 3] = (uint8_t) (val >> 24);
			break;
#ifdef RV64
		case(MEM_D):
			PERF_MODEL_MEMORY(8, addr);
			cpu.ram[paddr] = (uint8_t) val;
			cpu.ram[paddr + 1] = (uint8_t) (val >> 8);
			cpu.ram[paddr + 2] = (uint8_t) (val >> 16);
			cpu.ram[paddr + 3] = (uint8_t) (val >> 24);
			cpu.ram[paddr + 4] = (uint8_t) (val >> 32);
			cpu.ram[paddr + 5] = (uint8_t) (val >> 40);
			cpu.ram[paddr + 6] = (uint8_t) (val >> 48);
			cpu.ram[paddr + 7] = (uint8_t) (val >> 56);
			break;
#endif
		default:
			/* do something on invalid op */
			break;
	}
}

#endif /* CPU_H */