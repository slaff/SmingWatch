#pragma once

#include <driver/ledc.h>

class PWMBase
{
public:
	static constexpr ledc_mode_t mode{LEDC_LOW_SPEED_MODE};
	static constexpr ledc_timer_t timerNum{LEDC_TIMER_0};

	PWMBase(uint8_t pin, uint8_t channel) : pin(pin), channel(channel)
	{
	}

	virtual ~PWMBase();
	virtual void begin();
	virtual void adjust(uint8_t level);

protected:
	uint8_t pin;
	uint8_t channel;
};
