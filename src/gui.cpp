#include "gui.h"
#include "display.h"
#include <Graphics/RenderQueue.h>
#include <Graphics/TextBuilder.h>

using namespace Graphics;

namespace
{

RenderQueue* renderQueue;
AbstractDisplay* display;

constexpr Orientation portrait{Orientation::deg180};
constexpr Orientation landscape{Orientation::deg270};

void render(RenderQueue::Completed callback)
{
	if(display == nullptr) {
		return;
	}

	display->setOrientation(portrait);

	auto size = display->getSize();
	auto scene = new SceneObject(size, F("Color Tests"));
	scene->clear();

	Color color[] = {Color::RED, Color::GREEN, Color::BLUE, Color::WHITE};
	uint8_t numColors = ARRAY_SIZE(color);

	auto height = size.h / 4;
	auto width = size.w / numColors;

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

	if(renderQueue == nullptr) {
		renderQueue = new RenderQueue(*display);
	}

	renderQueue->render(scene, callback);
}

} // namespace gui

void initGui(RenderQueue::Completed callback)
{
	display = getDisplay();

	render(callback);
}
