// Stack overflow checking by using infinite recursion: task \texttt{boom}
// overflows the stack and the library catches the overflow during context
// switch.

#include "task.h"
#include "sched/rr.h"
#include "utils.h"

static void boom(void *arg)
{
	int *x = (int *)arg;

	int y = (*x)++;

	wait(100 * 120);

	boom(x);

	*x = y;
}

int main(void)
{
	int x = 0;

	task_init(&rr_scheduler, NULL);

	task_spawn(boom, &x);

	for (;;)
		task_yield();

	return 0;
}
