#pragma once

#include <focaltech.h>
#include <Delegate.h>
#include "watch.h"

using CapacitiveTouch = FocalTech_Class;
using TouchInterruptHandler = Delegate<void(CapacitiveTouch&)>;

CapacitiveTouch* initTouch(WatchState& watchState);

CapacitiveTouch& getTouch();
