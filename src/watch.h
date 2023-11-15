#pragma once

#include <cstdint>

#include <power.h>
#include <rtc.h>
#include <axis.h>
#include <touch.h>
#include <backlight.h>
#include <gui.h>
#include <MillisTaskManager.h>
#include <EventManager.h>

class Watch
{
public:
	enum class EventTypes {
		SLEEP,
		WAKE_UP,
		POWER_PLUGGED,
		POWER_SHORTPRESS,
		CLOCK,
		BACKLIGHT,
		ACCEL_COORD,
		READY
	};

	struct BoolEvent {
		bool on;
	};

	// Motion Event
	struct MotionEvent {
		int16_t accelerationX;
		int16_t accelerationY;
		int16_t accelerationZ;
	};

	// Battery Event
	struct BatteryEvent {
		int batteryLevel;
	};

	// Highlight Event
	struct BacklightEvent {
		uint8_t lightPercentage;
	};

	// Union Type
	union EventData {
		BoolEvent onOff;
		MotionEvent motion;
		BatteryEvent battery;
		BacklightEvent backlight;
	};

	using EventManagerType = EventManager<EventTypes, Watch, EventData>;

	Power power;
	RealTimeClock rtc;
	CapacitiveTouch touch;
	AxisSensor axis;
	BackLight backlight;
	Gui gui;

	struct State {
		uint32_t keyPress;
		uint16_t touchX;
		uint16_t touchY;
	};

	State state;
	unsigned long lastActive{0};

	Watch() : eventManager(new EventManagerType(*this))
	{
	}

	~Watch()
	{
		delete eventManager;
	}

	EventManagerType& getEventManager()
	{
		return *eventManager;
	}

	MillisTaskManager& getTaskManager()
	{
		return taskManager;
	}

private:
	MillisTaskManager taskManager;
	EventManagerType* eventManager = nullptr;
};
