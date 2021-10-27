#pragma once

#include "Device.h"
#include <focaltech.h>

class CapacitiveTouch : public Device<CapacitiveTouch>, public FocalTech_Class
{
public:
	bool begin(Callback callback);

private:
	static TwoWire Wire1;
};
