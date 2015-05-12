#ifndef _PIT_H_
#define _PIT_H_

#include "utils.h"

struct pit_desc;

void pit_init(void);

struct pit_desc *pit_reserve(void);
struct pit_desc *pit_reserve_chain(uint8_t count);
struct pit_desc *pit_next_desc(struct pit_desc *pit);

void pit_release(struct pit_desc *pit);

void pit_set_start_value(struct pit_desc *pit, uint32_t val);
uint32_t pit_get_current_value(struct pit_desc *pit);
bool pit_get_interrupt_flag(struct pit_desc *pit);

void pit_enable_interrupt(struct pit_desc *pit, void (*handler)(void));

void pit_enable_timer(struct pit_desc *pit);
void pit_disable_timer(struct pit_desc *pit);

#endif /* _PIT_H_ */
