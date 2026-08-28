/********************************************************************************
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
*   SoftDEL Systems Ltd.                     	india@softdel.com         
*   3rd Floor, Pentagon P4,						http://www.softdel.com  
*	Magarpatta City, Hadapsar
*	Pune - 411 028   							 
*                                                                         
*   FILE
*	bacnetStackMgmt.c
*                                                                      
*   AUTHORS                                                                             
*	Abhinay Masurekar, Sagar Limaye, Harshal Mangale, 
*	Prashant Badgujar, M.Venu, Ashish Verma, Pratham Murkute
*                                                                         
*   DESCRIPTION                                                            
*	Defines basics threads, stack init and de-init functions required 
*	for functioning of stack. 
*	- Process Thread
*	- Transmit Thread
*	- Receive Thread
*	- Deletion Thread
*
********************************************************************************/

/** header files */
#include "bacnetStackMgmt.h"
#include "osalFreeRTOS.h"
#include "bacDELStackConfig.h"
#include "pduServiceStructure.h"  
#include "bacnetAPDUHandler.h"
#include "objDevice.h"
#include "miscMiscellaneous.h"
//#include "Debug.h"
#include "propertyClearValues.h"


/*include MSTP layer functions */
#include "datalinkMSTPHandler.h"

/* To include A-side service support */
#ifdef INITIATE_SERVICE_ENABLED
#include "bacnetInitiateServiceMgmt.h"
#endif
/* To include Segmentation support */
#ifdef SEGMENTATION_SUPPORTED
#include "SegmentationGenericHandler.h"
#endif 
/* To include Intrinsic reporting support */
#if (defined INTRINSIC_REPORTING && defined BACDEL_SER_AE_EN_B) 
#include "serviceEventReportingIntrinsic.h"
#endif

/* Service Includes */
/* To include RP-B service */
#ifdef BACDEL_SER_DS_RP_B
#include "serviceReadProperty_B.h"
#endif

/* To include WP-B service */
#ifdef BACDEL_SER_DS_WP_B
#include "serviceWriteProperty_B.h"
#endif

/* To include RP-A service */
#ifdef BACDEL_SER_DS_RP_A
#include "serviceReadProperty_A.h"
#endif

/* To include RPM-B service */
#ifdef BACDEL_SER_DS_RPM_B
#include "serviceReadPropertyMultiple_B.h"
#endif

/* To include WPM-B service */
#ifdef BACDEL_SER_DS_WPM_B
#include "serviceWritePropertyMultiple_B.h"
#endif

/* To include RPM-A service */
#ifdef BACDEL_SER_DS_RPM_A
#include "serviceReadPropertyMultiple_A.h"
#endif

/* To include DDB-B Service */
#ifdef BACDEL_SER_DM_DDB_B
#include "serviceDynamicDeviceBinding_B.h"
#endif

/* To include DDB-A Service */
#ifdef BACDEL_SER_DM_DDB_A
#include "serviceDynamicDeviceBinding_A.h"
#endif

/* To include DOB-B Service */
#ifdef BACDEL_SER_DM_DOB_B
#include "serviceDynamicObjectBinding_B.h"
#endif

/* To include COV-B service */
#ifdef BACDEL_SER_DS_COV_B
#include "serviceChangeOfValue_B.h"
#endif

/* To include DCC-B service */
#ifdef BACDEL_SER_DM_DCC_B
#include "serviceDeviceCommControl.h"
#endif /* BACDEL_SER_DM_DCC_B */

/* To include AA-B Service */
#ifdef BACDEL_SER_AE_AA_B
#include "serviceAcknowledgeAlarm_B.h"
#endif

/* To include TS-B Service */
#ifdef BACDEL_SER_DM_TS_B
#include "serviceTimeSynchronization_B.h"
#endif

/* To include UTC-B Service */
#ifdef BACDEL_SER_DM_UTC_B
#include "serviceUtcTimeSynchronization_B.h"
#endif /* BACDEL_SER_DM_UTC_B */

/* To include RD-B Service */
#ifdef BACDEL_SER_DM_RD_B
#include "serviceReinitializeDevice_B.h"
#endif

/* To include GIE-B service */
#ifdef BACDEL_SER_AE_GEI_B
#include "serviceGetEventInformation_B.h"
#endif

