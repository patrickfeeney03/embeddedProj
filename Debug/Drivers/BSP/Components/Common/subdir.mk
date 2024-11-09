################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/Components/Common/hts221.c \
../Drivers/BSP/Components/Common/lis3mdl.c \
../Drivers/BSP/Components/Common/lps22hb.c \
../Drivers/BSP/Components/Common/lsm6dsl.c \
../Drivers/BSP/Components/Common/vl53l0x_api.c \
../Drivers/BSP/Components/Common/vl53l0x_api_calibration.c \
../Drivers/BSP/Components/Common/vl53l0x_api_core.c \
../Drivers/BSP/Components/Common/vl53l0x_api_ranging.c \
../Drivers/BSP/Components/Common/vl53l0x_api_strings.c \
../Drivers/BSP/Components/Common/vl53l0x_platform_log.c 

OBJS += \
./Drivers/BSP/Components/Common/hts221.o \
./Drivers/BSP/Components/Common/lis3mdl.o \
./Drivers/BSP/Components/Common/lps22hb.o \
./Drivers/BSP/Components/Common/lsm6dsl.o \
./Drivers/BSP/Components/Common/vl53l0x_api.o \
./Drivers/BSP/Components/Common/vl53l0x_api_calibration.o \
./Drivers/BSP/Components/Common/vl53l0x_api_core.o \
./Drivers/BSP/Components/Common/vl53l0x_api_ranging.o \
./Drivers/BSP/Components/Common/vl53l0x_api_strings.o \
./Drivers/BSP/Components/Common/vl53l0x_platform_log.o 

C_DEPS += \
./Drivers/BSP/Components/Common/hts221.d \
./Drivers/BSP/Components/Common/lis3mdl.d \
./Drivers/BSP/Components/Common/lps22hb.d \
./Drivers/BSP/Components/Common/lsm6dsl.d \
./Drivers/BSP/Components/Common/vl53l0x_api.d \
./Drivers/BSP/Components/Common/vl53l0x_api_calibration.d \
./Drivers/BSP/Components/Common/vl53l0x_api_core.d \
./Drivers/BSP/Components/Common/vl53l0x_api_ranging.d \
./Drivers/BSP/Components/Common/vl53l0x_api_strings.d \
./Drivers/BSP/Components/Common/vl53l0x_platform_log.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/Components/Common/%.o Drivers/BSP/Components/Common/%.su Drivers/BSP/Components/Common/%.cyclo: ../Drivers/BSP/Components/Common/%.c Drivers/BSP/Components/Common/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L475xx -DUSE_WIFI -DMSG_ERROR -DUSE_TIMEDATE -DGENERICMQTT -DUSE_CLEAR_TIMEDATE -DSTM32_THREAD_SAFE_STRATEGY=4 -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/BSP/B-L475E-IOT01 -I../Drivers/BSP/Components/es_wifi -I../Drivers/BSP/Components/Common -I../WiFi -I../Common/Shared/Inc -I../Middleware/cJSON -I../Middleware/mbedTLS/Inc -I../Middleware/MQTTPacket -I../Middleware/MQTTClient-C -I../Common/GenericMQTT/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Core/ThreadSafe -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-Components-2f-Common

clean-Drivers-2f-BSP-2f-Components-2f-Common:
	-$(RM) ./Drivers/BSP/Components/Common/hts221.cyclo ./Drivers/BSP/Components/Common/hts221.d ./Drivers/BSP/Components/Common/hts221.o ./Drivers/BSP/Components/Common/hts221.su ./Drivers/BSP/Components/Common/lis3mdl.cyclo ./Drivers/BSP/Components/Common/lis3mdl.d ./Drivers/BSP/Components/Common/lis3mdl.o ./Drivers/BSP/Components/Common/lis3mdl.su ./Drivers/BSP/Components/Common/lps22hb.cyclo ./Drivers/BSP/Components/Common/lps22hb.d ./Drivers/BSP/Components/Common/lps22hb.o ./Drivers/BSP/Components/Common/lps22hb.su ./Drivers/BSP/Components/Common/lsm6dsl.cyclo ./Drivers/BSP/Components/Common/lsm6dsl.d ./Drivers/BSP/Components/Common/lsm6dsl.o ./Drivers/BSP/Components/Common/lsm6dsl.su ./Drivers/BSP/Components/Common/vl53l0x_api.cyclo ./Drivers/BSP/Components/Common/vl53l0x_api.d ./Drivers/BSP/Components/Common/vl53l0x_api.o ./Drivers/BSP/Components/Common/vl53l0x_api.su ./Drivers/BSP/Components/Common/vl53l0x_api_calibration.cyclo ./Drivers/BSP/Components/Common/vl53l0x_api_calibration.d ./Drivers/BSP/Components/Common/vl53l0x_api_calibration.o ./Drivers/BSP/Components/Common/vl53l0x_api_calibration.su ./Drivers/BSP/Components/Common/vl53l0x_api_core.cyclo ./Drivers/BSP/Components/Common/vl53l0x_api_core.d ./Drivers/BSP/Components/Common/vl53l0x_api_core.o ./Drivers/BSP/Components/Common/vl53l0x_api_core.su ./Drivers/BSP/Components/Common/vl53l0x_api_ranging.cyclo ./Drivers/BSP/Components/Common/vl53l0x_api_ranging.d ./Drivers/BSP/Components/Common/vl53l0x_api_ranging.o ./Drivers/BSP/Components/Common/vl53l0x_api_ranging.su ./Drivers/BSP/Components/Common/vl53l0x_api_strings.cyclo ./Drivers/BSP/Components/Common/vl53l0x_api_strings.d ./Drivers/BSP/Components/Common/vl53l0x_api_strings.o ./Drivers/BSP/Components/Common/vl53l0x_api_strings.su ./Drivers/BSP/Components/Common/vl53l0x_platform_log.cyclo ./Drivers/BSP/Components/Common/vl53l0x_platform_log.d ./Drivers/BSP/Components/Common/vl53l0x_platform_log.o ./Drivers/BSP/Components/Common/vl53l0x_platform_log.su

.PHONY: clean-Drivers-2f-BSP-2f-Components-2f-Common

