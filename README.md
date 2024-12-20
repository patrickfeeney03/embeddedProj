This is a Project that I did for my Embedded Real Time Operating Systems Class.

I use FreeRTOS and MQTT Paho. The board is an STM32 Discovery Kit B-L475E-IOT01A1.

To edit the project and genereate required code I've used the STM32CubeIDE.

This is a weather station that uses Software Timers to trigger multiple FreeRTOS tasks to read and publish data. I've used multiple Communication Objects such as a Queue-by-copy and an Event Group.

I used Adafruit IO as the MQTT Broker. This is what my dashboard looks like:
![image](https://github.com/user-attachments/assets/fe21b446-4574-4814-bdb7-605e33e2e942)

This is what my feeds look like
![image](https://github.com/user-attachments/assets/97557853-6c08-4d56-9a6f-99fee94a429d)
