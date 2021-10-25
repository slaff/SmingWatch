#pragma once

#include <stdint.h>

struct WatchState {
	bool backLight;
	bool axisIrq;
	bool touchIrq;
	bool powerIrq;
	bool rtcIrq;
	uint32_t keyPress;
};

#ifdef INPUT_PULLUP
#undef INPUT_PULLUP
#define INPUT_PULLUP 0x05
#endif

#ifdef RISING
#undef RISING
#define RISING 0x01
#endif
