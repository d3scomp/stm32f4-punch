#pragma once

#include <stdint.h>




//////////////////////////////// CUSTOM HACKS


#define abs64(x) ({                             \
                 int64_t __x = (x);                  \
                 (__x < 0) ? -__x : __x;         \
         })


#define __do_div64





struct Punch {
	int32_t x_pos; // x position of the punch, nm from left size of the
	int32_t y_pos;
};

class Axis {
public:
	int8_t power; // -128 .. 127 speed
	uint8_t encoder; // quadratic encoder
	int32_t velocity; // head velocity on the axis um/s
	int32_t head_pos; // head position on the axis (nanometers)
};

#define SESSION_ID_LENGTH	10

class PunchPress {
public:
	int use_init_pos;
	int32_t x_init_pos;
	int32_t y_init_pos;

	Axis x_axis;
	Axis y_axis;

	uint8_t irq_enabled; // 1 means that interrupts are delivered
	uint8_t failed; // 1 means that we are in FAIL state
	uint8_t punch; // 1 means that punch was requested	

	int32_t remaining_punch_time; // time remaining to complete the current punch (ns)
	Punch last_punch;
	uint32_t punched_punches; // number of punched punches
};

#define US_NONE		0
#define US_ENC_X0	(1 << 0)
#define US_ENC_X1	(1 << 1)
#define US_ENC_Y0	(1 << 2)
#define US_ENC_Y1	(1 << 3)
#define US_SAFE_L	(1 << 4)
#define US_SAFE_R	(1 << 5)
#define US_SAFE_T	(1 << 6)
#define US_SAFE_B	(1 << 7)
#define US_HEAD_UP	(1 << 8)
#define US_FAIL		(1 << 9)

void pp_init(PunchPress * pp);
void pp_reinit(PunchPress * pp);
void pp_reset(PunchPress * pp);
uint32_t pp_update(PunchPress * pp, uint32_t us_period);
