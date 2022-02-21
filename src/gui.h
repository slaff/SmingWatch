#pragma once

#include <Graphics/Console.h>
#include <Graphics/AbstractDisplay.h>

extern Graphics::Console console;

class Gui
{
public:
	using Callback = Delegate<void(Gui& gui)>;

	bool begin(Callback callback);

	Graphics::AbstractDisplay& getDisplay();
};
