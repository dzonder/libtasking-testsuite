// Verifies that synchronization behaves properly when there is more than one
// mutex in the system.

#include "task.h"
#include "sched/rr.h"
#include "mutex.h"
#include "pit.h"

#define N_TASKS		10
#define N		100
#define M		10000

static struct mutex *mutex[2];

static volatile int bad = 0;

static volatile int val[2] = { 0, 0 };

static void test_bad(void *arg)
{
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < M; ++j)
			++bad;
	}
}

static void test_mutex(void *arg)
{
	int critidx = (int)arg;

	for (int i = 0; i < N; ++i) {
		mutex_lock(mutex[critidx]);

		for (int j = 0; j < M; ++j)
			++val[critidx];

		mutex_unlock(mutex[critidx]);

		task_yield();
	}
}

int main(void)
{
	tid_t tids[N_TASKS][3];

	pit_init();
	itm_enable();

	task_init(&rr_scheduler, NULL);

	mutex[0] = mutex_init();
	mutex[1] = mutex_init();

	for (int i = 0; i < N_TASKS; ++i) {
		tids[i][0] = task_spawn(test_mutex, (void *)0);
		tids[i][1] = task_spawn(test_mutex, (void *)1);
		tids[i][2] = task_spawn(test_bad, NULL);
	}

	for (int i = 0; i < N_TASKS; ++i) {
		task_join(tids[i][0]);
		task_join(tids[i][1]);
		task_join(tids[i][2]);
	}

	itm_printf("%d %d %d\n", bad, val[0], val[1]);

	return 0;
}
