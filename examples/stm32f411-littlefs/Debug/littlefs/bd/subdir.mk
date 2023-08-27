################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../littlefs/bd/lfs_emubd.c \
../littlefs/bd/lfs_filebd.c \
../littlefs/bd/lfs_rambd.c 

OBJS += \
./littlefs/bd/lfs_emubd.o \
./littlefs/bd/lfs_filebd.o \
./littlefs/bd/lfs_rambd.o 

C_DEPS += \
./littlefs/bd/lfs_emubd.d \
./littlefs/bd/lfs_filebd.d \
./littlefs/bd/lfs_rambd.d 


# Each subdirectory must supply rules for building sources it contributes
littlefs/bd/%.o littlefs/bd/%.su littlefs/bd/%.cyclo: ../littlefs/bd/%.c littlefs/bd/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -DM24CXX_MODEL=M24C08 -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/home/lth/src/stm32-m24cxx/examples/stm32f411-littlefs/m24cxx" -I"/home/lth/src/stm32-m24cxx/examples/stm32f411-littlefs/littlefs" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-littlefs-2f-bd

clean-littlefs-2f-bd:
	-$(RM) ./littlefs/bd/lfs_emubd.cyclo ./littlefs/bd/lfs_emubd.d ./littlefs/bd/lfs_emubd.o ./littlefs/bd/lfs_emubd.su ./littlefs/bd/lfs_filebd.cyclo ./littlefs/bd/lfs_filebd.d ./littlefs/bd/lfs_filebd.o ./littlefs/bd/lfs_filebd.su ./littlefs/bd/lfs_rambd.cyclo ./littlefs/bd/lfs_rambd.d ./littlefs/bd/lfs_rambd.o ./littlefs/bd/lfs_rambd.su

.PHONY: clean-littlefs-2f-bd