#define SOCKET_ERROR          (-1)

/** Thread exit flag for Receive thread.*/
bool g_bReceiveThreadExit = FALSE;

/** Thread exit flag for Process thread. */
bool g_bProcessThreadExit = FALSE;

/** Thread exit flag for Transmit thread.*/
bool g_bTransmitThreadExit = FALSE;

extern  struct netconn *conn ;
extern   struct netbuf *buf;

/** process q index for master state machine */
extern int32_t g_i32ProcessQIndex;

extern DB_t SMCfg;

/* Gloabl Stack Init Flag to keep track of BACnet stack initialization */
bool g_bBACnetStackInitFlag = FALSE;

/* Gloabl Flag used to stop receiving request from network */
bool g_bReception = FALSE;

/* Global flag used to start & stop Socket Communication */
bool g_bSocketCommFlag = FALSE;

/* Global flag used to clear complete device database in stack */
bool g_bDeleteDeviceDatabase = FALSE;

/** Global flag to contorl deletion of initiate queue data 
while disabling n/w communication */
bool g_bDeleteInitQueData = false;

/** Cretae Instance of Process Data Queue */
processDataQue_t g_stProcDataQ = {0}; 

/** The Receive thread buffer to hold data. */
uint8_t g_u8RxBuf[MAX_MPDU] = { 0 };

/** The Receive Message Queue, filled by receive thread. */
msgQue_t g_stRxMsgQue = {0};

/** Socket info for B side **/
BACnet_Socket_Pkt_t g_stSocketPkt = {0};

/** Stack configuration parameters */
extern BACApp_StackConfig_t g_stStackConfigParams;
uint32_t            gi32TimeDurationSec = 0;
uint32_t gui32StartupDelay = SM_STARTUP_DELAY;			/* miliseconds */
uint8_t gu8CommStatus = 0;

/* Callback ID mutex lock */
extern Mutex_H g_hCallbackIdMtxLock;
extern	osMutexId_t m_hRxSegMtxLockHandle;
extern	osMutexId_t m_hCmplxAckMtxLockHandle;
extern	osThreadId_t Task_ReceiveThreadHandle;
extern	osThreadId_t Task_Process_ThHandle;
extern	osThreadId_t Task_Transmit_THandle;
extern osThreadId_t Task_DvProcessTHandle;
extern osThreadId_t EthInterpreterHandle;
extern osSemaphoreId_t DvNotificationThreadHandle;
extern QueueHandle_t Cov_EventQueue;
extern osMutexId_t m_hProcessDataQMtxLockHandle;

/**
*
* DESCRIPTION                                                                          
* This function registers the handlers of all the services supported
* by the BACnet stack. 
* Call this function at stack initialization.
*
*/    
void Init_Service_Handlers(void)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Init_Service_Handlers: Entry \r\n");
	#endif

	 
	
	/***************** Unconfirmed Handlers ********************/

#ifdef BACDEL_SER_DM_DDB_B
    /* Handler to WHO-IS Request */
    APDU_Set_Unconfirmed_Handler(SERVICE_UNCONFIRMED_WHO_IS, 
        Who_Is_B_Decode_Handler);
#endif

#ifdef BACDEL_SER_DM_DDB_A
    /* Handler to I-AM Request */
    APDU_Set_Unconfirmed_Handler(SERVICE_UNCONFIRMED_I_AM, 
        I_Am_A_Decode_Handler);
#endif

#ifdef BACDEL_SER_DM_DOB_B
    /* Handler to WHO-HAS Request */
    APDU_Set_Unconfirmed_Handler(SERVICE_UNCONFIRMED_WHO_HAS, 
    	Who_Has_B_Decode_Handler);
#endif


#ifdef BACDEL_SER_DM_UTC_B
    /* Handler to UTC Time-Sync Request */
    APDU_Set_Unconfirmed_Handler(
		SERVICE_UNCONFIRMED_UTC_TIME_SYNCHRONIZATION,
        TimeSync_B_Decode_Handler);
#endif

#ifdef BACDEL_SER_DM_TS_B
    /* Handler to Time-Sync Request */
    APDU_Set_Unconfirmed_Handler(SERVICE_UNCONFIRMED_TIME_SYNCHRONIZATION,
        TimeSync_B_Decode_Handler);
