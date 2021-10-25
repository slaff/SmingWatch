#pragma once

#include <bma.h>
#include <Delegate.h>
#include "watch.h"

using AxisSensor = BMA423;
using AxisInterruptHandler = Delegate<void(AxisSensor&)>;

AxisSensor* initAxis(WatchState& watchState);

AxisSensor& getAxis();
