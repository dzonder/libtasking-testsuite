#include "buttons.h"

#include "vtimer.h"
#include "MK22F12.h"

#ifdef __TESTS
#include "tests.h"
#endif

const uint32_t SAMPLER_TIMEOUT = 60 * 1000 * 5; /* 5 ms */

static PORT_MemMapPtr port_base_ptrs[] = PORT_BASE_PTRS;
static GPIO_MemMapPtr gpio_base_ptrs[] = GPIO_BASE_PTRS;

static bool portInitialized[PORT_COUNT];
static bool pinInitialized[PORT_COUNT][PIN_COUNT];
static vid_t sampleVid[PORT_COUNT][PIN_COUNT];

struct handler {
	bool enabled;
	uint32_t lastTimestamp;
	void (*on_down)(uint32_t timestamp, void *usrData);
	void (*on_up)(uint32_t timestamp, void *usrData);
	void *usrData;
};

static struct handler handlers[PORT_COUNT][PIN_COUNT];

/*
 * Calculates difference between two timestamps.
 * Takes counter overflow into account (modulo 2^16 however).
 * Note: 'ts2' should be newer than 'ts1'
 */
uint32_t buttons_time_difference(uint32_t ts1, uint32_t ts2)
{
	return (ts2 >= ts1) ? (ts2 - ts1) : ((0xFFFF - ts1) + ts2);
}

static void buttons_pin_sampler(void *arg)
{
	enum port port = (uint32_t)arg >> 8;
	uint8_t pin = (uint32_t)arg & 0xFF;

	volatile uint32_t *gpio_pdir = &GPIO_PDIR_REG(gpio_base_ptrs[port]);
	struct handler *handler = &handlers[port][pin];

	uint32_t pdir1 = *gpio_pdir & (1U << pin);
	wait(100);
	uint32_t pdir2 = *gpio_pdir & (1U << pin);
	wait(100);
	uint32_t pdir3 = *gpio_pdir & (1U << pin);

	if (pdir1 != pdir2 || pdir2 != pdir3) {
		sampleVid[port][pin] = vtimer_timeout(buttons_pin_sampler,
				(void *)(uint32_t)(port << 8 | pin),
				SAMPLER_TIMEOUT);
	} else {
		void (*callback)(uint32_t timestamp, void *usrData) = NULL;
		void *usrData = handler->usrData;

		LPTMR0_CNR = 0x1; /* See 40.4.5 */
		uint32_t timestamp = LPTMR0_CNR;
		// TODO: use time_get() here

		bool isUp = pdir1 & (1U << pin);

		handler->lastTimestamp = timestamp;

		if (isUp && handler->on_up != NULL) {
			callback = handler->on_up;
		} else if (!isUp && handler->on_down != NULL) {
			callback = handler->on_down;
		}

		/* Execute callback */
		if (callback != NULL) {
#ifdef __TESTS
			tests_record_button(callback, timestamp, usrData);
#endif
			callback(timestamp, usrData);
		}
	}
}

static void buttons_port_handler(enum port port)
{
	volatile uint32_t *port_isfr = &PORT_ISFR_REG(port_base_ptrs[port]);

	for (uint8_t pin = 0U; pin < sizeof(*port_isfr) * 8; ++pin) {
		struct handler *handler = &handlers[port][pin];

		if (!(*port_isfr & (1U << pin)))
			continue;

		/* Clear interrupt flag */
		*port_isfr |= 1U << pin;

		if (!handler->enabled)
			continue;

		vtimer_cancel(sampleVid[port][pin]);
		sampleVid[port][pin] = vtimer_timeout(buttons_pin_sampler,
				(void *)(uint32_t)(port << 8 | pin),
				SAMPLER_TIMEOUT);
	}
}

extern void PORTA_IRQHandler() { buttons_port_handler(PORTA); }
extern void PORTB_IRQHandler() { buttons_port_handler(PORTB); }
extern void PORTC_IRQHandler() { buttons_port_handler(PORTC); }
extern void PORTD_IRQHandler() { buttons_port_handler(PORTD); }
extern void PORTE_IRQHandler() { buttons_port_handler(PORTE); }

