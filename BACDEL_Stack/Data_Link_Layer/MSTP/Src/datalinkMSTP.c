/**********************************************************************************
*                                                                        
*                   Copyright (c) by SoftDEL Systems Ltd.               
*                                                                        
*   This software is copyrighted by and is the sole property of SoftDEL
*   Systems Ltd. All rights, title, ownership, or other interests in the 
*   software remain the property of  SoftDEL Systems Ltd. This software 
*   may only be used in accordance with the corresponding license 
*   agreement. Any unauthorized use, duplication, transmission,
*   distribution, or disclosure of this software is expressly forbidden.  
*                                                                        
*   This Copyright notice may not be removed or modified without prior    
*   written consent of SoftDEL Systems Ltd.                                
*                                                                        
*   SoftDEL Systems Ltd. reserves the right to modify this software        
*   without notice.                                                       
*                                                                        
*   SoftDEL Systems Ltd.						india@softdel.com         
*   3rd Floor, Pentagon P4,						http://www.softdel.com  
*   Magarpatta City, Hadapsar
*   Pune - 411 028                                
*                                                                         
*   FILE
*	datalinkMSTP.c
*                                                                      
*   AUTHORS                                                                     
*	Pratham Murkute
*                                                                         
*   DESCRIPTION                                                            
*	Functions for MSTP layer of stack.
*	Functions related to initialization and de-initialization of MSTP layer.
*	Functions related to device set/get MSTP device parameters.
*
**********************************************************************************/


/** header file includes */
#include "../Inc/datalinkMSTP.h"
//#include "Debug.h"
#include "../../../Application_Layer/Stack_Mgmt/Inc/bacnetStackMgmt.h"
#include "../Inc/datalinkMSTPStateMachines.h"

/** global variable for device MAC address */
static uint8_t g_u8MacID = BACNET_MSTP_BROADCAST_MAC_ADDRESS;

/** global variable for BAUD rate */
static uint32_t g_u32BaudRate = MSTP_DEFAULT_BAUD_RATE;

/** global variable for one BIT-Time value */
static Float_t g_fOneBitTime = 1/MSTP_DEFAULT_BAUD_RATE;
extern BACNET_RETURN_TYPE BACDEL_Enable_Network_Communication(void);
/** handle and exit flag for mstp rx thread */
bool g_bMstpRxThreadExit = FALSE;

/** mstp state machine related variables and parameters */
BACnetMstpVariables_t g_stMstpVariables;

/** process q index for master state machine */
int32_t g_i32ProcessQIndex = -1;

/** response sent check flag for reply postpone mechanism */
bool b_ResponseSentOnNetwork = FALSE;

/** mstp tx data queue. */
/** array of tx data queue to handle data as per priority. 
1st index contains b-side postponed responses
2nd index contains a-side service requests 
*/
MstpTxDataQue_t g_astMstpTxQueue[MAX_MSTP_TX_QUE];

/** gloabl flag used to start/stop receiving data */
extern bool g_bReception;

/** 
*
* DESCRIPTION
* Function to set MAC address of device.
*
* @param  u8MacId [in] Value for the MAC address
* @return void [out] No return value
*
*/
void MSTP_SetMacId(uint8_t u8MacId)
{
	/* set value in global variable */
    g_u8MacID = u8MacId;
}

/** 
*
* DESCRIPTION
* Function to get MAC address of device.
*
* @param  pstAddress [in] Structure to fill address
* @return u8MacId [out] Mac address of device
*
*/
uint8_t MSTP_GetMacId(BACnetAddress_t *pstAddress)
{
	/* locals */
	uint8_t u8MacId = 0;

	/* read value from global variable */
	u8MacId = g_u8MacID;

	/* set MSTP device address in structure */
	if(NULL != pstAddress)
	{
		/* set length and address */
		pstAddress->u8mac_len = MSTP_ADDRESS_LEN;
		pstAddress->u8IpAddrs[0] = u8MacId;
	}

	/* return value */
    return u8MacId;
}

