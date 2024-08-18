/*
 * send.c
 *
 *  Created on: 12-08-2024
 *      Author: TrongNguyen
 */

#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "app/framework/include/af.h"
#include "send.h"
#include "Source/App/Receive/receive.h"
#include "zigbee-framework/zigbee-device-common.h"
#include "Source/Mid/LED/led.h"

static void SEND_SendCommandUnicast(uint8_t source,
							 uint8_t destination,
							 uint8_t address)
{
	emberAfSetCommandEndpoints(source, destination);
	(void) emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, address);
}

static void SEND_FillBufferGlobalCommand(EmberAfClusterId clusterID,
								  EmberAfAttributeId attributeID,
								  uint8_t globalCommand,
								  uint8_t* value,
								  uint8_t length,
								  uint8_t dataType)
{
	uint8_t data[MAX_DATA_COMMAND_SIZE];
	data[0] = (uint8_t)(attributeID & 0x00FF);
	data[1] = (uint8_t)((attributeID & 0xFF00)>>8);
	data[2] = EMBER_SUCCESS;
	data[3] = (uint8_t)dataType;
	memcpy(&data[4], value, length);

	(void) emberAfFillExternalBuffer((ZCL_GLOBAL_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER | ZCL_DISABLE_DEFAULT_RESPONSE_MASK),
									  clusterID,
									  globalCommand,
									  "b",
									  data,
									  length + 4);
}

void SEND_ReportInfoHc(void)
{
	uint8_t modelID[5] = {4,'T','M','P','1'};
	uint8_t manufactureID[5] = {4, 'L', 'u', 'm', 'i'};
	uint8_t version = 1;

	if(emberAfNetworkState() != EMBER_JOINED_NETWORK){
		return;
	}
	emberAfCorePrintln("testCmd");
	SEND_FillBufferGlobalCommand(ZCL_BASIC_CLUSTER_ID,
								 ZCL_MODEL_IDENTIFIER_ATTRIBUTE_ID,
								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 modelID,
								 5,
								 ZCL_CHAR_STRING_ATTRIBUTE_TYPE);
	SEND_SendCommandUnicast(SOURCE_ENDPOINT_PRIMARY,
							DESTINATTION_ENDPOINT,
							HC_NETWORK_ADDRESS);

	SEND_FillBufferGlobalCommand(ZCL_BASIC_CLUSTER_ID,
								 ZCL_MANUFACTURER_NAME_ATTRIBUTE_ID,
								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 manufactureID,
								 5,
								 ZCL_CHAR_STRING_ATTRIBUTE_TYPE);
	SEND_SendCommandUnicast(SOURCE_ENDPOINT_PRIMARY,
							DESTINATTION_ENDPOINT,
							HC_NETWORK_ADDRESS);
	SEND_FillBufferGlobalCommand(ZCL_BASIC_CLUSTER_ID,
								 ZCL_APPLICATION_VERSION_ATTRIBUTE_ID,
								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 &version,
								 1,
								 ZCL_INT8U_ATTRIBUTE_TYPE);
	SEND_SendCommandUnicast(SOURCE_ENDPOINT_PRIMARY,
							DESTINATTION_ENDPOINT,
							HC_NETWORK_ADDRESS);
}

void SEND_OnOffStateReport(uint8_t Endpoint, uint8_t value){
	SEND_FillBufferGlobalCommand(ZCL_ON_OFF_CLUSTER_ID,
						   ZCL_ON_OFF_ATTRIBUTE_ID,
						   ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
						   (uint8_t*) &value,
						   1,
						   ZCL_BOOLEAN_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(Endpoint,
								DESTINATTION_ENDPOINT,
								HC_NETWORK_ADDRESS);

	emberAfWriteServerAttribute(Endpoint,
								ZCL_ON_OFF_CLUSTER_ID,
								ZCL_ON_OFF_ATTRIBUTE_ID,
								(uint8_t*) &value,
								ZCL_BOOLEAN_ATTRIBUTE_TYPE);
}

void SEND_TempStateReport(uint8_t Endpoint, uint32_t value){
	SEND_FillBufferGlobalCommand(ZCL_TEMP_MEASUREMENT_CLUSTER_ID,
								 ZCL_TEMP_MEASURED_VALUE_ATTRIBUTE_ID,
								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 (uint32_t*) &value,
								 sizeof(value),
								 ZCL_INT32U_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(Endpoint,
								DESTINATTION_ENDPOINT,
								HC_NETWORK_ADDRESS);

	emberAfWriteServerAttribute(Endpoint,
								ZCL_TEMP_MEASUREMENT_CLUSTER_ID,
								ZCL_TEMP_MEASURED_VALUE_ATTRIBUTE_ID,
								(uint32_t*) &value,
								ZCL_INT32U_ATTRIBUTE_TYPE);
}

