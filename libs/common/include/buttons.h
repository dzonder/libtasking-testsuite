#ifndef _BUTTONS_H_
#define _BUTTONS_H_

#include "utils.h"
#include "gpio.h"

#define PRESS_THRESHOLD		600U /* in miliseconds */

void buttons_init(void);

void buttons_disable(enum port port, uint8_t pin);
void buttons_disable_all(void);

void buttons_register(enum port port, uint8_t pin,
		void (*on_down)(uint32_t timestamp, void *usrData),
		void (*on_up)(uint32_t timestamp, void *usrData),
		void *usrData);
void buttons_register_press(enum port port, uint8_t pin,
		void (*on_short_press)(void *usrData),
		void (*on_long_press)(void *usrData),
		void *usrData);

uint32_t buttons_time_difference(uint32_t ts1, uint32_t ts2);

#endif /* _BUTTONS_H_ */
