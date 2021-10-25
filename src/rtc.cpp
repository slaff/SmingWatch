#include "rtc.h"
#include <Wire.h>

namespace
{
RealTimeClock* rtc = nullptr;

} // namespace

RealTimeClock* initRtc(WatchState& watchState)
{
	if(rtc != nullptr) {
		return rtc;
	}

	rtc = new RealTimeClock();
	if(!rtc->begin()) {
		debug_e("Failed initializing the real time clock!");
		delete rtc;
		rtc = nullptr;
	}

	pinMode(RTC_INT_PIN, INPUT_PULLUP);
	attachInterrupt(
		RTC_INT_PIN, [&watchState] { watchState.rtcIrq = true; }, FALLING);

	rtc->disableAlarm();

	return rtc;
}

RealTimeClock& getRtc()
{
	return *rtc;
}
