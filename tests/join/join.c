// Task join: main task waits for the other task to finish.

#include "led.h"
#include "sched/rr.h"
#include "task.h"
#include "utils.h"

static void led_green_task(void *arg)
{
	led_ctrl(LED_GREEN);
	wait(10 * 1000 * 120);
}

int main(void)
{
	tid_t tid;

	led_init();

	task_init(&rr_scheduler, NULL);

	for (;;) {
		/* Test 1 */
		tid = task_spawn(led_green_task, NULL);

		/* Wait for task to finish */
		wait(100 * 1000 * 120);

		/* Check if join does not block on already finished task */
		task_join(tid);

		led_ctrl(LED_RED);
		wait(100 * 1000 * 120);

		/* Test 2 */
		tid = task_spawn(led_green_task, NULL);

		/* Check if join waits for the task to finish */
		task_join(tid);

		led_ctrl(LED_RED);
		wait(100 * 1000 * 120);

		/* Test 3 */
		tid = task_spawn(led_green_task, NULL);

		/* Busy wait until task terminates */
		while (!task_terminated(tid));

		led_ctrl(LED_RED);
		wait(100 * 1000 * 120);
	}

	return 0;
}
