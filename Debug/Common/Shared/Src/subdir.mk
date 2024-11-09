################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Common/Shared/Src/STM32CubeRTCInterface.c \
../Common/Shared/Src/c2c_net.c \
../Common/Shared/Src/entropy_hardware_poll.c \
../Common/Shared/Src/heap.c \
../Common/Shared/Src/http_lib.c \
../Common/Shared/Src/iot_flash_config.c \
../Common/Shared/Src/lwip_net.c \
../Common/Shared/Src/mbedtls_net.c \
../Common/Shared/Src/net.c \
../Common/Shared/Src/net_tcp_c2c.c \
../Common/Shared/Src/net_tcp_lwip.c \
../Common/Shared/Src/net_tcp_wifi.c \
../Common/Shared/Src/net_tls_mbedtls.c \
../Common/Shared/Src/paho_timer.c \
../Common/Shared/Src/rfu.c \
../Common/Shared/Src/sensors_data.c \
../Common/Shared/Src/timedate.c \
../Common/Shared/Src/timingSystem.c \
../Common/Shared/Src/wifi_net.c 

OBJS += \
./Common/Shared/Src/STM32CubeRTCInterface.o \
./Common/Shared/Src/c2c_net.o \
./Common/Shared/Src/entropy_hardware_poll.o \
./Common/Shared/Src/heap.o \
./Common/Shared/Src/http_lib.o \
./Common/Shared/Src/iot_flash_config.o \
./Common/Shared/Src/lwip_net.o \
./Common/Shared/Src/mbedtls_net.o \
./Common/Shared/Src/net.o \
./Common/Shared/Src/net_tcp_c2c.o \
./Common/Shared/Src/net_tcp_lwip.o \
./Common/Shared/Src/net_tcp_wifi.o \
./Common/Shared/Src/net_tls_mbedtls.o \
./Common/Shared/Src/paho_timer.o \
./Common/Shared/Src/rfu.o \
./Common/Shared/Src/sensors_data.o \
./Common/Shared/Src/timedate.o \
./Common/Shared/Src/timingSystem.o \
./Common/Shared/Src/wifi_net.o 

C_DEPS += \
./Common/Shared/Src/STM32CubeRTCInterface.d \
./Common/Shared/Src/c2c_net.d \
./Common/Shared/Src/entropy_hardware_poll.d \
./Common/Shared/Src/heap.d \
./Common/Shared/Src/http_lib.d \
./Common/Shared/Src/iot_flash_config.d \
./Common/Shared/Src/lwip_net.d \
./Common/Shared/Src/mbedtls_net.d \
./Common/Shared/Src/net.d \
./Common/Shared/Src/net_tcp_c2c.d \
./Common/Shared/Src/net_tcp_lwip.d \
./Common/Shared/Src/net_tcp_wifi.d \
./Common/Shared/Src/net_tls_mbedtls.d \
./Common/Shared/Src/paho_timer.d \
./Common/Shared/Src/rfu.d \
./Common/Shared/Src/sensors_data.d \
./Common/Shared/Src/timedate.d \
./Common/Shared/Src/timingSystem.d \
./Common/Shared/Src/wifi_net.d 


# Each subdirectory must supply rules for building sources it contributes
Common/Shared/Src/%.o Common/Shared/Src/%.su Common/Shared/Src/%.cyclo: ../Common/Shared/Src/%.c Common/Shared/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L475xx -DUSE_WIFI -DMSG_ERROR -DUSE_TIMEDATE -DGENERICMQTT -DUSE_CLEAR_TIMEDATE -DSTM32_THREAD_SAFE_STRATEGY=4 -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/BSP/B-L475E-IOT01 -I../Drivers/BSP/Components/es_wifi -I../Drivers/BSP/Components/Common -I../WiFi -I../Common/Shared/Inc -I../Middleware/cJSON -I../Middleware/mbedTLS/Inc -I../Middleware/MQTTPacket -I../Middleware/MQTTClient-C -I../Common/GenericMQTT/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Core/ThreadSafe -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Common-2f-Shared-2f-Src

