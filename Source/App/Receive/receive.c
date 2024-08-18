/*
 * receive.c
 *
 *  Created on: 12-08-2024
 *      Author: TrongNguyen
 */

#include <Source/Mid/LED/led.h>
#include "app/framework/include/af.h"
#include "Source/App/Send/send.h"
#include "Source/Mid/LED/led.h"
#include "receive.h"

boolean emberAfPreCommandReceivedCallback(EmberAfClusterCommand* cmd)
{
	if(cmd->clusterSpecific)
	{
		switch(cmd->apsFrame->clusterId)
		{
			case ZCL_ON_OFF_CLUSTER_ID:
				RECEIVE_HandleOnOffCluster(cmd);
				break;
			default:
				break;
		}
	}
	return false;
}

boolean emberAfPreMessageReceivedCallback(EmberAfIncomingMessage* incommingMessage)
{
	if(incommingMessage->apsFrame->clusterId == ACTIVE_ENDPOINTS_RESPONSE)
	{
		return true;
	}
 return false;
}

bool RECEIVE_HandleOnOffCluster(EmberAfClusterCommand* cmd)
{
	uint8_t commandID = cmd->commandId;
	uint8_t localEndpoint = cmd ->apsFrame -> destinationEndpoint;
	uint8_t remoteEndpoint = cmd->apsFrame -> sourceEndpoint;
	uint16_t IgnoreNodeID = cmd->source;
	emberAfCorePrintln("RECEIVE_HandleOnOffCluster SourceEndpoint = %d, RemoteEndpoint = %d, commandID = %d, nodeID %2X\n",remoteEndpoint,localEndpoint,commandID,IgnoreNodeID);
	switch(commandID)
	{
	case ZCL_OFF_COMMAND_ID:
		emberAfCorePrintln("Turn off LED");

		switch (cmd->type) {
			case EMBER_INCOMING_UNICAST:
				if(localEndpoint == 1)
				{
				offLed(LED_1);
				SEND_OnOffStateReport(localEndpoint, LED_OFF);
				}
				if(localEndpoint == 2)
				{
					SEND_OnOffStateReport(localEndpoint, LED_OFF);
				}
				break;
			case EMBER_INCOMING_MULTICAST:
				emberAfCorePrintln("Multicast");
				break;
			default:
				break;
		}

		break;
	case ZCL_ON_COMMAND_ID:

		emberAfCorePrintln("Turn on LED");
		switch (cmd->type) {
			case EMBER_INCOMING_UNICAST:
				if(localEndpoint == 1)
				{
					onLed(LED_1, ledBlue);
					SEND_OnOffStateReport(localEndpoint, LED_ON);
				}
				if(localEndpoint == 2)
				{
					SEND_OnOffStateReport(localEndpoint, LED_ON);
				}
				break;
			case EMBER_INCOMING_MULTICAST:
				emberAfCorePrintln("Multicast");
				break;
			default:
				break;
		}
		break;
	default:
		break;
	}
	return false;
}
