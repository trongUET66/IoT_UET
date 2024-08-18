#include <Source/App/SI7021/si7021.h>
#include "app/framework/include/af.h"
#include "Source/App/Network/network.h"
#include "Source/App/Receive/receive.h"
#include "Source/App/Send/send.h"
#include "Source/Mid/LED/led.h"
#include "Source/App/Timer/timer.h"

EmberStatus status;
EmberEventControl mainStateEventControl;
void USER_networkEventHandle(uint8_t networkResult);

boolean networkReady;
SystemState systemState;

void emberAfMainInitCallback(void){
	emberAfCorePrintln("emberAfMainInitCallback");
	ledInit();
	initPWM();
	Network_Init(USER_networkEventHandle);
	systemState = POWER_ON_STATE;
	emberEventControlSetActive(mainStateEventControl);
	SI7020_Init();
	SI7020_On();

}

void USER_networkEventHandle(uint8_t networkResult) {
    emberAfCorePrintln("USER_networkEventHandle");

    switch (networkResult) {
        case NETWORK_HAS_PARENT:
            networkReady = true;
            systemState = REPORT_STATE;
            emberEventControlSetDelayMS(mainStateEventControl, 1000);
            break;

        case NETWORK_JOIN_FAIL:
            systemState = IDLE_STATE;
            emberEventControlSetDelayMS(mainStateEventControl, 1000);
            break;

        case NETWORK_JOIN_SUCCESS:
            networkReady = true;
            systemState = REPORT_STATE;
            emberEventControlSetDelayMS(mainStateEventControl, 1000);
            break;

        case NETWORK_LOST_PARENT:
            systemState = IDLE_STATE;
            emberEventControlSetDelayMS(mainStateEventControl, 1000);
            break;

        case NETWORK_OUT_NETWORK:
            if (networkReady) {
                systemState = REBOOT_STATE;
                emberEventControlSetDelayMS(mainStateEventControl, 3000);
            }
            break;
    }
}

void mainStateEventHandle(void) {
    EmberNetworkStatus nwkStatusCurrent;

    emberEventControlSetInactive(mainStateEventControl);

    switch (systemState) {
        case POWER_ON_STATE:
            nwkStatusCurrent = emberAfNetworkState();
            if (nwkStatusCurrent == EMBER_NO_NETWORK) {
                NETWORK_FindAndJoin();
            } else {
            	SEND_ReportInfoHc();
            }
            break;

        case REPORT_STATE:
        	SEND_ReportInfoHc();
            break;

        case IDLE_STATE:
            break;

        case REBOOT_STATE:
            systemState = IDLE_STATE;
            halReboot();
            break;

        default:
            break;
    }
}
