################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.c 

C_DEPS += \
./Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.d 

OBJS += \
./Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.o 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/%.o Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/%.su Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/%.cyclo: ../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/%.c Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32H563xx '-D__weak=__attribute__((weak))' -DFAULT_ALGORITHM -DBACDEL_PR12 -DBACDEL_SER_AE_AA_B -DBACDEL_SER_AE_GEI_B -DBACDEL_SER_DM_RD_B -DINTRINSIC_REPORTING -DBACDEL_SER_DM_DDB_A -DBACDEL_SER_AE_EN_B -DBACDEL_SER_DM_UTC_B -DSEGMENTATION_SUPPORTED -DBACDEL_PR18 -DBACDEL_OBJ_NP -DBACDEL_OBJ_BSV -DBACDEL_OBJ_EE -DBACDEL_OBJ_NC -DBACDEL_OBJ_CAL -DBACDEL_OBJ_SDL -DOPTIONAL_PROPERTY -DBACDEL_SER_DS_COV_B -DBACDEL_FREERTOS '-D__packed=__attribute__((__packed__))' -DBACDEL_IP_STACK -DBACDEL_SER_DS_RP_B -DBACDEL_SER_DS_RPM_B -DBACDEL_SER_DS_WPM_B -DBACDEL_SER_DS_WP_B -DBACDEL_PR14 -DBACDEL_OBJ_DEV -DBACDEL_SER_DM_DDB_B -DBACDEL_SER_DM_DOB_B -DNETWORK_LAYER_MESSAGE_B -DBACDEL_SER_DM_DCC_B -DBACDEL_OBJ_BI -DBACDEL_OBJ_BO -DBACDEL_OBJ_BV -DBACDEL_OBJ_AI -DBACDEL_OBJ_AO -DBACDEL_OBJ_AV -DBACDEL_OBJ_MSI -DBACDEL_OBJ_MSO -DBACDEL_OBJ_MSV -DBACDEL_SER_DM_TS_B -DINITIATE_SERVICE_ENABLED -DOS_FREERTOS -DBBMD_ENABLED -DDEBUG -DNEW_RP_WP_INTERFACE -DUSE_NUCLEO_64 -c -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@" @"Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.c_includes.args"

clean: clean-Middlewares-2f-Third_Party-2f-FreeRTOS-2f-Source-2f-CMSIS_RTOS_V2

clean-Middlewares-2f-Third_Party-2f-FreeRTOS-2f-Source-2f-CMSIS_RTOS_V2:
	-$(RM) ./Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.cyclo ./Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.d ./Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.o ./Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.su

.PHONY: clean-Middlewares-2f-Third_Party-2f-FreeRTOS-2f-Source-2f-CMSIS_RTOS_V2

