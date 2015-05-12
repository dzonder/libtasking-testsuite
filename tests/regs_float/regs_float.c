// Preserving of floating-point registers: \texttt{x~==~1} assertion is true
// and segment display displays "11".

#include "pit.h"
#include "sched/fcfs.h"
#include "segment.h"
#include "task.h"
#include "utils.h"
#include "vtimer.h"

static void float_a(void *arg)
{
	float x = 1.0f;

	__asm ("VMOV.F32 s10, %0\n\t" : : "r" (x));
	__asm ("VMOV.F32 s22, %0\n\t" : : "r" (x));

	/* Let 'float_b' pollute registers */
	task_yield();

	/* Check hardware-preserved FP register */
	__asm ("VMOV.F32 %0, s10\n\t" : "=r" (x));
	assert((int)x == 1);
	segment_set(1U, SEGMENT_DIGITS[(int)x]);

	/* Check software-preserved FP register */
	__asm ("VMOV.F32 %0, s22\n\t" : "=r" (x));
	assert((int)x == 1);
	segment_set(0U, SEGMENT_DIGITS[(int)x]);
}

static void float_b(void *arg)
{
	float y = 2.0f;

	__asm ("VMOV.F32 s10, %0\n\t" : : "r" (y));
	__asm ("VMOV.F32 s22, %0\n\t" : : "r" (y));
}

int main(void)
{
	pit_init();
	vtimer_init();
	segment_init();

	task_init(&fcfs_scheduler, NULL);

	task_spawn(float_a, NULL);
	task_spawn(float_b, NULL);

	task_yield(); /* Let 'float_a' run */
	task_yield(); /* Once again let 'float_a' run */

	/* This is what we should see on the segment display
	   if FP registers are preserved correctly: 1 1 */

	for (;;);

	return 0;
}
