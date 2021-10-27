#pragma once

#include "Device.h"
#include <bma.h>

class AxisSensor : public Device<AxisSensor>, public BMA423
{
public:
	bool begin(Callback callback);
};
