#ifndef RISCV_H
#define RISCV_H

#include <stdint.h>

#include "common.h"
#include "arch.h"

#define OP_RTYPE 		0x33
#define OP_IMM 			0x13
#ifdef RV64
#define OP_RTYPEW		0x3B
#define OP_IMMW			0x1B
#endif /* RV64 */
#define OP_LOAD 		0x03
#define OP_STORE 		0x23
#define OP_BRANCH 		0x63
#define OP_FENCE 		0x0F
#define OP_AUIPC 		0x17
#define OP_LUI 			0x37
#define OP_JAL 			0x6F
#define OP_JALR 		0x67
#define OP_SYS 			0x73

#define ALU_ADD			0x0
#define ALU_SUB			0x8
#define ALU_SLL			0x1
#define ALU_SLT			0x2
#define ALU_SLTU		0x3
#define ALU_XOR			0x4
#define ALU_SRL			0x5
#define ALU_SRA			0xD
#define ALU_OR			0x6
#define ALU_AND			0x7

#if defined(RV32M) || defined(RV64M)
#define M_MUL			0x0
#define M_MULH			0x1
#define M_MULHSU		0x2
#define M_MULHU			0x3
#define M_DIV			0x4
#define M_DIVU			0x5
#define M_REM			0x6
#define M_REMU			0x7
#endif /* defined(RV32M) || defined(RV64M) */

#define MEM_B			0x0
#define MEM_H			0x1
#define MEM_W			0x2
#define MEM_D			0x3
#define MEM_BU			0x4
#define MEM_HU			0x5
#define MEM_WU			0x6

#define BRANCH_EQ		0x0
#define BRANCH_NE		0x1
#define BRANCH_LT		0x4
#define BRANCH_GE		0x5
#define BRANCH_LTU		0x6
#define BRANCH_GEU		0x7

#define ESYS			0x0
#define ECALL(uimm)		(uimm==(reg_t)1)
#define CSR_RW			0x1
#define CSR_RS			0x2
#define CSR_RC			0x3
#define CSR_RWI			0x5
#define CSR_RSI			0x6
#define CSR_RCI			0x7
#define CSR_IMM(op)		((op)&0x4)

#define A0				10
#define A1				11
#define E_EXIT			10
#define E_CHAR			11

/* instruction logic */
#define GET_OPCODE(instr) ((unsigned char) (instr) & 0x7F)
#define GET_RS1(instr) ((unsigned char) (((instr) & 0xF8000) >> 15))
#define GET_RS2(instr) ((unsigned char) (((instr) & 0x1F00000) >> 20))
#define GET_RD(instr) ((unsigned char) (((instr) & 0xF80) >> 7))
#define GET_IMM12(instr) ((reg_t) (((instr) & 0xFFF00000) >> 20))
#define GET_SIMM12(instr) ((sreg_t) SRAW((instr) & 0xFFF00000, 20))
#define GET_SIMM20(instr) ((sreg_t) (int32_t) ((instr) & 0xFFFFF000))
#define GET_FUNCT3(instr) ((unsigned char) (((instr) & 0x7000) >> 12))
#define CHECK_FUNCT7_BIT0(instr) (((instr) & 0x2000000) != 0)
#define CHECK_FUNCT7_BIT5(instr) (((instr) & 0x40000000) != 0)
#define ALU_FUNCT7_BIT5(instr) ((unsigned char) (((instr) & 0x40000000) >> 27))
#define GET_BRANCH_IMM(instr) ((sreg_t) SRAW((((instr) & 0x80000000)			/* bit 12 */	\
											| (((instr) & 0x80) << 23)    		/* bit 11 */	\
											| (((instr) & 0x7E000000) >> 1) 	/* bit 10-5 */	\
											| (((instr) & 0xF00) << 12)   		/* bit 4-1 */	\
                                ), 19))
#define GET_JAL_IMM(instr) ((sreg_t) SRAW((((instr) & 0x80000000)				/* bit 20 */	\
										   | (((instr) & 0xFF000) << 11)  		/* bit 19-12 */	\
										   | (((instr) & 0x100000) << 2)		/* bit 11 */	\
										   | (((instr) & 0x7FE00000) >> 9)		/* bit 10-1 */	\
										   ), 11))
#define GET_STORE_IMM(instr) (sreg_t) SRAW(((instr) & 0xFE000000) | (((instr) & 0xF80) << 13), 20)
#endif /* RISCV_H */