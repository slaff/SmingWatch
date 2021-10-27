#pragma once

#include <stdint.h>

struct WatchState {
	bool backLight;
	uint32_t keyPress;
	uint16_t touchX;
	uint16_t touchY;
};