static void buttons_init_port(enum port port)
{
	uint32_t sim_scgc5_port_mask = SIM_SCGC5_PORTA_MASK << port;

	SIM_SCGC5 |= sim_scgc5_port_mask;

	/* Enable Pin detect interrupt in NVIC */
	NVICICPR1 = 1U << (27U + port);
	NVICISER1 = 1U << (27U + port);

	portInitialized[port] = true;
}

void buttons_init(void)
{
	SIM_SCGC5 |= SIM_SCGC5_LPTIMER_MASK;

	/* Setup Low Power Timer for button event timestamps */
	LPTMR0_CSR = 0; /* Reset Low Power Timer */
	LPTMR0_CSR |= LPTMR_CSR_TFC_MASK; /* Free-running counter */
	// TODO: Change clock source to something more precise
	LPTMR0_PSR = /*LPTMR_PSR_PRESCALE(0b0011)*/ /* Divide by 16 */
		   /*|*/ LPTMR_PSR_PBYP_MASK /* Bypass prescaler */
		   | LPTMR_PSR_PCS(0b01); /* LPO as source */
	LPTMR0_CSR |= LPTMR_CSR_TEN_MASK; /* Enable LPTMR */
}

void buttons_disable(enum port port, uint8_t pin)
{
	assert(port < PORT_COUNT);
	assert(pin < PIN_COUNT);

	struct handler *handler = &handlers[port][pin];

	handler->enabled = false;
}

void buttons_disable_all(void)
{
	for (uint8_t port = 0U; port < PORT_COUNT; ++port)
		for (uint8_t pin = 0U; pin < PIN_COUNT; ++pin)
			buttons_disable(port, pin);
}

void buttons_register(enum port port, uint8_t pin,
		void (*on_down)(uint32_t timestamp, void *usrData),
		void (*on_up)(uint32_t timestamp, void *usrData),
		void *usrData)
{
	volatile uint32_t *port_pcr = &PORT_PCR_REG(port_base_ptrs[port], pin);
	struct handler *handler = &handlers[port][pin];

	if (!portInitialized[port])
		buttons_init_port(port);

	buttons_disable(port, pin);

	if (!pinInitialized[port][pin]) {
		/* Enable pin and configure as digital input */
		gpio_pin_mode(port, pin, GPIO_MODE_PIN_ENABLE);
		*port_pcr |= PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_PFE_MASK;
		*port_pcr |= PORT_PCR_IRQC(0b1011);

		wait(200); /* Wait for GPIO to initialize so we
			      don't generate invalid interrupt */

		pinInitialized[port][pin] = true;
	}

	handler->enabled = true;
	handler->lastTimestamp = ~0;
	handler->on_down = on_down;
	handler->on_up = on_up;
	handler->usrData = usrData;
}

struct press_desc {
	void (*on_short_press)(void *usrData);
	void (*on_long_press)(void *usrData);
	void *usrData;
	uint32_t timestampDown;
};

static struct press_desc pressDescriptors[PORT_COUNT][PIN_COUNT];

void handle_press_down(uint32_t timestamp, void *usrData)
{
	struct press_desc *pressDesc = (struct press_desc *)usrData;

	pressDesc->timestampDown = timestamp;
}

void handle_press_up(uint32_t timestamp, void *usrData)
{
	struct press_desc *pressDesc = (struct press_desc *)usrData;

	if (buttons_time_difference(pressDesc->timestampDown,
				timestamp) < PRESS_THRESHOLD) {
		if (pressDesc->on_short_press != NULL)
			pressDesc->on_short_press(pressDesc->usrData);
	} else {
		if (pressDesc->on_long_press != NULL)
			pressDesc->on_long_press(pressDesc->usrData);
	}
}

void buttons_register_press(enum port port, uint8_t pin,
		void (*on_short_press)(void *usrData),
		void (*on_long_press)(void *usrData),
		void *usrData)
{
	struct press_desc *pressDesc = &pressDescriptors[port][pin];

	pressDesc->on_short_press = on_short_press;
	pressDesc->on_long_press = on_long_press;
	pressDesc->usrData = usrData;

	buttons_register(port, pin, handle_press_down, handle_press_up, pressDesc);
}
