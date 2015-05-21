#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "time.h"

#define MUX_ANALOG	0b000
#define MUX_GPIO	0b001

#define SET_MUX(pcr, mux) \
	(pcr) = ((pcr) & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(mux)

#define PORT_COUNT		5U
#define PIN_COUNT		32U

#define min(a, b)		((a) < (b) ? (a) : (b))
#define max(a, b)		((a) > (b) ? (a) : (b))
#define sqr(a)			((a) * (a))
#define div_ceil(a, b)		(((a) + (b) - 1) / (b))

#define swap(a, b)						\
do {								\
	__typeof__(a) _a = b;					\
	b = a;							\
	a = _a;							\
} while (0)

void wait_us(int32_t i);
void wait(volatile uint32_t i);

void assert(bool cond);

void irq_enable(void);
void irq_disable(void);

void itm_enable(void);
void itm_sendmsg(const char *msg);
int itm_printf(const char *format, ...);

#endif /* _UTILS_H_ */
