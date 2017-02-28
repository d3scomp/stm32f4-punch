//#include <asm/div64.h>
#include <string.h>
#include <stdlib.h>

#include "simulation.h"
#include "xorshift.h"
//#include "json.h"

static int sign(int64_t x)
{
	return x < 0 ? -1 : 1;
}

static inline int32_t abint32_t(int32_t x)
{
	return x < 0 ? -x : x;
}

#define signed_do_div(dividend, divisor) \
({ \
	int s1 = sign(dividend); \
	int s2 = sign(divisor); \
	uint64_t udividend = abs64(dividend); \
	uint32_t udivisor = abs(divisor); \
	udividend /= udivisor; \
	dividend = (int64_t)udividend * s1 * s2; \
})


/*#define signed_do_div(dividend, divisor) \
({ \
	int s1 = sign(dividend); \
	int s2 = sign(divisor); \
	uint64_t udividend = abint64_t(dividend); \
	uint32_t udivisor = abs(divisor); \
	do_div(udividend, udivisor); \
	dividend = (int64_t)udividend * s1 * s2; \
})*/

Axis::Axis(): power(0), encoder(0), velocity_um_s(0), headPos_nm(0) {}

void Axis::setEncoder() {
	unsigned int enc;
	enc = (unsigned int)(headPos_nm / (PunchPress::ERS_QENC_PERIOD_NM / 4)) % 4;

	if (enc == 2)
		enc = 3;
	else if (enc == 3)
		enc = 2;

	encoder = enc;
}

uint32_t Axis::getState(int vertical, int32_t maxAxisValue) {
	uint32_t result = encoder << (vertical ? 2 : 0);

	int32_t headPos = headPos_nm;

	if (headPos < 0)
		result |= State::US_SAFE_L << (vertical ? 2 : 0);
	if (headPos > maxAxisValue)
		result |= State::US_SAFE_R << (vertical ? 2 : 0);
	if (headPos < -PunchPress::ERS_TABLE_SAFE_ZONE || headPos > maxAxisValue + PunchPress::ERS_TABLE_SAFE_ZONE)
		result |= State::US_FAIL;
	
	return result;
}

uint32_t Axis::updateAxis(uint32_t us_period, int vertical, int32_t max_axis_value) {
	const int32_t friction = PunchPress::ERS_FRICTION_KOEF * PunchPress::ERS_HEAD_MASS_G;
	const int32_t velDecreasedByFriction = (friction * us_period) / 1000;

	int64_t motor_force = (power * 2500 - velocity_um_s) * 5; // division by 80 is ok, 400 / 80 = 5, 1000000 / 80 = 12500
	int64_t random_force_factor = motor_force * (xorshift_rand() % 1000);

	signed_do_div(random_force_factor, 100000); // divide random_force_factor by 100000 -> (motor_force * (xorshift_rand() % 1000)) / 100000
	motor_force += random_force_factor;

	int64_t vel_change = motor_force * us_period;
	signed_do_div(vel_change, 1000000); // divide vel_change by 1000000 -> (int32_t)((motor_force * us_period) / 1000000)
	velocity_um_s += (int32_t)vel_change;


	if (abint32_t(velocity_um_s) <= 1) {
		velocity_um_s = 0;
	} else {
		if (abint32_t(velocity_um_s) <= velDecreasedByFriction) {
			velocity_um_s = 0;
		} else {
			if (velocity_um_s < 0) {
				velocity_um_s += velDecreasedByFriction;
			} else {
				velocity_um_s -= velDecreasedByFriction;
			}
		}
	}
	
	int64_t headPosChange = (int64_t)velocity_um_s * (int64_t)us_period;
	signed_do_div(headPosChange, 1000); // divide head_pos_change by 1000 -> (int32_t)((int64_t)vel * (int64_t)us_period / 1000)
	headPos_nm += (int32_t)headPosChange;

	setEncoder();

	return getState(vertical, max_axis_value);
}

PunchPress::PunchPress():
	irqEnabled(false), failed(false), punch(false), remainingPunchTime_ns(0), punchedPunches(0) {
	x.setEncoder();
	y.setEncoder();

	x.headPos_nm = xorshift_rand() % ERS_TABLE_PUNCH_AREA_WIDTH;
	y.headPos_nm = xorshift_rand() % ERS_TABLE_PUNCH_AREA_HEIGHT;

	xorshift_srand();
}

void PunchPress::setPos(int32_t x_nm, int32_t y_nm) {
	x.headPos_nm = x_nm;
	y.headPos_nm = y_nm;
}

State PunchPress::update(uint32_t us_period) {
	uint32_t retval;

	if (!failed) {
		retval = x.updateAxis(us_period, 0, ERS_TABLE_PUNCH_AREA_WIDTH);
		retval |= y.updateAxis(us_period, 1, ERS_TABLE_PUNCH_AREA_HEIGHT);
		
		if (retval & State::US_FAIL) {
			failed = 1;
		}
	
		if (punch) {
			if (remainingPunchTime_ns > 0 || abint32_t(x.velocity_um_s) > ERS_PUNCH_MAX_VEL_UM_S || abint32_t(y.velocity_um_s) > ERS_PUNCH_MAX_VEL_UM_S) {
				failed = 1;
				retval |= State::US_FAIL;
				return retval;
			}

			lastPunch.xPos = x.headPos_nm;
			lastPunch.yPos = y.headPos_nm;
			remainingPunchTime_ns = ERS_PUNCH_DURATION_MS * 1000;

//			mb(); // decrease the number of punches only after the punch has started
#warning mb() omitted !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

			punch = 0;

		} else {
			if (remainingPunchTime_ns > 0) {
				remainingPunchTime_ns -= us_period;

				if (abint32_t(x.velocity_um_s) > ERS_PUNCH_MAX_VEL_UM_S || abint32_t(y.velocity_um_s) > ERS_PUNCH_MAX_VEL_UM_S) {
					failed = 1;
					retval |= State::US_FAIL;
					return retval;
				}

				if (remainingPunchTime_ns <= 0)
				{
					retval |= State::US_HEAD_UP;
					remainingPunchTime_ns = 0;
					++punchedPunches;
				}
			} else {
				retval |= State::US_HEAD_UP;
			}
		}

	} else {
		retval = x.getState(0, ERS_TABLE_PUNCH_AREA_WIDTH);
		retval |= y.getState(1, ERS_TABLE_PUNCH_AREA_HEIGHT);

		if (remainingPunchTime_ns == 0) {
			retval |= State::US_HEAD_UP;
		}

		retval |= State::US_FAIL;
	}

	return State(retval);
}

State::State(TYPE state): state(state) {}

bool State::getEncXA() {
	return state & US_ENC_X0;
}

bool State::getEncXB() {
	return state & US_ENC_X1;
}

bool State::getEncYA() {
	return state & US_ENC_Y0;
}

bool State::getEncYB() {
	return state & US_ENC_Y1;
}

bool State::getSafeLeft() {
	return state & US_SAFE_L;
}

bool State::getSafeRight() {
	return state & US_SAFE_R;
}

bool State::getSafeTop() {
	return state & US_SAFE_T;
}

bool State::getSafeBottom() {
	return state & US_SAFE_B;
}

bool State::getHeadUp() {
	return state & US_HEAD_UP;
}

bool State::getFail() {
	return state & US_FAIL;
}
