#include "segment.h"

#include "gpio.h"
#include "vtimer.h"

#define SEGMENT_REDRAW_INTERVAL		400000

struct pinout {
	enum port port;
	int pin;
};

static const struct pinout pinoutSegments[] = {
	{ PORTB, 2u  },		/* A */
	{ PORTC, 17u },		/* B */
	{ PORTB, 11u },		/* C */
	{ PORTD, 0u  },		/* D */
	{ PORTB, 17u },		/* E */
	{ PORTC, 1u  },		/* F */
	{ PORTB, 1u  },		/* G */
	{ PORTB, 16u },		/* DP */
};

static const struct pinout pinoutDigits[] = {
	{ PORTC, 2u  },		/* D1 */
	{ PORTB, 3u  },		/* D2 */
};

const uint8_t SEGMENT_DIGITS[] = {
	0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F,
};

static uint8_t digits[DIGIT_COUNT];
static uint8_t currentDigit;

static void segment_init_pins(const struct pinout pinout[], int count)
{
	/* Enable port clocks and GPIO mux for pins */
	for (int i = 0; i < count; ++i) {
		enum port port = pinout[i].port;
		int pin = pinout[i].pin;
		gpio_pin_mode(port, pin, GPIO_MODE_MASK_ENABLE_OUTPUT);
		gpio_pin_set(port, pin, 1U);
	}
}

static void segment_redraw(void *arg)
{
	uint8_t leds = digits[currentDigit];

	for (int i = 0; i < DIGIT_COUNT; ++i)
		segment_low_set_digit(i, i == currentDigit);

	for (int i = 0; i < SEGMENT_COUNT; ++i, leds >>= 1)
		segment_low_set_segment(i, leds & 1);

	currentDigit = (currentDigit + 1) % DIGIT_COUNT;

	vtimer_timeout(segment_redraw, NULL, SEGMENT_REDRAW_INTERVAL);
}

void segment_init(void)
{
	segment_low_init();
	segment_redraw(NULL);
}

void segment_set(uint8_t digit, uint8_t leds)
{
	digits[digit] = leds;
}

void segment_toggle(uint8_t digit, uint8_t leds)
{
	digits[digit] ^= leds;
}

void segment_low_init(void)
{
	segment_init_pins(pinoutSegments, SEGMENT_COUNT);
	segment_init_pins(pinoutDigits, DIGIT_COUNT);
}

void segment_low_set_digit(uint8_t digit, uint8_t on)
{
	gpio_pin_set(pinoutDigits[digit].port, pinoutDigits[digit].pin, !on);
}

void segment_low_set_segment(uint8_t segment, uint8_t on)
{
	gpio_pin_set(pinoutSegments[segment].port, pinoutSegments[segment].pin, !on);
}