#endif

#ifdef BACDEL_PR23
#ifdef BACDEL_SER_DM_DDA_A
	/* Handler to Who-Am-I Request */
	 APDU_Set_Unconfirmed_Handler(SERVICE_UNCONFIRMED_WHO_AM_I,
		 Who_Am_I_A_Decode_Handler);
#endif
#endif /* BACDEL_PR23 */
	/***************** Confirmed Handlers ********************/

#ifdef BACDEL_SER_DS_RP_B    
    /* Handler to Read Property Request */
    APDU_Set_Confirmed_Handler(SERVICE_CONFIRMED_READ_PROPERTY,
        RP_B_Decode_Handler);
#endif

#ifdef BACDEL_SER_DS_RPM_B
    /* Handler to Read Multiple Property Request */
    APDU_Set_Confirmed_Handler(SERVICE_CONFIRMED_READ_PROP_MULTIPLE,
        RPM_B_Decode_Handler);
#endif

#ifdef BACDEL_SER_DS_WP_B
    /* Handler to Write Property Request */
     APDU_Set_Confirmed_Handler(SERVICE_CONFIRMED_WRITE_PROPERTY,
        WP_B_Decode_Handler); 
#endif

#ifdef BACDEL_SER_DS_WPM_B
    /* Handler to Write Property Multiple Request */
     APDU_Set_Confirmed_Handler(SERVICE_CONFIRMED_WRITE_PROP_MULTIPLE,
        WPM_B_Decode_Handler); 
#endif

#ifdef BACDEL_SER_DS_COV_B
    /* Handler to COV Subscribe Request */
     APDU_Set_Confirmed_Handler(SERVICE_CONFIRMED_SUBSCRIBE_COV,
        COV_B_Subscribe_Handler);
#endif

#ifdef BACDEL_SER_DM_DCC_B
    /* Handler to Device Communication Control Request */
    APDU_Set_Confirmed_Handler(SERVICE_CONFIRMED_DEVICE_COMMUNICATION_CONTROL,
        DCC_B_Decode_Handler);
#endif

#if (defined BACDEL_SER_AE_AA_B && defined BACDEL_OBJ_NC)
	 /* Handler to Confirmed Acknowledge Alarm request received */
	 APDU_Set_Confirmed_Handler(SERVICE_CONFIRMED_ACKNOWLEDGE_ALARM,
		 AckAlarm_B_Decode_Handler);
#endif

#ifdef BACDEL_SER_DM_RD_B
    /* Handler to Reintialize Device Request */ 
     APDU_Set_Confirmed_Handler(SERVICE_CONFIRMED_REINITIALIZE_DEVICE,
        RD_B_Decode_Handler); 
#endif

#ifdef BACDEL_SER_AE_GEI_B
    /* Handler to Get-Evevt-Information request */
     APDU_Set_Confirmed_Handler(SERVICE_CONFIRMED_GET_EVENT_INFORMATION,
        GEI_B_Decode_Handler);
#endif

#ifdef BACDEL_SER_DS_RR_B
	 /* Handler to Read Range Request */
	 APDU_Set_Confirmed_Handler(SERVICE_CONFIRMED_READ_RANGE,
        RR_B_Decode_Handler);
#endif
 	/***************** Confirmed Ack Handlers ********************/
 #ifdef  BACDEL_SER_DS_RP_A
     /* Handler to Read Property complex ack */
      APDU_Set_Confirmed_Ack_Handler(SERVICE_CONFIRMED_READ_PROPERTY,
         RP_A_Ack_Decode_Handler);
 #endif

 #ifdef  BACDEL_SER_DS_RPM_A
     /* Handler to Read Property Multiple complex ack */
      APDU_Set_Confirmed_Ack_Handler(SERVICE_CONFIRMED_READ_PROP_MULTIPLE,
         RPM_A_Ack_Decode_Handler);
 #endif

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Init_Service_Handlers: Exit \r\n");
	#endif
}

