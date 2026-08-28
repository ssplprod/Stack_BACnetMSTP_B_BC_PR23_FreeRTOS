/*
 * Tsk_Bacnet.c
 */

#include "stdlib.h"

#include "Tsk_Bacnet.h"
//#include <ApplicationGPIO.h>
#include "bacnetStackMgmt.h"
#include "bacDELApi.h"
#include "objDevice.h"
#include "bacnetInitiateServiceMgmt.h"
#include "bacDELDeviceConfig.h"
#include "bacDELDef.h"
#include "datalinkMSTPStateMachines.h"

#include "datalinkMSTP.h"
#include "stm32h563xx.h"
#include <string.h>
#include "DefineStruct.h"
#include "DataB.h"

#include "main.h"
//-------------------------------------------------------------------
//	Global variables
//

struct netconn *conn = NULL;
//struct netbuf *buf;
//struct ip_addr *addr;
extern struct netif gnetif;
extern uint32_t enumToBaud(uint8_t Baudrate);
extern BACnet_Socket_Pkt_t g_stSocketPkt;
//extern ip4_addr_t ipaddr;
extern RTC_HandleTypeDef hrtc;
extern UART_HandleTypeDef huart2;
extern uint32_t gi32TimeDurationSec;
extern uint8_t gu8CommStatus;
extern bool gbDisableInitiation;
bool gbCommControl = FALSE;	/** Communication Control flag*/
extern osSemaphoreId_t g_hsMstpRxSemaphoreHandle;
extern BACnetMstpVariables_t g_stMstpVariables;
extern osMutexId_t g_hsMstpTxMutexLockHandle;
extern DB_t SMCfg; //Added By SoftDEL
extern struct netif gnetif;
//-------------------------------------------------------------------
//	Local variables
//
extern volatile uint8_t u8BACnetRxByte;
devObject_t   gstDeviceObject;
bool gbDeviceStatus = TRUE;			               /** Provides the DCC service check*/
extern uint32_t gui32StartupDelay;

void MSTP_Recieve_Thread(void const * argument);
void Transmit_Thread(void const * argument);
void Initiate_Retry_Timer(void const * argument);
void Test_API(void);

typedef struct DT {
	/* Year value */
	uint16_t	m_u16Year;      /* AD */
	/* Month value */
	uint8_t		m_u8Month;      /* 1=Jan */
	/* Day value */
	uint8_t		m_u8Day;        /* 1..31 */
	/* Weekday value */
	uint8_t		m_u8Wday;       /* 1=Monday-7=Sunday */
	/* Hour value */
	uint8_t		m_u8Hour;
	/* Minutes value */
	uint8_t		m_u8Min;
	/* Second value */
	uint8_t		m_u8Sec;
	/* Hundredths value */
	uint8_t		m_u8Hundredths;

}DT_t;
DT_t dt={0,1,1,1,0,0,0,0};

Pr_BACnetConfigData_t BACnetConfigData;
Pr_BACnetConfigData_t *GetpBACnetfg()
{
	return &BACnetConfigData;
}

/**                                                                         
*
* DESCRIPTION
* Function to receive auto response for RP service request.
*
*/
BACNET_RETURN_TYPE AutoResponse_RP
(
	uint32_t u32DevId,
	uint32_t u32CallbackId,
	BACNET_SERVICES_SUPPORTED eServiceType,
	BACNET_ERROR_CLASS *peErrorClass,
	uint32_t *pu32ErrorCode,
	BACNET_PDU_TYPE *pePduType,
	void *pvServiceData,
	void *pvServiceResp,
	void *pvOtherData,
	BACnetAddress_t *pvRmtDvAddr,
	BACnetDateTime_t *pstTimeStamp
)
{
	/* local variables */

	BACNET_RETURN_TYPE eRetVal = BACDEL_SUCCESS;

	/* check input pointer */
	if(NULL == pvServiceData)
	{
		return BACDEL_ERROR;
	}

	/* return value */
	return eRetVal;
}


