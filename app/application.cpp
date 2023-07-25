#include <SmingCore.h>
#include <watch.h>
#include <power.h>
#include <rtc.h>
#include <axis.h>
#include <touch.h>
#include <backlight.h>
#include <gui.h>
#include <AnimatedGifTask.h>
#include <MillisTaskManager.h>

namespace
{
Timer mainTimer;
bool isReady = true;
WatchState watchState;
AnimatedGifTask* animation;

IMPORT_FSTR(gifData, PROJECT_DIR "/files/frog.gif")

// #define console Serial

#define CONSOLE_DBG(fmt, ...) console.printf(_F("%u " fmt "\r\n"), system_get_time(), ##__VA_ARGS__)

class Watch
{
public:
	Power power;
	RealTimeClock rtc;
	CapacitiveTouch touch;
	AxisSensor axis;
	BackLight backlight;
	Gui gui;
	MillisTaskManager taskManager;
};

Watch watch;

void onGuiReady(Gui& gui)
{
	// TODO: add here the rest...

	console.println("OK, GUI is ready.");
	// console.systemDebugOutput(true);
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
		CONSOLE_DBG("Power Connected");
	}
	if(power.isVbusRemoveIRQ()) {
		// TODO: emit EVENT_POWER_UNPLUGGED
		CONSOLE_DBG("Power Removed");
	}
	if(power.isPEKShortPressIRQ()) {
		// TODO: emit EVENT_POWER_SHORTPRESS
		CONSOLE_DBG("PowerKey Pressed");
		if(console.isPaused()) {
			console.pause(false);
		} else {
			console.print(">> PAUSE <<\r");
			console.pause(true);
		}
	}
	power.clearIRQ();
}

void onRtc(RealTimeClock& rtc)
{
	// TODO: emit EVENT_CLOCK
	CONSOLE_DBG("Got RTC alarm.");
	rtc.resetAlarm();

	if(animation == nullptr) {
		animation = new AnimatedGifTask(*watch.gui.getDisplay().createSurface(), gifData);
	}

	animation->resume();
}

void onTouch(CapacitiveTouch& touch)
{
	// TODO: emit EVENT_BACKLIGHT

	touch.getPoint(watchState.touchX, watchState.touchY);
	CONSOLE_DBG("Touched: X %u, Y %u", watchState.touchX, watchState.touchY);

	watch.backlight.reverse();

	if(animation == nullptr) {
		return;
	}

	if(watch.backlight.isOn()) {
		animation->resume();
	} else {
		animation->suspend();
	}
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

	CONSOLE_DBG("Axis: X %d, Y %d, Z %d", acc.x, acc.y, acc.z);

	// TODO: emit EVENT_ACCEL_COORD
}

void buttonUpdate()
{
}

void displayUpdate()
{
}

void powerAutoShutdownUpdate()
{
}

void cpuUsageUpdate()
{
}

void loop()
{
	watch.taskManager.running(millis());
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

	watch.taskManager.add(displayUpdate, 1);
	watch.taskManager.add(buttonUpdate, 10);
	watch.taskManager.add(powerAutoShutdownUpdate, 100);
	watch.taskManager.add(cpuUsageUpdate, 1000);

	mainTimer.initializeMs(1, loop).start();

	// TODO: emit EVENT_READY
}

} // namespace

void init()
{
	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true); // Allow debug output to serial

	initHardware();
}
