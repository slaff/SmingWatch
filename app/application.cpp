#include <SmingCore.h>
#include <watch.h>
#include <power.h>
#include <rtc.h>
#include <axis.h>
#include <touch.h>
#include <backlight.h>
#include <gui.h>

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
	BackLight backlight;
	Gui gui;
};

Watch watch;

void onGuiReady(Gui& gui)
{
	// TODO: add here the rest...

	console.println("OK, GUI is ready.");
	debug_i("OK, GUI is ready.");
}

void onPower(Power& power)
{
	if(power.readIRQ() != AXP_PASS) {
		debug_w("WARNING: Unable to read Power IRQ");
		return;
	}

	if(power.isVbusPlugInIRQ()) {
		// TODO: emit EVENT_POWER_PLUGGEDIN
		Serial.println("Power Connected");
	}
	if(power.isVbusRemoveIRQ()) {
		// TODO: emit EVENT_POWER_UNPLUGGED
		Serial.println("Power Removed");
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
	// TODO: emit EVENT_BACKLIGHT

	touch.getPoint(watchState.touchX, watchState.touchY);
	debug_d("Touched: X %u, Y %u", watchState.touchX, watchState.touchY);

	watch.backlight.reverse();
}

void onAxis(AxisSensor& axis)
{
	axis.getINT();

	// Get acceleration data
	Accel acc;
	bool res = axis.getAccel(acc);
	if(!res) {
		debug_e("FAILED: getAccel");
		return;
	}

	debug_d("Axis: X %d, Y %d, Z %d", acc.x, acc.y, acc.z);

	// TODO: emit EVENT_ACCEL_COORD
}

void loop()
{
	if(!isReady) {
		return;
	}

	isReady = false;

	// TODO: get accelerometer XYZ coordinates and store it in watch state

	// TODO: get touch coordinates and store it in watch state

	// TODO: Run JSVM loop

	// TODO: Run GUI loop

	isReady = true;
}

void initHardware()
{
	Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN); // this is the main I2C bus

	watch.power.begin(onPower);

	if(watch.rtc.begin(onRtc)) {
		// TEST RTC alarms
		watch.rtc.disableAlarm();

		watch.rtc.setDateTime(2021, 10, 26, 19, 20, 00);
		watch.rtc.setAlarmByMinutes(21);

		watch.rtc.enableAlarm();
	}

	watch.touch.begin(onTouch);
	watch.axis.begin(onAxis);
	watch.backlight.begin();
	watch.backlight.adjust(10);

	watch.gui.begin(onGuiReady);

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
