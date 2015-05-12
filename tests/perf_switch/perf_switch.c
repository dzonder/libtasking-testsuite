// Task switch timing measurement.

#include "task.h"
#include "sched/fcfs.h"
#include "utils.h"
#include "pit.h"

static volatile uint64_t time_a, time_b;

static void task_a(void *arg)
{
	time_a = time_get();

	task_yield(); /* We don't want to measure task termination time. */
}

static void task_b(void *arg)
{
	time_b = time_get();

	task_yield();
}

int main(void)
{
	pit_init();
	time_init();
	itm_enable();

	task_init(&fcfs_scheduler, NULL);

	task_spawn(task_a, NULL);
	task_spawn(task_b, NULL);

	task_yield();

	itm_printf("'%s'\n",
			"task switch time");

	itm_printf("%d\n", 2 * (uint32_t)(time_b - time_a));

	return 0;
}
