#pragma once

#include <pcf8563.h>
#include <Delegate.h>
#include "watch.h"

class RealTimeClock : public PCF8563_Class
{
public:
	using Callback = Delegate<void(RealTimeClock&)>;

	bool begin(Callback callback);

private:
	static void interruptHandler();

	static RealTimeClock* clock;
	static Callback callback;
};
