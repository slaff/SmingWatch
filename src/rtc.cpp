#include "rtc.h"
#include "config.h"
#include <Wire.h>
#include "watch.h"

bool RealTimeClock::begin(Watch& watch, Callback callback)
{
	if(class_ != nullptr) {
		return false;
	}

	if(!PCF8563_Class::begin()) {
		debug_e("[RTC] Init failed");
		return false;
	}

	disableAlarm();

	attachInterrupt(callback, RTC_INT_PIN);

	return true;
}
