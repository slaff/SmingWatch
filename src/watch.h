#pragma once

#include <stdint.h>

struct WatchState {
	bool backLight;
	bool touchIrq;
	bool powerIrq;
	bool rtcIrq;
	uint32_t keyPress;
	uint16_t touchX;
	uint16_t touchY;
};
