#pragma once

#include <pcf8563.h>
#include <Delegate.h>
#include "watch.h"

using RealTimeClock = PCF8563_Class;
using RealTimeClockCallback = Delegate<void(RealTimeClock&)>;

RealTimeClock* initRtc(WatchState& watchState);

RealTimeClock& getRtc();
