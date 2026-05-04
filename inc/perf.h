#ifndef PERF_H
#define PERF_H

#include "common.h"
#include "arch.h"
#include "csr.h"

typedef struct perf_model_struct {
	unsigned char prev_rd;
	unsigned char prev2_rd;
	bool prev_mem_load;
	bool prev2_mem_load;
	bool prev_reg_write;
} perf_model_t;

#define PERF_MODEL_INIT(model)										 MULTI_LINE_MACRO_BEGIN \
	model.prev_rd = 0;								  										\
	model.prev2_rd = 0;								  										\
	model.prev_mem_load = false;					  										\
	model.prev2_mem_load = false;					  										\
	model.prev_reg_write = false;					  										\
MULTI_LINE_MACRO_END

#define PERF_MODEL_CYCLE(perf_model, instr)  						 MULTI_LINE_MACRO_BEGIN \
	mcycle++;																				\
	minstret++;																				\
	perf_model.prev2_mem_load = perf_model.prev_mem_load;									\
	perf_model.prev_mem_load = instr.is_mem_load;											\
	perf_model.prev2_rd = perf_model.prev_rd;												\
	perf_model.prev_rd = instr.rd;															\
	perf_model.prev_reg_write = instr.is_reg_write;											\
MULTI_LINE_MACRO_END

#define PERF_MODEL_HAZARD(perf_model, instr)						 MULTI_LINE_MACRO_BEGIN \
	/* stall on data hazard */																\
	if (perf_model.prev_mem_load 															\
	&& perf_model.prev_rd != 0 																\
	&& (instr.rs1 == perf_model.prev_rd || instr.rs2 == perf_model.prev_rd)) 				\
		mcycle++;																			\
MULTI_LINE_MACRO_END

#define PERF_MODEL_BRANCH_HAZARD(perf_model, instr)					 MULTI_LINE_MACRO_BEGIN \
	/* stall on branch hazard */															\
	if (perf_model.prev_rd != 0																\
	&& perf_model.prev_reg_write 															\
	&& (instr.rs1 == perf_model.prev_rd || instr.rs2 == perf_model.prev_rd))				\
		mcycle++;  																			\
	/* memory-branch special */																\
	if (perf_model.prev2_rd != 0															\
	&& perf_model.prev2_mem_load															\
	&& (instr.rs1 == perf_model.prev2_rd || instr.rs2 == perf_model.prev2_rd)) 				\
		mcycle++;																			\
	/* stall on data hazard */																\
	if (perf_model.prev_mem_load															\
	&& perf_model.prev_rd != 0 																\
	&& (instr.rs1 == perf_model.prev_rd || instr.rs2 == perf_model.prev_rd)) 				\
		mcycle++;																			\
MULTI_LINE_MACRO_END

#define PERF_MODEL_BRANCH_TAKEN(perf_model, instr)					 MULTI_LINE_MACRO_BEGIN \
	mcycle++; /* flush pipeline for ID stage resolution */									\
MULTI_LINE_MACRO_END

#define PERF_MODEL_MEMORY(size, addr)								 MULTI_LINE_MACRO_BEGIN \
	/* stall on unaligned memory access */													\
	if ((addr & (size - 1)) != 0) 															\
		mcycle++;																			\
MULTI_LINE_MACRO_END

extern perf_model_t perf_model;

#endif /* PERF_H */