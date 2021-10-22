#pragma once

#include <Graphics/RenderQueue.h>
#include <Graphics/AbstractDisplay.h>

void initGui(Graphics::AbstractDisplay& display, Graphics::RenderQueue::Completed callback);
