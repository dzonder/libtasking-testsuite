// Atomic operations test: value is correctly incremented by many tasks.

#include "task.h"
#include "sched/rr.h"
#include "sync_low.h"
#include "utils.h"
#include "pit.h"

#define N		1000000
#define N_TASKS		20

static volatile uint32_t val;

static void task_increment(void *arg)
{
	for (int i = 0; i < N; ++i)
		sync_low_atomic_inc(&val);
}

int main(void)
{
	tid_t tids[N_TASKS];

	pit_init();
	time_init();
	itm_enable();

	task_init(&rr_scheduler, NULL);

	for (int i = 0; i < N_TASKS; ++i)
		tids[i] = task_spawn(task_increment, NULL);

	for (int i = 0; i < N_TASKS; ++i)
		task_join(tids[i]);

	itm_printf("%d %d\n", val, N * N_TASKS);

	return 0;
}
