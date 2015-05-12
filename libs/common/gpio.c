#include "gpio.h"

#include "MK22F12.h"

static PORT_MemMapPtr portBasePtrs[] = PORT_BASE_PTRS;
static GPIO_MemMapPtr gpioBasePtrs[] = GPIO_BASE_PTRS;

void gpio_pin_mask_mode(enum port port, uint32_t pin_mask, uint8_t mode_mask)
{
	/* This function does not support ENABLE/DISABLE modes */
	assert(!(mode_mask & GPIO_MODE_PORT_ENABLE) && !(mode_mask & GPIO_MODE_PORT_DISABLE));
	assert(!(mode_mask & GPIO_MODE_PIN_ENABLE) && !(mode_mask & GPIO_MODE_PIN_DISABLE));

	/* Only one of INPUT or OUTPUT can be set in the mask */
	assert(!(mode_mask & GPIO_MODE_INPUT && mode_mask & GPIO_MODE_OUTPUT));

	volatile uint32_t *gpioPddr = &GPIO_PDDR_REG(gpioBasePtrs[port]);

	if (mode_mask & GPIO_MODE_INPUT) {
		*gpioPddr &= ~pin_mask;
	} else if (mode_mask & GPIO_MODE_OUTPUT) {
		*gpioPddr |= pin_mask;
	}
}

void gpio_pin_mode(enum port port, uint8_t pin, uint8_t mode_mask)
{
	/* Only one of ENABLE or DISABLE can be set in the mask */
	assert(!(mode_mask & GPIO_MODE_PORT_ENABLE && mode_mask & GPIO_MODE_PORT_DISABLE));

	uint32_t sim_scgc5_port_mask = SIM_SCGC5_PORTA_MASK << port;

	if (mode_mask & GPIO_MODE_PORT_ENABLE) {
		SIM_SCGC5 |= sim_scgc5_port_mask;
	} else if (mode_mask & GPIO_MODE_PORT_DISABLE) {
		SIM_SCGC5 &= ~sim_scgc5_port_mask;
	}

	/* One should not disable port and enable pin on the port */
	assert(!(mode_mask & GPIO_MODE_PIN_ENABLE && mode_mask & GPIO_MODE_PORT_DISABLE));
	assert(!(mode_mask & GPIO_MODE_PIN_ENABLE && mode_mask & GPIO_MODE_PIN_DISABLE));

	volatile uint32_t *portPcr = &PORT_PCR_REG(portBasePtrs[port], pin);

	if (mode_mask & GPIO_MODE_PIN_ENABLE) {
		SET_MUX(*portPcr, MUX_GPIO);
	} else if (mode_mask & GPIO_MODE_PIN_DISABLE) {
		SET_MUX(*portPcr, MUX_ANALOG);
	}

	mode_mask &= ~(GPIO_MODE_PORT_ENABLE | GPIO_MODE_PORT_DISABLE);
	mode_mask &= ~(GPIO_MODE_PIN_ENABLE | GPIO_MODE_PIN_DISABLE);

	gpio_pin_mask_mode(port, 1U << pin, mode_mask);
}

void gpio_pin_mask_set(enum port port, uint32_t pin_mask, uint8_t high)
{
	if (high) {
		GPIO_PSOR_REG(gpioBasePtrs[port]) |= pin_mask;
	} else {
		GPIO_PCOR_REG(gpioBasePtrs[port]) |= pin_mask;
	}
}

void gpio_pin_set(enum port port, uint8_t pin, uint8_t high)
{
	gpio_pin_mask_set(port, 1U << pin, high);
}
