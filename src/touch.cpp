#include "touch.h"
#include <Digital.h>

namespace
{
CapacitiveTouch* touch = nullptr;
TwoWire Wire1;

} // namespace

bool initTouch(TouchInterruptHandler touchHandler)
{
	if(touch != nullptr) {
		return false;
	}

#if defined(TOUCH_RST)
	pinMode(TOUCH_RST, OUTPUT);
	digitalWrite(TOUCH_RST, LOW);
	delay(8);
	digitalWrite(TOUCH_RST, HIGH);
#endif

	touch = new CapacitiveTouch();
	Wire1.begin(TOUCH_SDA_PIN, TOUCH_SCL_PIN);
	if(!touch->begin(Wire1)) {
		debug_e("Begin touch FAIL");
		delete touch;
		touch = nullptr;
		return false;
	}

	touch->enableINT();
	/*
	 The time period of switching from active mode to monitor mode when there is no touching,
	 unit position, the manual indicates that the default value is 0xA,
	 and the default value is written here
	 */
	touch->setMonitorTime(0x0A);
	/*
	 Report rate in monitor mode,
	 unit location, default value is 0x28, 40ms?
	 */
	//	touch->setMonitorPeriod(0x28);

	/*
	 Interrupt polling is only compatible with 2020-V1, 2020-V2, others are not currently adapted
	 */
	pinMode(TOUCH_INT_PIN, INPUT);
	attachInterrupt(
		TOUCH_INT_PIN, [touchHandler]() { touchHandler(*touch); }, FALLING);

	return true;
}

CapacitiveTouch& getTouch()
{
	return *touch;
}
