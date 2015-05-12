// Basic preemption with RR scheduler: led indicator blinks red and green.

#include "task.h"
#include "sched/rr.h"
#include "led.h"
#include "utils.h"

static void led_green_task(void *arg)
{
	for (;;)
		led_ctrl(LED_GREEN);
}

int main(void)
{
	led_init();

	task_init(&rr_scheduler, NULL);

	task_spawn(led_green_task, NULL);

	for (;;)
		led_ctrl(LED_RED);

	return 0;
}
