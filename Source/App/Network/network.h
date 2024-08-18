/*
 * network.h
 *
 *  Created on: 11-08-2024
 *      Author: TrongNguyen
 */

#ifndef SOURCE_APP_NETWORK_NETWORK_H_
#define SOURCE_APP_NETWORK_NETWORK_H_

typedef enum{
	NETWORK_HAS_PARENT,
	NETWORK_JOIN_FAIL,
	NETWORK_JOIN_SUCCESS,
	NETWORK_LOST_PARENT,
	NETWORK_OUT_NETWORK,
}Network_Event_e;

typedef void (*networkEventHandler) (Network_Event_e networkEvent);

typedef enum {
    POWER_ON_STATE,
    REPORT_STATE,
    IDLE_STATE,
    REBOOT_STATE
} SystemState;

void NETWORK_FindAndJoin(void);
void joinNetworkEventHandler(void);
void NETWORK_StopFindAndJoin(void);

void Network_Init(networkEventHandler networkResult);


#endif /* SOURCE_APP_NETWORK_NETWORK_H_ */
