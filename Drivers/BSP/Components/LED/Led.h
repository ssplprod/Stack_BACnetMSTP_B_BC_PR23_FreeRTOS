//------------------------------------------------------------------------------
//        LED driver.
//------------------------------------------------------------------------------
#ifndef LED_H
#define LED_H

#include "stm32h5xx_nucleo.h"

#define BSP_Set_COM_LED_R_on()		BSP_LED_On(LED1);
#define BSP_Set_COM_LED_R_off()		BSP_LED_Off(LED1);
#define BSP_Set_COM_LED_R_toggle()	BSP_LED_Toggle(LED1);
#define BSP_Set_COM_LED_G_on()		BSP_LED_On(LED1);
#define BSP_Set_COM_LED_G_off()		BSP_LED_Off(LED1);
#define BSP_Set_COM_LED_G_toggle()	BSP_LED_Toggle(LED1);
#define BSP_Set_COM_LED_B_on()		BSP_LED_On(LED1);
#define BSP_Set_COM_LED_B_off()		BSP_LED_Off(LED1)
#define BSP_Set_COM_LED_B_toggle()	BSP_LED_Toggle(LED1);

#if 0
#define BSP_Set_COM_LED_R_on()		HAL_GPIO_WritePin(COM_LED_R_GPIO_Port, COM_LED_R_Pin, GPIO_PIN_RESET);
#define BSP_Set_COM_LED_R_off()		HAL_GPIO_WritePin(COM_LED_R_GPIO_Port, COM_LED_R_Pin, GPIO_PIN_SET);
#define BSP_Set_COM_LED_R_toggle()	HAL_GPIO_TogglePin(COM_LED_R_GPIO_Port, COM_LED_R_Pin);
#define BSP_Set_COM_LED_G_on()		HAL_GPIO_WritePin(COM_LED_G_GPIO_Port, COM_LED_G_Pin, GPIO_PIN_RESET);
#define BSP_Set_COM_LED_G_off()		HAL_GPIO_WritePin(COM_LED_G_GPIO_Port, COM_LED_G_Pin, GPIO_PIN_SET);
#define BSP_Set_COM_LED_G_toggle()	HAL_GPIO_TogglePin(COM_LED_G_GPIO_Port, COM_LED_G_Pin);
#define BSP_Set_COM_LED_B_on()		HAL_GPIO_WritePin(COM_LED_B_GPIO_Port, COM_LED_B_Pin, GPIO_PIN_RESET);
#define BSP_Set_COM_LED_B_off()		HAL_GPIO_WritePin(COM_LED_B_GPIO_Port, COM_LED_B_Pin, GPIO_PIN_SET);
#define BSP_Set_COM_LED_B_toggle()	HAL_GPIO_TogglePin(COM_LED_B_GPIO_Port, COM_LED_B_Pin);

#define BSP_Set_STATUS_LED_R_on()	HAL_GPIO_WritePin(STAT_LED_R_GPIO_Port, STAT_LED_R_Pin, GPIO_PIN_RESET);
#define BSP_Set_STATUS_LED_R_off()	HAL_GPIO_WritePin(STAT_LED_R_GPIO_Port, STAT_LED_R_Pin, GPIO_PIN_SET);
#define BSP_Set_STATUS_LED_G_on()	HAL_GPIO_WritePin(STAT_LED_G_GPIO_Port, STAT_LED_G_Pin, GPIO_PIN_RESET);
#define BSP_Set_STATUS_LED_G_off()	HAL_GPIO_WritePin(STAT_LED_G_GPIO_Port, STAT_LED_G_Pin, GPIO_PIN_SET);
#define BSP_Set_STATUS_LED_B_on()	HAL_GPIO_WritePin(STAT_LED_B_GPIO_Port, STAT_LED_B_Pin, GPIO_PIN_RESET);
#define BSP_Set_STATUS_LED_B_off()	HAL_GPIO_WritePin(STAT_LED_B_GPIO_Port, STAT_LED_B_Pin, GPIO_PIN_SET);

#define BSP_Set_ETH_LED_on()		HAL_GPIO_WritePin(ETH_LEDACT_GPIO_Port, ETH_LEDACT_Pin, GPIO_PIN_RESET)
#define BSP_Set_ETH_LED_off()		HAL_GPIO_WritePin(ETH_LEDACT_GPIO_Port, ETH_LEDACT_Pin, GPIO_PIN_SET)

#define BSP_Set_L1_LED_Y_on()		HAL_GPIO_WritePin(L1_LED_Y_GPIO_Port, L1_LED_Y_Pin, GPIO_PIN_RESET)
#define BSP_Set_L1_LED_Y_off()		HAL_GPIO_WritePin(L1_LED_Y_GPIO_Port, L1_LED_Y_Pin, GPIO_PIN_SET)
#define BSP_Set_L1_LED_G_on()		HAL_GPIO_WritePin(L1_LED_G_GPIO_Port, L1_LED_G_Pin, GPIO_PIN_RESET)
#define BSP_Set_L1_LED_G_off()		HAL_GPIO_WritePin(L1_LED_G_GPIO_Port, L1_LED_G_Pin, GPIO_PIN_SET)

#define BSP_Set_L2_LED_G_on()		HAL_GPIO_WritePin(L2_LED_G_GPIO_Port, L2_LED_G_Pin, GPIO_PIN_RESET)
#define BSP_Set_L2_LED_G_off()		HAL_GPIO_WritePin(L2_LED_G_GPIO_Port, L2_LED_G_Pin, GPIO_PIN_SET)

#define BSP_Set_L3_LED_G_on()		HAL_GPIO_WritePin(L3_LED_G_GPIO_Port, L3_LED_G_Pin, GPIO_PIN_RESET)
#define BSP_Set_L3_LED_G_off()		HAL_GPIO_WritePin(L3_LED_G_GPIO_Port, L3_LED_G_Pin, GPIO_PIN_SET)

#define BSP_Set_L4_LED_G_on()		HAL_GPIO_WritePin(L4_LED_G_GPIO_Port, L4_LED_G_Pin, GPIO_PIN_RESET)
#define BSP_Set_L4_LED_G_off()		HAL_GPIO_WritePin(L4_LED_G_GPIO_Port, L4_LED_G_Pin, GPIO_PIN_SET)

#define BSP_Set_L5_LED_Y_on()		HAL_GPIO_WritePin(L5_LED_Y_GPIO_Port, L5_LED_Y_Pin, GPIO_PIN_RESET)
#define BSP_Set_L5_LED_Y_off()		HAL_GPIO_WritePin(L5_LED_Y_GPIO_Port, L5_LED_Y_Pin, GPIO_PIN_SET)
#define BSP_Set_L5_LED_G_on()		HAL_GPIO_WritePin(L5_LED_G_GPIO_Port, L5_LED_G_Pin, GPIO_PIN_RESET)
#define BSP_Set_L5_LED_G_off()		HAL_GPIO_WritePin(L5_LED_G_GPIO_Port, L5_LED_G_Pin, GPIO_PIN_SET)
#endif

extern void BSP_Set_COM_LED(uint16_t Red,uint16_t Green, uint16_t Blue );
extern void BSP_Set_STATUS_LED(uint16_t Red,uint16_t Green, uint16_t Blue );
extern void BSP_Set_ALL_LED_Off();
extern void BSP_Set_ALL_LED_On();
void BSP_Display_Percent(uint32_t ui32Percent);

#endif 
