#include <SmingCore.h>
#include "hardware.h"
#include "display.h"
#include "gui.h"

namespace
{
void onGuiReady(Graphics::Object* object)
{
	// TODO: add here the rest...
}

void initHardware()
{
	initPower();
	initDisplay();
}

} // namespace

void init()
{
	initHardware();
	initGui(onGuiReady);
}
