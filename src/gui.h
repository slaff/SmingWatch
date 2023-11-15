#pragma once

#include <Graphics/Console.h>
#include <Graphics/AbstractDisplay.h>

extern Graphics::Console console;

class Watch;

class Gui
{
public:
	using Callback = Delegate<void(Gui& gui)>;

	bool begin(Watch& watch, Callback callback = nullptr);

	void update(Watch& watch, Callback callback = nullptr);

	Graphics::AbstractDisplay& getDisplay();
};
