#include <stdint.h>
#include <stdio.h>

#include "common.h"
#include "riscv.h"
#include "cpu.h"
#include "arch.h"
#include "perf.h"
#include "csr.h"
#include "bus.h"
#if defined(RV32M) || defined(RV64M)
#include "mul.h"
#endif /* defined(RV32M) || defined(RV64M) */

cpu_t cpu;
perf_model_t perf_model;

bool process_instr(const uint32_t raw_instr) {
	bool exec_continue = true;
	reg_t next_pc = cpu.pc + (reg_t) 4;

	const unsigned char opcode = GET_OPCODE(raw_instr);
	instr_t instr;

	instr.rs1 = 0;
	instr.rs2 = 0;
	instr.rd = 0;
	instr.is_reg_write = false;
	instr.is_mem_load = false;

	switch(opcode) {
		case OP_RTYPE: {
			const unsigned char funct3 = GET_FUNCT3(raw_instr);
			instr.rs1 = GET_RS1(raw_instr);
			instr.rs2 = GET_RS2(raw_instr);
			instr.rd = GET_RD(raw_instr);
			instr.is_reg_write = true;
			PERF_MODEL_HAZARD(perf_model, instr);
#if defined(RV32M) || defined(RV64M)
			if (CHECK_FUNCT7_BIT0(raw_instr)) {
				switch(funct3) {
					case M_MUL:
						cpu.regfile[instr.rd] = (sreg_t) rv_mul(cpu.regfile[instr.rs1], cpu.regfile[instr.rs2]);
						break;
					case M_MULH:
						cpu.regfile[instr.rd] = rv_mulh(cpu.regfile[instr.rs1], cpu.regfile[instr.rs2]);
						break;
					case M_MULHSU:
						cpu.regfile[instr.rd] = rv_mulhsu(cpu.regfile[instr.rs1], cpu.regfile[instr.rs2]);
						break;
					case M_MULHU:
						cpu.regfile[instr.rd] = rv_mulhu(cpu.regfile[instr.rs1], cpu.regfile[instr.rs2]);
						break;
					case M_DIV:
						cpu.regfile[instr.rd] = rv_div((sreg_t) cpu.regfile[instr.rs1], (sreg_t) cpu.regfile[instr.rs2]);
						break;
					case M_DIVU:
						cpu.regfile[instr.rd] = rv_divu(cpu.regfile[instr.rs1], cpu.regfile[instr.rs2]);
						break;
					case M_REM:
						cpu.regfile[instr.rd] = rv_rem((sreg_t) cpu.regfile[instr.rs1], (sreg_t) cpu.regfile[instr.rs2]);
						break;
					case M_REMU:
						cpu.regfile[instr.rd] = rv_remu(cpu.regfile[instr.rs1], cpu.regfile[instr.rs2]);
						break;
					default:
						break;
				}
			} else {
#else /* defined(RV32M) || defined(RV64M) */
			{
#endif /* defined(RV32M) || defined(RV64M) */
				const unsigned char aluop = ALU_FUNCT7_BIT5(raw_instr) | funct3;
				switch(aluop) {
					case ALU_ADD:
						cpu.regfile[instr.rd] = (sreg_t) cpu.regfile[instr.rs1] + (sreg_t) cpu.regfile[instr.rs2];
						break;
					case ALU_SUB:
						cpu.regfile[instr.rd] = (sreg_t) cpu.regfile[instr.rs1] - (sreg_t) cpu.regfile[instr.rs2];
						break;
					case ALU_SLL:
						cpu.regfile[instr.rd] = cpu.regfile[instr.rs1] << (cpu.regfile[instr.rs2] & SHIFT_MAX);
						break;
					case ALU_SRL:
						cpu.regfile[instr.rd] = cpu.regfile[instr.rs1] >> (cpu.regfile[instr.rs2] & SHIFT_MAX);
						break;
					case ALU_SRA:
						cpu.regfile[instr.rd] = SRA(cpu.regfile[instr.rs1], cpu.regfile[instr.rs2] & SHIFT_MAX);
						break;
					case ALU_SLT:
						cpu.regfile[instr.rd] = (sreg_t) cpu.regfile[instr.rs1] < (sreg_t) cpu.regfile[instr.rs2];
						break;
					case ALU_SLTU:
						cpu.regfile[instr.rd] = cpu.regfile[instr.rs1] < cpu.regfile[instr.rs2];
						break;
					case ALU_AND:
						cpu.regfile[instr.rd] = cpu.regfile[instr.rs1] & cpu.regfile[instr.rs2];
						break;
					case ALU_OR:
						cpu.regfile[instr.rd] = cpu.regfile[instr.rs1] | cpu.regfile[instr.rs2];
						break;
					case ALU_XOR:
						cpu.regfile[instr.rd] = cpu.regfile[instr.rs1] ^ cpu.regfile[instr.rs2];
						break;
					default:
						break;
				}
			}
			break;
		}
		case OP_IMM: {
			const unsigned char aluop = GET_FUNCT3(raw_instr);
			instr.rs1 = GET_RS1(raw_instr);
			instr.rd = GET_RD(raw_instr);
			instr.is_reg_write = true;
			instr.imms.imm = GET_SIMM12(raw_instr);
			PERF_MODEL_HAZARD(perf_model, instr);
			switch(aluop) {
				case ALU_ADD:
					cpu.regfile[instr.rd] = (sreg_t) cpu.regfile[instr.rs1] + instr.imms.imm;
					break;
				case ALU_SLL:
					cpu.regfile[instr.rd] = cpu.regfile[instr.rs1] << (instr.imms.imm & SHIFT_MAX);
					break;
				case ALU_SRL:
					if (CHECK_FUNCT7_BIT5(raw_instr))	/* SRA */
						cpu.regfile[instr.rd] = SRA(cpu.regfile[instr.rs1], instr.imms.imm & SHIFT_MAX);
					else
						cpu.regfile[instr.rd] = cpu.regfile[instr.rs1] >> (instr.imms.imm & SHIFT_MAX);
					break;
				case ALU_SLT:
					cpu.regfile[instr.rd] = (sreg_t) cpu.regfile[instr.rs1] < instr.imms.imm;
					break;
				case ALU_SLTU:
					cpu.regfile[instr.rd] = cpu.regfile[instr.rs1] < (reg_t) instr.imms.imm;
					break;
				case ALU_AND:
					cpu.regfile[instr.rd] = cpu.regfile[instr.rs1] & instr.imms.imm;
					break;
				case ALU_OR:
					cpu.regfile[instr.rd] = cpu.regfile[instr.rs1] | instr.imms.imm;
					break;
				case ALU_XOR:
					cpu.regfile[instr.rd] = cpu.regfile[instr.rs1] ^ instr.imms.imm;
					break;
				default:
					break;
			}
			break;
		}
#ifdef RV64
		case OP_RTYPEW: {
			const unsigned char funct3 = GET_FUNCT3(raw_instr);
			instr.rs1 = GET_RS1(raw_instr);
			instr.rs2 = GET_RS2(raw_instr);
			instr.rd = GET_RD(raw_instr);
			instr.is_reg_write = true;
			PERF_MODEL_HAZARD(perf_model, instr);
#ifdef RV64M
			if (CHECK_FUNCT7_BIT0(raw_instr)) {
				switch(funct3) {
					case M_MUL:
						cpu.regfile[instr.rd] = (sreg_t) (int32_t) rv_mulw((uint32_t) cpu.regfile[instr.rs1], (uint32_t) cpu.regfile[instr.rs2]);
						break;
					case M_DIV:
						cpu.regfile[instr.rd] = (sreg_t) rv_divw((int32_t) cpu.regfile[instr.rs1], (int32_t) cpu.regfile[instr.rs2]);
						break;
					case M_DIVU:
						cpu.regfile[instr.rd] = (sreg_t) (int32_t) rv_divuw((uint32_t) cpu.regfile[instr.rs1], (uint32_t) cpu.regfile[instr.rs2]);
						break;
					case M_REM:
						cpu.regfile[instr.rd] = (sreg_t) rv_remw((int32_t) cpu.regfile[instr.rs1], (int32_t) cpu.regfile[instr.rs2]);
						break;
					case M_REMU:
						cpu.regfile[instr.rd] = (sreg_t) (int32_t) rv_remuw((uint32_t) cpu.regfile[instr.rs1], (uint32_t) cpu.regfile[instr.rs2]);
						break;
					default:
						break;
				}
			} else {
#else /* RV64M */
			{
#endif /* RV64M */
				const unsigned char aluop = ALU_FUNCT7_BIT5(raw_instr) | funct3;
				switch(aluop) {
					case ALU_ADD:
						cpu.regfile[instr.rd] = (sreg_t) ((int32_t) cpu.regfile[instr.rs1] + (int32_t) cpu.regfile[instr.rs2]);
						break;
					case ALU_SUB:
						cpu.regfile[instr.rd] = (sreg_t) ((int32_t) cpu.regfile[instr.rs1] - (int32_t) cpu.regfile[instr.rs2]);
						break;
					case ALU_SLL:
						cpu.regfile[instr.rd] = (sreg_t) (int32_t) ((uint32_t) cpu.regfile[instr.rs1] << (cpu.regfile[instr.rs2] & SHIFT_MAX_WORD));
						break;
					case ALU_SRL:
						cpu.regfile[instr.rd] = (sreg_t) (int32_t) ((uint32_t) cpu.regfile[instr.rs1] >> (cpu.regfile[instr.rs2] & SHIFT_MAX_WORD));
						break;
					case ALU_SRA:
						cpu.regfile[instr.rd] = (sreg_t) SRAW((uint32_t) cpu.regfile[instr.rs1], cpu.regfile[instr.rs2] & SHIFT_MAX_WORD);
						break;
					default:
						break;
				}
			}
			break;
		}
		case OP_IMMW: {
			const unsigned char aluop = GET_FUNCT3(raw_instr);
			instr.rs1 = GET_RS1(raw_instr);
			instr.rd = GET_RD(raw_instr);
			instr.is_reg_write = true;
			instr.imms.imm = GET_SIMM12(raw_instr);
			PERF_MODEL_HAZARD(perf_model, instr);
			switch(aluop) {
				case ALU_ADD:
					cpu.regfile[instr.rd] = (sreg_t) ((int32_t) cpu.regfile[instr.rs1] + (int32_t) instr.imms.imm);
					break;
				case ALU_SLL:
					cpu.regfile[instr.rd] = (sreg_t) (int32_t) ((uint32_t) cpu.regfile[instr.rs1] << (instr.imms.imm & SHIFT_MAX_WORD));
					break;
				case ALU_SRL:
					if (CHECK_FUNCT7_BIT5(raw_instr))	/* SRA */
						cpu.regfile[instr.rd] = (sreg_t) SRAW((uint32_t) cpu.regfile[instr.rs1], instr.imms.imm & SHIFT_MAX_WORD);
					else
						cpu.regfile[instr.rd] = (sreg_t) (int32_t) ((uint32_t) cpu.regfile[instr.rs1] >> (instr.imms.imm & SHIFT_MAX_WORD));
					break;
				default:
					break;
			}
			break;
		}
#endif
		case OP_LOAD: {
			instr.rs1 = GET_RS1(raw_instr);
			instr.rd = GET_RD(raw_instr);
			instr.is_reg_write = true;
			instr.is_mem_load = true;
			instr.imms.imm = GET_SIMM12(raw_instr);
			PERF_MODEL_HAZARD(perf_model, instr);
			bus_read(cpu.regfile[instr.rs1] + instr.imms.imm, GET_FUNCT3(raw_instr), &cpu.regfile[instr.rd]);
			break;
		}
		case OP_STORE: {
			instr.rs1 = GET_RS1(raw_instr);
			instr.rs2 = GET_RS2(raw_instr);
			instr.imms.imm = GET_STORE_IMM(raw_instr);
			PERF_MODEL_HAZARD(perf_model, instr);
			bus_write(cpu.regfile[instr.rs1] + instr.imms.imm, GET_FUNCT3(raw_instr), cpu.regfile[instr.rs2]);
			break;
		}
		case OP_BRANCH: {
			const unsigned char type = GET_FUNCT3(raw_instr);
			instr.rs1 = GET_RS1(raw_instr);
			instr.rs2 = GET_RS2(raw_instr);
			instr.imms.imm = GET_BRANCH_IMM(raw_instr);
			PERF_MODEL_BRANCH_HAZARD(perf_model, instr);
			switch(type) {
				case(BRANCH_EQ):
					if (cpu.regfile[instr.rs1] == cpu.regfile[instr.rs2]) {
						PERF_MODEL_BRANCH_TAKEN(perf_model, instr);
						next_pc = cpu.pc + instr.imms.imm;
					}
					break;
				case(BRANCH_NE):
					if (cpu.regfile[instr.rs1] != cpu.regfile[instr.rs2]) {
						PERF_MODEL_BRANCH_TAKEN(perf_model, instr);
						next_pc = cpu.pc + instr.imms.imm;
					}
					break;
				case(BRANCH_LT):
					if ((sreg_t) cpu.regfile[instr.rs1] < (sreg_t) cpu.regfile[instr.rs2]) {
						PERF_MODEL_BRANCH_TAKEN(perf_model, instr);
						next_pc = cpu.pc + instr.imms.imm;
					}
					break;
				case(BRANCH_GE):
					if ((sreg_t) cpu.regfile[instr.rs1] >= (sreg_t) cpu.regfile[instr.rs2]) {
						PERF_MODEL_BRANCH_TAKEN(perf_model, instr);
						next_pc = cpu.pc + instr.imms.imm;
					}
					break;
				case(BRANCH_LTU):
					if (cpu.regfile[instr.rs1] < cpu.regfile[instr.rs2]) {
						PERF_MODEL_BRANCH_TAKEN(perf_model, instr);
						next_pc = cpu.pc + instr.imms.imm;
					}
					break;
				case(BRANCH_GEU):
					if (cpu.regfile[instr.rs1] >= cpu.regfile[instr.rs2]) {
						PERF_MODEL_BRANCH_TAKEN(perf_model, instr);
						next_pc = cpu.pc + instr.imms.imm;
					}
					break;
				default:
					break;
			}
			break;
		}
		case OP_FENCE:
			/* used for multicore, ignored for our single core processor */
			break;
		case OP_AUIPC: {
			instr.rd = GET_RD(raw_instr);
			instr.is_reg_write = true;
			instr.imms.imm = GET_SIMM20(raw_instr);
			cpu.regfile[instr.rd] = cpu.pc + instr.imms.imm;
			break;
		}
		case OP_LUI: {
			instr.rd = GET_RD(raw_instr);
			instr.is_reg_write = true;
			instr.imms.imm = GET_SIMM20(raw_instr);
			cpu.regfile[instr.rd] = instr.imms.imm;
			break;
		}
		case OP_JAL: {
			instr.rd = GET_RD(raw_instr);
			instr.is_reg_write = true;
			instr.imms.imm = GET_JAL_IMM(raw_instr);
			PERF_MODEL_BRANCH_TAKEN(perf_model, instr);
		    next_pc = cpu.pc + instr.imms.imm;
		    cpu.regfile[instr.rd] = cpu.pc + 4;
			break;
		}
		case OP_JALR: {
			instr.rs1 = GET_RS1(raw_instr);
			instr.rd = GET_RD(raw_instr);
			instr.is_reg_write = true;
			instr.imms.imm = GET_SIMM12(raw_instr);
			PERF_MODEL_BRANCH_HAZARD(perf_model, instr);
			PERF_MODEL_BRANCH_TAKEN(perf_model, instr);
			next_pc = (cpu.regfile[instr.rs1] + instr.imms.imm) & ~((reg_t) 1);
			cpu.regfile[instr.rd] = cpu.pc + 4;
			break;
		}
		case OP_SYS: {
			const unsigned char op = GET_FUNCT3(raw_instr);
			const bool is_imm = CSR_IMM(op);
			instr.rs1 = GET_RS1(raw_instr); /* or uimm */
			instr.rd = GET_RD(raw_instr);
			instr.imms.uimm = GET_IMM12(raw_instr);
			if (!is_imm)
				PERF_MODEL_HAZARD(perf_model, instr);
			switch(op) {
				case(ESYS):
					if (EBREAK(instr.imms.uimm)) {
						printf("[err] ebreak called. debug breakpoint reached.\n");
						exec_continue=false;
					} else {
						if(cpu.regfile[A0] == E_EXIT) {
							printf("[info] program exited gracefully (ecall with a0=10).\n");
							exec_continue = false;
						} else if (cpu.regfile[A0] == E_CHAR) {
							/* simple way to print for now */
							putchar((int) cpu.regfile[A1]);
						}
					}
					break;
				case(CSR_RW):
				case(CSR_RWI): {
					const reg_t target = is_imm ? instr.rs1 : cpu.regfile[instr.rs1];
					if (instr.rd != 0)
						cpu.regfile[instr.rd] = read_csr(instr.imms.uimm);
					write_csr(instr.imms.uimm, target);
					break;
				}
				case(CSR_RS):
				case(CSR_RSI): {
					const reg_t target = is_imm ? instr.rs1 : cpu.regfile[instr.rs1];
					const reg_t csrval = read_csr(instr.imms.uimm);
					cpu.regfile[instr.rd] = csrval;
					if (instr.rs1 != 0)
						write_csr(instr.imms.uimm, csrval | target);
					break;
				}
				case(CSR_RC):
				case(CSR_RCI): {
					const reg_t target = is_imm ? instr.rs1 : cpu.regfile[instr.rs1];
					const reg_t csrval = read_csr(instr.imms.uimm);
					cpu.regfile[instr.rd] = csrval;
					if (instr.rs1 != 0)
						write_csr(instr.imms.uimm, csrval & ~target);
					break;
				}
			}
			break;
		}
		default:
			printf("[warn] unrecognized opcode %u.\n", (unsigned) opcode);
			break;
	}

	PERF_MODEL_CYCLE(perf_model, instr);
	/* state */
	cpu.pc = next_pc;
	cpu.regfile[0] = 0;
	return exec_continue;
}
