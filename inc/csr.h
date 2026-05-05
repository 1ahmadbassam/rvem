#ifndef CSR_H
#define CSR_H

#include <stdint.h>

#include "common.h"
#include "arch.h"

/* machine mode */
#define CSR_MCYCLE      0xB00
#define CSR_MINSTRET    0xB02
#define CSR_MCYCLEH     0xB80
#define CSR_MINSTRETH   0xB82
/* user mode */
#define CSR_CYCLE       0xC00
#define CSR_INSTRET     0xC02
#define CSR_CYCLEH      0xC80
#define CSR_INSTRETH    0xC82

extern uint64_t mcycle;
extern uint64_t minstret;

static inline reg_t read_csr(unsigned int csr) {
	switch(csr) {
		case CSR_MCYCLE:
		case CSR_CYCLE:
			return (reg_t) mcycle;
		case CSR_MINSTRET:
		case CSR_INSTRET:
			return (reg_t) minstret;
#ifndef RV64
		case CSR_MCYCLEH:
		case CSR_CYCLEH:
			return (reg_t) (mcycle >> 32);
		case CSR_MINSTRETH:
		case CSR_INSTRETH:
			return (reg_t) (minstret >> 32);
#endif
		default:
			return 0;
	}
}

static inline void write_csr(unsigned int csr, reg_t val) {
	UNUSED_VARIABLE(csr);
	UNUSED_VARIABLE(val);
	/* not implemented */
}

#endif /* CSR_H */
