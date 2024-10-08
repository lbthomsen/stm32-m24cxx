################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../littlefs/lfs.c \
../littlefs/lfs_util.c 

OBJS += \
./littlefs/lfs.o \
./littlefs/lfs_util.o 

C_DEPS += \
./littlefs/lfs.d \
./littlefs/lfs_util.d 


# Each subdirectory must supply rules for building sources it contributes
littlefs/%.o littlefs/%.su littlefs/%.cyclo: ../littlefs/%.c littlefs/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -DM24CXX_MODEL=M24M01X4 -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/home/lth/src/stm32-m24cxx/examples/stm32f405-m24m01/m24cxx" -I"/home/lth/src/stm32-m24cxx/examples/stm32f405-m24m01/littlefs" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-littlefs

clean-littlefs:
	-$(RM) ./littlefs/lfs.cyclo ./littlefs/lfs.d ./littlefs/lfs.o ./littlefs/lfs.su ./littlefs/lfs_util.cyclo ./littlefs/lfs_util.d ./littlefs/lfs_util.o ./littlefs/lfs_util.su

.PHONY: clean-littlefs

