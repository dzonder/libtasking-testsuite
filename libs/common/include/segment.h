#ifndef _SEGMENT_H_
#define _SEGMENT_H_

#include "utils.h"

#define SEGMENT_COUNT		8
#define DIGIT_COUNT		2

extern const uint8_t SEGMENT_DIGITS[];

/*
 * High-level access (auto-refresh-draw)
 *
 * uint8_t digit: represents digit to update
 * uint8_t leds:  represents segment leds as follows:
 *
 *      [ b7 b6 b5 b4 b3 b2 b1 b0 ]
 *      [ DP  G  F  E  D  C  B  A ]
 *
 *         .---A---.
 *         |       |
 *         F       B
 *         |       |
 *         +---G---+
 *         |       |
 *         E       C
 *         |       |   DP
 *         '---D---'   *
 *
 */
void segment_init(void);
void segment_set(uint8_t digit, uint8_t leds);
void segment_toggle(uint8_t digit, uint8_t leds);

/* Low-level access (thread-safe) */
void segment_low_init(void);
void segment_low_set_digit(uint8_t digit, uint8_t on);
void segment_low_set_segment(uint8_t segment, uint8_t on);

#endif /* _SEGMENT_H_ */
