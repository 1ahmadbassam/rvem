#ifndef ARCH_H
#define ARCH_H

#include <stdint.h>

#include "common.h"

#define MAX_CYCLES			1000000000

#define ROM_START			0x0
#define RAM_START 			0x20000000U
#define ROM_SIZE 			(512 * 1024) /* 512KB */
#define RAM_SIZE 			(64 * 1024)  /* 64KB */

#define SIGN_BIT_WORD 		0x80000000
#define SHIFT_MAX_WORD		0x1F
#define SIGN_BIT_DWORD 		SUFFIX_U64(0x8000000000000000)
#define SHIFT_MAX_DWORD		0x3F
#ifdef RV64
typedef uint64_t			reg_t;
typedef int64_t				sreg_t;
#define REG_MIN				0
#define REG_MAX				UINT64_MAX
#define SREG_MIN			INT64_MIN
#define SREG_MAX			INT64_MAX
#define XLEN	 			64
#define SIGN_BIT			SIGN_BIT_DWORD
#define SHIFT_MAX			SHIFT_MAX_DWORD
#define BASE_SHIFT			SUFFIX_U64(1)
#define HEX_FMT				"%llX"
#else /* RV64 */
typedef uint32_t			reg_t;
typedef int32_t				sreg_t;
#define REG_MIN				0
#define REG_MAX				UINT32_MAX
#define SREG_MIN			INT32_MIN
#define SREG_MAX			INT32_MAX
#define XLEN	 			32
#define SIGN_BIT 			SIGN_BIT_WORD
#define SHIFT_MAX			SHIFT_MAX_WORD
#define BASE_SHIFT			1U
#define	HEX_FMT				"%X"
#endif /* RV64 */
#define XRF_SIZE 			32

bool process_instr(const uint32_t instr);

#endif /* ARCH_H */