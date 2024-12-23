/**
  ******************************************************************************
  * @file    wifi_net.c
  * @author  MCD Application Team
  * @brief   Wifi-specific NET initialization.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
#ifdef USE_WIFI

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "wifi.h"
#include "iot_flash_config.h"

/* Private defines -----------------------------------------------------------*/
#define  WIFI_CONNECT_MAX_ATTEMPT_COUNT  3

#ifdef ES_WIFI_PRODUCT_NAME_SIZE
#define WIFI_PRODUCT_INFO_SIZE                      ES_WIFI_PRODUCT_NAME_SIZE
#define WIFI_PAYLOAD_SIZE                           ES_WIFI_PAYLOAD_SIZE
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
int net_if_init(void * if_ctxt);
int net_if_deinit(void * if_ctxt);
int net_if_reinit(void * if_ctxt);
int wifi_net_if_init(void * if_ctxt);

/* Functions Definition ------------------------------------------------------*/
int wifi_net_if_init(void * if_ctxt)
{
	//const char *ssid = "eir98388741-2.4G";
	//const char  *psk = "zyp3jmqd";
	WIFI_Ecn_t security_mode = 3;
	char moduleinfo[WIFI_PRODUCT_INFO_SIZE];
	WIFI_Status_t wifiRes;
	uint8_t macAddress[6];
	int wifiConnectCounter = 0;
	bool skip_reconf = false;

	printf("\r\n*** WIFI connection ***\r\n\n");

	/*skip_reconf = (checkWiFiCredentials(&ssid, &psk, (uint8_t *) &security_mode) == HAL_OK);

	if (skip_reconf == true)
	{
	printf("Push the User button (Blue) within the next 5 seconds if you want to update"
		   " the WiFi network configuration.\n\n");

	skip_reconf = (Button_WaitForPush(5000) == BP_NOT_PUSHED);
	}

	if (skip_reconf == false)
	{
	printf("Your WiFi parameters need to be entered to proceed.\n");
	do
	{
	  updateWiFiCredentials();
	} while (checkWiFiCredentials(&ssid, &psk, (uint8_t *) &security_mode) != HAL_OK);
	}*/

	/*  Wifi Module initialization */
	printf("Initializing the WiFi module\r\n");

	wifiRes = WIFI_Init();
	if ( WIFI_STATUS_OK != wifiRes )
	{
	printf("\n\rFailed to initialize WIFI module\r\n");
	return -1;
	}

	/* Retrieve the WiFi module mac address to confirm that it is detected and communicating. */
	WIFI_GetModuleName(moduleinfo);
	printf("Module initialized successfully: %s",moduleinfo);

	WIFI_GetModuleID(moduleinfo);
	printf(" %s",moduleinfo);

	WIFI_GetModuleFwRevision(moduleinfo);
	printf(" %s\r\n",moduleinfo);

	printf("Retrieving the WiFi module MAC address:");
	wifiRes = WIFI_GetMAC_Address( (uint8_t*)macAddress);
	if ( WIFI_STATUS_OK == wifiRes)
	{
	printf(" %02x:%02x:%02x:%02x:%02x:%02x\n\r",
		 macAddress[0], macAddress[1], macAddress[2],
		 macAddress[3], macAddress[4], macAddress[5]);
	}
	else
	{
	   printf("Failed to get MAC address\r\n");
	}
	/* Connect to the specified SSID. */

	printf("\n");
	do
	{
	printf("\r\nConnecting to AP: %s  Attempt %d/%d ...",SSID, ++wifiConnectCounter,WIFI_CONNECT_MAX_ATTEMPT_COUNT);
	wifiRes = WIFI_Connect(SSID, PASSWORD, security_mode);
	if (wifiRes == WIFI_STATUS_OK) break;
	}
	while (wifiConnectCounter < WIFI_CONNECT_MAX_ATTEMPT_COUNT);

	if (wifiRes == WIFI_STATUS_OK)
	{
	printf("\r\nConnected to AP %s\r\n",SSID);
	}
	else
	{
	printf("\r\nFailed to connect to AP %s\r\n",SSID);
	}

	return (wifiRes == WIFI_STATUS_OK)?0:-1;
}