/** 
*
* DESCRIPTION
* Function to get broadcast MAC address.
*
* @param  pstAddress [in] Structure to fill address
* @param  eDestType  [in] Type of destination
* @return u8MacId [out] Mac address of device
*
*/
uint8_t MSTP_GetBroadcastMacId(DESTINATION_TYPE eDestType, 
	BACnetAddress_t *pstAddress)
{
	/* locals */
	uint8_t u8MacId = 0;

	/* set MSTP broadcast address in structure */
	if(NULL != pstAddress)
	{
		/* set length and address */
		pstAddress->u8mac_len = MSTP_ADDRESS_LEN;
		pstAddress->u8IpAddrs[0] = BACNET_MSTP_BROADCAST_MAC_ADDRESS;

		/* check if global or local broadcast */
		if(DESTINATION_IS_GLOBAL_BROADCAST == eDestType)
		{
			/* set DNET and DLEN to 0 */
			pstAddress->u8dlen = 0;
			pstAddress->u16net = BACNET_GLOBAL_BROADCAST_NETWORK_NO;
		}
		else if(DESTINATION_IS_LOCAL_BROADCAST == eDestType)
		{
			/* set DNET and DLEN to 0 */
			pstAddress->u8dlen = 0;
			pstAddress->u16net = BACNET_LOCAL_BROADCAST_NETWORK_NO;
		}
		else
		{
			;//do nothing
		}
	}

	/* set MSTP broadcast address */
	u8MacId = BACNET_MSTP_BROADCAST_MAC_ADDRESS;

	/* return value */
    return u8MacId;
}

/** 
*
* DESCRIPTION
* Function to set BAUD rate of device for serial communication.
*
* @param  u32BaudRate [in] BAUD rate of serial communication
* @return void [out] No return value
*
*/
void MSTP_SetBaudRate(uint32_t u32BaudRate)
{
	/* set value in global variable */
    g_u32BaudRate = u32BaudRate;
}

/** 
*
* DESCRIPTION
* Function to get BAUD rate of device for serial communication.
*
* @param  void [in] No input parameter
* @return u32BaudRate [out] Baud rate current value
*
*/
uint32_t MSTP_GetBaudRate(void)
{
	/* locals */
	uint32_t u32BaudRate = 0;

	/* read value from global variable */
	u32BaudRate = g_u32BaudRate;

	/* return value */
    return u32BaudRate;
}

/** 
*
* DESCRIPTION
* Function to set one bit time value based on BAUD rate of 
* serial communication.
*
* @param  u32BaudRate [in] BAUD rate of serial communication
* @return void [out] No return value
*
*/
void MSTP_SetOneBitTime(uint32_t u32BaudRate)
{
	/* set value in global variable */
	g_fOneBitTime = (Float_t)1/u32BaudRate;
}

/** 
*
* DESCRIPTION
* Function to get one bit time value based on BAUD rate of 
* serial communication.
*
* @param  void [in] No input parameter
* @return u32BaudRate [out] Baud rate current value
*
*/
Float_t MSTP_GetOneBitTime(void)
{
	/* locals */
	Float_t fOneBitTime = 0.0;

	/* read value from global variable */
	fOneBitTime = g_fOneBitTime;

	/* return value */
    return fOneBitTime;
}

/**
*
* DESCRIPTION
* Function to validate MAC ID value for master device.
*
* @param  u8MacId [in]  ID for the MAC address
* @return bRetVal [out] Return TRUE if valid else FALSE
*
*/
bool MSTP_IsValidMasterMacId(uint8_t u8MacId)
{
	/* locals */
	bool bRetVal = FALSE;

	/* check mac address */
	if(u8MacId <= MSTP_MAX_MASTER_MAC_ADDRESS)
	{
		/* address is valid for master */
		bRetVal = TRUE;
	}

	/* return value */
    return bRetVal;
}

