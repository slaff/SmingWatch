#include <Wire.h>
#include "power.h"

namespace
{
Power* power;

} // namespace

bool initPower(PowerCallback callback)
{
	if(power != nullptr) {
		return false;
	}

	power = new Power();
	int ret = power->begin();
	if(ret == AXP_FAIL) {
		debug_e("AXP Power begin failed");
		delete power;
		power = nullptr;
		return false;
	}

	//Change the shutdown time to 4 seconds
	power->setShutdownTime(AXP_POWER_OFF_TIME_4S);
	// Turn off the charging instructions, there should be no
	power->setChgLEDMode(AXP20X_LED_OFF);
	// Turn off external enable
	power->setPowerOutPut(AXP202_EXTEN, false);
	//axp202 allows maximum charging current of 1800mA, minimum 300mA
	power->setChargeControlCur(300);

	power->setLDO2Voltage(3300);

	//In the 2020V1 version, the ST7789 chip power supply
	//is shared with the backlight, so LDO2 cannot be turned off
	power->setPowerOutPut(AXP202_LDO2, AXP202_ON);

	if(callback) {
		callback(*power);
	}

	return true;
}
