################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Lib/msp/src/msp_crc.c \
../Core/Lib/msp/src/msp_endian.c \
../Core/Lib/msp/src/msp_exp_callback.c \
../Core/Lib/msp/src/msp_exp_frame.c \
../Core/Lib/msp/src/msp_exp_state.c \
../Core/Lib/msp/src/msp_handlers.c \
../Core/Lib/msp/src/msp_seqflags.c 

OBJS += \
./Core/Lib/msp/src/msp_crc.o \
./Core/Lib/msp/src/msp_endian.o \
./Core/Lib/msp/src/msp_exp_callback.o \
./Core/Lib/msp/src/msp_exp_frame.o \
./Core/Lib/msp/src/msp_exp_state.o \
./Core/Lib/msp/src/msp_handlers.o \
./Core/Lib/msp/src/msp_seqflags.o 

C_DEPS += \
./Core/Lib/msp/src/msp_crc.d \
./Core/Lib/msp/src/msp_endian.d \
./Core/Lib/msp/src/msp_exp_callback.d \
./Core/Lib/msp/src/msp_exp_frame.d \
./Core/Lib/msp/src/msp_exp_state.d \
./Core/Lib/msp/src/msp_handlers.d \
./Core/Lib/msp/src/msp_seqflags.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Lib/msp/src/%.o Core/Lib/msp/src/%.su: ../Core/Lib/msp/src/%.c Core/Lib/msp/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L053xx -c -I../Core/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L0xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Lib-2f-msp-2f-src

clean-Core-2f-Lib-2f-msp-2f-src:
	-$(RM) ./Core/Lib/msp/src/msp_crc.d ./Core/Lib/msp/src/msp_crc.o ./Core/Lib/msp/src/msp_crc.su ./Core/Lib/msp/src/msp_endian.d ./Core/Lib/msp/src/msp_endian.o ./Core/Lib/msp/src/msp_endian.su ./Core/Lib/msp/src/msp_exp_callback.d ./Core/Lib/msp/src/msp_exp_callback.o ./Core/Lib/msp/src/msp_exp_callback.su ./Core/Lib/msp/src/msp_exp_frame.d ./Core/Lib/msp/src/msp_exp_frame.o ./Core/Lib/msp/src/msp_exp_frame.su ./Core/Lib/msp/src/msp_exp_state.d ./Core/Lib/msp/src/msp_exp_state.o ./Core/Lib/msp/src/msp_exp_state.su ./Core/Lib/msp/src/msp_handlers.d ./Core/Lib/msp/src/msp_handlers.o ./Core/Lib/msp/src/msp_handlers.su ./Core/Lib/msp/src/msp_seqflags.d ./Core/Lib/msp/src/msp_seqflags.o ./Core/Lib/msp/src/msp_seqflags.su

.PHONY: clean-Core-2f-Lib-2f-msp-2f-src

