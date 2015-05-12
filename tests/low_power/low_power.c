// Entering low power mode when all tasks are sleeping and waking up
// \texttt{led\_task} on an interrupt: led blinks green.

#include "task.h"
#include "sched/rr.h"
#include "MK22F12.h"
#include "led.h"
#include "utils.h"
#include "pit.h"

struct wait_queue wait_queue_led_task;

static void led_task(void *arg)
{
	enum led_status status = LED_OFF;

	for (int i = 0; i < 10; ++i) {
		led_ctrl(status ^= 1);

		task_wait_queue_wait(&wait_queue_led_task);
	}
}

void LPTimer_IRQHandler()
{
	LPTMR0_CSR |= LPTMR_CSR_TCF_MASK;

	task_wait_queue_signal(&wait_queue_led_task);
}

int main(void)
{
	pit_init();
	itm_enable();
	led_init();

	task_init(&rr_scheduler, NULL);

	{ /* Enable low-power timer */
		SIM_SCGC5 |= SIM_SCGC5_LPTIMER_MASK;

		NVICICPR1 = 1U << 26U;
		NVICISER1 = 1U << 26U;

		/* Setup Low Power Timer for button event timestamps */
		LPTMR0_CSR = 0;
		LPTMR0_CMR = LPTMR_CMR_COMPARE(100);
		LPTMR0_CSR |= LPTMR_CSR_TEN_MASK; /* Enable LPTMR */
		LPTMR0_PSR = LPTMR_PSR_PBYP_MASK /* Bypass prescaler */
				| LPTMR_PSR_PCS(0b01); /* LPO as source */
		LPTMR0_CSR |= LPTMR_CSR_TIE_MASK;
	}

	task_wait_queue_init(&wait_queue_led_task);

	tid_t tid = task_spawn(led_task, NULL);

	task_join(tid);

	itm_printf("OK\n");

	return 0;
}
