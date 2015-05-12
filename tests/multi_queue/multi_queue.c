// Multi queue scheduler: tasks are prioritized and scheduled correctly.

#include "task.h"
#include "sched/fcfs.h"
#include "sched/rr.h"
#include "sched/multi_queue.h"
#include "mutex.h"
#include "buttons.h"
#include "led.h"
#include "pit.h"
#include "segment.h"
#include "utils.h"
#include "vtimer.h"

static struct mutex *mutex;
static struct wait_queue wait_queue;

static void segment_task(void *arg)
{
	int i = (int)arg;

	assert(i >= 0 && i < 4);

	for (;;) {
		mutex_lock(mutex);

		segment_set(i >> 1, SEGMENT_DIGITS[i + 1]);

		wait(50 * 1000 * 120);

		mutex_unlock(mutex);

		wait(50 * 1000 * 120);
	}
}

static void led_task(void *arg)
{
	int wait_iters = (int)arg;

	for (;;) {
		led_ctrl(LED_RED);

		wait(wait_iters * 1000 * 120);

		led_ctrl(LED_OFF);

		wait(wait_iters * 1000 * 120);
	}
}

int main(void)
{
	pit_init();
	vtimer_init();
	segment_init();
	buttons_init();
	led_init();

	struct multi_queue_conf multi_queue_conf;

	multi_queue_conf_init(&multi_queue_conf, 3);

	multi_queue_conf.scheduler_setups[0].scheduler = &rr_scheduler;
	multi_queue_conf.scheduler_setups[1].scheduler = &rr_scheduler;
	multi_queue_conf.scheduler_setups[2].scheduler = &rr_scheduler;
	multi_queue_conf.scheduler_setups[3].scheduler = &fcfs_scheduler;

	task_init(&multi_queue_scheduler, &multi_queue_conf);

	task_wait_queue_init(&wait_queue);

	buttons_register_press(PORTE, 2, (void (*)(void *))task_wait_queue_wait, NULL, &wait_queue);
	buttons_register_press(PORTE, 3, (void (*)(void *))task_wait_queue_signal, NULL, &wait_queue);

	mutex = mutex_init();

	task_spawn_prio(segment_task, (void *)0b00, 1U);
	task_spawn_prio(segment_task, (void *)0b01, 1U);

	task_spawn_prio(segment_task, (void *)0b10, 2U);
	task_spawn_prio(segment_task, (void *)0b11, 2U);
	task_spawn_prio(led_task, (void *)10, 2U);

	/* Highest priority tasks */
	task_spawn_prio(led_task, (void *)100, 3U);
	task_spawn_prio(led_task, (void *)500, 3U);

	/* Main task is priority 0 (lowest) */
	for (;;) {
		led_ctrl(LED_GREEN);

		wait(10 * 1000 * 120);

		led_ctrl(LED_OFF);

		wait(10 * 1000 * 120);
	}

	return 0;
}
