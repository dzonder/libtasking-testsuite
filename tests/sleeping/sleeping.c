// Preemption test with sleeping on mutex: \texttt{critical~==~1} assertion
// is always true, system never deadlocks and \texttt{test\_led} task works
// independently from \texttt{test\_mutex} tasks.

#include "task.h"
#include "sched/rr.h"
#include "mutex.h"
#include "led.h"
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

		for(volatile int j = (rand_r(&seed[i - 1]) % 10000) * 120; j > 0; --j)
			assert(critical == 1);

		segment_set(1U, 0U);

		--critical;

		mutex_unlock(mutex);
	}
}

static void test_led(void *arg)
{
	for (;;) {
		led_ctrl(LED_GREEN);
		wait(10 * 1000 * 120);
		led_ctrl(LED_RED);
		wait(10 * 1000 * 120);
	}
}

int main(void)
{
	pit_init();
	vtimer_init();
	segment_init();
	led_init();

	task_init(&rr_scheduler, NULL);

	mutex = mutex_init();

	task_spawn(test_led, NULL);

	for (int i = 1; i <= N; ++i)
		task_spawn(test_mutex, (void *)i);

	for (;;)
		task_yield();

	return 0;
}
