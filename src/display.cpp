#include "display.h"

#ifdef ENABLE_VIRTUAL_SCREEN
#include <Graphics/Display/Virtual.h>
#else
#include <Graphics/Display/ST7789V.h>
#endif

namespace
{
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
constexpr uint8_t TOUCH_CS_PIN{Graphics::PIN_NONE};

HSPI::Controller spi(spiBus, spiPins);
Graphics::Display::ST7789V tft(spi, {240, 240});
#endif

} // namespace

Graphics::AbstractDisplay& display{tft};

bool initDisplay(DisplayCallback onDisplay)
{
#ifdef ENABLE_VIRTUAL_SCREEN
	if(!tft.begin()) {
		return false;
	}
	// tft.setMode(Display::Virtual::Mode::Debug);
	return true;
#else
	// Touch CS
	if(TOUCH_CS_PIN != Graphics::PIN_NONE) {
		pinMode(TOUCH_CS_PIN, OUTPUT);
		digitalWrite(TOUCH_CS_PIN, HIGH);
	}

	if(!spi.begin()) {
		return false;
	}

	/*
	 * ILI9341 min. clock cycle is 100ns write, 150ns read.
	 * In practice, writes work at 40MHz, reads at 27MHz.
	 * Attempting to read at 40MHz results in colour corruption.
	 */
	if(!tft.begin(TFT_PINSET, TFT_CS, TFT_DC_PIN, TFT_RESET_PIN, 40000000)) {
		return false;
	}

	onDisplay(tft);

	return true;
#endif
}
