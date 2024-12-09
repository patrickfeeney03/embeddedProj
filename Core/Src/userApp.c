/*
 * userApp.c
 *
 *  Created on: Created on: Nov-Dec 2024
 *      Author: Patrick Feeney
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

SemaphoreHandle_t publishSemaphore = NULL;
TimerHandle_t temperatureTimerHandler = NULL, humidityTimerHandler = NULL, pressureTimerHandler = NULL;
EventGroupHandle_t timersEventGroupHandler = NULL;
QueueHandle_t publishQueue = NULL;
uint8_t queueSize = 0, ch;
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
    char endpoint[65];
    char payload[255];
    size_t payloadlen;
    enum QoS qos;
} MyMQTTMessage;

/*
 * This is the handler for the Software Timers. Each timer sets a specific bit in the event group.
 */
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
	switch (GPIO_Pin)
	{
	case (GPIO_PIN_1):	//IRQ generated by WiFi module
		{
		SPI_WIFI_ISR();
		break;
		}

	case (BUTTON_EXTI13_Pin) :	//IRQ generated by user button
		{
		printf("User button pressed. I'm not using this for anything LOL!\r\n");
		break;
		}

	default:
	{
		break;
	}
	}
}

/*
 * This function is used to set a switch and slider to a specific value. Both at the same time.
 */
void publishDataToGroupISR(char sensorName[], uint16_t slider, bool control, BaseType_t *xHigherPriorityTaskWoken) {
	char payload[100], topic[60];
	MyMQTTMessage mqmsg;

	sprintf(topic, "g00409592/groups/%s", sensorName);
	sprintf(payload, "{\"feeds\": {\"%sslider\":%d, \"%scontrol\":%d}}", sensorName, slider, sensorName, control);

	memset(&mqmsg, 0, sizeof(MyMQTTMessage));
	mqmsg.qos = QOS0;
	strcpy(mqmsg.endpoint, topic);
	strcpy(mqmsg.payload, payload);
	mqmsg.payloadlen = strlen(payload);

	queueSize++;
	if (xQueueSendFromISR(publishQueue, &mqmsg, xHigherPriorityTaskWoken) != pdPASS) {
		queueSize--;
		printf("Could not send data to queue for: %s\r\n", sensorName);
	}
}

/*
 * Sets all three pairs of switches and sliders to a specific value.
 */
void publishDataToThreeGroupsISR(
		uint16_t tempTime, uint8_t tempControl,
		uint16_t pressureTime, uint8_t pressureControl,
		uint16_t humidityTime, uint8_t humidityControl,
		BaseType_t *xHigherPriorityTaskWoken) {
	/*
	 * I can't publish and update the values with a single publish because they are each on different groups.
	 */
	publishDataToGroupISR("temperature", tempTime, tempControl, xHigherPriorityTaskWoken);
	publishDataToGroupISR("pressure", pressureTime, pressureControl, xHigherPriorityTaskWoken);
	publishDataToGroupISR("humidity", humidityTime, humidityControl, xHigherPriorityTaskWoken);
}

void HAL_UART_RxCpltCallback (UART_HandleTypeDef * huart) {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	switch (ch) {
	case 'r': // Reset back to default
		publishDataToThreeGroupsISR(8000, 1, 8000, 1, 8000, 1, &xHigherPriorityTaskWoken);
		break;
	case '0':  // Turn all off
		publishDataToThreeGroupsISR(8000, 0, 8000, 0, 8000, 0, &xHigherPriorityTaskWoken);
		break;
	case '1': // All on, quick temp
		publishDataToThreeGroupsISR(4000, 1, 35000, 1, 35000, 1, &xHigherPriorityTaskWoken);
		break;
	case '2': // All on quick pressure
		publishDataToThreeGroupsISR(35000, 1, 4000, 1, 35000, 1, &xHigherPriorityTaskWoken);
		break;
	case '3': // All on quick humidity
		publishDataToThreeGroupsISR(35000, 1, 35000, 1, 4000, 1, &xHigherPriorityTaskWoken);
		break;
	case '4': // Only temp on
		publishDataToThreeGroupsISR(8000, 1, 8000, 0, 8000, 0, &xHigherPriorityTaskWoken);
		break;
	case '5': // Only pressure on
		publishDataToThreeGroupsISR(8000, 0, 8000, 1, 8000, 0, &xHigherPriorityTaskWoken);
		break;
	case '6': // Only humidity on
		publishDataToThreeGroupsISR(8000, 0, 8000, 0, 8000, 1, &xHigherPriorityTaskWoken);
		break;
	case 'c':
		printf("\033[2J\033[3J\033[H\r\n"); // \033[2J Clears Screen, \033[3J Clears scrollback buff, \033[H resets cursor pos
		// https://en.wikipedia.org/wiki/ANSI_escape_code
		break;
	default:
		printf("Pressing %c does nothing.\r\n", ch);
	}
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	HAL_UART_Receive_IT(&huart1, &ch, 1);
}

