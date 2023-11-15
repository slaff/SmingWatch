#pragma once

#include "Device.h"
#include <pcf8563.h>

class Watch;

class RealTimeClock : public Device<RealTimeClock>, public PCF8563_Class
{
public:
	bool begin(Watch& watch, Callback callback);
};
