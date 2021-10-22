#include <SmingCore.h>
#include "power.h"
#include "rtc.h"
#include "axis.h"
#include "touch.h"
#include "display.h"
#include "gui.h"

namespace
{
bool backLightState = false;

void onGuiReady(Graphics::Object* object)
{
	// TODO: add here the rest...
}

void IRAM_ATTR onTouch(CapacitiveTouch& touch)
{
	debug_d("Touched!");
	digitalWrite(BACKLIGHT_PIN, backLightState);
	backLightState = !backLightState;
}

void IRAM_ATTR onAxis(AxisSensor& axis)
{
	static uint8_t prevRotation = 0;
	debug_d("Tilted!");
	uint8_t rotation = axis.getDirection();
	if(prevRotation != rotation) {
		prevRotation = rotation;
		switch(rotation) {
		case DIRECTION_DISP_DOWN:
			Serial.println("DIRECTION_DISP_DOWN");
			break;
		case DIRECTION_DISP_UP:
			Serial.println("DIRECTION_DISP_UP");
			break;
		case DIRECTION_BOTTOM_EDGE:
			Serial.println("DIRECTION_BOTTOM_EDGE");
			break;
		case DIRECTION_TOP_EDGE:
			Serial.println("DIRECTION_TOP_EDGE");
			break;
		case DIRECTION_RIGHT_EDGE:
			Serial.println("DIRECTION_RIGHT_EDGE");
			break;
		case DIRECTION_LEFT_EDGE:
			Serial.println("DIRECTION_LEFT_EDGE");
			break;
		default:
			Serial.println("ERROR!!!");
			break;
		}
	}
}

void onRtcReady(RealTimeClock& rtc)
{
	// TODO: set the correct time...
	// TODO: set alarms, etc.
	debug_d("RTC ready! Timer is: %s", (rtc.isTimerActive() ? "active" : "inactive"));
}

void initHardware()
{
	Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN); // this is the main I2S bus

	initPower();
	initRtc(onRtcReady);
	initTouch(onTouch);
	initAxis(onAxis);
	initDisplay([](Graphics::AbstractDisplay& display) { initGui(display, onGuiReady); });
}

} // namespace

void init()
{
	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true); // Allow debug output to serial

	initHardware();
}
