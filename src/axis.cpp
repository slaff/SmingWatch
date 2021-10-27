#include "axis.h"
#include <Digital.h>
#include <Platform/System.h>

AxisSensor* AxisSensor::sensor;
AxisSensor::Callback AxisSensor::callback;

namespace
{
//
// Low-level I2C Communication
// Provided to BMA423_Library communication interface
//
uint16_t readRegister(uint8_t address, uint8_t reg, uint8_t* data, uint16_t len)
{
	Wire.beginTransmission(address);
	Wire.write(reg);
	Wire.endTransmission(false);
	Wire.requestFrom(address, uint8_t(len), uint8_t(true));
	uint8_t i = 0;
	while(Wire.available()) {
		if(i >= len) {
			return 1 << 14;
		}
		data[i++] = Wire.read();
	}
	return 0; //Pass
}

uint16_t writeRegister(uint8_t address, uint8_t reg, uint8_t* data, uint16_t len)
{
	Wire.beginTransmission(address);
	Wire.write(reg);
	Wire.write(data, len);
	auto ret = Wire.endTransmission();
	return ret ? 1 << 12 : ret;
}

} // namespace

bool AxisSensor::begin(Callback callback)
{
	if(sensor != nullptr) {
		debug_e("[AXIS] Already initialised");
		return false;
	}

	if(!BMA423::begin(readRegister, writeRegister, delay)) {
		debug_e("BMA423 was not found. Please check wiring/power. ");
		return false;
	}

	sensor = this;
	this->callback = callback;

	bma423_axes_remap remap_data = {
		.x_axis = 0,
		.x_axis_sign = 1,
		.y_axis = 1,
		.y_axis_sign = 0,
		.z_axis = 2,
		.z_axis_sign = 1,
	};
	setRemapAxes(&remap_data);

	// Configure the BMA423 accelerometer
	Acfg cfg = {
		.odr = BMA4_OUTPUT_DATA_RATE_100HZ,
		.bandwidth = BMA4_ACCEL_NORMAL_AVG4,
		.perf_mode = BMA4_CONTINUOUS_MODE,
		.range = BMA4_ACCEL_RANGE_2G,
	};
	setAccelConfig(cfg);

	// Enable BMA423 accelerometer
	enableAccel();

	// The correct trigger interrupt needs to be configured as needed
	bma4_int_pin_config config = {
		.edge_ctrl = BMA4_LEVEL_TRIGGER,
		.lvl = BMA4_ACTIVE_HIGH,
		.od = BMA4_PUSH_PULL,
		.output_en = BMA4_OUTPUT_ENABLE,
		.input_en = BMA4_INPUT_DISABLE,
	};
	setINTPinConfig(config, BMA4_INTR1_MAP);

	pinMode(AXIS_INT_PIN, INPUT);
	attachInterrupt(AXIS_INT_PIN, interruptHandler, RISING);

	// Enable BMA423 isStepCounter feature
	enableFeature(BMA423_STEP_CNTR, true);
	// Enable BMA423 isTilt feature
	enableFeature(BMA423_TILT, true);
	// Enable BMA423 isDoubleClick feature
	enableFeature(BMA423_WAKEUP, true);

	// Reset steps
	resetStepCounter();

	// Turn on feature interrupt
	enableStepCountInterrupt();
	enableTiltInterrupt();
	// It corresponds to isDoubleClick interrupt
	enableWakeupInterrupt();

	return true;
}

void IRAM_ATTR AxisSensor::interruptHandler()
{
	if(callback != nullptr) {
		System.queueCallback(InterruptCallback([]() { callback(*sensor); }));
	}
}
