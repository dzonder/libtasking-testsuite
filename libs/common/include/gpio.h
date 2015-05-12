#ifndef _GPIO_H_
#define _GPIO_H_

#include "utils.h"

enum port {
	PORTA = 0U,
	PORTB,
	PORTC,
	PORTD,
	PORTE,
};

enum {
	GPIO_MODE_PORT_ENABLE	= 1U << 0U,
	GPIO_MODE_PORT_DISABLE	= 1U << 1U,
	GPIO_MODE_PIN_ENABLE	= 1U << 2U,
	GPIO_MODE_PIN_DISABLE	= 1U << 3U,
	GPIO_MODE_INPUT		= 1U << 4U,
	GPIO_MODE_OUTPUT	= 1U << 5U,
};

#define GPIO_MODE_MASK_ENABLE		\
	(GPIO_MODE_PORT_ENABLE | GPIO_MODE_PIN_ENABLE)

#define GPIO_MODE_MASK_ENABLE_OUTPUT	\
	(GPIO_MODE_MASK_ENABLE | GPIO_MODE_OUTPUT)

void gpio_pin_mask_mode(enum port port, uint32_t pin_mask, uint8_t mode_mask);
void gpio_pin_mode(enum port port, uint8_t pin, uint8_t mode_mask);

void gpio_pin_mask_set(enum port port, uint32_t pin_mask, uint8_t high);
void gpio_pin_set(enum port port, uint8_t pin, uint8_t high);

#endif /* _GPIO_H_ */
