// Mutex synchronization: identifier of each task is displayed on segment
// display, \texttt{critical~==~1} assertion is always true and system never
// deadlocks.

#include "task.h"
#include "sched/rr.h"
#include "mutex.h"
#include "pit.h"
#include "segment.h"
#include "vtimer.h"

#define N		4

static struct mutex *mutex;

static volatile int critical = 0;

static unsigned seed[N];

static void test_mutex(void *arg)
{
	int i = (int)arg;

	for (;;) {
		// TODO: remove this when mutex_lock will be non-starving
		wait((rand_r(&seed[i - 1]) % 10000) * 120);

		mutex_lock(mutex);

		++critical;

		segment_set(1U, SEGMENT_DIGITS[i]);

		for (volatile int j = (rand_r(&seed[i - 1]) % 10000) * 120; j > 0; --j)
			assert(critical == 1);

		segment_set(1U, 0U);

		--critical;

		mutex_unlock(mutex);
	}
}

int main(void)
{
	pit_init();
	vtimer_init();
	segment_init();

	task_init(&rr_scheduler, NULL);

	mutex = mutex_init();

	for (int i = 1; i <= N; ++i)
		task_spawn(test_mutex, (void *)i);

	for (;;)
		task_yield();

	return 0;
}
