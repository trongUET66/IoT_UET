/*
 * si7020.h
 *
 *  Created on: 14-08-2024
 *      Author: TrongNguyen
 */
#include <stdbool.h>
#include <stdint.h>

#ifndef SOURCE_APP_SI7021_SI7021_H_
#define SOURCE_APP_SI7021_SI7021_H_

void SI7020_Init(void);
void temperatureMeasurementEventHandler(void);
void SensorMesuaringCmd();
void temperatureMeasurementEventHandler(void);
void halTemperatureReadingCompleteCallback(int32_t temperatureReading, bool readSuccess);
void SI7020_Off(void);
void SI7020_On(void);

static int32_t temperature;

#endif /* SOURCE_APP_SI7021_SI7021_H_ */
