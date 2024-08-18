/*
 * timer.c
 *
 *  Created on: 16-08-2024
 *      Author: TrongNguyen
 */

#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_i2c.h"
#include "em_cmu.h"
#include "em_gpio.h"

#include "af.h"
#include "em_iadc.h"
#include "i2cspm.h"
#include "em_timer.h"
#include "em_cmu.h"
#include "timer.h"

void initPWM(void) {
		CMU_ClockEnable(cmuClock_GPIO, true);
		CMU_ClockEnable(cmuClock_TIMER0, true);
		GPIO_PinModeSet(gpioPortD, 0, gpioModePushPull, 0);


	    // Initialize TIMER for PWM
	    TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
	    TIMER_Init(TIMER0, &timerInit);

	    // Initialize TIMER PWM mode
	    TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
	    timerCCInit.mode = timerCCModePWM;
	    timerCCInit.outInvert = true;
	    timerCCInit.cufoa = _TIMER_CC_CTRL_CUFOA_CLEAR;
	    timerCCInit.cmoa = _TIMER_CC_CTRL_CMOA_CLEAR;
	    GPIO->TIMERROUTE[0].ROUTEEN  = GPIO_TIMER_ROUTEEN_CC0PEN;
	    GPIO->TIMERROUTE[0].CC0ROUTE = (gpioPortD << _GPIO_TIMER_CC0ROUTE_PORT_SHIFT)
	                      | (0 << _GPIO_TIMER_CC0ROUTE_PIN_SHIFT);
	    TIMER_InitCC(TIMER0, 0, &timerCCInit); // Use channel 0 for PWM

	    // Set PWM frequency
	    uint32_t timerFreq = CMU_ClockFreqGet(cmuClock_TIMER0) / (timerInit.prescale + 1);
	    uint32_t topValue = (timerFreq / PWM_FREQUENCY);
	    TIMER_TopSet(TIMER0, topValue);

	    // Set initial duty cycle to 0%
	    TIMER_CompareSet(TIMER0, 0, 0);

	    // Start PWM
	    TIMER_Enable(TIMER0, true);
}
void setSpeed(int speed){
	uint32_t dutyCycle = (speed * TIMER_TopGet(TIMER0)) / 100;
	TIMER_CompareBufSet(TIMER0, 0, dutyCycle);
}

