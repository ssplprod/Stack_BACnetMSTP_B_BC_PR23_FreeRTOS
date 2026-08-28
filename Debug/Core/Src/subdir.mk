################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/app_freertos.c \
../Core/Src/crc.c \
../Core/Src/gpio.c \
../Core/Src/main.c \
../Core/Src/memorymap.c \
../Core/Src/rtc.c \
../Core/Src/stm32h5xx_hal_msp.c \
../Core/Src/stm32h5xx_hal_timebase_tim.c \
../Core/Src/stm32h5xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32h5xx.c \
../Core/Src/tim.c \
../Core/Src/usart.c 

C_DEPS += \
./Core/Src/app_freertos.d \
./Core/Src/crc.d \
./Core/Src/gpio.d \
./Core/Src/main.d \
./Core/Src/memorymap.d \
./Core/Src/rtc.d \
./Core/Src/stm32h5xx_hal_msp.d \
./Core/Src/stm32h5xx_hal_timebase_tim.d \
./Core/Src/stm32h5xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32h5xx.d \
./Core/Src/tim.d \
./Core/Src/usart.d 

OBJS += \
./Core/Src/app_freertos.o \
./Core/Src/crc.o \
./Core/Src/gpio.o \
./Core/Src/main.o \
./Core/Src/memorymap.o \
./Core/Src/rtc.o \
./Core/Src/stm32h5xx_hal_msp.o \
./Core/Src/stm32h5xx_hal_timebase_tim.o \
./Core/Src/stm32h5xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32h5xx.o \
./Core/Src/tim.o \
./Core/Src/usart.o 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32H563xx '-D__weak=__attribute__((weak))' -DFAULT_ALGORITHM -DBACDEL_PR12 -DBACDEL_SER_AE_AA_B -DBACDEL_SER_AE_GEI_B -DBACDEL_SER_DM_RD_B -DINTRINSIC_REPORTING -DBACDEL_SER_DM_DDB_A -DBACDEL_SER_AE_EN_B -DBACDEL_SER_DM_UTC_B -DSEGMENTATION_SUPPORTED -DBACDEL_PR18 -DBACDEL_OBJ_NP -DBACDEL_OBJ_BSV -DBACDEL_OBJ_EE -DBACDEL_OBJ_NC -DBACDEL_OBJ_CAL -DBACDEL_OBJ_SDL -DOPTIONAL_PROPERTY -DBACDEL_SER_DS_COV_B -DBACDEL_FREERTOS '-D__packed=__attribute__((__packed__))' -DBACDEL_IP_STACK -DBACDEL_SER_DS_RP_B -DBACDEL_SER_DS_RPM_B -DBACDEL_SER_DS_WPM_B -DBACDEL_SER_DS_WP_B -DBACDEL_PR14 -DBACDEL_OBJ_DEV -DBACDEL_SER_DM_DDB_B -DBACDEL_SER_DM_DOB_B -DNETWORK_LAYER_MESSAGE_B -DBACDEL_SER_DM_DCC_B -DBACDEL_OBJ_BI -DBACDEL_OBJ_BO -DBACDEL_OBJ_BV -DBACDEL_OBJ_AI -DBACDEL_OBJ_AO -DBACDEL_OBJ_AV -DBACDEL_OBJ_MSI -DBACDEL_OBJ_MSO -DBACDEL_OBJ_MSV -DBACDEL_SER_DM_TS_B -DINITIATE_SERVICE_ENABLED -DOS_FREERTOS -DBBMD_ENABLED -DDEBUG -DNEW_RP_WP_INTERFACE -DUSE_NUCLEO_64 -c -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@" @"Core/Src/app_freertos.c_includes.args"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/app_freertos.cyclo ./Core/Src/app_freertos.d ./Core/Src/app_freertos.o ./Core/Src/app_freertos.su ./Core/Src/crc.cyclo ./Core/Src/crc.d ./Core/Src/crc.o ./Core/Src/crc.su ./Core/Src/gpio.cyclo ./Core/Src/gpio.d ./Core/Src/gpio.o ./Core/Src/gpio.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/memorymap.cyclo ./Core/Src/memorymap.d ./Core/Src/memorymap.o ./Core/Src/memorymap.su ./Core/Src/rtc.cyclo ./Core/Src/rtc.d ./Core/Src/rtc.o ./Core/Src/rtc.su ./Core/Src/stm32h5xx_hal_msp.cyclo ./Core/Src/stm32h5xx_hal_msp.d ./Core/Src/stm32h5xx_hal_msp.o ./Core/Src/stm32h5xx_hal_msp.su ./Core/Src/stm32h5xx_hal_timebase_tim.cyclo ./Core/Src/stm32h5xx_hal_timebase_tim.d ./Core/Src/stm32h5xx_hal_timebase_tim.o ./Core/Src/stm32h5xx_hal_timebase_tim.su ./Core/Src/stm32h5xx_it.cyclo ./Core/Src/stm32h5xx_it.d ./Core/Src/stm32h5xx_it.o ./Core/Src/stm32h5xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32h5xx.cyclo ./Core/Src/system_stm32h5xx.d ./Core/Src/system_stm32h5xx.o ./Core/Src/system_stm32h5xx.su ./Core/Src/tim.cyclo ./Core/Src/tim.d ./Core/Src/tim.o ./Core/Src/tim.su ./Core/Src/usart.cyclo ./Core/Src/usart.d ./Core/Src/usart.o ./Core/Src/usart.su

.PHONY: clean-Core-2f-Src

