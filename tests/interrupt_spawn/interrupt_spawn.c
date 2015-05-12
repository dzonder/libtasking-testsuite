// Spawning task inside interrupt handler: led blinks green.

#include "task.h"
#include "sched/rr.h"
#include "led.h"
#include "pit.h"
#include "utils.h"

static void led_task(void *arg)
{
	enum led_status status = (enum led_status)arg;

	led_ctrl(status);

	wait(10 * 1000 * 120);

	task_spawn(led_task, (void *)(status ^= 1));
}

static void idle_task(void *arg)
{
	wait(10 * 120);
}

static void interrupt_spawn(void)
{
	task_spawn(idle_task, NULL);
}

int main(void)
{
	led_init();
	pit_init();

	task_init(&rr_scheduler, NULL);

	struct pit_desc *pit = pit_reserve();
	pit_enable_interrupt(pit, interrupt_spawn);
	pit_set_start_value(pit, 100000);
	pit_enable_timer(pit);

	task_spawn(led_task, (void *)LED_GREEN);

	for (;;)
		task_yield();

	return 0;
}