/**
*
* DISCREPTION
* This function will stop writing new packet data in receive queue.
* All new packets received will be silently discarded.
*
*/
BOOL Stop_Receive_Req(void)
{
	/* local variables */
	bool bRetVal = FALSE;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Stop_Receive_Req: Entry \r\n");
	#endif

	/* check the global flag */
    if(g_bReception == TRUE)
    {
		/* stop reception */
        g_bReception = FALSE;
        bRetVal = TRUE;
    }
    else
    {
		/* reception already stopped */
        bRetVal = FALSE;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Stop_Receive_Req: Exit \r\n");
	#endif
	return bRetVal;
}
/**                                                                         
*
* DESCRIPTION                                                                          
* The receive thread API will read data from n/w over UDP socket
* and stores in receive queue. 
* It generates event for process thread.
*    
* @param lpThreadParameter [in] not used.
* @returns [out] Zero.
*
*/

                                                                        
/**                                                                    
*
* DESCRIPTION                                                                          
* Proces thread API which reads data from receive queue and performs
* decoding of request/response. 
* It generates event for Transmit thread.
*    
* @param lpThreadParameter [in] not used.
* @returns [out] Zero.
*
*/
void Process_Thread_Task(void)
{
	/* BVLC decode return value */
    int32_t i32RetType = 0;

    /* Proess data Queue counter */
	uint32_t u32IdDQCnt = 0;

    /* Exit Flag byte */
	uint8_t	u8ExitCode = 0;

    /* Decode enable flag bit */
	bool bDecodeEnable = 0;        
	
	/* Get receive queue process pointer */
	msgBuffer_t * pstPktData = NULL;

	/* Get process queue raw data pointer */
	msgBuffer_t * pstRawData = NULL;

	/* process queue full flag */
	bool bProcQNodeFree = FALSE;
	
	/* count value */
	uint32_t u32Counter = 0;

	/* max count value */
	uint32_t u32MaxCounter = 0;

	/* calculate max counter value for timeout monitoring */
	/*  
	 * 10 milliseconds is the sleep time used in thread 
	 * 10 is buffer count i.e. total wait time will be 1.1 * timeout
	 */
	u32MaxCounter = ((g_stStackConfigParams.m_u32ProcessQTimeout) / 10 ) + 10; 
    

	/* thread entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: Process_Thread: Entry \r\n");
	#endif

	/* thread routine */
	 /* Wait for g_hStackInitEvent Event to set
	    This event will be set if Threads are created properly and BIP port is set */
	OSAL_Wait_Sem(g_hPxSemaphoreHandle, INFINITE);
	for(;;)
    { 
		
		/* set default values */
		bProcQNodeFree = FALSE;
		u32Counter = 0;

		/* Wait for infinite until the receive thread releases semaphore */
       OSAL_Wait_Sem(g_hPxSemaphoreHandle, INFINITE);


		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: Process_Thread: \
		Process received packet \r\n");
		#endif

		do{
			/* Lock the Process Queue for accquiring an avaliable queue 
                * location */				
            if(Osal_Wait_Mutex(m_hProcessDataQMtxLockHandle, INFINITE) != WAIT_OBJECT_0)
			{
				continue;
			}

            /* Intialize the flat bit */
			bDecodeEnable = 0;
			u8ExitCode = 0;

				/* Check for empty frame of Process Queue */
			for(u32IdDQCnt = g_stProcDataQ.m_u32QFillIndex; u32IdDQCnt < MAX_PACKET;
                u32IdDQCnt++)
			{
                /* Got the valid queue index */
				if(PROC_INIT 
                    == g_stProcDataQ.m_stProcessQueue[u32IdDQCnt].m_eState)
				{
                    /* Fill the queue and set decode enbable flag */
					g_stProcDataQ.m_u32QFillIndex = u32IdDQCnt;
					bDecodeEnable = 1;
				    /* Modify the status of the queue index so that it
                        * does not get overwritten */
					g_stProcDataQ.m_stProcessQueue[u32IdDQCnt].m_eState 
                        = PROC_CONTINUE;
					/* set flag */
					bProcQNodeFree = TRUE;
					break;
				}
				/* Reset the Queue Index & exit code */
				else if(((MAX_PACKET - 1) ==  u32IdDQCnt ) && (0 == u8ExitCode))
				{
					u32IdDQCnt = UINT32_MAX;
					u8ExitCode = 1;
				}
			}

			/* Increment QFill index to point to next location */
			if(u32IdDQCnt < MAX_PACKET)
			{
				g_stProcDataQ.m_u32QFillIndex++;
				if(MAX_PACKET <=  g_stProcDataQ.m_u32QFillIndex)
				{
					/* set to min value */
					g_stProcDataQ.m_u32QFillIndex = MIN_PACKET;
				}
			}
									
				/* Unlock Mutex */
			Osal_Release_Mutex(m_hProcessDataQMtxLockHandle);

			/* if queue location is not free in Px queue */
			if(!bProcQNodeFree)
			{
				/* wait for some time */
				Osal_Sleep(10);
				/* increment the counter */
				u32Counter++;
				if(u32Counter > u32MaxCounter)
				{
					/* no thread is free, timeout value exceeded */
					break;
				}
			}
		}while(!bProcQNodeFree);
				
				

				/* Get pointer of Receive queue to extract data */
			pstRawData = &g_stRxMsgQue.m_astRxQueue[g_stRxMsgQue.m_u32QProcIdx];

			/* check if this is valid process queue location */
			if(u32IdDQCnt < MAX_PACKET)
			{
				/* copy raw hex data from receive queue to process queue */
				pstPktData = &g_stProcDataQ.m_stProcessQueue[u32IdDQCnt].m_stPacketData;
				memcpy(pstPktData, pstRawData, sizeof(msgBuffer_t));
			}
			else
			{
				/* invalid process queue location - process queue is full */
				bDecodeEnable = 0;
			}

			/* As data is extracted make the Receive queue location 
                * avaliable */
			pstRawData->m_eState = FALSE;

			/* Increment start index of Receive queue after processing
                * received command */
			g_stRxMsgQue.m_u32QProcIdx++;
			if(MAX_PACKET <= g_stRxMsgQue.m_u32QProcIdx)
				g_stRxMsgQue.m_u32QProcIdx = MIN_PACKET;

            /* Decode the request frame */
			if(bDecodeEnable && (PROC_CONTINUE == 
                g_stProcDataQ.m_stProcessQueue[u32IdDQCnt].m_eState))
			{
				/* Debug Print */
				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: Process_Thread: \
				Process Queue index = %d \r\n", u32IdDQCnt);
				#endif

				/* Decode data from the received packet */
				i32RetType = MSTP_Decode_Handler(&pstPktData->m_u8Buffer[0],
					(uint16_t)pstPktData->m_u32PktLen,
					&g_stProcDataQ.m_stProcessQueue[u32IdDQCnt]);

				/* If Decoding not success make Process Queue slot free */
				if(BACDEL_ERROR == i32RetType)
				{
                    /* After sending the Error frame set all parameters at 
                        * that location to zero */
                    /* clear the queue location */
                    memset(&g_stProcDataQ.m_stProcessQueue[u32IdDQCnt], 0, sizeof(processInfo_t));
                        
                    /* Make the process queue location avaliable */
					g_stProcDataQ.m_stProcessQueue[u32IdDQCnt].m_eState = PROC_INIT;
               }
                /* If Decoding is success or error & need to send reply, activate Transmit Thread */
				else if(BACDEL_SUCCESS == i32RetType)
				{
                    /* Modify the status of Process queue slot to indicated 
                        * processing is complete */
					g_stProcDataQ.m_stProcessQueue[u32IdDQCnt].m_eState = PROC_DONE;
    									
                    /* This function increases the count of the Transmit 
                        * semaphore object by a 1.
                        * ReleaseSemaphore(handle to semaphore, count) */
                    if(!OSAL_Release_Sem(g_hTxSemaphoreHandle, BACNET_ONE))
					{
					/* semaphore release error */
					#ifdef DEBUG_PRINTF
						Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: Process_Thread: \
					Release semaphore error: %ld \r\n", Osal_Get_Last_Error());
					#endif
												/* clear the queue location */
												memset(&g_stProcDataQ.m_stProcessQueue[u32IdDQCnt], 0, sizeof(processInfo_t));
												g_stProcDataQ.m_stProcessQueue[u32IdDQCnt].m_eState = PROC_INIT;
					}
				}
                /* If Decoding is successful & data is sent for further processing, do nothing */
                else if(BACDEL_CONTINUE == i32RetType)
                {
                    ;// do nothing
                }
                else
                {
                    /* clear the queue location */
                    memset(&g_stProcDataQ.m_stProcessQueue[u32IdDQCnt], 0, sizeof(processInfo_t));
                        
                    /* Make the process queue location avaliable */
					g_stProcDataQ.m_stProcessQueue[u32IdDQCnt].m_eState = PROC_INIT;
                }
            }
            else
            {
				;
                /* Process queue is not empty */
				#ifdef DEBUG_PRINTF
			    Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: Process_Thread: \
				Process queue is not empty to store packet \r\n");
				#endif
            }

    }

	/* thread exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: Process_Thread: Exit \r\n");
	#endif
//	return 0;
}


/**
*
* DESCRIPTION                                                                          
* Tx Thread API which gets data from process Queue and performs 
* encoding of response to send it over NETWORK using UDP socket.
*
* @param lpThreadParameter [in] not used.
* @returns [out] Zero.
*
*/
void Transmit_Thread_Task(void)
{ 
    /* Process data buffer pointer */
    processInfo_t* pstProcQIndex = NULL;

    /* Data Queue Counter */
    uint32_t u32IdDQCnt = 0;
	
    /* Exit Flag byte*/
    uint8_t	u8ExitCode = 0;

	/* thread entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: Transmit_Thread: Entry \r\n");
	#endif
	OSAL_Wait_Sem(g_hTxSemaphoreHandle, INFINITE);
	/* thread routine */
	for(;;)
    { 
				/* Wait for infinite until the Transmit Thread is not signaled */
				OSAL_Wait_Sem(g_hTxSemaphoreHandle, INFINITE);

				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: Transmit_Thread: \
				Encode and Transmit Packet \r\n");
				#endif
				
                /* Intialize the flat bit */
                u8ExitCode = 0;
				
                /* Check for Process Queue with status PROC_DONE */
				for(u32IdDQCnt = g_stProcDataQ.m_u32QProcIdx; u32IdDQCnt < MAX_PACKET;
                    u32IdDQCnt++)
				{
                    /* Got the valid queue index */
					if(PROC_DONE == g_stProcDataQ.m_stProcessQueue[u32IdDQCnt].m_eState)
					{
						g_stProcDataQ.m_u32QProcIdx = u32IdDQCnt;
						break;
					}
					/* Reset the Queue Index & exit code */
					else if(((MAX_PACKET - 1) == u32IdDQCnt) && (0 == u8ExitCode))
					{
						u32IdDQCnt = UINT32_MAX;
						u8ExitCode = 1;
					}
				}
                
                /* Check if the location has status PROC_DONE  */
				if(PROC_DONE == g_stProcDataQ.m_stProcessQueue[g_stProcDataQ.m_u32QProcIdx].m_eState)
				{
					/* Get pointer of Process queue to extract data */
                    pstProcQIndex = &g_stProcDataQ.m_stProcessQueue[g_stProcDataQ.m_u32QProcIdx];

                    /* Check for Socket communication flag */
                    if(g_bSocketCommFlag == TRUE)
                    {
					    /* Encode the data present in Process Queue */
                    	APDU_Encode_Handler(pstProcQIndex);
                    }

                    /* free malloc memory for service request */
                    if(NULL != pstProcQIndex->m_stProcessData.m_stAPDU.m_pvServiceRequestData)
                    {
                        OSAL_Free(pstProcQIndex->m_stProcessData.m_stAPDU.m_pvServiceRequestData,
                            __FILE__, __FUNCTION__, __LINE__);
                        pstProcQIndex->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;
                    }
                    
                    /* After transmitting the frame set all parameters at 
                     * that location to zero */
                    memset(&pstProcQIndex->m_stProcessData, 0, sizeof(bacnetRequestData_t));
		
					/* Make Process Queue Location avaliable */
					pstProcQIndex->m_eState = PROC_INIT;

					/* Increment Process index after processing command */
					g_stProcDataQ.m_u32QProcIdx++;
					if(MAX_PACKET <= g_stProcDataQ.m_u32QProcIdx)
						g_stProcDataQ.m_u32QProcIdx = MIN_PACKET;
				}
                else
                {
					;
					/* no data for processing */
					#ifdef DEBUG_PRINTF
                    Print_DebugMsg(DEBUG_LEVEL1, "BACnetStackAppLayer: Transmit_Thread: \
					No data for Processing \r\n", Osal_Get_Current_ThreadId());
					#endif
                }

    }
    
	/* thread exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: Transmit_Thread : Exit \r\n");
	#endif

}


/**
*
* DESCRIPTION
* This function is used to acquire empty location in Process queue.
* 
* @paran - none.
* @return - process queue location index on success else -1.
*
*/
int32_t Find_Empty_Queue_Index()
{
    /* local variables */
    uint32_t u32IdDQCnt = UINT32_MAX;
    uint8_t	u8ExitCode = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Find_Empty_Queue_Index: entry \r\n");
	#endif
    
    /* Lock the Process Queue for accquiring an avaliable queue location */				
    if(Osal_Wait_Mutex(m_hProcessDataQMtxLockHandle, INFINITE) != WAIT_OBJECT_0)
	{
		/* mutex not acquired */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: Find_Empty_Queue_Index: \
		Osal_Wait_Mutex failed \r\n");
		#endif
        return u32IdDQCnt;
	}

	/* Initialize index count */
	u32IdDQCnt = 0;
    /* Intialize the flat bit */
	u8ExitCode = 0;

	/* Check for empty frame of Process Queue */
	for(u32IdDQCnt = g_stProcDataQ.m_u32QFillIndex; u32IdDQCnt < MAX_PACKET;
        u32IdDQCnt++)
	{
        /* Got the valid queue index */
		if(g_stProcDataQ.m_stProcessQueue[u32IdDQCnt].m_eState 
            == PROC_INIT)
		{
            /* Fill the queue and set decode enbable flag */
			g_stProcDataQ.m_u32QFillIndex = u32IdDQCnt;
            /* Modify the status of the queue index so that it
             * does not get overwritten */
			g_stProcDataQ.m_stProcessQueue[u32IdDQCnt].m_eState 
                = PROC_CONTINUE;
			break;
		}
		/* Reset the Queue Index & exit code */
		else if(((MAX_PACKET - 1) == u32IdDQCnt ) && (0 == u8ExitCode))
		{
			u32IdDQCnt = UINT32_MAX;
			u8ExitCode = 1;
		} 
	}

	/* Increment QFill index to point to next location */
	g_stProcDataQ.m_u32QFillIndex++;
	if(MAX_PACKET ==  g_stProcDataQ.m_u32QFillIndex)
	{
		/* set to MIN value */
		g_stProcDataQ.m_u32QFillIndex = MIN_PACKET;
	}
						
	/* Unlock Mutex */
	Osal_Release_Mutex(m_hProcessDataQMtxLockHandle);  

	/* if invalid location, return -1 */
	if(u32IdDQCnt >= MAX_PACKET)
	{
		/* return error, queue not free */
		u32IdDQCnt = UINT32_MAX;
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Find_Empty_Queue_Index: exit \r\n");
	#endif
    return(((int32_t)u32IdDQCnt));
}


/**
*
* DESCRIPTION                                                                          
* This API finds the device with id "u32DeviceId" in host device linklist.
* If device is found, its address is returned else null.
*
* @param u32DeviceId [in] Device id to find.
* @returns [out] void pointer either NULL or pointer of matched device.
*
*/
void * Find_In_Host_Device_List(uint32_t u32DeviceId) 
{
	/* local variables */
	#ifdef SUPPORT_MULTIPLE_DEVICE
	BACnet_Device_Struct_t *pstHostDev = NULL;
	#endif
	void *pvAddress = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Find_In_Host_Device_List : entry \r\n");
	#endif

	/* check with 1st device */
	if(gstHostDevice.m_u32DeviceInstace == u32DeviceId)
	{
		/* Match found, return the address */
		pvAddress = gstHostDevice.m_pstDeviceStruct;
	}


	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Find_In_Host_Device_List : exit \r\n");
	#endif

	/* return value */
	return pvAddress;
}


/**
*
* DESCRIPTION                                                                          
* This API finds the device with id "u32DeviceId" in remote device linklist.
* If device is found, its address is returned else null.
*
* @param u32DeviceId [in] Device id to find.
* @returns [out] void pointer either NULL or pointer of matched device.
*
*/
void * Find_In_Remote_Device_List(uint32_t u32DeviceId)
{
	/* local variables */

	void *pvAddress = NULL;

	/* return value */
	return pvAddress;
}

/************************ end of bacnetStackMgmt.c file **************************/
