// Verifies that synchronization behaves properly when there is more than one
// mutex in the system (more than one memory location is being tagged by
// exclusive access instructions): \texttt{critical[critidx]~==~1} assertion is
// always true, led blinks red and green and segment display shows the
// identifier of each task out of \texttt{N} tasks.

#include "task.h"
#include "sched/rr.h"
#include "mutex.h"
#include "led.h"
#include "pit.h"
#include "segment.h"
#include "vtimer.h"

#define N		6

static struct mutex *mutex[2];

static volatile int critical[2] = { 0, 0 };

static unsigned seed[N + 2];

static void test_mutex(void *arg)
{
	int i = (int)arg;
	int critidx = i >= 2;

	for (;;) {
		// TODO: remove this when mutex_lock will be non-starving
		wait((rand_r(&seed[i]) % 10000) * 120);

		mutex_lock(mutex[critidx]);

		++critical[critidx];

		if (i < 2) {
			led_ctrl(i == 0 ? LED_GREEN : LED_RED);
		} else {
			segment_set(1U, SEGMENT_DIGITS[i - 1]);
		}

		for(volatile int j = (rand_r(&seed[i]) % 10000) * 120; j > 0; --j)
			assert(critical[critidx] == 1);

		if (i < 2) {
			led_ctrl(LED_OFF);
		} else {
			segment_set(1U, 0U);
		}

		--critical[critidx];

		mutex_unlock(mutex[critidx]);
	}
}

int main(void)
{
	pit_init();
	vtimer_init();
	segment_init();
	led_init();

	task_init(&rr_scheduler, NULL);

	mutex[0] = mutex_init();
	mutex[1] = mutex_init();

	task_spawn(test_mutex, (void *)0);
	task_spawn(test_mutex, (void *)1);

	for (int i = 0; i < N; ++i)
		task_spawn(test_mutex, (void *)(i + 2));

	for (;;)
		task_yield();

	return 0;
}
