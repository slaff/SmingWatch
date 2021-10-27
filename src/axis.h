#pragma once

#include <bma.h>
#include <Delegate.h>

class AxisSensor : public BMA423
{
public:
	using Callback = Delegate<void(AxisSensor& axis)>;

	bool begin(Callback callback);

private:
	static void interruptHandler();

	static AxisSensor* sensor;
	static Callback callback;
};
