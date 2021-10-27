#pragma once

#include "Device.h"
#include <pcf8563.h>
#include "watch.h"

class RealTimeClock : public Device<RealTimeClock>, public PCF8563_Class
{
public:
	bool begin(Callback callback);
};
