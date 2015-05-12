#include "vtimer.h"

#include "pit.h"

#define VTIMER_MAX_TIMEOUTS		32

struct vtimeout {
	vid_t vid;

	vcb_t cb;
	void *arg;
	uint32_t ticks;

	struct vtimeout *list_prev;
	struct vtimeout *list_next;
};

static struct vtimeout vtimeout_pool[VTIMER_MAX_TIMEOUTS];
static struct vtimeout *vtimeout_list_head_free = NULL;

static struct vtimeout *vtimeout_list_head_scheduled = NULL;
static struct vtimeout *vtimeout_list_tail_scheduled = NULL;

static struct pit_desc *vtimer_pit;

static struct vtimeout *vtimer_get_vtimeout(vid_t vid)
{
	return &vtimeout_pool[vid % VTIMER_MAX_TIMEOUTS];
}

static struct vtimeout * vtimer_vtimeout_alloc(void)
{
	struct vtimeout *vtimeout = vtimeout_list_head_free;

	assert(vtimeout != NULL);

	vtimeout_list_head_free = vtimeout->list_next;

	assert(vtimeout->cb == NULL);

	vtimeout->list_prev = NULL;
	vtimeout->list_next = NULL;

	vtimeout->vid += VTIMER_MAX_TIMEOUTS;

	return vtimeout;
}

static void vtimer_vtimeout_free(struct vtimeout *vtimeout)
{
	vtimeout->cb = NULL;
	vtimeout->arg = NULL;
	vtimeout->ticks = 0U;

	vtimeout->list_prev = NULL;
	vtimeout->list_next = vtimeout_list_head_free;

	vtimeout_list_head_free = vtimeout;
}

static void vtimer_pit_irq_handler(void)
{
	struct vtimeout *vtimeout = vtimeout_list_head_scheduled;

	pit_disable_timer(vtimer_pit);

	if (vtimeout == NULL)
		return;

	/* First setup timer for the next vtimeout */
	vtimeout_list_head_scheduled = vtimeout->list_next;
	if (vtimeout_list_head_scheduled != NULL) {
		pit_set_start_value(vtimer_pit, vtimeout_list_head_scheduled->ticks);
		pit_enable_timer(vtimer_pit);
	}

	vcb_t cb = vtimeout->cb;
	void *arg = vtimeout->arg;

	vtimer_vtimeout_free(vtimeout);

	cb(arg);
}

void vtimer_init(void)
{
	/* Initialize list of free vtimeouts */
	for (uint16_t i = 0U; i < VTIMER_MAX_TIMEOUTS; ++i) {
		struct vtimeout *vtimeout = vtimeout_pool + i;

		vtimeout->vid = i;
		vtimeout->cb = NULL;
		vtimeout->arg = NULL;
		vtimeout->ticks = 0U;

		vtimeout->list_prev = NULL;
		vtimeout->list_next = vtimeout + 1;
	}
	vtimeout_pool[VTIMER_MAX_TIMEOUTS - 1].list_next = NULL;

	vtimeout_list_head_free = vtimeout_pool;

	/* Initialize PIT timer */
	vtimer_pit = pit_reserve();
	pit_enable_interrupt(vtimer_pit, vtimer_pit_irq_handler);
}

void vtimer_free(void)
{
	assert(vtimeout_list_head_scheduled == NULL);
	assert(vtimeout_list_tail_scheduled == NULL);

	pit_release(vtimer_pit);
}

vid_t vtimer_timeout(vcb_t cb, void *arg, uint32_t ticks)
{
	assert(cb != NULL);
	assert(ticks > 0U);

	irq_disable();

	struct vtimeout *vtimeout = vtimer_vtimeout_alloc();

	vtimeout->cb = cb;
	vtimeout->arg = arg;

	if (vtimeout_list_head_scheduled == NULL) {
		vtimeout->ticks = ticks;

		vtimeout_list_head_scheduled = vtimeout;
		vtimeout_list_tail_scheduled = vtimeout;
	} else {
		uint32_t counter_value = pit_get_current_value(vtimer_pit);
		pit_disable_timer(vtimer_pit);

		/* Adjust how many ticks left */
		if (counter_value > vtimeout_list_head_scheduled->ticks || pit_get_interrupt_flag(vtimer_pit)) {
			vtimeout_list_head_scheduled->ticks = 0U;
		} else {
			vtimeout_list_head_scheduled->ticks = counter_value;
		}

		uint32_t ticks_prefix_sum = 0U;

		struct vtimeout *vtimeout_pre = NULL;

		/* Walk the scheduled list and find after which vtimeout_pre
		   vtimeout should be inserted */
		for (struct vtimeout *vtimeout_it = vtimeout_list_head_scheduled;
				vtimeout_it != NULL;
				vtimeout_it = vtimeout_it->list_next) {
			if (ticks < ticks_prefix_sum + vtimeout_it->ticks)
				break; /* vtimeout should be inserted after vtimeout_pre */

			ticks_prefix_sum += vtimeout_it->ticks;
			vtimeout_pre = vtimeout_it;
		}

		/* Insert vtimeout into the list... */
		if (vtimeout_pre == NULL) {
			/* ... at the beginning */

			vtimeout->list_prev = NULL;

			if (vtimeout_list_head_scheduled != NULL)
				vtimeout_list_head_scheduled->list_prev = vtimeout;

			vtimeout->list_next = vtimeout_list_head_scheduled;

			vtimeout_list_head_scheduled = vtimeout;
		} else {
			/* ... simewhere in the middle or at the end */

			vtimeout->list_prev = vtimeout_pre;

			if (vtimeout_pre->list_next != NULL)
				vtimeout_pre->list_next->list_prev = vtimeout;

			vtimeout->list_next = vtimeout_pre->list_next;

			vtimeout_pre->list_next = vtimeout;
		}

		/* Adjust ticks values */
		vtimeout->ticks = ticks - ticks_prefix_sum;
		if (vtimeout->list_next != NULL)
			vtimeout->list_next->ticks -= vtimeout->ticks;
	}

	pit_set_start_value(vtimer_pit, vtimeout_list_head_scheduled->ticks);
	pit_enable_timer(vtimer_pit);

	irq_enable();

	return vtimeout->vid;
}

void vtimer_cancel(vid_t vid)
{
	struct vtimeout *vtimeout = vtimer_get_vtimeout(vid);

	irq_disable();

	if (vtimeout->cb != NULL && vtimeout->vid == vid) {
		if (vtimeout_list_head_scheduled == vtimeout) {
			/* Adjust how many ticks left */
			vtimeout->ticks = pit_get_current_value(vtimer_pit);
			pit_disable_timer(vtimer_pit);

			vtimeout_list_head_scheduled = vtimeout->list_next;

			if (vtimeout_list_head_scheduled != NULL) {
				vtimeout_list_head_scheduled->list_prev = NULL;

				/* Increment next vtimeout's ticks by removed vtimeout ticks */
				vtimeout_list_head_scheduled->ticks += vtimeout->ticks;

				pit_set_start_value(vtimer_pit, vtimeout_list_head_scheduled->ticks);
				pit_enable_timer(vtimer_pit);
			}
		} else {
			vtimeout->list_prev->list_next = vtimeout->list_next;

			if (vtimeout->list_next != NULL) {
				vtimeout->list_next->list_prev = vtimeout->list_prev;

				/* Increment next vtimeout's ticks by removed vtimeout ticks */
				vtimeout->list_next->ticks += vtimeout->ticks;
			}
		}

		vtimer_vtimeout_free(vtimeout);
	}

	irq_enable();
}
