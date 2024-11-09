/*
 * userApp.c
 *
 *  Created on: Created on: Oct 18, 2024
 *      Author: Niall.OKeeffe@atu.ie
 */

#include "main.h"
#include "userApp.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"


//Peripheral Handles
extern UART_HandleTypeDef huart1;

//Network variables
extern int network_wr(Network* n, unsigned char* buffer, int len, int timeout_ms);
extern int network_rd(Network* n, unsigned char* buffer, int len, int timeout_ms);
extern int net_if_init(void * if_ctxt);
extern int net_if_deinit(void * if_ctxt);
extern int net_if_reinit(void * if_ctxt);
extern int wifi_net_if_init(void * if_ctxt);

//RTOS task function prototypes and object declarations
static void initTask(void * pvParameters);
static void temperatureTask(void * pvParameters);
static void RTC_Task(void * pvParameters);
SemaphoreHandle_t publishSemaphore = NULL, oneSecondSemaphore = NULL;

uint8_t RTC_TaskRunning = 0;

uint8_t timeDisplay = 0, readSensor = 0;
net_hnd_t hnet;
Network network;
MQTTPacket_connectData options = MQTTPacket_connectData_initializer;
net_sockhnd_t socket;
MQTTClient client;


typedef struct {
  char *HostName;
  char *HostPort;
  char *ConnSecurity;
  char *MQClientId;
  char *MQUserName;
  char *MQUserPwd;
#ifdef LITMUS_LOOP
  char *LoopTopicId;
#endif
} device_config_t;


/*--------------------------------------------------------------------
 * EXTI interrupt handler callback function
 * Will give a semaphore when the user button is pressed
 --------------------------------------------------------------------*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	BaseType_t xHigherPriorityTaskWoken;
	switch (GPIO_Pin)
	{
	case (GPIO_PIN_1):	//IRQ generated by WiFi module
		{
		SPI_WIFI_ISR();
		break;
		}

	case (BUTTON_EXTI13_Pin) :	//IRQ generated by user button
		{
		printf("User button pressed\r\n\n");
		xHigherPriorityTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR(publishSemaphore, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}

	default:
	{
		break;
	}
	}
}

/*--------------------------------------------------------------------
 * RTC timer event interrupt handler callback function
 * Runs every second and gives a semaphore if the RTC task is running
 --------------------------------------------------------------------*/
