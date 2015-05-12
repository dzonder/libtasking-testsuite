// Time measurement error test.

#include "utils.h"
#include "pit.h"

static uint64_t time_a, time_b;

int main(void)
{
	pit_init();
	time_init();
	itm_enable();

	time_a = time_get();
	time_b = time_get();

	itm_printf("'%s'\n",
			"time error");

	itm_printf("%d\n", 2 * (uint32_t)(time_b - time_a));

	return 0;
}
