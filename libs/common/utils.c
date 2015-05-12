#include "utils.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include "pit.h"

#if (__FPU_PRESENT == 1)
#include "ARMCM4_FP.h"
#else
#include "ARMCM4.h"
#endif

#include "core_cm4.h"

#define MSG_SIZE			128

static char msg[MSG_SIZE];

void wait_us(int32_t us)
{
	const uint32_t MAX_SECONDS = 10;

	assert(us < MAX_SECONDS * 1000 * 1000);

	struct pit_desc *pitWait = pit_reserve();

	/* PIT is 60 MHz */
	uint32_t prevValue = MAX_SECONDS * 2 * 60 * 1000 * 1000;

	pit_set_start_value(pitWait, prevValue);
	pit_enable_timer(pitWait);

	while (us > 0) {
		uint32_t currValue = pit_get_current_value(pitWait);
		uint32_t diff = prevValue - currValue;
		uint32_t useconds = diff / 60;

		us -= useconds;
		prevValue -= useconds * 60;
	}

	pit_release(pitWait);
}

void wait(volatile uint32_t i)
{
	for (; i > 0; --i);
}

static inline void breakpoint(void) __attribute__((noreturn));
static inline void breakpoint(void) {
	__asm volatile("bkpt");
	goto *0xFFFFFFFFU;
}

void assert(bool cond) __attribute__((noinline));
void assert(bool cond)
{
	if (!cond)
		breakpoint();
}

__attribute__ ((used))
void *_sbrk(ptrdiff_t increment)
{
	const size_t STACK_SIZE = 2048; /* Some silly maximum stack size approximation */

	extern unsigned long _end; /* Heap's end set by linker script */
	extern unsigned long _estack; /* Bottom of the stack */

	static uint8_t *brk = NULL;

	if (brk == NULL)
		brk = (uint8_t *)&_end; /* Initialize with heap's end. */

	assert(increment >= 0);

	void *prevbrk = brk;

	brk += increment;

	assert(brk < (uint8_t *)&_estack - STACK_SIZE);

	// TODO: make this stack/heap overflow checking better

	return prevbrk;
}

void irq_enable(void)
{
	__enable_irq();
}

void irq_disable(void)
{
	__disable_irq();
}

void itm_enable(void)
{
	uint32_t SwoBaudRate = 1000 * 1000;
	uint32_t SwoPrescaler = ((120 * 1000 * 1000) / SwoBaudRate) - 1;

	CoreDebug->DEMCR = CoreDebug_DEMCR_TRCENA_Msk;

	TPI->SPPR = 0b10; /* Asynchronous Serial Wire Output */
	TPI->ACPR = SwoPrescaler;

	ITM->LAR = 0xC5ACCE55;
	ITM->TCR = ITM_TCR_TraceBusID_Msk | ITM_TCR_SWOENA_Msk | ITM_TCR_SYNCENA_Msk | ITM_TCR_ITMENA_Msk;
	ITM->TPR = ITM_TPR_PRIVMASK_Msk;
	ITM->TER = (1UL << 0); /* Enable port 0 */

	DWT->CTRL = 0x400003FE;

	TPI->FFCR = TPI_FFCR_TrigIn_Msk;
}

void itm_sendmsg(const char *msg)
{
	while (*msg) {
		ITM_SendChar(*msg++);
		wait_us(100);
	}
}

int itm_printf(const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	int ret = vsnprintf(msg, MSG_SIZE, format, ap);
	va_end(ap);

	itm_sendmsg(msg);

	return ret;
}
