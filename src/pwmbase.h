#pragma once

#include <cstdint>

class PWMBase
{
public:
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
