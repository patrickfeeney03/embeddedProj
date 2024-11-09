################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middleware/MQTTPacket/MQTTConnectClient.c \
../Middleware/MQTTPacket/MQTTConnectServer.c \
../Middleware/MQTTPacket/MQTTDeserializePublish.c \
../Middleware/MQTTPacket/MQTTFormat.c \
../Middleware/MQTTPacket/MQTTPacket.c \
../Middleware/MQTTPacket/MQTTSerializePublish.c \
../Middleware/MQTTPacket/MQTTSubscribeClient.c \
../Middleware/MQTTPacket/MQTTSubscribeServer.c \
../Middleware/MQTTPacket/MQTTUnsubscribeClient.c \
../Middleware/MQTTPacket/MQTTUnsubscribeServer.c 

OBJS += \
./Middleware/MQTTPacket/MQTTConnectClient.o \
./Middleware/MQTTPacket/MQTTConnectServer.o \
./Middleware/MQTTPacket/MQTTDeserializePublish.o \
./Middleware/MQTTPacket/MQTTFormat.o \
./Middleware/MQTTPacket/MQTTPacket.o \
./Middleware/MQTTPacket/MQTTSerializePublish.o \
./Middleware/MQTTPacket/MQTTSubscribeClient.o \
./Middleware/MQTTPacket/MQTTSubscribeServer.o \
./Middleware/MQTTPacket/MQTTUnsubscribeClient.o \
./Middleware/MQTTPacket/MQTTUnsubscribeServer.o 

C_DEPS += \
./Middleware/MQTTPacket/MQTTConnectClient.d \
./Middleware/MQTTPacket/MQTTConnectServer.d \
./Middleware/MQTTPacket/MQTTDeserializePublish.d \
./Middleware/MQTTPacket/MQTTFormat.d \
./Middleware/MQTTPacket/MQTTPacket.d \
./Middleware/MQTTPacket/MQTTSerializePublish.d \
./Middleware/MQTTPacket/MQTTSubscribeClient.d \
./Middleware/MQTTPacket/MQTTSubscribeServer.d \
./Middleware/MQTTPacket/MQTTUnsubscribeClient.d \
./Middleware/MQTTPacket/MQTTUnsubscribeServer.d 


# Each subdirectory must supply rules for building sources it contributes
Middleware/MQTTPacket/%.o Middleware/MQTTPacket/%.su Middleware/MQTTPacket/%.cyclo: ../Middleware/MQTTPacket/%.c Middleware/MQTTPacket/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L475xx -DUSE_WIFI -DMSG_ERROR -DUSE_TIMEDATE -DGENERICMQTT -DUSE_CLEAR_TIMEDATE -DSTM32_THREAD_SAFE_STRATEGY=4 -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/BSP/B-L475E-IOT01 -I../Drivers/BSP/Components/es_wifi -I../Drivers/BSP/Components/Common -I../WiFi -I../Common/Shared/Inc -I../Middleware/cJSON -I../Middleware/mbedTLS/Inc -I../Middleware/MQTTPacket -I../Middleware/MQTTClient-C -I../Common/GenericMQTT/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Core/ThreadSafe -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middleware-2f-MQTTPacket

clean-Middleware-2f-MQTTPacket:
	-$(RM) ./Middleware/MQTTPacket/MQTTConnectClient.cyclo ./Middleware/MQTTPacket/MQTTConnectClient.d ./Middleware/MQTTPacket/MQTTConnectClient.o ./Middleware/MQTTPacket/MQTTConnectClient.su ./Middleware/MQTTPacket/MQTTConnectServer.cyclo ./Middleware/MQTTPacket/MQTTConnectServer.d ./Middleware/MQTTPacket/MQTTConnectServer.o ./Middleware/MQTTPacket/MQTTConnectServer.su ./Middleware/MQTTPacket/MQTTDeserializePublish.cyclo ./Middleware/MQTTPacket/MQTTDeserializePublish.d ./Middleware/MQTTPacket/MQTTDeserializePublish.o ./Middleware/MQTTPacket/MQTTDeserializePublish.su ./Middleware/MQTTPacket/MQTTFormat.cyclo ./Middleware/MQTTPacket/MQTTFormat.d ./Middleware/MQTTPacket/MQTTFormat.o ./Middleware/MQTTPacket/MQTTFormat.su ./Middleware/MQTTPacket/MQTTPacket.cyclo ./Middleware/MQTTPacket/MQTTPacket.d ./Middleware/MQTTPacket/MQTTPacket.o ./Middleware/MQTTPacket/MQTTPacket.su ./Middleware/MQTTPacket/MQTTSerializePublish.cyclo ./Middleware/MQTTPacket/MQTTSerializePublish.d ./Middleware/MQTTPacket/MQTTSerializePublish.o ./Middleware/MQTTPacket/MQTTSerializePublish.su ./Middleware/MQTTPacket/MQTTSubscribeClient.cyclo ./Middleware/MQTTPacket/MQTTSubscribeClient.d ./Middleware/MQTTPacket/MQTTSubscribeClient.o ./Middleware/MQTTPacket/MQTTSubscribeClient.su ./Middleware/MQTTPacket/MQTTSubscribeServer.cyclo ./Middleware/MQTTPacket/MQTTSubscribeServer.d ./Middleware/MQTTPacket/MQTTSubscribeServer.o ./Middleware/MQTTPacket/MQTTSubscribeServer.su ./Middleware/MQTTPacket/MQTTUnsubscribeClient.cyclo ./Middleware/MQTTPacket/MQTTUnsubscribeClient.d ./Middleware/MQTTPacket/MQTTUnsubscribeClient.o ./Middleware/MQTTPacket/MQTTUnsubscribeClient.su ./Middleware/MQTTPacket/MQTTUnsubscribeServer.cyclo ./Middleware/MQTTPacket/MQTTUnsubscribeServer.d ./Middleware/MQTTPacket/MQTTUnsubscribeServer.o ./Middleware/MQTTPacket/MQTTUnsubscribeServer.su

.PHONY: clean-Middleware-2f-MQTTPacket

