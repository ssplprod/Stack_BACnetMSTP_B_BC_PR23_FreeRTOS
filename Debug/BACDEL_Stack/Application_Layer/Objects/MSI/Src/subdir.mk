################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BACDEL_Stack/Application_Layer/Objects/MSI/Src/objMultiStateInput.c 

C_DEPS += \
./BACDEL_Stack/Application_Layer/Objects/MSI/Src/objMultiStateInput.d 

OBJS += \
./BACDEL_Stack/Application_Layer/Objects/MSI/Src/objMultiStateInput.o 


# Each subdirectory must supply rules for building sources it contributes
BACDEL_Stack/Application_Layer/Objects/MSI/Src/%.o BACDEL_Stack/Application_Layer/Objects/MSI/Src/%.su BACDEL_Stack/Application_Layer/Objects/MSI/Src/%.cyclo: ../BACDEL_Stack/Application_Layer/Objects/MSI/Src/%.c BACDEL_Stack/Application_Layer/Objects/MSI/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32H563xx '-D__weak=__attribute__((weak))' -DFAULT_ALGORITHM -DBACDEL_PR12 -DBACDEL_SER_AE_AA_B -DBACDEL_SER_AE_GEI_B -DBACDEL_SER_DM_RD_B -DINTRINSIC_REPORTING -DBACDEL_SER_DM_DDB_A -DBACDEL_SER_AE_EN_B -DBACDEL_SER_DM_UTC_B -DSEGMENTATION_SUPPORTED -DBACDEL_PR18 -DBACDEL_OBJ_NP -DBACDEL_OBJ_BSV -DBACDEL_OBJ_EE -DBACDEL_OBJ_NC -DBACDEL_OBJ_CAL -DBACDEL_OBJ_SDL -DOPTIONAL_PROPERTY -DBACDEL_SER_DS_COV_B -DBACDEL_FREERTOS '-D__packed=__attribute__((__packed__))' -DBACDEL_IP_STACK -DBACDEL_SER_DS_RP_B -DBACDEL_SER_DS_RPM_B -DBACDEL_SER_DS_WPM_B -DBACDEL_SER_DS_WP_B -DBACDEL_PR14 -DBACDEL_OBJ_DEV -DBACDEL_SER_DM_DDB_B -DBACDEL_SER_DM_DOB_B -DNETWORK_LAYER_MESSAGE_B -DBACDEL_SER_DM_DCC_B -DBACDEL_OBJ_BI -DBACDEL_OBJ_BO -DBACDEL_OBJ_BV -DBACDEL_OBJ_AI -DBACDEL_OBJ_AO -DBACDEL_OBJ_AV -DBACDEL_OBJ_MSI -DBACDEL_OBJ_MSO -DBACDEL_OBJ_MSV -DBACDEL_SER_DM_TS_B -DINITIATE_SERVICE_ENABLED -DOS_FREERTOS -DBBMD_ENABLED -DDEBUG -DNEW_RP_WP_INTERFACE -DUSE_NUCLEO_64 -c -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@" @"BACDEL_Stack/Application_Layer/Objects/MSI/Src/objMultiStateInput.c_includes.args"

clean: clean-BACDEL_Stack-2f-Application_Layer-2f-Objects-2f-MSI-2f-Src

clean-BACDEL_Stack-2f-Application_Layer-2f-Objects-2f-MSI-2f-Src:
	-$(RM) ./BACDEL_Stack/Application_Layer/Objects/MSI/Src/objMultiStateInput.cyclo ./BACDEL_Stack/Application_Layer/Objects/MSI/Src/objMultiStateInput.d ./BACDEL_Stack/Application_Layer/Objects/MSI/Src/objMultiStateInput.o ./BACDEL_Stack/Application_Layer/Objects/MSI/Src/objMultiStateInput.su

.PHONY: clean-BACDEL_Stack-2f-Application_Layer-2f-Objects-2f-MSI-2f-Src

