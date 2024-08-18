/*
 * led.c
 *
 *  Created on: 12-08-2024
 *      Author: TrongNguyen
 */

#include "app/framework/include/af.h"
#include "led.h"
#include "em_timer.h"
#include "led.h"
#include "em_cmu.h"



ledStruct   ledRgbArray[LED_RGB_COUNT][LED_RGB_ELEMENT] = {LED_RGB_1,LED_RGB_2};

ledActionInfo_t ledAction[LED_RGB_COUNT];

static uint32_t tickPerUs=0;

EmberEventControl led1ToggleEventControl,led2ToggleEventControl;
EmberEventControl *ledEventControl[LED_RGB_COUNT];

static void toggleLedHandle(ledNumber ledIndex);
static void dimLedHandle(ledNumber ledIndex);
static void LED_SetChangeDimStateTime(uint32_t time);
static void LED_TimeReset(void);

void TIMER3_IRQHandler(void){

	emberAfCorePrintln("Timer OV\n");

	TIMER3->CMD = TIMER_CMD_STOP;

	for (uint8_t i = 0; i < LED_RGB_COUNT; i++)
	{
		if(ledAction[i].ledBlinkMode==LED_DIM)
		{
			emberEventControlSetActive(*ledEventControl[i]);
			break;
		}
	}


	TIMER_IntClear(TIMER3, _TIMER_IF_OF_MASK);
}

void ledInit(void)
{

  TIMER_Init_TypeDef timerInit = TIMER_DIM_INIT;

  CMU_ClockEnable(cmuClock_GPIO, true);

  for(int i=0; i<LED_RGB_COUNT; i++)
  {
	  for(int j=0; j<LED_RGB_ELEMENT; j++)
	  {
		  GPIO_PinModeSet(ledRgbArray[i][j].port,
				  	  	  ledRgbArray[i][j].pin,
						  gpioModePushPull,
						  0);
	  }
  }





  CMU_ClockEnable(cmuClock_TIMER3, true);

  TIMER_Init(TIMER3, &timerInit);
  TIMER_IntEnable(TIMER3, _TIMER_IF_OF_MASK);
  NVIC_EnableIRQ(TIMER3_IRQn);

  TIMER3->CMD = TIMER_CMD_STOP;

  tickPerUs=(CMU_ClockFreqGet(cmuClock_TIMER3)/(timerInit.prescale+1))/1000000 +1;

  emberAfCorePrintln("tickPerUs %d\n",tickPerUs);

  ledEventControl[LED_1]=(EmberEventControl *)&led1ToggleEventControl;
  ledEventControl[LED_2]=(EmberEventControl *)&led2ToggleEventControl;
  // offLed after Init
  offLed(LED_1);
  offLed(LED_2);
}

void toggleLed(ledNumber  ledIndex, ledColor color, uint8_t toggleTime,uint32_t onTimeMs,uint32_t offTimeMs)
{
	LED_TimeReset();

	ledAction[ledIndex].ledBlinkMode=LED_TOGGLE;
	ledAction[ledIndex].color=color;

	ledAction[ledIndex].ledAct.ledBlink.onTime=onTimeMs;
	ledAction[ledIndex].ledAct.ledBlink.offTime=offTimeMs;
	ledAction[ledIndex].ledAct.ledBlink.blinkTime=toggleTime*2;

	emberEventControlSetActive(*ledEventControl[ledIndex]);
}

void led1ToggleEventHandle(void)
{
	//emberAfCorePrintln("led1ToggleEventHandle\n");

	emberEventControlSetInactive(led1ToggleEventControl);

	switch (ledAction[LED_1].ledBlinkMode)
	{
	case LED_TOGGLE:
		toggleLedHandle(LED_1);
		break;
	case LED_DIM:
		dimLedHandle(LED_1);
		break;
	default:
		break;
	}
}

void led2ToggleEventHandle(void)
{
	//emberAfCorePrintln("led2ToggleEventHandle\n");

	emberEventControlSetInactive(led2ToggleEventControl);

	switch (ledAction[LED_2].ledBlinkMode)
	{
	case LED_TOGGLE:
		emberAfCorePrintln("LED_TOGGLE2\n");
		toggleLedHandle(LED_2);
		break;
	case LED_DIM:
		emberAfCorePrintln("LED_DIM2\n");
		dimLedHandle(LED_2);
		break;
	default:
		break;
	}
}


