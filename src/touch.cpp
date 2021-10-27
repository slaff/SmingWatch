#include "touch.h"
#include <Digital.h>

TwoWire CapacitiveTouch::Wire1;

bool CapacitiveTouch::begin(Callback callback)
{
	if(class_ != nullptr) {
		return false;
	}

#if defined(TOUCH_RST)
	pinMode(TOUCH_RST, OUTPUT);
	digitalWrite(TOUCH_RST, LOW);
	delay(8);
	digitalWrite(TOUCH_RST, HIGH);
#endif

	Wire1.begin(TOUCH_SDA_PIN, TOUCH_SCL_PIN);
	if(!FocalTech_Class::begin(Wire1)) {
		debug_e("[TOUCH] begin FAIL");
		return false;
	}

	enableINT();
	/*
	 The time period of switching from active mode to monitor mode when there is no touching,
	 unit position, the manual indicates that the default value is 0xA,
	 and the default value is written here
	 */
	setMonitorTime(0x0A);
	/*
	 Report rate in monitor mode,
	 unit location, default value is 0x28, 40ms?
	 */
	//	touch->setMonitorPeriod(0x28);

	/*
	 Interrupt polling is only compatible with 2020-V1, 2020-V2, others are not currently adapted
	 */
	attachInterrupt(callback, TOUCH_INT_PIN);

	return true;
}
