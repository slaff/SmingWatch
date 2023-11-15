#include <SmingCore.h>

#include <watch.h>
#include <AnimatedGifTask.h>

// If you want, you can define WiFi settings globally in Eclipse Environment Variables
#ifndef WIFI_SSID
#define WIFI_SSID "PleaseEnterSSID" // Put your SSID and password here
#define WIFI_PWD "PleaseEnterPass"
#endif

namespace
{
Timer loopTimer;
Timer clockTimer;
bool isReady = true;
AnimatedGifTask* animation;
NtpClient* ntpClient;

IMPORT_FSTR(gifData, PROJECT_DIR "/files/frog.gif")

// #define console Serial

#define CONSOLE_DBG(fmt, ...) console.printf(_F("%u " fmt "\r\n"), system_get_time(), ##__VA_ARGS__)

Watch watch;

void onGuiReady(Gui& gui)
{
	// TODO: add here the rest...
	debug_i("OK, GUI is ready.");
}

void onPower(Power& power)
{
	if(power.readIRQ() != AXP_PASS) {
		debug_w("WARNING: Unable to read Power IRQ");
		return;
	}

	if(power.isVbusPlugInIRQ()) {
		watch.getEventManager().trigger(Watch::EventTypes::POWER_PLUGGED, Watch::EventData{
			.onOff = {
					.on = true
			}
		});
		CONSOLE_DBG("Power Connected");
	}
	if(power.isVbusRemoveIRQ()) {
		watch.getEventManager().trigger(Watch::EventTypes::POWER_PLUGGED, Watch::EventData{
			.onOff = {
					.on = false
			}
		});
		CONSOLE_DBG("Power Removed");
	}
	if(power.isPEKShortPressIRQ()) {
		watch.getEventManager().trigger(Watch::EventTypes::POWER_SHORTPRESS);
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
	watch.getEventManager().trigger(Watch::EventTypes::CLOCK);

	CONSOLE_DBG("Got RTC alarm.");
	rtc.resetAlarm();

	if(animation == nullptr) {
		animation = new AnimatedGifTask(*watch.gui.getDisplay().createSurface(), gifData);
	}

	animation->resume();
}

void onTouch(CapacitiveTouch& touch)
{
	touch.getPoint(watch.state.touchX, watch.state.touchY);
	CONSOLE_DBG("Touched: X %u, Y %u", watch.state.touchX, watch.state.touchY);

	watch.backlight.reverse();

	watch.getEventManager().trigger(Watch::EventTypes::BACKLIGHT, Watch::EventData{
		.backlight = {
				.lightPercentage = 100
		}
	});

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

	watch.getEventManager().trigger(Watch::EventTypes::ACCEL_COORD, Watch::EventData{
		.motion = {
			.accelerationX = acc.x,
			.accelerationY = acc.y,
			.accelerationZ = acc.z
		}
	});
}

void buttonUpdate()
{
}

void backlightDimmer()
{
	int32_t level = watch.backlight.getLevel();
	if(!level) {
		return;
	}

	if(watch.lastActive > micros() + 1000) {
		return;
	}

	level -= 30;
	if(level < 0) {
		level = 0;
	}

	watch.backlight.adjust(level);
	debug_d("Backlight level: %d", level);
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
	watch.getTaskManager().running(millis());
}

// Will be called when WiFi station timeout was reached
void connectFail(const String& ssid, MacAddress bssid, WifiDisconnectReason reason)
{
	debug_w("I'm NOT CONNECTED!");
}

void runEverySecond()
{
	watch.gui.update(watch);
}

void runOnce()
{
	runEverySecond();
	clockTimer.initializeMs<60000>(runEverySecond).start();
}

void onTimeUpdate(NtpClient& client, time_t ntpTime)
{
	DateTime dateTime = DateTime(ntpTime);

	watch.rtc.setDateTime(dateTime.Year, dateTime.Month, dateTime.Day, dateTime.Hour, dateTime.Minute, dateTime.Second);

	// TEST RTC alarms
	watch.rtc.disableAlarm();
	watch.rtc.setAlarmByMinutes(21);
	watch.rtc.enableAlarm();

	watch.gui.update(watch);
	clockTimer.stop();
	clockTimer.initializeMs((60 - dateTime.Second) * 1000, runOnce).startOnce();
}

void gotIP(IpAddress ip, IpAddress netmask, IpAddress gateway)
{
	// Set specific parameters if started by option 1 or 2
	// Set client to do automatic time requests every 60 seconds.
	// NOTE: you should have longer interval in a real world application
	// no need for query for time every 60 sec, should be at least 10 minutes or so.
	//	ntpClient.setAutoQueryInterval(60);
	//	ntpClient.setAutoQuery(true);
	//  ntpClient.setAutoUpdateSystemClock(true);
	// Request to update time now.
	// Otherwise the set interval will pass before time
	// is updated.
	//	ntpClient.requestTime();

	//  When using option 4 -> create client after connect OK
	//  ntpClient = new NtpClient("my_ntp_server", myrefreshinterval);

	//	When using Delegate Callback Option 2
	if(ntpClient == nullptr) {
		ntpClient = new NtpClient(onTimeUpdate);
		ntpClient->setAutoQuery(false);
		ntpClient->requestTime();
	}
}

void initHardware()
{
	watch.getEventManager().listen(Watch::EventTypes::SLEEP, [](const Watch::EventData& value, Watch& watch) {
		Serial.println("This will be executed last");
		return true;
	});

	watch.getEventManager().listen(
		Watch::EventTypes::WAKE_UP,
		[](const Watch::EventData& value, Watch& watch) {
			Serial.println("This must be executed first on wake up.");
			return true;
		},
		255);

	watch.getEventManager().listen(Watch::EventTypes::WAKE_UP, [](const Watch::EventData& value, Watch& watch) {
		Serial.println("S");
		return true;
	});

	watch.getEventManager().listen(Watch::EventTypes::WAKE_UP, [](const Watch::EventData& value, Watch& watch) {
		Serial.println("T");
		return true;
	});

	watch.getEventManager().listen(
		Watch::EventTypes::WAKE_UP,
		[](const Watch::EventData& value, Watch& watch) {
			Serial.println("200");
			return true;
		},
		200);

	watch.getEventManager().listen(
		Watch::EventTypes::WAKE_UP,
		[](const Watch::EventData& value, Watch& watch) {
			Serial.println("1");
			return true;
		},
		1);

	watch.getEventManager().listen(
		Watch::EventTypes::WAKE_UP,
		[](const Watch::EventData& value, Watch& watch) {
			Serial.println("201");
			return true;
		},
		201);

	watch.getEventManager().listen(
		Watch::EventTypes::WAKE_UP,
		[](const Watch::EventData& value, Watch& watch) {
			Serial.println("00");
			return true;
		},
		0);

	watch.getEventManager().trigger(Watch::EventTypes::SLEEP);

	watch.getEventManager().trigger(Watch::EventTypes::WAKE_UP);

	// initialize the main I2C bus
	Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

	// Start Watch Power Management
	watch.power.begin(onPower);

	// Station - WiFi client
	WifiStation.enable(true);
	WifiStation.config(WIFI_SSID, WIFI_PWD); // Put your SSID and password here

	WifiEvents.onStationDisconnect(connectFail);
	WifiEvents.onStationGotIP(gotIP);

	// Setup Real Time Clock (RTC)
	if(watch.rtc.begin(watch, onRtc)) {
	}

	// Setup Tocuh Pad
	watch.touch.begin(onTouch);

	// Setup Gyro
	watch.axis.begin(onAxis);

	// Setup Backlight
	watch.backlight.begin();
	watch.backlight.adjust(10);

	// Prepare GUI
	watch.gui.begin(watch, onGuiReady);

	// Add tasks to task manager
	watch.getTaskManager().add(displayUpdate, 1);
	watch.getTaskManager().add(buttonUpdate, 10);
	watch.getTaskManager().add(backlightDimmer, 300);
	watch.getTaskManager().add(powerAutoShutdownUpdate, 100);
	watch.getTaskManager().add(cpuUsageUpdate, 1000);

	watch.lastActive = micros();
	loopTimer.initializeMs(1, loop).start();

	// TODO: emit EVENT_READY
}

} // namespace

void init()
{
	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true); // Allow debug output to serial
	Serial.println("Starting ...");

	initHardware();
}
