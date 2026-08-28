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
*	datalinkMSTP.h
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

#ifndef DATALINK_MSTP_H
#define DATALINK_MSTP_H


/** header file includes */
#include "bacDELStackConfig.h"
#include "osalFreeRTOS.h"
#include <string.h>
/**************************************************************************
 **** Macros
 **************************************************************************/

/** macro definations */
#define MSTP_MIN_MASTER_MAC_ADDRESS			0
#define MSTP_MAX_MASTER_MAC_ADDRESS			127
#define MSTP_MIN_SLAVE_MAC_ADDRESS			0
#define MSTP_MAX_SLAVE_MAC_ADDRESS			254

/** standard baud rate values */
#define MSTP_BAUD_RATE_9600					9600
#define MSTP_BAUD_RATE_19200				19200
#define MSTP_BAUD_RATE_38400				38400
#define MSTP_BAUD_RATE_57600				57600
#define MSTP_BAUD_RATE_76800				76800
#define MSTP_BAUD_RATE_115200				115200

/** macro defination for mstp layer general variables */
#define MSTP_DEFAULT_BAUD_RATE			MSTP_BAUD_RATE_38400
#define MSTP_ADDRESS_LEN					  MAX_MSTP_LEN
#define MSTP_PREAMBLE_FIRST					0x55
#define MSTP_PREAMBLE_SECOND				0xFF
#define MSTP_TURNAROUND						  40

/** macro definations for mstp layer packet data */
/* mstp data crc size */
#define MSTP_DATA_CRC_SIZE					2

/** encoding locations for mstp layer data */
/* preamble */
#define MSTP_PREAMBLE_LOCATION				0
/* frame type */
#define MSTP_FRAME_TYPE_LOCATION			2
/* destination address */
#define MSTP_DA_LOCATION					3
/* source address */
#define MSTP_SA_LOCATION					4
/* data length */
#define MSTP_LENGTH_LOCATION				5
/* header crc */
#define MSTP_HEADER_CRC_LOCATION			7
/* 1st byte of data */
#define MSTP_DATA_LOCATION					8

/** compile as C code */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**************************************************************************
 **** Globals and Externs
 **************************************************************************/

/** mutex for tx data on mstp serial bus */
extern Mutex_H g_hMstpTxMutexLock;

/** semaphore for rx thread to receive data from mstp serial bus */
extern osSemaphoreId_t g_hsMstpRxSemaphoreHandle;;

/** process q index for master state machine */
extern int32_t g_i32ProcessQIndex;

/** response sent check flag for reply postpone mechanism */
extern bool b_ResponseSentOnNetwork;

/**
*
* Name : BACnetMstpVariables_t
*
* DESCRIPTION
* Variables and Parameters related to MSTP state machines.
*
*/
typedef struct BACnetMstpVariables
{
   uint8_t      TokenRequiredSegmentation;  //Token required for sending fragments
   uint8_t		Binaries;				// Total Binaries
   uint8_t		NextStation;			// Next station
   uint8_t		PollStation;			// Polling station
   uint8_t		MaxMaster;				// Maximum master
   uint8_t		TokenCount;				// Token count
   uint8_t		ThisStation;			// Node address
   uint8_t		ReceivedDestination;	// Received destination address
   uint8_t		ReceivedSource;			// Received source
   uint8_t		ReceivedFrameType;		// Received frame type
   uint8_t		SourceAddress;			// Source address
   uint8_t		RetryCount;				// Number of retry counts
   uint8_t		EventCount;				// Number of Event counts
   uint8_t		RS485_DisableTime;		// RS485 Disable time
   uint16_t		FrameCount;				// Number of frame counts
   uint16_t		TnoTokens;				// Transmit no. of tokens
   uint16_t		TreplyTimeout;			// Transmit reply time out
   uint16_t		Tframeabort;			// Transmit frame abort
   uint16_t		SubTsilence;			// SoftTimer count
   uint16_t		MaxInfoFrames;			// Information frames
   uint16_t		Tsilence;				// SoftTimer count

}BACnetMstpVariables_t;