/**                                                                         
*
* DESCRIPTION
* Function to receive auto response for WP service request.
*
*/
BACNET_RETURN_TYPE AutoResponse_WP
(
	uint32_t u32DevId,
	uint32_t u32CallbackId,
	BACNET_SERVICES_SUPPORTED eServiceType,
	BACNET_ERROR_CLASS *peErrorClass,
	uint32_t *pu32ErrorCode,
	BACNET_PDU_TYPE *pePduType,
	void *pvServiceData,
	void *pvServiceResp,
	void *pvOtherData,
	BACnetAddress_t *pvRmtDvAddr,
	BACnetDateTime_t *pstTimeStamp
)
{
	/* local variables */

	BACNET_RETURN_TYPE eRetVal = BACDEL_SUCCESS;

	/* check input pointer */
	if(NULL == pvServiceData)
	{
		return BACDEL_ERROR;
	}

	/* return value */
	return eRetVal;
}
/**
*
* DESCRIPTION
* Function to receive auto response for RPM service request.
*
*/
BACNET_RETURN_TYPE AutoResponse_RPM
(
	uint32_t u32DevId,
	uint32_t u32CallbackId,
	BACNET_SERVICES_SUPPORTED eServiceType,
	BACNET_ERROR_CLASS *peErrorClass,
	uint32_t *pu32ErrorCode,
	BACNET_PDU_TYPE *pePduType,
	void *pvServiceData,
	void *pvServiceResp,
	void *pvOtherData,
	BACnetAddress_t *pvRmtDvAddr,
	BACnetDateTime_t *pstTimeStamp
)
{
	/* local variables */
	BACNET_RETURN_TYPE eRetVal = BACDEL_SUCCESS;

	/* check input pointer */
	if(NULL == pvServiceResp)
	{
		return BACDEL_ERROR;
	}

	/* Write application code here*/


	/* return value */
	return eRetVal;
}
#ifdef BACDEL_SER_DM_RD_B
/**
*
* DESCRIPTION
* Function to receive auto response for Reinitialize device service request.
*
*/
BACNET_RETURN_TYPE AutoResponse_RD
(
	uint32_t u32DevId,
	uint32_t u32CallbackId,
	BACNET_SERVICES_SUPPORTED eServiceType,
	BACNET_ERROR_CLASS *peErrorClass,
	uint32_t *pu32ErrorCode,
	BACNET_PDU_TYPE *pePduType,
	void *pvServiceData,
	void *pvServiceResp,
	void *pvOtherData,
	BACnetAddress_t *pvRmtDvAddr,
	BACnetDateTime_t *pstTimeStamp
)
{
	/* local variables */
	BACNET_RETURN_TYPE eRetVal = BACDEL_SUCCESS;


	/* check input pointer */
	if(NULL == pvServiceData)
	{
		return BACDEL_ERROR;
	}

	/* Write application code here*/

	/* return value */
	return eRetVal;
}

#endif
/**
*
* DESCRIPTION
* Function to receive auto response for DCC service request.
*
*/
BACNET_RETURN_TYPE AutoResponse_DCC
(
	uint32_t u32DevId,
	uint32_t u32CallbackId,
	BACNET_SERVICES_SUPPORTED eServiceType,
	BACNET_ERROR_CLASS *peErrorClass,
	uint32_t *pu32ErrorCode,
	BACNET_PDU_TYPE *pePduType,
	void *pvServiceData,
	void *pvServiceResp,
	void *pvOtherData,
	BACnetAddress_t *pvRmtDvAddr,
	BACnetDateTime_t *pstTimeStamp
)
{
	/* local variables */

	BACNET_RETURN_TYPE eRetVal = BACDEL_SUCCESS;

	/* check input pointer */
	if(NULL == pvServiceData)
	{
		return BACDEL_ERROR;
	}

	/* Write application code here*/


	/* return value */
	return eRetVal;
}

/**                                                                         
*
* DESCRIPTION
* Function to receive auto response for TimeSync. service request.
*
*/
void RTC_Read_Callback(BACnetDate_t *pstLocalDate, BACnetTime_t *pstLocalTime)
{

	pstLocalDate->m_u8Day				= dt.m_u8Day;			/* store date */
	pstLocalDate->m_u8Wday				= dt.m_u8Wday;			/* store week day */
	pstLocalDate->m_u8Month				= dt.m_u8Month;			/* store month */
	pstLocalDate->m_u16Year		 		= dt.m_u16Year;			/* store year */

	pstLocalTime->m_u8Hour		 		= dt.m_u8Hour;			/* store hours */
	pstLocalTime->m_u8Min		 		= dt.m_u8Min;			/* store minutes */
	pstLocalTime->m_u8Sec		 		= dt.m_u8Sec;			/* store seconds */
	pstLocalTime->m_u8Hundredths		= dt.m_u8Hundredths;	/* system does not return milliseconds so bacnet default is X'FF */
}


