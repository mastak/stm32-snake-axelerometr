################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/gpio.c \
../Src/i2c.c \
../Src/main.c \
../Src/stm32f4xx_hal_msp.c \
../Src/stm32f4xx_it.c \
../Src/usart.c 

OBJS += \
./Src/gpio.o \
./Src/i2c.o \
./Src/main.o \
./Src/stm32f4xx_hal_msp.o \
./Src/stm32f4xx_it.o \
./Src/usart.o 

C_DEPS += \
./Src/gpio.d \
./Src/i2c.d \
./Src/main.d \
./Src/stm32f4xx_hal_msp.d \
./Src/stm32f4xx_it.d \
./Src/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -D__weak="__attribute__((weak))" -D__packed="__attribute__((__packed__))" -DUSE_HAL_DRIVER -DSTM32F446xx -I"/Users/mastak/micro/source/13_i2c_usart/Inc" -I"/Users/mastak/micro/source/13_i2c_usart/Drivers/I2Cdev" -I"/Users/mastak/micro/source/13_i2c_usart/Drivers/display" -I"/Users/mastak/micro/source/13_i2c_usart/Drivers/MPU9250" -I"/Users/mastak/micro/source/13_i2c_usart/Drivers/MadgwickAHRS" -I"/Users/mastak/micro/source/13_i2c_usart/Drivers/BMP280" -I"/Users/mastak/micro/source/13_i2c_usart/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/mastak/micro/source/13_i2c_usart/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/mastak/micro/source/13_i2c_usart/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/mastak/micro/source/13_i2c_usart/Drivers/CMSIS/Include"  -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


