// Preserving of core registers: \texttt{x~==~1} assertion is true and segment
// display displays ,,\textvisiblespace 1''.

#include "pit.h"
#include "sched/fcfs.h"
#include "segment.h"
#include "task.h"
#include "utils.h"
#include "vtimer.h"

static void regs_a(void *arg)
{
	int x = 1;

	__asm ("MOV r11, %0\n\t" : : "r" (x));

	/* Let 'regs_b' pollute registers */
	task_yield();

	/* Check software-preserved register */
	__asm ("MOV %0, r11\n\t" : "=r" (x));
	assert(x == 1);
	segment_set(1U, SEGMENT_DIGITS[x]);
}

static void regs_b(void *arg)
{
	int y = 2;

	__asm ("MOV r11, %0\n\t" : : "r" (y));
}

int main(void)
{
	pit_init();
	vtimer_init();
	segment_init();

	task_init(&fcfs_scheduler, NULL);

	task_spawn(regs_a, NULL);
	task_spawn(regs_b, NULL);

	task_yield(); /* Let 'regs_a' run */
	task_yield(); /* Once again let 'regs_a' run */

	/* This is what we should se on the segment display
	   if registers are preserved correctly: 1 */

	for (;;);

	return 0;
}
