#include "rtc.h"
#include <Wire.h>

namespace
{
RealTimeClock* rtc = nullptr;

} // namespace

bool initRtc(RealTimeClockCallback callback)
{
	if(rtc != nullptr) {
		return false;
	}

	rtc = new RealTimeClock();
	if(!rtc->begin()) {
		debug_e("Failed initializing the real time clock!");
		delete rtc;
		rtc = nullptr;
		return false;
	}

	if(callback != nullptr) {
		callback(*rtc);
	}

	return true;
}

RealTimeClock& getRtc()
{
	return *rtc;
}
