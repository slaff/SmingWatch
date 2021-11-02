#include "pwmbase.h"
#include "driver/ledc.h"
#include <Digital.h>
#include <driver/ledc.h>

namespace
{
constexpr ledc_mode_t mode{LEDC_LOW_SPEED_MODE};
constexpr ledc_timer_t timerNum{LEDC_TIMER_0};
} // namespace

PWMBase::~PWMBase()
{
	ledc_stop(mode, ledc_channel_t(channel), 0);
	pinMode(pin, INPUT);
}

void PWMBase::begin()
{
	ledc_timer_config_t ledc_timer = {
		.speed_mode = mode,
		.duty_resolution = LEDC_TIMER_8_BIT,
		.timer_num = timerNum,
		.freq_hz = 12000,
		.clk_cfg = LEDC_AUTO_CLK, // Auto select the source clock
	};
	ledc_timer_config(&ledc_timer);

	ledc_channel_config_t ledc_channel = {
		.gpio_num = pin,
		.speed_mode = mode,
		.channel = ledc_channel_t(channel),
		.intr_type = LEDC_INTR_DISABLE,
		.timer_sel = timerNum,
		.duty = 100,
		.hpoint = 0,
	};
	ledc_channel_config(&ledc_channel);
	ledc_set_duty(mode, ledc_channel_t(channel), 0);
}

void PWMBase::adjust(uint8_t level)
{
	ledc_set_duty(mode, ledc_channel_t(channel), level);
	ledc_update_duty(mode, ledc_channel_t(channel));
}
