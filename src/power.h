#pragma once

#include <Device.h>
#include <axp20x.h>

class Power : public Device<Power>, public AXP20X_Class
{
public:
	bool begin(Callback callback);
};