clean-Common-2f-Shared-2f-Src:
	-$(RM) ./Common/Shared/Src/STM32CubeRTCInterface.cyclo ./Common/Shared/Src/STM32CubeRTCInterface.d ./Common/Shared/Src/STM32CubeRTCInterface.o ./Common/Shared/Src/STM32CubeRTCInterface.su ./Common/Shared/Src/c2c_net.cyclo ./Common/Shared/Src/c2c_net.d ./Common/Shared/Src/c2c_net.o ./Common/Shared/Src/c2c_net.su ./Common/Shared/Src/entropy_hardware_poll.cyclo ./Common/Shared/Src/entropy_hardware_poll.d ./Common/Shared/Src/entropy_hardware_poll.o ./Common/Shared/Src/entropy_hardware_poll.su ./Common/Shared/Src/heap.cyclo ./Common/Shared/Src/heap.d ./Common/Shared/Src/heap.o ./Common/Shared/Src/heap.su ./Common/Shared/Src/http_lib.cyclo ./Common/Shared/Src/http_lib.d ./Common/Shared/Src/http_lib.o ./Common/Shared/Src/http_lib.su ./Common/Shared/Src/iot_flash_config.cyclo ./Common/Shared/Src/iot_flash_config.d ./Common/Shared/Src/iot_flash_config.o ./Common/Shared/Src/iot_flash_config.su ./Common/Shared/Src/lwip_net.cyclo ./Common/Shared/Src/lwip_net.d ./Common/Shared/Src/lwip_net.o ./Common/Shared/Src/lwip_net.su ./Common/Shared/Src/mbedtls_net.cyclo ./Common/Shared/Src/mbedtls_net.d ./Common/Shared/Src/mbedtls_net.o ./Common/Shared/Src/mbedtls_net.su ./Common/Shared/Src/net.cyclo ./Common/Shared/Src/net.d ./Common/Shared/Src/net.o ./Common/Shared/Src/net.su ./Common/Shared/Src/net_tcp_c2c.cyclo ./Common/Shared/Src/net_tcp_c2c.d ./Common/Shared/Src/net_tcp_c2c.o ./Common/Shared/Src/net_tcp_c2c.su ./Common/Shared/Src/net_tcp_lwip.cyclo ./Common/Shared/Src/net_tcp_lwip.d ./Common/Shared/Src/net_tcp_lwip.o ./Common/Shared/Src/net_tcp_lwip.su ./Common/Shared/Src/net_tcp_wifi.cyclo ./Common/Shared/Src/net_tcp_wifi.d ./Common/Shared/Src/net_tcp_wifi.o ./Common/Shared/Src/net_tcp_wifi.su ./Common/Shared/Src/net_tls_mbedtls.cyclo ./Common/Shared/Src/net_tls_mbedtls.d ./Common/Shared/Src/net_tls_mbedtls.o ./Common/Shared/Src/net_tls_mbedtls.su ./Common/Shared/Src/paho_timer.cyclo ./Common/Shared/Src/paho_timer.d ./Common/Shared/Src/paho_timer.o ./Common/Shared/Src/paho_timer.su ./Common/Shared/Src/rfu.cyclo ./Common/Shared/Src/rfu.d ./Common/Shared/Src/rfu.o ./Common/Shared/Src/rfu.su ./Common/Shared/Src/sensors_data.cyclo ./Common/Shared/Src/sensors_data.d ./Common/Shared/Src/sensors_data.o ./Common/Shared/Src/sensors_data.su ./Common/Shared/Src/timedate.cyclo ./Common/Shared/Src/timedate.d ./Common/Shared/Src/timedate.o ./Common/Shared/Src/timedate.su ./Common/Shared/Src/timingSystem.cyclo ./Common/Shared/Src/timingSystem.d ./Common/Shared/Src/timingSystem.o ./Common/Shared/Src/timingSystem.su ./Common/Shared/Src/wifi_net.cyclo ./Common/Shared/Src/wifi_net.d ./Common/Shared/Src/wifi_net.o ./Common/Shared/Src/wifi_net.su

.PHONY: clean-Common-2f-Shared-2f-Src

