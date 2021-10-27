#pragma once

#include "pwmbase.h"
#include "config.h"

class BackLight : public PWMBase
{
public:
	BackLight() : PWMBase(BACKLIGHT_PIN, 0)
	{
	}

	uint8_t getLevel()
	{
		return level;
	}

	void adjust(uint8_t level)
	{
		PWMBase::adjust(level);
		this->level = level;
		enabled = true;
	}

	bool isOn()
	{
		return enabled;
	};

	void on()
	{
		adjust(level);
		enabled = true;
	}

	void off()
	{
		PWMBase::adjust(0);
		enabled = false;
	}

	bool reverse()
	{
		if(enabled) {
			off();
		} else {
			on();
		}
		return enabled;
	}

private:
	bool enabled{false};
	uint8_t level{255};
};
