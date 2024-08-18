/*
 * si7020.c
 *
 *  Created on: 14-08-2024
 *      Author: TrongNguyen
 */


#include <Source/App/SI7021/si7021.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_i2c.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "af.h"
#include "hal/plugin/temperature/temperature.h"
#include "em_iadc.h"
#include "i2cspm.h"
#include "Source/Mid/LED/led.h"
#include "em_timer.h"
#include "em_cmu.h"
#include "Source/App/Send/send.h"
#include "Source/App/Timer/timer.h"

EmberEventControl temperatureMeasurementEventControl;

void SI7020_Init(void){
	I2CSPM_Init_TypeDef i2cInit = I2CSPM_INIT_SENSOR;
	I2CSPM_Init(&i2cInit);
	halTemperatureInit();
	emberEventControlSetInactive(temperatureMeasurementEventControl);
}
void SI7020_Off(void){
	emberAfCorePrintln("Received Off command - Stop measuring temperature");
	emberEventControlSetInactive(temperatureMeasurementEventControl);
}
void SI7020_On(void){
	emberAfCorePrintln("Received On command - Start measuring temperature");
	emberEventControlSetActive(temperatureMeasurementEventControl);
}
void temperatureMeasurementEventHandler(void) {
	SensorMesuaringCmd();
	emberEventControlSetDelayMS(temperatureMeasurementEventControl, 500);
}

void SensorMesuaringCmd(){
	emberAfCorePrintln("Start Reading");
	halTemperatureInit();
	halTemperatureStartRead();
}

void halTemperatureReadingCompleteCallback(int32_t temperatureReading, bool readSuccess){
	if(readSuccess)
		{
			temperature = temperatureReading;
			if(temperature/1000 > 33){
				setSpeed(100);
			}
			else if(temperature/1000 > 30) {
				setSpeed(50);
			} else {
				setSpeed(0);
			}
			SEND_TempStateReport(1,temperature/1000);
		}
		else
		{
			emberAfCorePrintln("fail");
		}
}
