// Task joining: main task waits for the other tasks to finish.

#include "sched/rr.h"
#include "task.h"
#include "utils.h"
#include "pit.h"

#define N		10000000

static volatile uint32_t val;

static void task_increment(void *arg)
{
	for (int i = 0; i < N; ++i)
		++val;
}

static void test_0(void)
{
	val = 0;

	tid_t tid = task_spawn(task_increment, NULL);

	task_yield();
	/* Task should not finish yet */

	/* Lets check if this tests even works */
	assert(!task_terminated(tid));
	assert(val > 0 && val < N);

	task_join(tid);

	itm_printf("OK\n");
}

static void test_1(void)
{
	val = 0;

	tid_t tid = task_spawn(task_increment, NULL);

	/* Check if task exists */
	assert(!task_terminated(tid));

	/* Wait for task to finish */
	wait(100 * 1000 * 120);

	assert(task_terminated(tid));
	assert(val == N);

	/* Check if join does not block on already finished task */
	task_join(tid);

	itm_printf("OK\n");
}

static void test_2(void)
{
	val = 0;

	tid_t tid = task_spawn(task_increment, NULL);

	/* Check if join waits for the task to finish */
	task_join(tid);

	assert(task_terminated(tid));
	assert(val == N);

	itm_printf("OK\n");
}

static void test_3(void)
{
	val = 0;

	tid_t tid = task_spawn(task_increment, NULL);

	/* Busy wait until task terminates */
	while (!task_terminated(tid));

	assert(val == N);

	itm_printf("OK\n");
}

int main(void)
{
	pit_init();
	itm_enable();

	task_init(&rr_scheduler, NULL);

	test_0();
	test_1();
	test_2();
	test_3();

	return 0;
}