/**
*
* DESCRIPTION
* Function to validate MAC ID value for slave device.
*
* @param  u8MacId [in]  ID for the MAC address
* @return bRetVal [out] Return TRUE if valid else FALSE
*
*/
bool MSTP_IsValidSlaveMacId(uint8_t u8MacId)
{
	/* locals */
	bool bRetVal = FALSE;

	/* check mac address */
	if(u8MacId <= MSTP_MAX_SLAVE_MAC_ADDRESS)
	{
		/* address is valid for master */
		bRetVal = TRUE;
	}

	/* return value */
    return bRetVal;
}

/**
*
* DESCRIPTION
* Function to validate BAUD rate value for serial communication.
*
* @param  u32BaudRate [in] Baud rate for serial communication
* @return bRetVal [out] Return TRUE if valid else FALSE
*
*/
bool MSTP_IsValidBaudRate(uint32_t u32BaudRate)
{
	/* locals */
	bool bRetVal = FALSE;

	/* check mac address */
	if(MSTP_BAUD_RATE_9600 == u32BaudRate || MSTP_BAUD_RATE_19200 == u32BaudRate || 
		MSTP_BAUD_RATE_38400 == u32BaudRate	|| MSTP_BAUD_RATE_57600 == u32BaudRate || 
		/*MSTP_BAUD_RATE_76800 == u32BaudRate || */ MSTP_BAUD_RATE_115200 == u32BaudRate)
	{
		/* address is valid for master */
		bRetVal = TRUE;
	}

	/* return value */
    return bRetVal;
}

/**
*
* DESCRIPTION
* Function to cleanup or reset MSTP global data.
*
* @param  void [in]  No input parameters
* @return void [out] No return value
*
*/
void MSTP_CleanUp(void)
{
	/* set default values */
  g_u8MacID = BACNET_MSTP_BROADCAST_MAC_ADDRESS;
	g_u32BaudRate = MSTP_DEFAULT_BAUD_RATE;
	g_fOneBitTime = 1/MSTP_DEFAULT_BAUD_RATE;
}

/**
*
* DESCRIPTION                                                                          
* Timer for MSTP state machines.
*
* @param lpParam [in] Unused
* @param TimerOrWaitFired [in] Unused
*
* @returns [out] No return value
*
*/
void MSTP_StateMachine_Timer_Task(void)
{
	/* local variables */
	static bool bMstpSmReEntryFlag = FALSE;
	#ifdef DEBUG_LOGFILES
	static uint32_t uiCntBusy = 0;
	#endif

	/* making function non re-entrant */
    if(TRUE == bMstpSmReEntryFlag)
	{
		#ifdef DEBUG_LOGFILES
		uiCntBusy++;
		printf("\n MSTP_StateMachine_Timer Busy = %u", uiCntBusy);
		#endif
		/* return from timer */
		#ifdef __linux
		return NULL;
		#else
		return;
		#endif
	}

	#ifdef DEBUG_LOGFILES
	uiCntBusy = 0;
	#endif

	/* set the flag */
	bMstpSmReEntryFlag = TRUE;

    /* timer entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackMSTPLayer: \
	MSTP_StateMachine_Timer: Entry \r\n");
	#endif
	/* extract received valid data from Receive state machine */
	MSTP_ExtractValidData();
	/* master state machine */
	BACDEL_MSTP_Master_Node_Tsm();

	/* reset the flag */
	bMstpSmReEntryFlag = FALSE;

    /* timer exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackMSTPLayer: \
	MSTP_StateMachine_Timer: Exit \r\n");
	#endif

	/* return from timer */
	#ifdef __linux
	return NULL;
	#endif	
}

