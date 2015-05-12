// Recursive fibonacci calculation timing measurement (for reference).

#include "utils.h"
#include "pit.h"

#define N		30

static uint64_t time_a, time_b;

static int fib(int n)
{
	if (n == 0)
		return 0;

	if (n == 1)
		return 1;

	return fib(n - 1) + fib(n - 2);
}

int main(void)
{
	pit_init();
	time_init();
	itm_enable();

	itm_printf("'%s' '%s' '%s'\n",
			"n",
			"fib(n)",
			"time");

	for (int i = 0; i <= N; ++i) {
		time_a = time_get();

		int fib_n = fib(i);

		time_b = time_get();

		/* Output fibonacci number and calculation time to SWO */
		itm_printf("%d %d %d\n", i, fib_n, 2 * (uint32_t)(time_b - time_a));
	}

	return 0;
}