void HAL_RTCEx_WakeUpTimerEventCallback (RTC_HandleTypeDef * hrtc) {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if(RTC_TaskRunning){
		xHigherPriorityTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR(oneSecondSemaphore, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}

/*--------------------------------------------------------------------------
 * Subscribe message callback function
 * Called every time a publish meassage is received from a subscribed topic
 --------------------------------------------------------------------------*/
void subscribeMessageHandler(MessageData* data)
{
	static char mqtt_msg[MQTT_MSG_BUFFER_SIZE], mqtt_topic[MQTT_TOPIC_BUFFER_SIZE];
	snprintf(mqtt_msg, data->message->payloadlen+1, "%s", (char *)data->message->payload);
	snprintf(mqtt_topic, data->topicName->lenstring.len+1, "%s", data->topicName->lenstring.data);
	printf("\r\nPublished message from MQTT broker\r\n");
	printf("Topic: %s, Payload: %s\r\n\n", mqtt_topic, mqtt_msg);

	//Change the string parsing to match your Ubidots settings
	if(strstr(mqtt_topic, "rtos/switch")) {	//check topic
		if(strstr(mqtt_msg, "\"value\": 1.0")) {	//check data published from topic
			HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
		}
		else {
			HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
		}
	}
}

static void temperatureTask(void * pvParameters) {
	MQTTMessage mqmsg;
	char temperature[25];
	printf("Starting Publish Task\r\n");
	BSP_TSENSOR_Init();		//Initialise temperature sensor
	while(1) {
		if(xSemaphoreTake(publishSemaphore, 0) == pdTRUE) {
			float tempF = BSP_TSENSOR_ReadTemp();
			uint16_t tempI = tempF*10;
			sprintf(temperature, "{\"temperature\":%d.%d}", tempI/10, tempI%10);
			printf("Publishing Temperature: %sC\r\n", temperature);
			memset(&mqmsg, 0, sizeof(MQTTMessage));
			mqmsg.qos = QOS0;
			mqmsg.payload = (char *) temperature;
			mqmsg.payloadlen = strlen(temperature);

			//change the device api label to match your Ubidots configuration
			MQTTPublish(&client, "/v1.6/devices/rtos", &mqmsg);
		}
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}

/*-----------------------------------------------------------
 * Task to print the date and time when a semaphore is taken
-------------------------------------------------------------*/
static void RTC_Task(void * pvParameters) {
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;
	char timeBuffer[40];

	RTC_TaskRunning = 1;	//flag set to allow RTC event interrupt handler callback to send semaphores

	while(1) {
		MQTTYield(&client, 200);	//Yield needed to allow check for received published messages from subscribed topics

		if(xSemaphoreTake(oneSecondSemaphore, 0) == pdTRUE) {
			timeDisplay = 0;
			HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
			HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
			sprintf(timeBuffer, "%02d/%02d/%02d %02d:%02d:%02d\r\n", sDate.Date, sDate.Month, sDate.Year, sTime.Hours+1, sTime.Minutes, sTime.Seconds);
			printf("%s", timeBuffer);
		}
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}

/*--------------------------------------
 * Initialisation Task
 * 1. Connects to ubidots MQTT broker
 * 2. Initialises temperature sensor
 * 3. Creates semaphores
 * 4. Creates temperature and RTC tasks
 * 5. Subscribes to LED_Control variable
 * 6. Task delates itself
 ----------------------------------------*/
static void initTask(void * pvParameters) {
	uint32_t ret;
	printf("Starting Init Task\r\n");

	while(1) {
		HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
		brokerConnect(&client);	//connect to WiFi access point and then to MQTT broker
		publishSemaphore = xSemaphoreCreateBinary();
		vQueueAddToRegistry(publishSemaphore, "Publish Semaphore");
		oneSecondSemaphore = xSemaphoreCreateBinary();
		vQueueAddToRegistry(oneSecondSemaphore, "RTC Semaphore");
		if(xTaskCreate(temperatureTask, "Temperature Task", 500, NULL, configMAX_PRIORITIES - 3, NULL) == pdTRUE) {
			printf("Temperature task created\n\r");
		}
		else {
			printf("Could not create temperature task\n\r");
		}
		if(xTaskCreate(RTC_Task, "RTC Task", 500, NULL, configMAX_PRIORITIES - 2, NULL) == pdTRUE) {
			printf("RTC task created\n\r");
		}
		else {
			printf("Could not create RTC task\n\r");
		}

		//Subscribe to topics here
		//change the device name and variable name in the function call to match your Ubidots configuration
		ret = MQTTSubscribe(&client, "/v1.6/devices/rtos/switch", QOS0, (subscribeMessageHandler));
		if (ret != MQSUCCESS) {
			printf("\n\rSubscribe failed: %ld\n\r", ret);
		}
		else {
			printf("\n\rSubscribed to topic \n\r");
			ret = MQTTYield(&client, 500);
		}

		printf("Deleting Init Task\r\n\n");
		vTaskDelete(NULL);
	}
}

/*-----------------------------------------------
 * User application function
 * Creates Init task and starts ROS scheduler
-------------------------------------------------*/
void userApp() {
	printf("Starting user application\r\n\n");
	xTaskCreate(initTask, "Init Task", 800, NULL, configMAX_PRIORITIES - 1, NULL);
	vTaskStartScheduler();

	while(1) {
	}
}

/*-------------------------------------------------
 * Function to establish connection to cloud server
 * 1. Connects to WiFi access point (AP)
 * 2. Gets date and time from network
 * 3. Connects to ubidots web server
 * 4. Connects to Ubidots MQTT broker
---------------------------------------------------*/
void brokerConnect(MQTTClient * client) {
	int32_t ret;
	//Network and MQTT variables
	device_config_t MQTT_Config;
	static unsigned char mqtt_send_buffer[MQTT_SEND_BUFFER_SIZE];
	static unsigned char mqtt_read_buffer[MQTT_READ_BUFFER_SIZE];
	net_ipaddr_t ipAddr;
	net_macaddr_t macAddr;


	//Initialise MQTT broker structure
	//Fill in this section with MQTT broker credentials from header file
	MQTT_Config.HostName = CloudBroker_HostName;
	MQTT_Config.HostPort = CloudBroker_Port;
	MQTT_Config.ConnSecurity = "0";	//plain TCP connection with no security
	MQTT_Config.MQUserName = CloudBroker_Username;
	MQTT_Config.MQUserPwd = CloudBroker_Password;
	MQTT_Config.MQClientId = CloudBroker_ClientID;

	//Initialise WiFi network
	if (net_init(&hnet, NET_IF, (wifi_net_if_init)) != NET_OK) {
		printf("\n\rError");
	}
	else {
		printf("\n\rOK");
	}
	HAL_Delay(500);

	printf("\n\rRetrieving the IP address.");

	if (net_get_ip_address(hnet, &ipAddr) != NET_OK) {
		printf("\n\rError 2");
	}
	else
	{
		switch(ipAddr.ipv) {
			case NET_IP_V4:
				printf("\n\rIP address: %d.%d.%d.%d\n\r", ipAddr.ip[12], ipAddr.ip[13], ipAddr.ip[14], ipAddr.ip[15]);
				break;
			case NET_IP_V6:
			default:
				printf("\n\rError 3");
		}
	}

	if (net_get_mac_address(hnet, &macAddr) == NET_OK) {
		printf("\n\rMac Address: %02x:%02x:%02x:%02x:%02x:%02x\r\n",
	               macAddr.mac[0], macAddr.mac[1], macAddr.mac[2], macAddr.mac[3], macAddr.mac[4], macAddr.mac[5]);
	}

	/*
	* Fetch the epoch time from st.com and use it to set the RTC time
	*/
	if (setRTCTimeDateFromNetwork(true) != TD_OK) {
		printf("Fail setting time\r\n");
	}
	else {
		printf("Time set, Starting RTC\r\n");
		//RTC started with a 1-second wake-up interrupt
		HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 2047, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
	}

	printf("Connecting to MQTT Broker Server\r\n\n");
	//Create network socket
	ret = net_sock_create(hnet, &socket, NET_PROTO_TCP);
	if (ret != NET_OK)
	{
		printf("\n\rCould not create the socket.\r\n");
		printf("Check MQTT broker configuration settings.\r\n");
		while(1);
	}
	else
	{
		ret |= net_sock_setopt(socket, "sock_noblocking", NULL, 0);
	}

	ret = net_sock_open(socket, MQTT_Config.HostName, 1883, 0);
	if (ret != NET_OK)
	{
		printf("\n\rCould not open the socket.");
		while(1);
	}
	else {
		printf("\r\nConnected to MQTT Broker Server\r\n");
		HAL_Delay(1000);
	}

	network.my_socket = socket;
	network.mqttread = (network_rd);
	network.mqttwrite = (network_wr);

	MQTTClientInit(client, &network, MQTT_CMD_TIMEOUT, mqtt_send_buffer, MQTT_SEND_BUFFER_SIZE,
			mqtt_read_buffer, MQTT_READ_BUFFER_SIZE);

	/* MQTT connect */
	options.clientID.cstring = MQTT_Config.MQClientId;
	options.username.cstring = MQTT_Config.MQUserName;
	options.password.cstring = MQTT_Config.MQUserPwd;

	HAL_Delay(1000);

	printf("Connecting client to MQTT Broker\r\n\n");
	ret = MQTTConnect(client, &options);
	if (ret != 0)
	{
		printf("\n\rMQTTConnect() failed: %ld\n", ret);
		printf("Check MQTT client credential settings.\r\n");
		while(1);
	}
	else
	{
		printf("\n\rClient Connected to MQTT Broker\r\n");
		HAL_Delay(1000);
	}
	HAL_Delay(1000);

}