void TIM6_Handler() {
}

/*--------------------------------------------------------------------
 * RTC timer event interrupt handler callback function
 * Runs every second and gives a semaphore if the RTC task is running
 --------------------------------------------------------------------*/
void HAL_RTCEx_WakeUpTimerEventCallback (RTC_HandleTypeDef * hrtc) {
	/*
	 * Removed the code. Didn't use it. Didn't need oneSecondSemaphore
	 */
}

void subscribeLogic(char mqtt_msg[], TimerHandle_t timerHandler, char loggingName[]) {
	uint8_t spaces;
	char slider[] = "slider";
	char control[] = "control";
	/*
	 * This helps handle both CSV and JSON
	 */
	if (strchr(mqtt_msg, '{')) {
		spaces = 3;
	} else {
		spaces = 1;
	}

	/*
	 * Examples
	 * {"feeds": {"temperatureslider":"20000"}}
	 * {"feeds": {"temperatureslider":"20000", "temperaturecontrol":"0"}}
	 * temperatureslider,2000	temperaturecontrol,0
	 */
	if (strstr(mqtt_msg, slider)) {
		char *ptr = strstr(mqtt_msg, slider) + strlen(slider) + spaces;
		uint16_t sliderVal = atoi(ptr);
		printf("Setting %s Timer to: %dms\r\n", loggingName, sliderVal);
		xTimerChangePeriod(timerHandler, pdMS_TO_TICKS(sliderVal), pdMS_TO_TICKS(10));
	}
	if (strstr(mqtt_msg, control)) { // Not else if because a single subscription can have more than 1 variable
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
		subscribeLogic(mqtt_msg, temperatureTimerHandler, "Temperature");
	} else if (strstr(mqtt_topic, "groups/pressure")) {
		subscribeLogic(mqtt_msg, pressureTimerHandler, "Pressure");
	} else if (strstr(mqtt_topic, "groups/humidity")) {
		subscribeLogic(mqtt_msg, humidityTimerHandler, "Humidity");
	}
}

void buildMessageAndSendToQueueForDisplay(uint16_t valueI, char sensorName[]) {
	char buff[100], endpoint[] = "g00409592/groups/sensorDisplay";
	MyMQTTMessage mqmsg;

	sprintf(buff, "{\"feeds\": {\"%s\":%d.%d}}", sensorName, valueI/10, valueI%10);

	memset(&mqmsg, 0, sizeof(MyMQTTMessage));
	strcpy(mqmsg.endpoint, endpoint);
	mqmsg.qos = QOS0;
	strcpy(mqmsg.payload, buff);
	mqmsg.payloadlen = strlen(buff);

	queueSize++;
	if (xQueueSend(publishQueue, &mqmsg, 0) != pdPASS) {
		queueSize--;
		printf("Could not send data to queue. Temperature\r\n");
	}
}

static void temperatureTask(void * pvParameters) {
	printf("Starting Temperature Publish Task\r\n");
	BSP_TSENSOR_Init();
	while(1) {
		if (xEventGroupWaitBits(timersEventGroupHandler, temperatureBit, pdTRUE, pdFALSE, portMAX_DELAY)) {
			float tempF = BSP_TSENSOR_ReadTemp();
			uint16_t tempI = tempF*10;
			buildMessageAndSendToQueueForDisplay(tempI, "temperature");
		}
	}
}

