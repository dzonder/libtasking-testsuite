// Semaphore synchronization: segment representing each task is displayed on
// the segment display (at most \texttt{CRIT\_LIMIT} segments at a time),
// \texttt{critical~<=~CRIT\_LIMIT} assertion is always true and system never
// deadlocks.

#include "task.h"
#include "sched/rr.h"
#include "semaphore.h"
#include "segment.h"
#include "mutex.h"

#define N		5
#define CRIT_LIMIT	3

static struct mutex *mutex;
static struct semaphore *sem;

static volatile int critical = 0;

static unsigned seed[N];

static void test_semaphore(void *arg)
{
	int i = (int)arg;

	for (;;) {
		// TODO: remove this when mutex_lock will be non-starving
		wait((rand_r(&seed[i]) % 10000) * 120);

		semaphore_wait(sem);

		mutex_lock(mutex);
		++critical;
		mutex_unlock(mutex);

		segment_low_set_segment(i, 1U);

		for (volatile int i = (rand_r(&seed[i]) % 10000) * 120; i > 0; --i)
			assert(critical <= CRIT_LIMIT);

		segment_low_set_segment(i, 0U);

		mutex_lock(mutex);
		--critical;
		mutex_unlock(mutex);

		semaphore_post(sem);
	}
}

int main(void)
{
	segment_low_init();
	segment_low_set_digit(1U, 1U);

	task_init(&rr_scheduler, NULL);

	mutex = mutex_init();
	sem = semaphore_init(CRIT_LIMIT);

	for (int i = 0; i < N; ++i)
		task_spawn(test_semaphore, (void *)i);

	for (;;)
		task_yield();

	return 0;
}
