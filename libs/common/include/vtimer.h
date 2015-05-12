#ifndef _VTIMER_H_
#define _VTIMER_H_

#include "utils.h"

#define VID_INVALID		(vid_t)(-1)

typedef uint64_t vid_t;
typedef void (*vcb_t)(void *arg);

void vtimer_init(void);
void vtimer_free(void);

vid_t vtimer_timeout(vcb_t cb, void *arg, uint32_t ticks);

void vtimer_cancel(vid_t vid);

#endif /* _VTIMER_H_ */
