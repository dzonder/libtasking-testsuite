#ifndef _LED_H_
#define _LED_H_

enum led_status {
	LED_OFF = 0,
	LED_GREEN,
	LED_RED,
};

void led_init(void);

void led_ctrl(enum led_status status);

#endif /* _LED_H_ */
