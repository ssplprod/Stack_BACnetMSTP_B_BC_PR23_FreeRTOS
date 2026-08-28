/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32h5xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32h5xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */
#include "datalinkMSTP.h"

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart6;
extern TIM_HandleTypeDef htim17;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/******************************************************************************/
/* STM32H5xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32h5xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */

  /* USER CODE END TIM2_IRQn 0 */
//  HAL_TIM_IRQHandler(&htim2);
  /* USER CODE BEGIN TIM2_IRQn 1 */
	/**Local Variables*/
  static uint8_t count2ms = 0;

	if(__HAL_TIM_GET_FLAG(&htim2, TIM_FLAG_UPDATE) != RESET && __HAL_TIM_GET_IT_SOURCE(&htim2, TIM_IT_UPDATE) !=RESET)
	{
		__HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);
	}

	MSTP_Counters_Timer_Task();

	/**Increment for 2ms count */
	++count2ms;

	/** Execute Function for Every Two Millisecond */
	if(count2ms == 2)
	{
		/**Clear the 2ms count*/
		count2ms = 0;

		/**Execute MSTP state machine task*/
		MSTP_StateMachine_Timer_Task();
	}

	HAL_TIM_IRQHandler(&htim2);
  /* USER CODE END TIM2_IRQn 1 */
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */

  /* USER CODE END USART2_IRQn 0 */
  /* USER CODE BEGIN USART2_IRQn 1 */
	/** Local Variable*/
		uint32_t tmp_flag;
		uint32_t tmp_it_source;

		/** Receive BACnet Data in IRQ handler*/
		tmp_flag = __HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE);
		tmp_it_source = __HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_RXNE);

		if((tmp_flag != RESET) && (tmp_it_source != RESET))
		{
			HAL_UART_IRQHandler(&huart2);

			/**Receive the BACnet Data in interrupt */
			HAL_UART_Receive_IT(&huart2,(uint8_t *)&u8BACnetRxByte,1);

			/** Read character into receive buffer */
			BACRS485_Interrupt_Rx();
		}

		/** Transmit BACnet Data in IRQ handler*/
		tmp_flag = __HAL_UART_GET_FLAG(&huart2, UART_FLAG_TXE);
		tmp_it_source = __HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_TXE);

		if((tmp_flag != RESET) && (tmp_it_source != RESET))
		{
			HAL_UART_IRQHandler(&huart2);
		}

		/*Transmit Tx Complete IRQ handler*/
		tmp_flag = __HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC);
		tmp_it_source = __HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_TC);

		if((tmp_flag != RESET) && (tmp_it_source != RESET))
		{
			HAL_UART_IRQHandler(&huart2);

			/**Disable UART Transmission Interrupt*/
			__HAL_UART_DISABLE_IT(&huart2, UART_IT_TXE);

			/**Enable RS485 Receive*/
			UartRxEnable();

			/**Enable UART Receive Interrupt*/
			__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);

			/**Disable UART Transmission Complete Interrupt*/
			__HAL_UART_DISABLE_IT(&huart2, UART_IT_TC);
				g_stMstpVariables.Tsilence=0;
		}
  /* USER CODE END USART2_IRQn 1 */
}

/**
  * @brief This function handles TIM17 global interrupt.
  */
void TIM17_IRQHandler(void)
{
  /* USER CODE BEGIN TIM17_IRQn 0 */

  /* USER CODE END TIM17_IRQn 0 */
  HAL_TIM_IRQHandler(&htim17);
  /* USER CODE BEGIN TIM17_IRQn 1 */

  /* USER CODE END TIM17_IRQn 1 */
}

/**
  * @brief This function handles USART6 global interrupt.
  */
void USART6_IRQHandler(void)
{
  /* USER CODE BEGIN USART6_IRQn 0 */

  /* USER CODE END USART6_IRQn 0 */
  HAL_UART_IRQHandler(&huart6);
  /* USER CODE BEGIN USART6_IRQn 1 */

  /* USER CODE END USART6_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