static void humidityTask(void * pvParameters) {
	printf("Starting Humidity Publish Task\r\n");
	BSP_HSENSOR_Init();
	while(1) {
		if (xEventGroupWaitBits(timersEventGroupHandler, humidityBit, pdTRUE, pdFALSE, portMAX_DELAY)) {
			float humidityF = BSP_HSENSOR_ReadHumidity();
			uint16_t humidityI = humidityF*10;
			buildMessageAndSendToQueueForDisplay(humidityI, "humidity");
		}
	}
}

static void pressureTask(void * pvParameters) {
	printf("Starting Pressure Sensor Task\r\n");
	BSP_PSENSOR_Init();
	while(1) {
		if (xEventGroupWaitBits(timersEventGroupHandler, pressureBit, pdTRUE, pdFALSE, portMAX_DELAY)) {
			float pressureF = BSP_PSENSOR_ReadPressure();
			uint16_t pressureI = pressureF*10;
			buildMessageAndSendToQueueForDisplay(pressureI, "pressure");
		}
	}
}

static void publishTask(void * pvParameters) {
	MyMQTTMessage msgToPublish;
	MQTTMessage mqmsg;
	printf("Starting publish task\r\n");
	while (1) {
		if (xQueueReceive(publishQueue, &msgToPublish, 0) == pdPASS) {
			printf("Publishing message: %s. Current queue size: %d\r\n", msgToPublish.payload, queueSize);

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
	while(1) {
		MQTTYield(&client, 200);	//Yield needed to allow check for received published messages from subscribed topics

		if (client.isconnected) HAL_IWDG_Refresh(&hiwdg);

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

/*
 * Each of these groups has 2 feeds
 */
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
	MyMQTTMessage mqmsg;
	memset(&mqmsg, 0, sizeof(MQTTMessage));
	mqmsg.qos = QOS0;
	mqmsg.payloadlen = 0;

	strcpy(mqmsg.endpoint, "g00409592/groups/temperature/get");
	queueSize++;
	if (xQueueSend(publishQueue, &mqmsg, 0) != pdPASS) {
		queueSize--;
		printf("Could not send data to queue. Initial Temperature\r\n");
	}

	strcpy(mqmsg.endpoint, "g00409592/groups/humidity/get");
	queueSize++;
	if (xQueueSend(publishQueue, &mqmsg, 0) != pdPASS) {
		queueSize--;
		printf("Could not send data to queue. Initial Humidity\r\n");
	}

	strcpy(mqmsg.endpoint, "g00409592/groups/pressure/get");
	queueSize++;
	if (xQueueSend(publishQueue, &mqmsg, 0) != pdPASS) {
		queueSize--;
		printf("Could not send data to queue. Initial Pressure\r\n");
	}
}

void createTimer(TimerHandle_t *timerHandler, char timerName[], uint16_t initialDelay) {
	*timerHandler = xTimerCreate(timerName, pdMS_TO_TICKS(initialDelay), pdTRUE, NULL, timersCallback);
	if (*timerHandler == NULL) {
		printf("%s timer creation failed.\r\n", timerName);
	} else {
		if (xTimerStart(*timerHandler, 0) == pdTRUE) {
			printf("%s timer started.\r\n", timerName);
		} else {
			printf("%s timer start failed.\r\n", timerName);
		}
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
//	uint32_t ret;
	printf("Starting Init Task\r\n");

	while(1) {
		HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
		brokerConnect(&client);	//connect to WiFi access point and then to MQTT broker

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

		// Create Software Timers
		createTimer(&temperatureTimerHandler, "Temperature", 8000);
		createTimer(&humidityTimerHandler, "Humidity", 8000);
		createTimer(&pressureTimerHandler, "Pressure", 8000);

		HAL_IWDG_Refresh(&hiwdg);

		// Subscribe to the Control Switches and The Time Sliders
		performSubscriptions();
		// Get current state of those to get in-sync
		getLatestValues();

		// Enable UART Interrupt
		HAL_UART_Receive_IT(&huart1, &ch, 1);

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
