/*
 * receive.h
 *
 *  Created on: 12-08-2024
 *      Author: TrongNguyen
 */

#ifndef SOURCE_APP_RECEIVE_RECEIVE_H_
#define SOURCE_APP_RECEIVE_RECEIVE_H_						1

/* Function prototypes -----------------------------------------------*/

boolean emberAfPreCommandReceivedCallback(EmberAfClusterCommand* cmd);
boolean RECEIVE_HandleOnOffCluster(EmberAfClusterCommand* cmd);

#endif /* SOURCE_APP_RECEIVE_RECEIVE_H_ */
