#include <SmingCore.h>
#include "watch.h"
#include "power.h"
#include "rtc.h"
#include "axis.h"
#include "touch.h"
#include "display.h"
#include "gui.h"

namespace
{
Timer mainTimer;
bool isReady = true;
WatchState watchState;

class Watch
{
public:
	Power power;
	RealTimeClock rtc;
	CapacitiveTouch touch;
	AxisSensor axis;
};

Watch watch;

void onGuiReady(Graphics::Object* object)
{
	// TODO: add here the rest...
}

void onPower(Power& power)
{
	if(power.readIRQ() != AXP_PASS) {
		debug_w("WARNING: Unable to read Power IRQ");
		return;
	}

	if(power.isVbusPlugInIRQ()) {
		// TODO: emit EVENT_POWER_PLUGGEDIN
		Serial.println("Power Plug In");
	}
	if(power.isVbusRemoveIRQ()) {
		// TODO: emit EVENT_POWER_UNPLUGGED
		Serial.println("Power Remove");
	}
	if(power.isPEKShortPressIRQ()) {
		// TODO: emit EVENT_POWER_SHORTPRESS
		Serial.println("PowerKey Pressed");
	}
	power.clearIRQ();
}

void onRtc(RealTimeClock& rtc)
{
	// TODO: emit EVENT_CLOCK
	debug_d("Got RTC alarm.");
	rtc.resetAlarm();
}

void onTouch(CapacitiveTouch& touch)
{
	debug_d("Touched!");
	digitalWrite(BACKLIGHT_PIN, watchState.backLight);
	watchState.backLight = !watchState.backLight;

	touch.getPoint(watchState.touchX, watchState.touchY);

	debug_d("X: %d, Y: %d", watchState.touchX, watchState.touchY);
	// TODO: emit EVENT_BACKLIGHT
}

void onAxis(AxisSensor& axis)
{
	Accel acc;

	// Get acceleration data
	bool res = axis.getAccel(acc);
	if(!res) {
		debug_e("FAILED: getAccel");
		return;
	}

	Serial.printf("X: %d, Y: %d, Z: %d\n", acc.x, acc.y, acc.z);
	// TODO: emit EVENT_ACCEL_COORD
}

void loop()
{
	if(!isReady) {
		return;
	}

	isReady = false;

	isReady = true;
}

void initHardware()
{
	Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN); // this is the main I2C bus

	if(!watch.power.begin(onPower)) {
		return;
	}

	if(!watch.rtc.begin(onRtc)) {
		return;
	}

	// TEST RTC alarms
	watch.rtc.disableAlarm();

	watch.rtc.setDateTime(2021, 10, 26, 19, 20, 00);
	watch.rtc.setAlarmByMinutes(21);

	watch.rtc.enableAlarm();

	watch.touch.begin(onTouch);
	watch.axis.begin(onAxis);

	initDisplay([](Graphics::AbstractDisplay& display) { initGui(display, onGuiReady); });

	mainTimer.initializeMs(10, loop).start();

	// TODO: emit EVENT_READY
}

} // namespace

void init()
{
	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true); // Allow debug output to serial

	initHardware();
}
