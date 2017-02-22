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

#define ERS_TABLE_SAFE_ZONE 20000000 //nm /* this is zone around the punching area, exiting this zone with the center of the punching head leads to failure */
#define ERS_TABLE_FAIL_ZONE 20000000 //nm /* this is the area around the punching area which is still displayed */

#define ERS_TABLE_PUNCH_AREA_WIDTH 1500000000 /* nm */
#define ERS_TABLE_PUNCH_AREA_HEIGHT 1000000000 /* nm */

#define ERS_TABLE_FULL_WIDTH (ERS_TABLE_PUNCH_AREA_WIDTH + 2 * (ERS_TABLE_SAFE_ZONE + ERS_TABLE_FAIL_ZONE)) /* nm */
#define ERS_TABLE_FULL_HEIGHT (ERS_TABLE_PUNCH_AREA_HEIGHT + 2 * (ERS_TABLE_SAFE_ZONE + ERS_TABLE_FAIL_ZONE)) /* nm */

#define ERS_HEAD_MASS_G 2000
#define ERS_FRICTION_KOEF 0.1


#define ERS_PUNCH_DURATION_MS 100
#define ERS_PUNCH_MAX_VEL_UM_S	100

// The lenght in mm after which the four combinations of the encoder again repeat
#define ERS_QENC_PERIOD_NM 1000000 // in nanometers

Axis::Axis(): power(0), encoder(0), velocity_um_s(0), headPos_nm(0) {}

void Axis::setEncoder() {
	unsigned int enc;
	enc = (unsigned int)(headPos_nm / (ERS_QENC_PERIOD_NM / 4)) % 4;

	if (enc == 2)
		enc = 3;
	else if (enc == 3)
		enc = 2;

	encoder = enc;
}

uint32_t Axis::getState(int vertical, int32_t max_axis_value) {
	uint32_t result = encoder << (vertical ? 2 : 0);

	int32_t head_pos = headPos_nm;

	if (head_pos < 0)
		result |= US_SAFE_L << (vertical ? 2 : 0);
	if (head_pos > max_axis_value)
		result |= US_SAFE_R << (vertical ? 2 : 0);
	if (head_pos < -ERS_TABLE_SAFE_ZONE || head_pos > max_axis_value + ERS_TABLE_SAFE_ZONE)
		result |= US_FAIL;
	
	return result;
}

uint32_t Axis::updateAxis(uint32_t us_period, int vertical, int32_t max_axis_value) {
	int64_t head_pos_change;

	int32_t friction = ERS_FRICTION_KOEF * ERS_HEAD_MASS_G;
	int32_t vel_decreased_by_friction = (friction * us_period) / 1000;

	int32_t vel = velocity_um_s;
	int64_t vel_change;
	int64_t motor_force = (power * 2500 - vel) * 5; // division by 80 is ok, 400 / 80 = 5, 1000000 / 80 = 12500
	int64_t random_force_factor = motor_force * (xorshift_rand() % 1000);

	signed_do_div(random_force_factor, 100000); // divide random_force_factor by 100000 -> (motor_force * (xorshift_rand() % 1000)) / 100000
	motor_force += random_force_factor;

	vel_change = motor_force * us_period;
	signed_do_div(vel_change, 1000000); // divide vel_change by 1000000 -> (int32_t)((motor_force * us_period) / 1000000)
	vel += (int32_t)vel_change;


	if (abint32_t(vel) <= 1) {
		vel = 0;
	} else {
		int32_t vel_decr = vel_decreased_by_friction;
		if (abint32_t(vel) <= vel_decr) {
			vel = 0;
		} else {
			if (vel < 0) {
				vel += vel_decr;
			} else {
				vel -= vel_decr;
			}
		}
	}

	velocity_um_s = vel;
	
	head_pos_change = (int64_t)vel * (int64_t)us_period;
	signed_do_div(head_pos_change, 1000); // divide head_pos_change by 1000 -> (int32_t)((int64_t)vel * (int64_t)us_period / 1000)
	headPos_nm += (int32_t)head_pos_change;

	setEncoder();

	return getState(vertical, max_axis_value);
}

PunchPress::PunchPress(): useInitPosition(false), initPosX(0), initPosY(0) {}

uint32_t PunchPress::update(uint32_t us_period) {
	uint32_t retval;

	if (!failed) {
		retval = x.updateAxis(us_period, 0, ERS_TABLE_PUNCH_AREA_WIDTH);
		retval |= y.updateAxis(us_period, 1, ERS_TABLE_PUNCH_AREA_HEIGHT);
		
		if (retval & US_FAIL) {
			failed = 1;
		}
	
		if (punch) {
			if (remainingPunchTime_ns > 0 || abint32_t(x.velocity_um_s) > ERS_PUNCH_MAX_VEL_UM_S || abint32_t(y.velocity_um_s) > ERS_PUNCH_MAX_VEL_UM_S) {
				failed = 1;
				retval |= US_FAIL;
				return retval;
			}

			lastPunch.x_pos = x.headPos_nm;
			lastPunch.y_pos = y.headPos_nm;
			remainingPunchTime_ns = ERS_PUNCH_DURATION_MS * 1000;

//			mb(); // decrease the number of punches only after the punch has started
#warning mb() omitted !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

			punch = 0;

		} else {
			if (remainingPunchTime_ns > 0) {
				remainingPunchTime_ns -= us_period;

				if (abint32_t(x.velocity_um_s) > ERS_PUNCH_MAX_VEL_UM_S || abint32_t(y.velocity_um_s) > ERS_PUNCH_MAX_VEL_UM_S) {
					failed = 1;
					retval |= US_FAIL;
					return retval;
				}

				if (remainingPunchTime_ns <= 0)
				{
					retval |= US_HEAD_UP;
					remainingPunchTime_ns = 0;
					++punchedPunches;
				}
			} else {
				retval |= US_HEAD_UP;
			}
		}

	} else {
		retval = x.getState(0, ERS_TABLE_PUNCH_AREA_WIDTH);
		retval |= x.getState(1, ERS_TABLE_PUNCH_AREA_HEIGHT);

		if (remainingPunchTime_ns == 0) {
			retval |= US_HEAD_UP;
		}

		retval |= US_FAIL;
	}

	return retval;
}

void PunchPress::initCommon() {
	x.setEncoder();
	y.setEncoder();
}

void PunchPress::reinit() {
	if (useInitPosition) {
		x.headPos_nm = initPosX;
		y.headPos_nm = initPosY;
	} else {
		x.headPos_nm = xorshift_rand() % ERS_TABLE_PUNCH_AREA_WIDTH;
		y.headPos_nm = xorshift_rand() % ERS_TABLE_PUNCH_AREA_HEIGHT;
	}

	initCommon();
}

void PunchPress::init() {
	xorshift_srand();
	reinit();
}

void PunchPress::reset() {
	memset(this, 0, sizeof(*this));
	initCommon();
}