BACNET_RETURN_TYPE init_sld_bacnet_objects()
{
	BACNET_RETURN_TYPE ret = BACDEL_SUCCESS;

	ret &= BACDEL_Add_Object(SMCfg.g_BACnetCfg.ObjID,OBJECT_ANALOG_INPUT,0,(const char *)"My Analog i/p 0");

	ret &= BACDEL_Add_Object(SMCfg.g_BACnetCfg.ObjID,OBJECT_ANALOG_OUTPUT,0,(const char *)"My Analog o/p 0");

	ret &= BACDEL_Add_Object(SMCfg.g_BACnetCfg.ObjID,OBJECT_ANALOG_VALUE,0,(const char *)"My Analog val 0");

	ret &= BACDEL_Add_Object(SMCfg.g_BACnetCfg.ObjID,OBJECT_BINARY_INPUT,0,(const char *)"My Binary i/p 0");

	ret &= BACDEL_Add_Object(SMCfg.g_BACnetCfg.ObjID,OBJECT_BINARY_OUTPUT,0,(const char  *)"My Binary o/p 0");

	ret &= BACDEL_Add_Object(SMCfg.g_BACnetCfg.ObjID,OBJECT_BINARY_VALUE,0,(const char *)"My Binary val 0");

	ret &= BACDEL_Add_Object(SMCfg.g_BACnetCfg.ObjID,OBJECT_MULTI_STATE_INPUT,0,(const char *)"My MSI 0");

	ret &= BACDEL_Add_Object(SMCfg.g_BACnetCfg.ObjID,OBJECT_MULTI_STATE_OUTPUT,0,(const char *)"My MSO 0");

	ret &= BACDEL_Add_Object(SMCfg.g_BACnetCfg.ObjID,OBJECT_MULTI_STATE_VALUE,0,(const char *)"My MSV 0");

	ret &= BACDEL_Add_Object(SMCfg.g_BACnetCfg.ObjID,OBJECT_CALENDAR,0,(const char *)"My CAL 0");

	ret &= BACDEL_Add_Object(SMCfg.g_BACnetCfg.ObjID,OBJECT_NOTIFICATION_CLASS,0,(const char *)"My NC 0");

	ret &= BACDEL_Add_Object(SMCfg.g_BACnetCfg.ObjID,OBJECT_SCHEDULE,0,(const char *)"My SCH 0");


	/******************** Network Port Object for MSTP *******************/
	BACnetCharStr_t stObjName = {0};
	memcpy(&stObjName.m_pu8CharStr[0], "BACnet/MSTP Network Port", 25);
	stObjName.m_u32StrLen = 25;
	NetworkPort_Add_Object(SMCfg.g_BACnetCfg.ObjID,0, &stObjName, NETWORK_TYPE_MSTP);

	return ret;
}
void app_bacnet_init()
{
	BACNET_RETURN_TYPE ret = BACDEL_SUCCESS;

	uint8_t  uwBacMacAddr = SMCfg.g_BACnetCfg.ComID;
	uint32_t uwBacBaudRate = ebr_76800;//enumToBaud(SMCfg.g_Com485Cfg.Baudrate);
	BACDEL_MSTP_Stack_Init(uwBacMacAddr,uwBacBaudRate,0 );

	Init_Service_Handlers();

	BACDEL_App_RTC_CallBack_Register(APP_CB_RTC_READ,RTC_Read_Callback);

	ret &= BACDEL_Add_Device(0,SMCfg.g_BACnetCfg.ObjID,0);

	ret &= init_sld_bacnet_objects();

	BACDEL_App_CallBack_Register(APP_CB_READ_PROPERTY,AutoResponse_RP);
	BACDEL_App_CallBack_Register(APP_CB_WRITE_PROPERTY,AutoResponse_WP);
	BACDEL_App_CallBack_Register(APP_CB_TIME_SYNC,AutoResponse_TimeSync);
	BACDEL_App_CallBack_Register(APP_CB_UTC_TIME_SYNC,AutoResponse_UTC_TimeSync);

	BACDEL_App_CallBack_Register(APP_CB_READ_PROPERTY_MULTIPLE,AutoResponse_RPM);
	BACDEL_App_CallBack_Register(APP_CB_REINITIALIZE_DEVICE,AutoResponse_RD);
	BACDEL_App_CallBack_Register(APP_CB_DEVICE_COMMUNICATION_CONTROL,AutoResponse_DCC);
}


/* USER CODE BEGIN Application */

/* DvManagementThread function */
void DvManagementThread(void const * argument)
{
  /* USER CODE BEGIN DvManagementThread */
  /* Infinite loop */
  for(;;)
  {
     DvManagementThread_Task();
  }
  /* USER CODE END DvManagementThread */
}

