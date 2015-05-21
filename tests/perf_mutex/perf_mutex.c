// Mutex synchronization overhead and correctness: value is correctly
// incremented by many tasks.

#include "task.h"
#include "sched/rr.h"
#include "mutex.h"
#include "utils.h"
#include "pit.h"

#define N		100000
#define N_TASKS		10

static struct mutex *mutexes[N_TASKS];

static uint64_t time_a, time_b;

static volatile uint32_t val;

static void task_without_lock(void *arg)
{
	for (int i = 0; i < N; ++i)
		++val;
}

static void task_with_lock(void *arg)
{
	struct mutex *mutex = (struct mutex *)arg;

	for (int i = 0; i < N; ++i) {
		mutex_lock(mutex);

		++val;

		mutex_unlock(mutex);
	}
}

static tid_t spawn_without_lock(int j)
{
	return task_spawn(task_without_lock, NULL);
}

static tid_t spawn_with_lock_private_mutex(int j)
{
	return task_spawn(task_with_lock, mutexes[j]);
}

static tid_t spawn_with_lock(int j)
{
	return task_spawn(task_with_lock, mutexes[0]);
}

static uint32_t run_test(int n_tasks, tid_t (*spawner)(int j))
{
	tid_t tids[n_tasks];

	val = 0;

	time_a = time_get();

	for (int j = 0; j < n_tasks; ++j)
		tids[j] = spawner(j);

	for (int j = 0; j < n_tasks; ++j)
		task_join(tids[j]);

	time_b = time_get();

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
		uint32_t val_1 = run_test(i, spawn_without_lock);
		uint32_t time_1 = time_b - time_a;
		uint32_t val_2 = run_test(i, spawn_with_lock_private_mutex);
		uint32_t time_2 = time_b - time_a;
		uint32_t val_3 = run_test(i, spawn_with_lock);
		uint32_t time_3 = time_b - time_a;

		itm_printf("%d %d %d %d %d %d %d\n", i,
				val_1, val_2, val_3,
				2 * time_1,
				2 * time_2,
				2 * time_3);
	}

	itm_printf("%d\nOK\n", N);

	return 0;
}
