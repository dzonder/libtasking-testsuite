#include "time.h"

#include "pit.h"

struct pit_desc *time_pit_high, *time_pit_low;

void time_init(void)
{
	time_pit_low = pit_reserve_chain(2);
	time_pit_high = pit_next_desc(time_pit_low);
	pit_set_start_value(time_pit_low, 0xFFFFFFFFU);
	pit_set_start_value(time_pit_high, 0xFFFFFFFFU);
	pit_enable_timer(time_pit_high);
	pit_enable_timer(time_pit_low);
}

uint64_t time_get(void)
{
	uint32_t ticks_low = pit_get_current_value(time_pit_low);
	uint32_t ticks_high = pit_get_current_value(time_pit_high);

	uint32_t ticks_low_new = pit_get_current_value(time_pit_low);
	if (ticks_low_new > ticks_low) {
		ticks_low = ticks_low_new;
		ticks_high = pit_get_current_value(time_pit_high);
	}

	return ~0ULL - (((uint64_t)ticks_high << 32) | ticks_low);
}