/**
*
* DESCRIPTION                                                                          
* Timer for MSTP timeout counters.
*
* @param lpParam [in] Unused
* @param TimerOrWaitFired [in] Unused
*
* @returns [out] No return value
*
*/
void MSTP_Counters_Timer_Task(void)
{
	/* local variables */
	static bool bMstpCntReEntryFlag = FALSE;

	/* making function non re-entrant */
    if(TRUE == bMstpCntReEntryFlag)
	{
		/* return from timer */
		#ifdef __linux
		return NULL;
		#else
		return;
		#endif
	}

	/* set the flag */
	bMstpCntReEntryFlag = TRUE;

    /* timer entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackMSTPLayer: \
	MSTP_Counters_Timer: Entry \r\n");
	#endif

	#ifdef DEBUG_LOGFILES
	/* increment count every millisecond */
	g_iTheTime++;
	#endif

	/* updated all mstp state machine counters */
	BACnet_StateMachine_Timers();

	/* reset the flag */
	bMstpCntReEntryFlag = FALSE;

    /* timer exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackMSTPLayer: \
	MSTP_Counters_Timer: Exit \r\n");
	#endif

	/* return from timer */
	#ifdef __linux
	return NULL;
	#endif	
}

/**
*
* DESCRIPTION                                                                          
* MSTP receive thread function.
* MSTP receive state machine will run in this thread.
*
* @param lpThreadParameter [in] No data
* @returns [out] Zero on exit
*
*/

/* MSTP_Recieve_Thread function */
void MSTP_Recieve_Thread_Task(void)
{
  /* USER CODE BEGIN MSTP_Recieve_Thread */
	/* local variable */
	//DWORD dwStatus = 0;
	/* Wait for Event to set */
//    Osal_Wait_Event(g_hStackInitEvent, INFINITE);
//    Osal_Set_Event(g_hStackInitEvent);

	/* thread entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackMSTPLayer: \
	MSTP_Receive_Thread: Entry \r\n");
	#endif
    OSAL_Wait_Sem(g_hsMstpRxSemaphoreHandle,portMAX_DELAY);
	/* thread routine */
	for(;;)
    {
		/* wait for infinite, until the semaphore is released */
			OSAL_Wait_Sem(g_hsMstpRxSemaphoreHandle,2);

		/* thread exit condition */
		if(TRUE == g_bMstpRxThreadExit)            
        {
			/* break thread routine */
            break;
        }

		/* receive state machine */
		BACDEL_MSTP_Receive_Frame_TSM();

	}// for loop ends

	/* thread exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackMSTPLayer: \
	MSTP_Receive_Thread: Exit \r\n");
	#endif

  /* USER CODE END MSTP_Recieve_Thread */
}




/**
*
* DESCRIPTION
* Function for initialization of MSTP layer data in stack.
* Mac-address and Baud-rate are validated and set in this function.
*
* @param u8MacId	 [in] Mac address of device
* @param u32BaudRate [in] Baud rate for serial communication
* @param pu8FileName [in] Serial communication port or file

* @return [out] Success or suitable error code
*
*/
BACNET_RETURN_TYPE BACDEL_MSTP_Stack_Init(uint8_t u8MacId, uint32_t u32BaudRate,
	uint8_t *pu8FileName)
{
	/* locals */
	BACNET_RETURN_TYPE eRetVal = BACDEL_SUCCESS;
	/* clean up previous data */
	MSTP_CleanUp();
	/* set mac address */
	MSTP_SetMacId(u8MacId);
	/* set baud rate */
	MSTP_SetBaudRate(u32BaudRate);
	/* set one bit time */
	MSTP_SetOneBitTime(u32BaudRate);

	/* initialize serial port and mstp state machine parameters */
	eRetVal = BACDEL_MSTP_InitializeStack(pu8FileName);
	BACDEL_Enable_Network_Communication(); //SK

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackMSTPLayer: \
	MSTP_Init: Exit \r\n");
	#endif
	return eRetVal;
}
/************************** end of datalinkMSTP.c file ***************************/
