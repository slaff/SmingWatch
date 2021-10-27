#pragma once

#include <focaltech.h>
#include <Delegate.h>
#include "watch.h"

class CapacitiveTouch : public FocalTech_Class
{
public:
	using InterruptCallback = Delegate<void(CapacitiveTouch&)>;

	bool begin(InterruptCallback callback);

private:
	static void interruptHandler();

	static CapacitiveTouch* touch;
	static InterruptCallback callback;
	static TwoWire Wire1;
};
