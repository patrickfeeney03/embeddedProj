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
#include "timers.h"
#include "event_groups.h"

//Peripheral Handles
extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim6;

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
static void humidityTask(void * pvParameters);
static void RTC_Task(void * pvParameters);

#define temperatureBit (1 << 0)
#define humidityBit (1 << 1)
#define pressureBit (1 << 2)

SemaphoreHandle_t publishSemaphore = NULL, oneSecondSemaphore = NULL, publishHumiditySemaphore = NULL, sensorMutex = NULL, queueSizeMutex = NULL;
TimerHandle_t temperatureTimerHandler = NULL, humidityTimerHandler = NULL, pressureTimerHandler = NULL;
EventGroupHandle_t timersEventGroupHandler = NULL;
QueueHandle_t publishQueue = NULL;
//IWDG_HandleTypeDef hiwdg;

uint8_t RTC_TaskRunning = 0, queueSize = 0, ch;

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

typedef struct {
    char endpoint[40];
    char payload[255];
    size_t payloadlen;
    enum QoS qos;
} MyMQTTMessage;

void timersCallback(TimerHandle_t xTimer) {
	if (xTimer == temperatureTimerHandler) {
		xEventGroupSetBits(timersEventGroupHandler, temperatureBit);
	} else if (xTimer == humidityTimerHandler) {
		xEventGroupSetBits(timersEventGroupHandler, humidityBit);
	} else if (xTimer == pressureTimerHandler) {
		xEventGroupSetBits(timersEventGroupHandler, pressureBit);
	}
}

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

