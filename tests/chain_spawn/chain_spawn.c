// Chained task spawning: first task blinks led red and green, second
// task displays ,,HELLO'' on the segment display.

#include "task.h"
#include "sched/fcfs.h"
#include "led.h"
#include "pit.h"
#include "segment.h"
#include "utils.h"
#include "vtimer.h"

static const uint8_t string[] = {
	0b01110110,
	0b01111001,
	0b00111000,
	0b00111000,
	0b00111111,
	0b00000000,
}; /* "HELLO " */

static const uint32_t string_len = sizeof(string) / sizeof(string[0]);

static void print_hello_world(void *arg)
{
	uint32_t idx = (uint32_t)arg;
	uint32_t idx_prev = (idx > 0) ? (idx - 1) : (string_len - 1);

	segment_set(0U, string[idx_prev]);
	segment_set(1U, string[idx]);

	idx += 1;
	idx %= string_len;

	wait(200 * 1000 * 10);

	task_spawn(print_hello_world, (void *)idx);
}

static void change_led_status(void *arg)
{
	enum led_status led_status = (enum led_status)arg;

	/* Change LED status */
	led_ctrl(led_status);

	/* Make sure user notices */
	wait(1000 * 1000 * 10);

	/* Toggle status */
	led_status ^= 1;

	/* Add next task to queue */
	task_spawn(change_led_status, (void *)led_status);
}

int main(void)
{
	pit_init();
	vtimer_init();
	segment_init();
	led_init();

	task_init(&fcfs_scheduler, NULL);

	task_spawn(change_led_status, (void *)LED_GREEN);
	task_spawn(print_hello_world, (void *)0);

	for (;;)
		task_yield();

	return 0;
}