/* Initiate_Thread function */
void Bacnet_Initiate_Thread(void const * argument)
{
  /* USER CODE BEGIN Initiate_Thread */

  /* Infinite loop */
  for(;;)
  {
    Initiate_Thread_Task();
  }
  /* USER CODE END Initiate_Thread */
}

/* ClearInitiateQTimer_Timer function */
void ClearInitiateQTimer_Timer(void const * argument)
{
  /* USER CODE BEGIN  */
	ClearInitiateQTimer();
  /* USER CODE END */
}

/* MSTP_Counters_Timer function */
void MSTP_Counters_Timer(void const * argument)
{
	if(BACnetConfigData.m_eBACStack == BACNET_STACK_MSTP)
	{
		/* USER CODE BEGIN MSTP_Counters_Timer */
		MSTP_Counters_Timer_Task();
		/* USER CODE END MSTP_Counters_Timer */
	}
}

/* MSTP_StateMachine_Timer function */
void MSTP_StateMachine_Timer(void const * argument)
{
	if(BACnetConfigData.m_eBACStack == BACNET_STACK_MSTP)
	{
		/* USER CODE BEGIN MSTP_StateMachine_Timer */
		MSTP_StateMachine_Timer_Task();
		/* USER CODE END MSTP_StateMachine_Timer */
	}
}

// Add MSTP ifdef
void EnableIntUartTim()
{
	/* Set Interrupt Group Priority */
	  // Enable USART Interrupt
	  HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);

	  HAL_NVIC_EnableIRQ(USART2_IRQn);

	  // Enable Timer Interrupt
	  /*Configure the TIM2 IRQ priority */
	  HAL_NVIC_SetPriority(TIM2_IRQn, 5 ,0);
	  /* Enable the TIM2 global Interrupt */
	  HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

//UartRxEnable
void UartRxEnable()
{
	BSP_Set_485_EN_On();
	BSP_Set_485_RTS_Off();
}

//UartTxEnable
void UartTxEnable()
{
	BSP_Set_485_EN_Off();
	BSP_Set_485_RTS_On();
}

/**
 * @fn 	    uint8_t SendByteBacnet(uint8_t byte)
 * @brief   Send 1 byte to BACnet bus
 * @param   None
 * @retval  1 if OK
 */
uint8_t SendPacketBacnet(UART_HandleTypeDef *huart, uint8_t* data, uint16_t size)
{
	if(huart->gState == HAL_UART_STATE_READY)
	{
		/* set the flag to notify master tsm to stop reply postpone */
		b_ResponseSentOnNetwork = TRUE;
		/** Disable UART Receive Interrupt */
		__HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);
		/** Enable Transmission Of RS485*/
		UartTxEnable();

		HAL_UART_Transmit_IT(&huart2,data,size);

		/**Enable UART Transmission Interrupt*/
		__HAL_UART_ENABLE_IT(&huart2, UART_IT_TXE);

		/**Enable UART Transmission Complete Interrupt*/
		__HAL_UART_ENABLE_IT(&huart2, UART_IT_TC);
		return 1; //nw
	}
	else //nw
	{
		return 0;
	}
}

void BACRS485_Interrupt_Rx(void)
{
	BnRxQue.Data[BnRxQue.i16Head] = u8BACnetRxByte;

	/** Update header and soft timers */
	BnRxQue.i16Head++;
	/** Check buffer overflow */
	if(BnRxQue.i16Head > (BN_QUESIZE-1))
	{
		/* set to zero */
		BnRxQue.i16Head = 0;
	}
	g_stMstpVariables.Tsilence = 0;
	g_stMstpVariables.SubTsilence = 0;

	/* trigger the mstp receive thread */
	if(!OSAL_Release_Sem(g_hsMstpRxSemaphoreHandle, BACNET_ONE))
	{
	   return;
	}
}

void UartConfig(void)
{
	  __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
	  BSP_Set_485_EN_On();//On
	  BSP_Set_485_RTS_Off();
}

/* MSTP_Recieve_Thread function */
void MSTP_Recieve_Thread(void const * argument)
{
  /* USER CODE BEGIN MSTP_Recieve_Thread */
  /* Infinite loop */
	for(;;)
	{
	   MSTP_Recieve_Thread_Task();
	}
  /* USER CODE END MSTP_Recieve_Thread */
}

void Test_API(void)
{
	return;
}
