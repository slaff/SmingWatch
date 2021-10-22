#pragma once

#include <pcf8563.h>
#include <Delegate.h>

using RealTimeClock = PCF8563_Class;
using RealTimeClockCallback = Delegate<void(RealTimeClock&)>;

bool initRtc(RealTimeClockCallback handler = nullptr);

RealTimeClock& getRtc();
