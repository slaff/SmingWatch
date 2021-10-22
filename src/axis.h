#pragma once

#include <bma.h>
#include <Delegate.h>

using AxisSensor = BMA423;
using AxisInterruptHandler = Delegate<void(AxisSensor&)>;

bool initAxis(AxisInterruptHandler handler);

AxisSensor& getAxis();