void HAL_UART_RxCpltCallback (UART_HandleTypeDef * huart) {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	char endpoint[] = "/v1.6/devices/rtos";
	char payload[] = "{\"temptime\": 4000, \"presstime\": 4000, \"humidtime\": 4000}";
	MyMQTTMessage myMQTTMessage;
	myMQTTMessage.qos = QOS0;
	strcpy(myMQTTMessage.payload, payload);
	strcpy(myMQTTMessage.endpoint, endpoint);
	myMQTTMessage.payloadlen = strlen(payload);

	if (ch == 'r') {
		printf("Received character R");
		// reset timers back to 4 and send that to Ubidots...

		xQueueSendFromISR(publishQueue, &myMQTTMessage, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}

	HAL_UART_Receive_IT(&huart1, &ch, 1);
}

void TIM6_Handler() {

//	printf("Running TIM6 Handler\r\n");
//	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//	xSemaphoreGiveFromISR(publishHumiditySemaphore, &xHigherPriorityTaskWoken);
//	xTaskNotifyFromISR(toggleLedHandler, (uint32_t) 1, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
//	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

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

void subscribeLogic(char mqtt_msg[], char slider[], char control[], TimerHandle_t timerHandler, char loggingName[]) {
	uint8_t spaces;
	if (strchr(mqtt_msg, ',')) {
		spaces = 1;
	} else {
		spaces = 3;
	}

	if (strstr(mqtt_msg, slider)) {
		char *ptr = strstr(mqtt_msg, slider) + strlen(slider) + spaces;
		uint16_t sliderVal = atoi(ptr);
		printf("Setting %s Timer to: %dms\r\n", loggingName, sliderVal);
		xTimerChangePeriod(timerHandler, pdMS_TO_TICKS(sliderVal), pdMS_TO_TICKS(10));
	}
	if (strstr(mqtt_msg, control)) {
		char *ptr = strstr(mqtt_msg, control) + strlen(control) + spaces;
		uint8_t state = atoi(ptr);
		if (state) {
			printf("Turning %s Timer ON\r\n", loggingName);
			xTimerStart(timerHandler, pdMS_TO_TICKS(10));
		} else {
			printf("Turning %s Timer OFF\r\n", loggingName);
			xTimerStop(timerHandler, pdMS_TO_TICKS(10));
		}
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

	// There are 3 groups in Adafruit. One for each sensor.
	if (strstr(mqtt_topic, "groups/temperature")) {
		subscribeLogic(mqtt_msg, "temperatureslider", "temperaturecontrol", temperatureTimerHandler, "Temperature");
	} else if (strstr(mqtt_topic, "groups/pressure")) {
		subscribeLogic(mqtt_msg, "pressureslider", "pressurecontrol", pressureTimerHandler, "Pressure");
	} else if (strstr(mqtt_topic, "groups/humidity")) {
		subscribeLogic(mqtt_msg, "humidityslider", "humiditycontrol", humidityTimerHandler, "Humidity");
	}
}

static void temperatureTask(void * pvParameters) {
	MyMQTTMessage mqmsg;
	char temperature[100];
	char endpoint[35] = "g00409592/groups/sensorDisplay";
	printf("Starting Temperature Publish Task\r\n");
	BSP_TSENSOR_Init();
	while(1) {
		if (xEventGroupWaitBits(timersEventGroupHandler, temperatureBit, pdTRUE, pdFALSE, portMAX_DELAY)) {
			if (xSemaphoreTake(sensorMutex, portMAX_DELAY) == pdTRUE) {
				float tempF = BSP_TSENSOR_ReadTemp();
				uint16_t tempI = tempF*10;
				sprintf(temperature, "{\"feeds\": {\"temperature\":%d.%d}}", tempI/10, tempI%10);

				memset(&mqmsg, 0, sizeof(MyMQTTMessage));
				mqmsg.qos = QOS0;
				strcpy(mqmsg.payload, temperature);
				mqmsg.payloadlen = strlen(temperature);
				strcpy(mqmsg.endpoint, endpoint);

				if (xQueueSend(publishQueue, &mqmsg, 0) != pdPASS) {
					printf("Could not send data to queue. Temperature\r\n");
				} else {
					xSemaphoreTake(queueSizeMutex, portMAX_DELAY);
					queueSize++;
					xSemaphoreGive(queueSizeMutex);
				}
				xSemaphoreGive(sensorMutex);
			}
		}
	}
}

static void humidityTask(void * pvParameters) {
	MyMQTTMessage mqmsg;
	char humidityStrBuffer[50];
	char endpoint[35] = "g00409592/groups/sensorDisplay";
	printf("Starting Humidity Publish Task\r\n");
	BSP_HSENSOR_Init();
	while(1) {
		if (xEventGroupWaitBits(timersEventGroupHandler, humidityBit, pdTRUE, pdFALSE, portMAX_DELAY)) {
			if (xSemaphoreTake(sensorMutex, portMAX_DELAY) == pdTRUE) {
				float humidityF = BSP_HSENSOR_ReadHumidity();
				uint16_t humidityI = humidityF*10;
				sprintf(humidityStrBuffer, "{\"feeds\": {\"humidity\":%d.%d}}", humidityI/10, humidityI%10);

				memset(&mqmsg, 0, sizeof(MyMQTTMessage));
				mqmsg.qos = QOS0;
				strcpy(mqmsg.payload, humidityStrBuffer);
				mqmsg.payloadlen = strlen(humidityStrBuffer);
				strcpy(mqmsg.endpoint, endpoint);

				if (xQueueSend(publishQueue, &mqmsg, 0) != pdPASS) {
					printf("Could not send data to queue. Humidity\r\n");
				} else {
					xSemaphoreTake(queueSizeMutex, portMAX_DELAY);
					queueSize++;
					xSemaphoreGive(queueSizeMutex);
				}
				xSemaphoreGive(sensorMutex);
			}
		}
	}
}

static void pressureTask(void * pvParameters) {
	MyMQTTMessage mqmsg;
	char pressureStrBuffer[50];
	char endpoint[35] = "g00409592/groups/sensorDisplay";
	printf("Starting Pressure Sensor Task\r\n");
	BSP_PSENSOR_Init();
	while(1) {
		if (xEventGroupWaitBits(timersEventGroupHandler, pressureBit, pdTRUE, pdFALSE, portMAX_DELAY)) {
			if (xSemaphoreTake(sensorMutex, portMAX_DELAY) == pdTRUE) {
				float pressureF = BSP_PSENSOR_ReadPressure();
				uint16_t pressureI = pressureF*10;
				sprintf(pressureStrBuffer, "{\"feeds\": {\"pressure\":%d.%d}}", pressureI/10, pressureI%10);

				memset(&mqmsg, 0, sizeof(MyMQTTMessage));
				mqmsg.qos = QOS0;
				strcpy(mqmsg.payload, pressureStrBuffer);
				mqmsg.payloadlen = strlen(pressureStrBuffer);
				strcpy(mqmsg.endpoint, endpoint);

				if (xQueueSend(publishQueue, &mqmsg, 0) != pdPASS) {
					printf("Could not send data to queue. Pressure\r\n");
				} else {
					xSemaphoreTake(queueSizeMutex, portMAX_DELAY);
					queueSize++;
					xSemaphoreGive(queueSizeMutex);
				}
				xSemaphoreGive(sensorMutex);
			}
		}
	}
}

static void publishTask(void * pvParameters) {
	MyMQTTMessage msgToPublish;
	MQTTMessage mqmsg;
	printf("Starting publish task\r\n");
	while (1) {
		if (xQueueReceive(publishQueue, &msgToPublish, 0) == pdPASS) {
			xSemaphoreTake(queueSizeMutex, portMAX_DELAY);
			printf("Publishing message: %s. Current queue size: %d\r\n", msgToPublish.payload, queueSize); // There was a race condition
			xSemaphoreGive(queueSizeMutex);

			memset(&mqmsg, 0, sizeof(MQTTMessage));
			mqmsg.qos = msgToPublish.qos;
			mqmsg.payload = (char *)msgToPublish.payload;
			mqmsg.payloadlen = msgToPublish.payloadlen;

			HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 1);
			MQTTPublish(&client, msgToPublish.endpoint, &mqmsg);
			HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 0);
			queueSize--;
		}
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

/*-----------------------------------------------------------
 * Task to print the date and time when a semaphore is taken
-------------------------------------------------------------*/
static void RTC_Task(void * pvParameters) {
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;
//	char timeBuffer[40];
	uint32_t count = 0;

	RTC_TaskRunning = 1;	//flag set to allow RTC event interrupt handler callback to send semaphores

	while(1) {
		MQTTYield(&client, 200);	//Yield needed to allow check for received published messages from subscribed topics

//		if(xSemaphoreTake(oneSecondSemaphore, 0) == pdTRUE) {
			count++;
//			if (count % 5 == 0) { // print every 5 seconds
				timeDisplay = 0;
				HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
				HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
				if (client.isconnected) HAL_IWDG_Refresh(&hiwdg);
//				sprintf(timeBuffer, "%02d/%02d/%02d %02d:%02d:%02d\r\n", sDate.Date, sDate.Month, sDate.Year, sTime.Hours+1, sTime.Minutes, sTime.Seconds);
//				printf("%s", timeBuffer);
//			}
//		}
		vTaskDelay(pdMS_TO_TICKS(200));
	}
}

void createSensorTasks() {
	// Temperature task
	if(xTaskCreate(temperatureTask, "Temperature Task", 500, NULL, configMAX_PRIORITIES - 4, NULL) == pdTRUE) {
		printf("Temperature task created\n\r");
	} else printf("Could not create temperature task\n\r");

	// Humidity task
	if(xTaskCreate(humidityTask, "Humidity Task", 500, NULL, configMAX_PRIORITIES - 4, NULL) == pdTRUE) {
		printf("Humidity task created\n\r");
	} else printf("Could not create Humidity Task\n\r");

	// Pressure task
	if(xTaskCreate(pressureTask, "Pressure Task", 500, NULL, configMAX_PRIORITIES - 4, NULL) == pdTRUE) {
		printf("Pressure task created\r\n");
	} else printf("Could not create Pressure Task\r\n");
}

void performSubscriptions() {
	uint32_t ret;
	ret = MQTTSubscribe(&client, "g00409592/groups/temperature", QOS0, subscribeMessageHandler);
	ret = MQTTYield(&client, 500);
	if (ret != MQSUCCESS) printf("\n\rSubscribe to temperature group failed: %ld\n\r", ret);

	HAL_IWDG_Refresh(&hiwdg);

	ret = MQTTSubscribe(&client, "g00409592/groups/humidity", QOS0, subscribeMessageHandler);
	ret = MQTTYield(&client, 500);
	if (ret != MQSUCCESS) printf("\n\rSubscribe to humidity group failed: %ld\n\r", ret);

	HAL_IWDG_Refresh(&hiwdg);

	ret = MQTTSubscribe(&client, "g00409592/groups/pressure", QOS0, subscribeMessageHandler);
	ret = MQTTYield(&client, 500);
	if (ret != MQSUCCESS) printf("\n\rSubscribe to pressure group failed: %ld\n\r", ret);
}

void getLatestValues() {
	MQTTMessage mqmsg;
	mqmsg.qos = QOS0;
	MQTTPublish(&client, "g00409592/groups/temperature/get", &mqmsg);
	MQTTPublish(&client, "g00409592/groups/humidity/get", &mqmsg);
	MQTTPublish(&client, "g00409592/groups/pressure/get", &mqmsg);
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
//	uint32_t ret;
	printf("Starting Init Task\r\n");

	while(1) {
		HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
		brokerConnect(&client);	//connect to WiFi access point and then to MQTT broker

		oneSecondSemaphore = xSemaphoreCreateBinary();
		vQueueAddToRegistry(oneSecondSemaphore, "RTC Semaphore");

		sensorMutex = xSemaphoreCreateMutex();
		vQueueAddToRegistry(sensorMutex, "Sensor Mutex");

		queueSizeMutex = xSemaphoreCreateMutex();
		vQueueAddToRegistry(queueSizeMutex, "Queue Size Mutex");

		// Timers event groups
		timersEventGroupHandler = xEventGroupCreate();
		if (timersEventGroupHandler == NULL) {
			printf("Timers event group handler creation failed\r\n");
		} else {
			printf("Timers event group created");
		}

		// Queue
		publishQueue = xQueueCreate(5, sizeof(MyMQTTMessage));
		vQueueAddToRegistry(publishQueue, "Publish Queue");

		createSensorTasks();



		// Publish task
		if(xTaskCreate(publishTask, "Publish Task", 500, NULL, configMAX_PRIORITIES - 3, NULL) == pdTRUE) {
			printf("Publish task created\n\r");
		} else printf("Could not create Publish Task\n\r");

		if(xTaskCreate(RTC_Task, "RTC Task", 500, NULL, configMAX_PRIORITIES - 2, NULL) == pdTRUE) {
			printf("RTC task created\n\r");
		} else printf("Could not create RTC task\n\r");

		// Temperature timer creation
		temperatureTimerHandler = xTimerCreate("Temperature timer", pdMS_TO_TICKS(5000), pdTRUE, NULL, timersCallback);
		if (temperatureTimerHandler == NULL) {
			printf("Temperature timer creation failed.\r\n");
		} else {
			if (xTimerStart(temperatureTimerHandler, 0) == pdTRUE) {
				printf("Temperature timer started.\r\n");
			} else {
				printf("Temperature timer start failed.\r\n");
			}
		}

		// Humidity timer creation
		humidityTimerHandler = xTimerCreate("Humidity timer", pdMS_TO_TICKS(5000), pdTRUE, NULL, timersCallback);
		if (humidityTimerHandler == NULL) {
			printf("Humidity timer creation failed.\r\n");
		} else {
			if (xTimerStart(humidityTimerHandler, 0) == pdTRUE) {
				printf("Humidity timer started.\r\n");
			} else {
				printf("Humidity timer start failed.\r\n");
			}
		}
		HAL_IWDG_Refresh(&hiwdg);

		// Pressure timer creation
		pressureTimerHandler = xTimerCreate("Pressure timer", pdMS_TO_TICKS(5000), pdTRUE, NULL, timersCallback);
		if (pressureTimerHandler == NULL) {
			printf("Pressure timer creation failed.\r\n");
		} else {
			if (xTimerStart(pressureTimerHandler, 0) == pdTRUE) {
				printf("Pressure timer started.\r\n");
			} else {
				printf("Humidity timer start failed.\r\n");
			}
		}

		HAL_IWDG_Refresh(&hiwdg);

		performSubscriptions();
		getLatestValues();


//		__HAL_TIM_CLEAR_IT(&htim6, TIM_IT_UPDATE);
//		HAL_NVIC_GetPendingIRQ(TIM6_DAC_IRQn);
//		HAL_TIM_Base_Start_IT(&htim6);

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

	xTaskCreate(initTask, "Init Task", 1000, NULL, configMAX_PRIORITIES - 1, NULL);
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
	HAL_IWDG_Refresh(&hiwdg);
	HAL_Delay(500);
	HAL_IWDG_Refresh(&hiwdg);

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
	HAL_IWDG_Refresh(&hiwdg);

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
	HAL_IWDG_Refresh(&hiwdg);

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
	HAL_IWDG_Refresh(&hiwdg);

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
	HAL_IWDG_Refresh(&hiwdg);

	network.my_socket = socket;
	network.mqttread = (network_rd);
	network.mqttwrite = (network_wr);

	MQTTClientInit(client, &network, MQTT_CMD_TIMEOUT, mqtt_send_buffer, MQTT_SEND_BUFFER_SIZE,
			mqtt_read_buffer, MQTT_READ_BUFFER_SIZE);
	HAL_IWDG_Refresh(&hiwdg);

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
	HAL_IWDG_Refresh(&hiwdg);
}
