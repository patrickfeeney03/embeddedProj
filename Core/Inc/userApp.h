/*
 * uaerApp.h
 *
 *  Created on: Dec 12, 2023
 *      Author: Niall.OKeeffe@atu.ie
 */

#ifndef INC_USERAPP_H_
#define INC_USERAPP_H_

#include "main.h"
#include <stdio.h>
#include <string.h>
#include "wifi.h"
#include "net_internal.h"
#include "..\..\Common\Shared\Inc\net.h"
#include "stm32l475e_iot01_tsensor.h"
#include "stm32l475e_iot01_psensor.h"
#include "stm32l475e_iot01_hsensor.h"
#include "WiFi_Credentials.h"
//#include "Ubidots_Credentials.h"
#include "CloudBrokerCredentials.h"
#include "timedate.h"
#include <time.h>

#define CONNECTION_TRIAL_MAX		10
#define WIFI_WRITE_TIMEOUT			10000
#define WIFI_READ_TIMEOUT			10000
#define CONNECTION_TRIAL_MAX		10
#define MQTT_SEND_BUFFER_SIZE		600
#define MQTT_READ_BUFFER_SIZE		600
#define MQTT_TOPIC_BUFFER_SIZE		100  /**< Maximum length of the application-defined topic names. */
#define MQTT_MSG_BUFFER_SIZE		MQTT_SEND_BUFFER_SIZE /**< Maximum length of the application-defined MQTT messages. */
#define MQTT_CMD_TIMEOUT			5000
#define MODEL_MAC_SIZE				13
#define MODEL_DEFAULT_MAC			"0102030405"
#define BROKER_CONNECT_SUCCESS		0
#define BROKER_CONNECT_FAIL		-1

void userApp();
void TIM6_Handler();
void brokerConnect(MQTTClient * client);
extern int _write(int file, char *ptr, int len);

#endif /* INC_USERAPP_H_ */
