#include "led.h"

#include "gpio.h"
#include "utils.h"

#define LED_A1		PORTB, 18U
#define LED_A2		PORTB, 19U

void led_init(void)
{
	gpio_pin_mode(LED_A1, GPIO_MODE_MASK_ENABLE_OUTPUT);
	gpio_pin_mode(LED_A2, GPIO_MODE_MASK_ENABLE_OUTPUT);

	gpio_pin_set(LED_A1, 0U);
	gpio_pin_set(LED_A2, 0U);
}

void led_ctrl(enum led_status status)
{
	switch (status) {
	case LED_GREEN:
		gpio_pin_set(LED_A2, 0U);
		gpio_pin_set(LED_A1, 1U);
		break;
	case LED_RED:
		gpio_pin_set(LED_A1, 0U);
		gpio_pin_set(LED_A2, 1U);
		break;
	default:
		gpio_pin_set(LED_A1, 0U);
		gpio_pin_set(LED_A2, 0U);
		break;
	}
}
