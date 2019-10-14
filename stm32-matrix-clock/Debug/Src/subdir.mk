################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/ds3231.c \
../Src/i2c.c \
../Src/main.c \
../Src/max7219.c \
../Src/max7219font.c \
../Src/stm32f0xx_hal_msp.c \
../Src/stm32f0xx_it.c \
../Src/syscalls.c \
../Src/system_stm32f0xx.c 

OBJS += \
./Src/ds3231.o \
./Src/i2c.o \
./Src/main.o \
./Src/max7219.o \
./Src/max7219font.o \
./Src/stm32f0xx_hal_msp.o \
./Src/stm32f0xx_it.o \
./Src/syscalls.o \
./Src/system_stm32f0xx.o 

C_DEPS += \
./Src/ds3231.d \
./Src/i2c.d \
./Src/main.d \
./Src/max7219.d \
./Src/max7219font.d \
./Src/stm32f0xx_hal_msp.d \
./Src/stm32f0xx_it.d \
./Src/syscalls.d \
./Src/system_stm32f0xx.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F030x6 -I"D:/stm32-workspace/stm32-matrix-clock/Inc" -I"D:/stm32-workspace/stm32-matrix-clock/Drivers/STM32F0xx_HAL_Driver/Inc" -I"D:/stm32-workspace/stm32-matrix-clock/Drivers/STM32F0xx_HAL_Driver/Inc/Legacy" -I"D:/stm32-workspace/stm32-matrix-clock/Drivers/CMSIS/Device/ST/STM32F0xx/Include" -I"D:/stm32-workspace/stm32-matrix-clock/Drivers/CMSIS/Include"  -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


