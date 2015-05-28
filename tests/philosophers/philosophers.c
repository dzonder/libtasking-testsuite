// The Dining Philosophers example: each fork is used by at most one
// philosopher, there is no deadlock, no starvation and philosophers are eating
// effectively.

/* Based on solution from 'Modern Operating Systems' by Andrew S. Tanenbaum. */

#include "task.h"
#include "sched/rr.h"
#include "sync_low.h"
#include "mutex.h"
#include "segment.h"

#define N		5

#define LEFT(i)		((i + N - 1) % N)
#define RIGHT(i)	((i + 1) % N)

enum state {
	THINKING,
	HUNGRY,
	EATING,
};

static enum state state[N] = { THINKING };
static struct mutex *mutex;
static struct mutex *s[N];

static int eaten[N] = { 0 };
static volatile uint32_t eating = 0;

static unsigned seed[N];

static void think(int i)
{
	// TODO: enable ith led on left display

	for (volatile int j = 10; j > 0; --j);

	// TODO: disable ith led on left display
}

static void eat(int i)
{
	sync_low_atomic_inc(&eating);

	segment_low_set_segment(i, 1U);

	++eaten[i]; /* Record that i-th philosopher eated (to verify equal share) */

	for (volatile int j = (rand_r(&seed[i]) % 1000) + 1000; j > 0; --j)
		assert(eating <= N / 2);

	segment_low_set_segment(i, 0U);

	sync_low_atomic_dec(&eating);
}

static void test(int i)
{
	if (state[i] == HUNGRY && state[LEFT(i)] != EATING && state[RIGHT(i)] != EATING) {
		state[i] = EATING;
		mutex_unlock(s[i]);
	}
}

static void take_forks(int i)
{
	mutex_lock(mutex);
	state[i] = HUNGRY;
	test(i);
	mutex_unlock(mutex);
	mutex_lock(s[i]);
}

static void put_forks(int i)
{
	mutex_lock(mutex);
	state[i] = THINKING;
	test(LEFT(i));
	test(RIGHT(i));
	mutex_unlock(mutex);
}

static void philosopher(void *arg)
{
	int i = (int)arg;

	for (;;) {
		think(i);
		take_forks(i);
		eat(i);
		put_forks(i);
	}
}

int main(void)
{
	segment_low_init();
	segment_low_set_digit(1U, 1U);

	task_init(&rr_scheduler, NULL);

	mutex = mutex_init();
	for (int i = 0; i < N; ++i) {
		s[i] = mutex_init();
		mutex_lock(s[i]);
	}

	for (int i = 0; i < N; ++i)
		task_spawn(philosopher, (void *)i);

	for (;;)
		task_yield();

	return 0;
}
