#include "power.h"
#include "config.h"
#include <Wire.h>

bool Power::begin(Callback callback)
{
	if(class_ != nullptr) {
		return false;
	}

	int err = AXP20X_Class::begin();
	if(err == AXP_FAIL) {
		debug_e("AXP Power begin failed");
		return false;
	}

	// Change the shutdown time to 4 seconds
	setShutdownTime(AXP_POWER_OFF_TIME_4S);
	// Turn off the charging instructions, there should be no
	setChgLEDMode(AXP20X_LED_OFF);
	// Turn off external enable
	setPowerOutPut(AXP202_EXTEN, false);
	// axp202 allows maximum charging current of 1800mA, minimum 300mA
	setChargeControlCur(300);

	setLDO2Voltage(3300);

	// In the 2020V1 version, the ST7789 chip power supply
	// is shared with the backlight, so LDO2 cannot be turned off
	setPowerOutPut(AXP202_LDO2, AXP202_ON);

	enableIRQ(AXP202_PEK_SHORTPRESS_IRQ | AXP202_VBUS_REMOVED_IRQ | AXP202_VBUS_CONNECT_IRQ | AXP202_CHARGING_IRQ,
			  true);
	clearIRQ();

	attachInterrupt(callback, POWER_INT_PIN);

	return true;
}