int net_if_init(void * if_ctxt)
{
  const char *ssid = "eir98388741-2.4G";
  const char  *psk = "zyp3jmqd";
  WIFI_Ecn_t security_mode = 0;
  char moduleinfo[WIFI_PRODUCT_INFO_SIZE];
  WIFI_Status_t wifiRes;
  uint8_t macAddress[6];
  int wifiConnectCounter = 0;
  bool skip_reconf = false;

  printf("\r\n*** WIFI connection ***\r\n\n");

  skip_reconf = (checkWiFiCredentials(&ssid, &psk, (uint8_t *) &security_mode) == HAL_OK);

  if (skip_reconf == true)
  {
    printf("Push the User button (Blue) within the next 5 seconds if you want to update"
           " the WiFi network configuration.\r\n\n");

    skip_reconf = (Button_WaitForPush(5000) == BP_NOT_PUSHED);
  }
  
  if (skip_reconf == false)
  {
    printf("Your WiFi parameters need to be entered to proceed.\r\n");
    do
    {
      updateWiFiCredentials();
    } while (checkWiFiCredentials(&ssid, &psk, (uint8_t *) &security_mode) != HAL_OK);
  }
  
  /*  Wifi Module initialization */
  printf("Initializing the WiFi module\r\n");
  
  wifiRes = WIFI_Init();
  if ( WIFI_STATUS_OK != wifiRes )
  {
    printf("Failed to initialize WIFI module\r\n");
    return -1;
  }
    
  /* Retrieve the WiFi module mac address to confirm that it is detected and communicating. */
  WIFI_GetModuleName(moduleinfo);
  printf("Module initialized successfully: %s",moduleinfo);
  
  WIFI_GetModuleID(moduleinfo);
  printf(" %s",moduleinfo);
  
  WIFI_GetModuleFwRevision(moduleinfo);
  printf(" %s\n",moduleinfo);
  
  printf("Retrieving the WiFi module MAC address:");
  wifiRes = WIFI_GetMAC_Address( (uint8_t*)macAddress);
  if ( WIFI_STATUS_OK == wifiRes)
  {
    printf(" %02x:%02x:%02x:%02x:%02x:%02x\n",
         macAddress[0], macAddress[1], macAddress[2],
         macAddress[3], macAddress[4], macAddress[5]);
  }
  else
  {
       printf("Failed to get MAC address\n");
  }
  /* Connect to the specified SSID. */

  printf("\n");
  do 
  {
    printf("\r\nConnecting to AP: %s  Attempt %d/%d ...",ssid, ++wifiConnectCounter,WIFI_CONNECT_MAX_ATTEMPT_COUNT);
    wifiRes = WIFI_Connect(ssid, psk, security_mode);
    if (wifiRes == WIFI_STATUS_OK) break;
  } 
  while (wifiConnectCounter < WIFI_CONNECT_MAX_ATTEMPT_COUNT);
  
  if (wifiRes == WIFI_STATUS_OK)
  {
    printf("\r\nConnected to AP %s\n",ssid);
  }
  else
  {
    printf("\r\nFailed to connect to AP %s\n",ssid);
  }
  
  return (wifiRes == WIFI_STATUS_OK)?0:-1;
}


int net_if_deinit(void * if_ctxt)
{
  return 0;
}


int net_if_reinit(void * if_ctxt)
{
  int ret = 0;
  const char *ssid;
  const char  *psk;
  WIFI_Ecn_t security_mode;
  WIFI_Status_t wifiRes;
  int wifiConnectCounter = 0;
  
  // wifiRes = WIFI_Disconnect(); // Disconnect() is not enough for the Inventek module which does not clean the previous state properly.
  wifiRes = WIFI_Init();
  if (wifiRes != WIFI_STATUS_OK)
  {
    msg_error("WIFI_Init() failed.\n");
  }
  
  if (checkWiFiCredentials(&ssid, &psk, (uint8_t *) &security_mode) != HAL_OK)
  {
    ret = -1;
  }
  else
  {
    do 
    {
      printf("\r\nConnecting to AP: %s  Attempt %d/%d ...",ssid, ++wifiConnectCounter,WIFI_CONNECT_MAX_ATTEMPT_COUNT);
      wifiRes = WIFI_Connect(ssid, psk, security_mode);
      if (wifiRes == WIFI_STATUS_OK) break;
    } 
    while (wifiConnectCounter < WIFI_CONNECT_MAX_ATTEMPT_COUNT);
    
    if (wifiRes == WIFI_STATUS_OK)
    {
      printf("\r\nRe-connected to AP %s\n",ssid);
    }
    else
    {
      printf("\r\nFailed to re-connect to AP %s\n",ssid);
      ret = -1;
    }
  }
    
  return ret;
}

#endif /* USE_WIFI */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
