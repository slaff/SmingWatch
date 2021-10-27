#include "rtc.h"
#include <Wire.h>

RealTimeClock* RealTimeClock::clock;
RealTimeClock::Callback RealTimeClock::callback;

bool RealTimeClock ::begin(Callback callback)
{
	if(clock != nullptr) {
		return false;
	}

	if(!PCF8563_Class::begin()) {
		debug_e("[RTC] Init failed");
		return false;
	}

	clock = this;
	this->callback = callback;

	pinMode(RTC_INT_PIN, INPUT_PULLUP);
	attachInterrupt(RTC_INT_PIN, interruptHandler, FALLING);

	disableAlarm();

	return true;
}

void IRAM_ATTR RealTimeClock::interruptHandler()
{
	if(callback != nullptr) {
		System.queueCallback(InterruptCallback([]() { callback(*clock); }));
	}
}