/** mstp state machine related variables and parameters */
extern BACnetMstpVariables_t g_stMstpVariables;

/** enumerations for mstp tx queue */
typedef enum 
{
	MSTP_TX_QUE_RESPONSE = 0,
	MSTP_TX_QUE_REQUEST = 1,
	/* add more enums above this */
	MAX_MSTP_TX_QUE
}MSTP_TX_QUE;

/**
*
* Name : MstpTxDataQueNode_t
* 
* DESCRIPTION 
* Structure for mstp tx queue node.
* This queue node holds the requests/response encoded data that
* needs token for data transmission.
*
*/
typedef struct MstpTxDataQueNode
{
	/* state of the node */
	/* free if value is false */
	bool				m_bState;
	/* buffer to save encoded data */
	uint8_t				m_au8buffer[MAX_MSTP_MPDU];
	/* encoded data length */
	uint16_t			m_u16DataLength;
	/*  mstp frame type */
	MSTP_FRAME_TYPE		m_eFrameType;
	/* initiate queue node refernce */
	/* required only for requests */
	void				*m_pvInitQueueRef;
}MstpTxDataQueNode_t;

/**
*
* Name : MstpTxDataQue_t
* 
* DESCRIPTION 
* Structure for mstp tx queue.
* This queue is used to save data for requests or response that
* need token for data transmission.
*
*/
typedef struct MstpTxDataQue
{
	/* number of packets added to queue */
	uint32_t		 m_u32PacketCount;
	/* queue index to fill data */
	uint32_t		 m_u32FillIndex;
	/* queue index to process data */
	uint32_t		 m_u32ProcIndex;
	/* mstp tx data queue */
	MstpTxDataQueNode_t m_stDataNode[MAX_MSTP_TX_QUE_LENGTH];
}MstpTxDataQue_t;

/** mstp tx data queue. */
/** array of tx data queue to handle data as per priority. 
1st index contains b-side postponed responses
2nd index contains a-side service requests 
*/
extern MstpTxDataQue_t g_astMstpTxQueue[MAX_MSTP_TX_QUE];

/**************************************************************************
 **** Function Declarations
 **************************************************************************/

/** 
*
* DESCRIPTION
* Function to get MAC address of device.
*
* @param  pstAddress [in] Structure to fill address
* @return u8MacId [out] Mac address of device
*
*/
uint8_t MSTP_GetMacId(BACnetAddress_t *pstAddress);

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
	BACnetAddress_t *pstAddress);

/** 
*
* DESCRIPTION
* Function to get BAUD rate of device for serial communication.
*
* @param  void [in] No input parameter
* @return u32BaudRate [out] Baud rate current value
*
*/
uint32_t MSTP_GetBaudRate(void);

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
Float_t MSTP_GetOneBitTime(void);

/**
*
* DESCRIPTION
* Function to cleanup or reset MSTP global data.
*
* @param  void [in]  No input parameters
* @return void [out] No return value
*
*/
void MSTP_CleanUp(void);

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

void MSTP_StateMachine_Timer_Task(void);
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
void MSTP_Counters_Timer_Task(void);

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
void MSTP_Recieve_Thread_Task(void);

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
	uint8_t *pu8FileName);

/**
*
* DESCRIPTION
* Function for initialization of MSTP layer in stack.
* All threads, timer, mutex, semaphore for MSTP layer are created.
*
* @param u8MacId	 [in] Mac address of device
* @param u32BaudRate [in] Baud rate for serial communication

* @return [out] Success or suitable error code
*
*/
BACNET_RETURN_TYPE MSTP_InitLayer(uint8_t u8MacId, uint32_t u32BaudRate);

/**
*
* DESCRIPTION
* Function for de-initialization of MSTP layer in stack.
*
* @param void [in] No input parameter
* @return [out] Success or suitable error code
*
*/
BACNET_RETURN_TYPE MSTP_DeInitLayer(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DATALINK_MSTP_H */
