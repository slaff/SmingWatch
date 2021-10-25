#pragma once

#include <axp20x.h>
#include <Delegate.h>
#include "watch.h"

using Power = AXP20X_Class;
using PowerCallback = Delegate<void(Power&)>;

Power* initPower(WatchState& watchState);
