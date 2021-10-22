#pragma once

#include <focaltech.h>
#include <Delegate.h>

using CapacitiveTouch = FocalTech_Class;
using TouchInterruptHandler = Delegate<void(CapacitiveTouch&)>;

bool initTouch(TouchInterruptHandler handler);

CapacitiveTouch& getTouch();
