// Basic task spawning (FCFS scheduler): led indicator blinks red and green.

#include "task.h"
#include "sched/fcfs.h"
#include "utils.h"
#include "led.h"

static void led_green_task(void *arg)
{
	for (;;) {
		led_ctrl(LED_GREEN);
		wait(100 * 1000 * 120);

		task_yield();
	}
}

int main(void)
{
	led_init();

	task_init(&fcfs_scheduler, NULL);

	task_spawn(led_green_task, NULL);

	for (;;) {
		led_ctrl(LED_RED);
		wait(100 * 1000 * 120);

		task_yield();
	}

	return 0;
}
