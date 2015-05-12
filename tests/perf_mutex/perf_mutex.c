// Mutex synchronization overhead and correctness: value is correctly
// incremented by many tasks.

#include "task.h"
#include "sched/rr.h"
#include "mutex.h"
#include "utils.h"
#include "pit.h"

#define N		1000000
#define N_TASKS		10

static struct mutex *mutexes[N_TASKS];

static uint64_t time_a, time_b, time_c, time_d, time_e, time_f;

static volatile uint32_t val;

static void task_without_lock(void *arg)
{
	for (int i = 0; i < N; ++i)
		++val;
}

static void task_with_lock(void *arg)
{
	struct mutex *mutex = (struct mutex *)arg;

	for (int i = 0; i < N / 10; ++i) {
		mutex_lock(mutex);

		for (int j = 0; j < 10; ++j)
			++val;

		mutex_unlock(mutex);
	}
}

static uint32_t test_1(int n_tasks)
{
	tid_t tids[n_tasks];

	val = 0;

	time_a = time_get();

	for (int j = 0; j < n_tasks; ++j)
		tids[j] = task_spawn(task_without_lock, NULL);

	for (int j = 0; j < n_tasks; ++j)
		task_join(tids[j]);

	time_b = time_get();

	return val;
}

static uint32_t test_2(int n_tasks)
{
	tid_t tids[n_tasks];

	val = 0;

	time_c = time_get();

	for (int j = 0; j < n_tasks; ++j)
		tids[j] = task_spawn(task_with_lock, mutexes[j]);

	for (int j = 0; j < n_tasks; ++j)
		task_join(tids[j]);

	time_d = time_get();

	return val;
}

static uint32_t test_3(int n_tasks)
{
	tid_t tids[n_tasks];

	val = 0;

	time_e = time_get();

	for (int j = 0; j < n_tasks; ++j)
		tids[j] = task_spawn(task_with_lock, mutexes[0]);

	for (int j = 0; j < n_tasks; ++j)
		task_join(tids[j]);

	time_f = time_get();

	return val;
}

int main(void)
{
	pit_init();
	time_init();
	itm_enable();

	task_init(&rr_scheduler, NULL);

	for (int i = 0; i < N_TASKS; ++i)
		mutexes[i] = mutex_init();

	itm_printf("'%s' ",
			"number of processes");
	itm_printf("'%s' '%s' '%s' ",
			"result value without synchronization",
			"result value with synchronization (tasks never sleep)",
			"result value with synchronization");
	itm_printf("'%s' '%s'\n",
			"time without synchronization",
			"time with synchronization");

	for (int i = 1; i <= N_TASKS; ++i) {
		uint32_t val_a = test_1(i);
		uint32_t val_b = test_2(i);
		uint32_t val_c = test_3(i);

		itm_printf("%d %d %d %d %d %d %d\n", i, val_a, val_b, val_c,
				2 * (uint32_t)(time_b - time_a),
				2 * (uint32_t)(time_d - time_c),
				2 * (uint32_t)(time_f - time_e));
	}

	itm_printf("%d\nOK\n", N);

	return 0;
}
