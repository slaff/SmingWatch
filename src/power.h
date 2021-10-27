#pragma once

#include <axp20x.h>
#include <Delegate.h>

class Power : public AXP20X_Class
{
public:
	using Callback = Delegate<void(Power&)>;

	bool begin(Callback callback);

private:
	static void interruptHandler();

	static Power* power;
	static Callback callback;
};
