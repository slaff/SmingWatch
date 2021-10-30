#pragma once

#include <Graphics/RenderQueue.h>
#include <Graphics/AbstractDisplay.h>

class Gui
{
public:
	using Callback = Delegate<void(Gui& gui)>;

	bool begin(Callback callback);
};