static void toggleLedHandle(ledNumber ledIndex)
{
	if(ledAction[ledIndex].ledAct.ledBlink.blinkTime !=0)
	{
		if(ledAction[ledIndex].ledAct.ledBlink.blinkTime % 2)
		{
			for(int i=0; i<LED_RGB_ELEMENT; i++)
			{
				if( ((ledAction[ledIndex].color >> i) & 0x01) == 1)
				{
					GPIO_PinOutClear(ledRgbArray[ledIndex][i].port,ledRgbArray[ledIndex][i].pin);
				}
				else
				{
					GPIO_PinOutSet(ledRgbArray[ledIndex][i].port,ledRgbArray[ledIndex][i].pin);
				}
			}
			emberEventControlSetDelayMS(*ledEventControl[ledIndex],ledAction[ledIndex].ledAct.ledBlink.onTime);
		}else
		{
			for(int j=0; j<LED_RGB_ELEMENT; j++)
			{
				GPIO_PinOutSet(ledRgbArray[ledIndex][j].port,ledRgbArray[ledIndex][j].pin);
			}
			emberEventControlSetDelayMS(*ledEventControl[ledIndex],ledAction[ledIndex].ledAct.ledBlink.offTime);
		}

		ledAction[ledIndex].ledAct.ledBlink.blinkTime --;
	}
	else
	{
		ledAction[ledIndex].ledBlinkMode=LED_FREE;
		for(int j=0; j<LED_RGB_ELEMENT; j++)
		{
			GPIO_PinOutSet(ledRgbArray[ledIndex][j].port,ledRgbArray[ledIndex][j].pin);
		}
	}
}

static void dimLedHandle(ledNumber ledIndex)
{
	uint32_t cycles;

	if(ledAction[ledIndex].ledAct.ledDim.dimHigh)
	{
		//onLed(ledIndex,ledAction[ledIndex].ledAct.ledDim.color);
		for(int i=0; i<LED_RGB_ELEMENT; i++)
		{

			if( ((ledAction[ledIndex].color >> i) & 0x01) == 1)
			{
				GPIO_PinOutClear(ledRgbArray[ledIndex][i].port,ledRgbArray[ledIndex][i].pin);
			}
			else
			{
				GPIO_PinOutSet(ledRgbArray[ledIndex][i].port,ledRgbArray[ledIndex][i].pin);
			}
		}
		ledAction[ledIndex].ledAct.ledDim.dimHigh=false;
		cycles=ledAction[ledIndex].ledAct.ledDim.dimLevel;
	}else
	{
		for(int j=0; j<LED_RGB_ELEMENT; j++)
		{
			GPIO_PinOutSet(ledRgbArray[ledIndex][j].port,ledRgbArray[ledIndex][j].pin);
		}

		ledAction[ledIndex].ledAct.ledDim.dimHigh=true;
		cycles=LED_RGB_DIM_T-ledAction[ledIndex].ledAct.ledDim.dimLevel;
	}

	LED_SetChangeDimStateTime(cycles);
}


void dimLed(ledNumber ledIndex, ledColor color,uint8_t dimLevel)
{
	LED_TimeReset();

	if(dimLevel>100)
	{
		dimLevel=100;
	}

	ledAction[ledIndex].ledBlinkMode=LED_DIM;
	ledAction[ledIndex].color=color;

	ledAction[ledIndex].ledAct.ledDim.dimLevel=dimLevel;
	ledAction[ledIndex].ledAct.ledDim.dimHigh=false;

	LED_SetChangeDimStateTime(LED_RGB_DIM_T-dimLevel);
}


void offLed(ledNumber ledIndex)
{
	LED_TimeReset();
	emberEventControlSetInactive(*ledEventControl[ledIndex]);

	for(int j=0; j<LED_RGB_ELEMENT; j++)
	{
		GPIO_PinOutSet(ledRgbArray[ledIndex][j].port,ledRgbArray[ledIndex][j].pin);
	}
}

void onLed(ledNumber ledIndex, ledColor color)
{
	LED_TimeReset();
	emberEventControlSetInactive(*ledEventControl[ledIndex]);

	for(int i=0; i<LED_RGB_ELEMENT; i++)
	{

		if( ((color >> i) & 0x01) == 1)
		{
			GPIO_PinOutClear(ledRgbArray[ledIndex][i].port,ledRgbArray[ledIndex][i].pin);
		}
		else
		{
			GPIO_PinOutSet(ledRgbArray[ledIndex][i].port,ledRgbArray[ledIndex][i].pin);
		}
	}
}

static void LED_SetChangeDimStateTime(uint32_t time)
{
	emberAfCorePrintln("LED_SetChangeDimStateTime %d\n",time);
	TIMER_CounterSet(TIMER3, 0);
	TIMER_TopSet(TIMER3, time*tickPerUs);

	TIMER3->CMD = TIMER_CMD_START;
}

static void LED_TimeReset(void)
{
	TIMER_CounterSet(TIMER3, 0);
	TIMER3->CMD = TIMER_CMD_STOP;
}


