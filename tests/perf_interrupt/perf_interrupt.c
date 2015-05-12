// Interrupt enter and exit timing measurement.

#include "utils.h"
#include "pit.h"

static volatile uint64_t time_a, time_b, time_c;

void SVC_Handler(void)
{
	time_b = time_get();
}

int main(void)
{
	pit_init();
	time_init();
	itm_enable();

	time_a = time_get();

	/* Enter interrupt */
	__asm volatile("SVC 0x0; ISB;");

	time_c = time_get();

	itm_printf("'%s' '%s'\n",
			"interrupt enter time",
			"interrupt exit time");

	itm_printf("%d %d\n",
			2 * (uint32_t)(time_b - time_a),
			2 * (uint32_t)(time_c - time_b));

	return 0;
}
