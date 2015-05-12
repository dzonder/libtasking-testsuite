#include "pit.h"

#include "MK22F12.h"

#define CHANNEL_COUNT 4

struct pit_desc {
	uint8_t channel;
	bool reserved;
	void (*handler)(void);
};

static struct pit_desc channels[CHANNEL_COUNT];

static void pit_irq_default_handler(void)
{
	assert(false);
}

static void pit_irq_handler(uint8_t channel)
{
	/* Reset interrupt */
	PIT_TFLG_REG(PIT_BASE_PTR, channel) |= PIT_TFLG_TIF_MASK;

	channels[channel].handler();
}

extern void PIT0_IRQHandler() { pit_irq_handler(0); }
extern void PIT1_IRQHandler() { pit_irq_handler(1); }
extern void PIT2_IRQHandler() { pit_irq_handler(2); }
extern void PIT3_IRQHandler() { pit_irq_handler(3); }

void pit_init(void)
{
	/* Initialize channel descriptors */
	for (int i = 0; i < CHANNEL_COUNT; ++i) {
		struct pit_desc *pit = &channels[i];
		pit->channel = i;
		pit->handler = pit_irq_default_handler;
		pit->reserved = false;
	}

	/* Enable PIT module and set freeze in Debug mode */
	SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;

	PIT_MCR = PIT_MCR_FRZ_MASK; /* Timers are stopped when in Debug mode */

	/* PIT is clocked by Bus clock (60 MHz) */
}

struct pit_desc *pit_reserve(void)
{
	for (int i = 0; i < CHANNEL_COUNT; ++i) {
		struct pit_desc *pit = &channels[i];
		if (!pit->reserved) {
			pit->reserved = true;
			return pit;
		}
	}

	/* No free PIT channel found */
	assert(false);

	return NULL;
}

static int pit_find_chain(uint8_t count)
{
	for (int i = 0; i <= CHANNEL_COUNT - count; ++i) {
		for (int j = i; j - i < count; ++j) {
			struct pit_desc *pit = &channels[j];
			if (pit->reserved) {
				break;
			} else if (j - i + 1 == count) {
				return i;
			}
		}
	}

	return -1;
}

struct pit_desc *pit_reserve_chain(uint8_t count)
{
	assert(count > 1);

	int first_pit = pit_find_chain(count);
	if (first_pit == -1) {
		assert(false);
		return NULL;
	}

	for (uint32_t i = first_pit; i < first_pit + count; ++i) {
		struct pit_desc *pit = &channels[i];
		assert(!pit->reserved);

		pit->reserved = true;

		if (i != first_pit)
			PIT_TCTRL_REG(PIT_BASE_PTR, pit->channel) |= PIT_TCTRL_CHN_MASK;
	}

	return &channels[first_pit];
}

struct pit_desc *pit_next_desc(struct pit_desc *pit)
{
	return pit + 1;
}

void pit_set_start_value(struct pit_desc *pit, uint32_t val)
{
	PIT_LDVAL_REG(PIT_BASE_PTR, pit->channel) = PIT_LDVAL_TSV(val - 1);
}

uint32_t pit_get_current_value(struct pit_desc *pit)
{
	return PIT_CVAL_REG(PIT_BASE_PTR, pit->channel);
}

bool pit_get_interrupt_flag(struct pit_desc *pit)
{
	return PIT_TFLG_REG(PIT_BASE_PTR, pit->channel) & PIT_TFLG_TIF_MASK;
}

void pit_enable_interrupt(struct pit_desc *pit, void (*handler)(void))
{
	pit->handler = handler;

	/* Enable PIT channel n interrupt in NVIC */
	NVICICPR1 = 1u << (16u + pit->channel);
	NVICISER1 = 1u << (16u + pit->channel);

	PIT_TCTRL_REG(PIT_BASE_PTR, pit->channel) |= PIT_TCTRL_TIE_MASK;
}

static void pit_disable_interrupt(struct pit_desc *pit)
{
	PIT_TCTRL_REG(PIT_BASE_PTR, pit->channel) &= ~PIT_TCTRL_TIE_MASK;

	/* Disable PIT channel n interrupt in NVIC */
	NVICICER1 = 1u << (16u + pit->channel);
	NVICICPR1 = 1u << (16u + pit->channel);

	pit->handler = pit_irq_default_handler;
}

void pit_enable_timer(struct pit_desc *pit)
{
	PIT_TCTRL_REG(PIT_BASE_PTR, pit->channel) |= PIT_TCTRL_TEN_MASK;
}

void pit_disable_timer(struct pit_desc *pit)
{
	PIT_TCTRL_REG(PIT_BASE_PTR, pit->channel) &= ~PIT_TCTRL_TEN_MASK;
}

void pit_release(struct pit_desc *pit)
{
	pit_disable_timer(pit);
	pit_disable_interrupt(pit);
	pit->reserved = false;
}
