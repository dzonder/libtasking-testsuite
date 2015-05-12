// Task spawn timing measurement.

#include "task.h"
#include "sched/fcfs.h"
#include "utils.h"
#include "pit.h"

#define N		100

static uint64_t time_a, time_b;

static void task_nop(void *arg)
{
	__asm("NOP");
}

int main(void)
{
	pit_init();
	time_init();
	itm_enable();

	task_init(&fcfs_scheduler, NULL);

	itm_printf("'%s' '%s'\n",
			"number of tasks",
			"spawn time");

	for (int i = 0; i <= N; i += 10) {
		int k = i == 0 ? 1 : i;

		time_a = time_get();

		for (int j = 1; j <= k; ++j)
			task_spawn(task_nop, NULL);

		time_b = time_get();

		task_yield(); /* Allow the tasks to terminate */

		itm_printf("%d %d\n", k, 2 * (uint32_t)(time_b - time_a));
	}

	return 0;
}
