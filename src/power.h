#pragma once

#include <axp20x.h>
#include <Delegate.h>

using Power = AXP20X_Class;
using PowerCallback = Delegate<void(Power&)>;

bool initPower(PowerCallback callback = nullptr);
