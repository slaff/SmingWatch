#include "axis.h"
#include <Digital.h>

namespace
{
AxisSensor* axis = nullptr;

//
// Low-level I2C Communication
// Provided to BMA423_Library communication interface
//
uint16_t readRegister(uint8_t address, uint8_t reg, uint8_t* data, uint16_t len)
{
	Wire.beginTransmission(address);
	Wire.write(reg);
	Wire.endTransmission();
	Wire.requestFrom((uint8_t)address, (uint8_t)len);
	uint8_t i = 0;
	while(Wire.available()) {
		data[i++] = Wire.read();
	}
	return 0; //Pass
}

uint16_t writeRegister(uint8_t address, uint8_t reg, uint8_t* data, uint16_t len)
{
	Wire.beginTransmission(address);
	Wire.write(reg);
	Wire.write(data, len);
	return (0 != Wire.endTransmission());
}

} // namespace

AxisSensor* initAxis(WatchState& watchState)
{
	if(axis != nullptr) {
		return axis;
	}

	axis = new AxisSensor();
	if(axis->begin(readRegister, writeRegister, delay) == false) {
		debug_e("BMA423 was not found. Please check wiring/power. ");
		delete axis;
		axis = nullptr;
		return axis;
	}

	bma423_axes_remap remap_data;
	remap_data.x_axis = 0;
	remap_data.x_axis_sign = 1;
	remap_data.y_axis = 1;
	remap_data.y_axis_sign = 0;
	remap_data.z_axis = 2;
	remap_data.z_axis_sign = 1;

	axis->setRemapAxes(&remap_data);

	// Accel parameter structure
	Acfg cfg;
	/*!
        Output data rate in Hz, Optional parameters:
            - BMA4_OUTPUT_DATA_RATE_0_78HZ
            - BMA4_OUTPUT_DATA_RATE_1_56HZ
            - BMA4_OUTPUT_DATA_RATE_3_12HZ
            - BMA4_OUTPUT_DATA_RATE_6_25HZ
            - BMA4_OUTPUT_DATA_RATE_12_5HZ
            - BMA4_OUTPUT_DATA_RATE_25HZ
            - BMA4_OUTPUT_DATA_RATE_50HZ
            - BMA4_OUTPUT_DATA_RATE_100HZ
            - BMA4_OUTPUT_DATA_RATE_200HZ
            - BMA4_OUTPUT_DATA_RATE_400HZ
            - BMA4_OUTPUT_DATA_RATE_800HZ
            - BMA4_OUTPUT_DATA_RATE_1600HZ
    */
	cfg.odr = BMA4_OUTPUT_DATA_RATE_100HZ;
	/*!
        G-range, Optional parameters:
            - BMA4_ACCEL_RANGE_2G
            - BMA4_ACCEL_RANGE_4G
            - BMA4_ACCEL_RANGE_8G
            - BMA4_ACCEL_RANGE_16G
    */
	cfg.range = BMA4_ACCEL_RANGE_2G;
	/*!
        Bandwidth parameter, determines filter configuration, Optional parameters:
            - BMA4_ACCEL_OSR4_AVG1
            - BMA4_ACCEL_OSR2_AVG2
            - BMA4_ACCEL_NORMAL_AVG4
            - BMA4_ACCEL_CIC_AVG8
            - BMA4_ACCEL_RES_AVG16
            - BMA4_ACCEL_RES_AVG32
            - BMA4_ACCEL_RES_AVG64
            - BMA4_ACCEL_RES_AVG128
    */
	cfg.bandwidth = BMA4_ACCEL_NORMAL_AVG4;

	/*! Filter performance mode , Optional parameters:
        - BMA4_CIC_AVG_MODE
        - BMA4_CONTINUOUS_MODE
    */
	cfg.perf_mode = BMA4_CONTINUOUS_MODE;

	// Configure the BMA423 accelerometer
	axis->setAccelConfig(cfg);

	// Enable BMA423 accelerometer
	axis->enableAccel();

	bma4_int_pin_config config;
	config.edge_ctrl = BMA4_LEVEL_TRIGGER;
	config.lvl = BMA4_ACTIVE_HIGH;
	config.od = BMA4_PUSH_PULL;
	config.output_en = BMA4_OUTPUT_ENABLE;
	config.input_en = BMA4_INPUT_DISABLE;
	// The correct trigger interrupt needs to be configured as needed
	axis->setINTPinConfig(config, BMA4_INTR1_MAP);

	pinMode(AXIS_INT_PIN, INPUT);
	attachInterrupt(
		AXIS_INT_PIN, [&watchState]() { watchState.axisIrq = true; }, RISING);

	// Enable BMA423 isStepCounter feature
	axis->enableFeature(BMA423_STEP_CNTR, true);
	// Enable BMA423 isTilt feature
	axis->enableFeature(BMA423_TILT, true);
	// Enable BMA423 isDoubleClick feature
	axis->enableFeature(BMA423_WAKEUP, true);

	// Reset steps
	axis->resetStepCounter();

	// Turn on feature interrupt
	axis->enableStepCountInterrupt();
	axis->enableTiltInterrupt();
	// It corresponds to isDoubleClick interrupt
	axis->enableWakeupInterrupt();

	return axis;
}

AxisSensor& getAxis()
{
	return *axis;
}
