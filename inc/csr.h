#ifndef CSR_H
#define CSR_H

#include <stdint.h>

#include "common.h"
#include "arch.h"

#define CSR_MCYCLE			0xC00
#define CSR_MINSTRET		0xC02
#define CSR_MCYCLEH			0xC80
#define CSR_MINSTRETH		0xC82

extern uint64_t mcycle;
extern uint64_t minstret;

static inline reg_t read_csr(unsigned int csr) {
	switch(csr) {
		case CSR_MCYCLE:
			return (reg_t) mcycle;
		case CSR_MINSTRET:
			return (reg_t) minstret;
#ifndef RV64
		case CSR_MCYCLEH:
			return (reg_t) (mcycle >> 32);
		case CSR_MINSTRETH:
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
