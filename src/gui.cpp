#include "gui.h"
#include <Graphics/RenderQueue.h>
#include <Graphics/TextBuilder.h>

#ifdef ENABLE_VIRTUAL_SCREEN
#include <Graphics/Display/Virtual.h>
#else
#include <Graphics/Display/ST7789V.h>
#endif

using namespace Graphics;

namespace
{
Graphics::Size guiSize{240, 240};

#ifdef ENABLE_VIRTUAL_SCREEN
Graphics::Display::Virtual tft;
#else
constexpr HSPI::SpiBus spiBus{HSPI::SpiBus::DEFAULT};
constexpr HSPI::PinSet TFT_PINSET{HSPI::PinSet::normal};
HSPI::SpiPins spiPins = {
	.sck = 18,
	.miso = HSPI::SPI_PIN_NONE,
	.mosi = 19,
};
constexpr uint8_t TFT_CS{5};
constexpr uint8_t TFT_RESET_PIN{Graphics::PIN_NONE};
constexpr uint8_t TFT_DC_PIN{27};

HSPI::Controller spi(spiBus, spiPins);
Graphics::Display::ST7789V tft(spi, guiSize);
#endif

RenderQueue renderQueue(tft);

constexpr Orientation portrait{Orientation::deg180};
constexpr Orientation landscape{Orientation::deg270};

void drawTestCard(RenderQueue::Completed callback)
{
	tft.setOrientation(portrait);

	auto scene = new SceneObject(guiSize, F("Color Tests"));
	scene->clear();

	Color color[] = {Color::RED, Color::GREEN, Color::BLUE, Color::WHITE};
	uint8_t numColors = ARRAY_SIZE(color);

	auto height = guiSize.h / 4;
	auto width = guiSize.w / numColors;

	TextBuilder text(*scene);
	text.setColor(Color::White, Color::Black);
	text.setScale(2);
	text.setTextAlign(Align::Centre);
	text.setLineAlign(Align::Centre);
	for(unsigned i = 0; i < numColors; i++) {
		for(unsigned j = 0; j < 4; j++) {
			PixelBuffer pix{color[i]};
			auto scale = [&](uint8_t& c) { c = c * (j + 1) / 5; };
			scale(pix.bgr24.r);
			scale(pix.bgr24.g);
			scale(pix.bgr24.b);
			Rect r(i * width, j * height, width, height);
			scene->fillRect(pix.color, r);

			text.setClip(r);
			text.printf("%c%c", 'A' + i, '0' + j);
		}
	}
	text.commit(*scene);

	renderQueue.render(scene, callback);
}

} // namespace

Graphics::Console console(tft, renderQueue);

bool Gui::begin(Gui::Callback callback)
{
#ifdef ENABLE_VIRTUAL_SCREEN
	if(!tft.begin(guiSize.w, guiSize.h)) {
		return false;
	}
	// tft.setMode(Display::Virtual::Mode::Debug);
#else
	if(!spi.begin()) {
		return false;
	}

	if(!tft.begin(TFT_PINSET, TFT_CS, TFT_DC_PIN, TFT_RESET_PIN, 40000000)) {
		return false;
	}

#endif

	drawTestCard([this, callback](Object*) { callback(*this); });

	return true;
}
