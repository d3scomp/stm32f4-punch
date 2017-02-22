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

/**
 * @brief Axis
 *
 * This holds one axis settings and state.
 */
class Axis {
public:
	Axis();
	static const int8_t MAX_POWER = 127;
	static const int8_t MIN_POWER = -128;

	void setEncoder();

	uint32_t getState(int vertical, int32_t max_axis_value);

	uint32_t updateAxis(uint32_t us_period, int vertical, int32_t max_axis_value);

	/**
	 * @brief Current power
	 *
	 * @see MAX_POWER
	 * @see MIN_POWER
	 */
	int8_t power;

	/**
	 * @brief Quadratic encoder state
	 *
	 * First 2 bits valid ???
	 */
	uint8_t encoder;

	/**
	 * @brief Head velocity on the axis
	 *
	 * In um/s.
	 */
	int32_t velocity_um_s;

	/**
	 * @brief Head position
	 *
	 * In nanometers
	 */
	int32_t headPos_nm;
};

/**
 * @brief PunchPress state
 */
class State {
private:
	typedef uint32_t TYPE;

public:
	static const TYPE US_NONE = 0;
	static const TYPE  US_ENC_X0	= (1 << 0);
	static const TYPE  US_ENC_X1	= (1 << 1);
	static const TYPE  US_ENC_Y0	= (1 << 2);
	static const TYPE  US_ENC_Y1	= (1 << 3);
	static const TYPE  US_SAFE_L	= (1 << 4);
	static const TYPE  US_SAFE_R	= (1 << 5);
	static const TYPE  US_SAFE_T	= (1 << 6);
	static const TYPE  US_SAFE_B	= (1 << 7);
	static const TYPE  US_HEAD_UP = (1 << 8);
	static const TYPE  US_FAIL = (1 << 9);

	State(TYPE state);

	bool getEncXA();
	bool getEncXB();
	bool getEncYA();
	bool getEncYB();

	bool getSafeLeft();
	bool getSafeRight();
	bool getSafeTop();
	bool getSafeBottom();

	bool getHeadUp();

	bool getFail();

private:
	TYPE state;
};

/**
 * @brief The main PunchPress class
 *
 * This holds punch press state and configuration
 */
class PunchPress {
public:
	PunchPress();

	State update(uint32_t us_period);
	void setPos(int32_t x_nm, int32_t y_nm);

	Axis x;
	Axis y;

	/**
	 * @brief Whenever interrupts are delivered
	 */
	bool irqEnabled;

	/**
	 * @brief Whenever we are in failed state`
	 */
	bool failed;

	/**
	 * @brief Whenever punch was requested
	 */
	bool punch;

	/**
	 * @brief Time remaining to complete the current punch (ns)
	 */
	int32_t remainingPunchTime_ns;

	Punch lastPunch;

	/**
	 * @brief Number of punched punches
	 */
	uint32_t punchedPunches;
};


