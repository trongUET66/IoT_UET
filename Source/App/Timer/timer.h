/*
 * timer.h
 *
 *  Created on: 16-08-2024
 *      Author: TrongNguyen
 */

#ifndef SOURCE_APP_TIMER_TIMER_H_
#define SOURCE_APP_TIMER_TIMER_H_

#define PWM_MAX_DUTY_CYCLE 100
#define PWM_FREQUENCY 1000
void initPWM(void);
void setSpeed(int speed);

#endif /* SOURCE_APP_TIMER_TIMER_H_ */
