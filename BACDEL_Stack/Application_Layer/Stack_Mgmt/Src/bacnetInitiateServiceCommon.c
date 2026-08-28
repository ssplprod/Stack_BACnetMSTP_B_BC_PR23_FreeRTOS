/******************************************************************************
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
*   SoftDEL Systems Ltd.                     		    india@softdel.com         
*   3rd Floor, Pentagon P4,								http://www.softdel.com  
*	Magarpatta City, Hadapsar
*	Pune - 411 028   							 
*                                                                         
******************************************************************************/
/****************************************************************************** 
*                                                                         
*   FILE
*                                                                      
*   AUTHORS                                                                             
*	 Harshal Mangale
*                                                                         
*   DESCRIPTION                                                            
*                                                                                
*   RELEASE HISTORY
*	DATE            NAME				DESCRIPTION
*   12/28/2011      Prashant Badgujar   File Created
*
******************************************************************************/

#include "osalFreeRTOS.h"

#include "bacnetInitiateServiceMgmt.h"
#include "bacnetAPDUHandler.h"
#include "bacnetStackMgmt.h"
#include "bacDELDef.h"
#include "propertyGenricHandler.h"
#include "propertyValueWrite.h"
#include "propertydef.h"
#include "pduDataEncodeDecode.h"
#include "pduEncodeDecode.h"
#include "miscMiscellaneous.h"
#include <string.h>

#ifdef BACDEL_SER_DS_RPM_B
#include "serviceReadPropertyMultiple_B.h"
#endif
#include "propertyValueRead.h"
#include "propertyClearValues.h"
#include "bacDELDeviceConfig.h"

#ifdef SEGMENTATION_SUPPORTED
#include "SegmentationGenericHandler.h"
#endif
#if (defined INTRINSIC_REPORTING && defined BACDEL_SER_AE_EN_B)
#include "serviceEventReportingIntrinsic.h"
#endif
#if (defined BACDEL_SER_AE_EN_B && defined BACDEL_SER_AE_AA_B)
#include "serviceEventNotification_B.h"
#endif 
#ifdef BACDEL_OBJ_SDL
#include "serviceScheduling_B.h"
#endif

/* Last Invoke Id used */
static uint16_t g_u16Current_Invoke_Id = 0;
/* Array to search free Invoke ID */
uint8_t g_a8Invoke_Id[INVOKE_ID_RANGE] = {0};

/** Mutex lock */
extern osMutexId_t g_hIDMtxLockHandle;

static uint32_t g_u32Current_Token_Id = 0;

/* This flag is used to start & stop Socket Communication */
extern bool g_bSocketCommFlag;

/** Stack config parameters */
extern BACApp_StackConfig_t g_stStackConfigParams;
extern DB_t SMCfg;

/** Variables for initiate thread trigger count */
/* Dynamic Queue to hold Initiated request parameters */
InitiateDataQue_t g_stReqInitiateDataQue;

extern osMutexId_t m_hInitMtxLockHandle;
extern osSemaphoreId_t g_hAckSemaphoreHandle;
extern osMutexId_t m_hRxSegMtxLockHandle;
extern osMutexId_t m_hCmplxAckMtxLockHandle;
extern osSemaphoreId_t g_hSegmentationSemaphoreHandle;
extern osThreadId_t Task_Initiate_THandle;
extern osThreadId_t AcknowledgementThreadHandle;
extern osThreadId_t SegmentationThreadHandle;
extern osTimerId_t g_Initiate_Service_Retry_TimerHandle;
extern osTimerId_t g_ClearInitiateQTimerHandle;
extern osTimerId_t g_DvManagementTimerHandle;
extern osTimerId_t g_SegmentationQTimerHandle;

bool g_bInitiateThreadExit;
bool g_bAcknowledgementThreadExit;
/**
*
*DESCRIPTION
*   This function decides if an initiated request is to be saved in 
*	initiateQ.
*
*@param ePduType		[in]	Type of PDU.
*@param u8ServiceChoice [in]	Service choice no.
*
*@returns bool - True if request is to be saved in initiateQ else false.
*
*/
static bool Save_Request(BACNET_PDU_TYPE ePduType, uint8_t u8ServiceChoice)
{
	/* default return value */
	bool bReturnValue = false;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Save_Request: Entry \r\n");
	#endif

	switch(ePduType)
	{
		case PDU_TYPE_CONFIRMED_SERVICE_REQUEST:
			/* All Confirmed service expect reply, hence needs to be saved */
			bReturnValue = true;
			break;
		
		case PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST:
			/* By defination Unconfirmed service does not expect a reply, hence 
				need not to be saved. But we are saving certain services for our 
				reference */
			switch(u8ServiceChoice)
			{
				#if ((CALLBACK_FOR_WHO_IS_A) || (CALLBACK_FOR_WHO_HAS_A))
				#if (CALLBACK_FOR_WHO_IS_A)
				case SERVICE_UNCONFIRMED_WHO_IS:
				#endif
				#if (CALLBACK_FOR_WHO_HAS_A)
				case SERVICE_UNCONFIRMED_WHO_HAS:
				#endif
					/* save who-is & who-has for further references */
					bReturnValue = true;
					break;
				#endif

				case SERVICE_UNCONFIRMED_I_AM:
				case SERVICE_UNCONFIRMED_I_HAVE:
				case SERVICE_UNCONFIRMED_COV_NOTIFICATION:
				case SERVICE_UNCONFIRMED_EVENT_NOTIFICATION:
				case SERVICE_UNCONFIRMED_PRIVATE_TRANSFER:
				case SERVICE_UNCONFIRMED_TEXT_MESSAGE:
				case SERVICE_UNCONFIRMED_TIME_SYNCHRONIZATION:
				case SERVICE_UNCONFIRMED_UTC_TIME_SYNCHRONIZATION:
				default:
					/* do not save this */
					bReturnValue = false;
					break;
			}
			break;

		case PDU_TYPE_SIMPLE_ACK:
		case PDU_TYPE_COMPLEX_ACK:
		case PDU_TYPE_SEGMENT_ACK:
		case PDU_TYPE_ERROR:
		case PDU_TYPE_REJECT:
		case PDU_TYPE_ABORT:
		default:
			/* do not save requests with this pdu type, 
				ideally this is dead case */
			bReturnValue = false;
			break;
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Save_Request: Exit \r\n");
	#endif
	return bReturnValue;
}

/**
*
* DESCRIPTION
* Function to fill initiate thread error response & pass the data
* to acknowledgement thread.
*
* @param  pstInitiateQIndex  [in]  initiate Q node
* @param  eErrorClass		 [in]  error class for response
* @param  eErrorCode		 [in]  error code for response
* @param  bFillErrorFlag	 [in]  error code & error code are updated
*
* @return [out] BACDEL_SUCCESS or any other error code
*
*/
BACNET_RETURN_TYPE Fill_Aside_Error_Response(
	InitiateInfo_t *pstInitiateQIndex,
	BACNET_ERROR_CLASS eErrorClass,
	BACNET_ERROR_CODE eErrorCode,
	bool bFillErrorFlag)
{
	/* local variables */
	processInfo_t *pstProcQInfo = NULL;
	BACNET_RETURN_TYPE eReturnValue = BACDEL_SUCCESS;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Fill_Aside_Error_Response: Entry \r\n");
	#endif

	/* allocate memory for data to be passed to ACK thread */
	pstProcQInfo = OSAL_Malloc(sizeof(processInfo_t), 
		__FILE__, __FUNCTION__, __LINE__);
	if(NULL == pstProcQInfo)
	{
		/* handle the error */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: \
		Fill_Aside_Error_Response: memory allocation failed \r\n");
		#endif
		/* wait for next thread trigger */
		return BACDEL_MALLOC_ERROR;
	}

	/* copy the data */
	memcpy(&pstProcQInfo->m_stProcessData, 
		&pstInitiateQIndex->m_ReqProcInfo.m_stProcessData, 
		sizeof(bacnetRequestData_t));
	pstProcQInfo->m_stProcessData.m_i32ClearIQTimer = 0;
	pstProcQInfo->m_stProcessData.m_i32DevRetryCnt = 0;
	pstProcQInfo->m_stProcessData.m_i32DevTimeout = 0;
	pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;

	/* set the flag to free memory later */
	pstProcQInfo->m_bIsDynamic = TRUE;

	/* set error from client */
	if(bFillErrorFlag)
	{
		/* set error */
		Service_Error_Handler(BACNET_STATUS_ERROR, eErrorClass, 
			eErrorCode, pstProcQInfo, FALSE);
	}

	/* pass the ack data to acknowledgement thread */
	eReturnValue = Initiate_Service_Interface(pstProcQInfo);  //<@>
	if(BACDEL_CONTINUE != eReturnValue)
    {
        /* debug log */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: \
		Fill_Aside_Error_Response: Segment Ack discarded \r\n");
		#endif
		/* free allocated memory */
		OSAL_Free(pstProcQInfo, __FILE__,__FUNCTION__,__LINE__);
    }
	else
	{
		/* set success */
		eReturnValue = BACDEL_SUCCESS;
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Fill_Aside_Error_Response: Exit \r\n");
	#endif
	return eReturnValue;
}

/**
*******************************************************************************
*DESCRIPTION
*   This thread function handles Sending A-side request and processing of response
*   of initiated request.
*
*1. Scan "InitiateDataQue_t" and check for "STATE_SEND_REQUEST" requests
*    - send command again, call "APDU_Encode_Handler(InitiateInfo_t->m_ReqProcInfo);"
*    - Check return type and change state as
*        - If failed "STATE_REQUEST_SEND_FAILED" or 
*        - If Success "STATE_AWAIT_RESPONSE"
*
*
*1. get pointer from RspProcess queue
*2. Check Same Invoke ID in "InitiateDataQue_t" array
*    - Store response (Prop value, Error class, Error code)
*    - InitiateDataQue_t-> service data pointer = RspProcess-> service data pointer
*    - Make Status as "STATE_RESPONSE_RECEIVED"
*3. Here: No need of received response parameters
*    - RspProcess-> service Data = NULL
*    - change status RspProcess queue as PROC_INIT
*    - make RspProcess queue pointer NULL
*
*@param lparam [in] pointer of Virtual Device data
*
******************************************************************************/
void Initiate_Thread_Task(void)
{
    /* Device Instace Pointer */
//	DWORD dwStatus = 0;
    virtualDevData_t* pstVirtualDev = NULL;
	InitiateInfo_t *pstInitiateQIndex = NULL;
    bacnetRequestData_t *pstReqParam = NULL;
	bool bSaveRequestFlag = FALSE;
	bool bNwLayerMsg = FALSE;
	BACNET_RETURN_TYPE eFunVal = BACDEL_SUCCESS;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Initiate_Thread: Entry \r\n");
	#endif

    while(1)
    {

		/* wait for infinite, until the semaphore is released */
	OSAL_Wait_Sem(g_hInitiateSemaphoreHandle, INFINITE);

	/* reset pointers & set defaults */
	pstVirtualDev = NULL;
	pstInitiateQIndex = NULL;
	eFunVal = BACDEL_SUCCESS;


		/* Find "STATE_SEND_REQUEST" node from Initiate Queue */
      pstInitiateQIndex = Traverse_InitiateInfo_Queue(FIND_STATE, 
			STATE_SEND_REQUEST, 0, NULL, 0, NULL);

		/* check pointers */
		if(NULL == pstInitiateQIndex)
		{
			/* handle the error */
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL1, "BACnetStackAppLayer: \
			Initiate_Thread: null pointers \r\n");
			#endif
			continue;
		}

		/* set the in use flag for initiate thread */
		BIT_SET(pstInitiateQIndex->m_u8NodeInUse, INITIATE_Q_INIT_THREAD_BIT_NO);

        /* get pointer to structure to fill request parameters */
        pstReqParam = &pstInitiateQIndex->m_ReqProcInfo.m_stProcessData;

		
		/* get virtual device pointer */
		#ifdef SUPPORT_MULTIPLE_DEVICE
        if(NULL != pstInitiateQIndex->m_ReqProcInfo.m_pvReqDevStruct)
		{
			/* get virtual device pointer */
            pstVirtualDev = (virtualDevData_t *)pstInitiateQIndex->m_ReqProcInfo.m_pvReqDevStruct;
		}
        else
		#endif
		{
			/* assign 1 host device pointer */
            pstVirtualDev = gstHostDevice.m_pstDeviceStruct;
		}

		/* flag to save or discard request in initiate Q */
		bSaveRequestFlag = Save_Request(pstReqParam->m_ePDUType, 
			pstReqParam->m_stAPDU.m_u8ServiceChoice);

		/* check if this is n/w layer message */
		bNwLayerMsg = pstReqParam->m_stNPDU.network_layer_message;

		/* encode and send request on network */
		/* check for socket communication flag is enabled */
        if(TRUE == g_bSocketCommFlag)
        {
			/* check if reply is expected */
			if(bSaveRequestFlag)
			{
				/* reset apdu_timeout counter */
				pstInitiateQIndex->m_ReqProcInfo.m_stProcessData.m_i32DevTimeout =
					(pstVirtualDev->m_stDevObject.m_stAPDUTimeout.m_u32Val)/CONVERT_TO_SECONDS;

				/* mstp requires token for initiating any request */
				/* change the state to fixed so that this node remains in initiate Q
				till the time its state is changed to await */
				/* master state machnine will change the state to await in use-token state */
				pstInitiateQIndex->m_eServiceState = STATE_FIXED;
			}

			/* call apdu encoder */
			eFunVal = APDU_Encode_Handler(&pstInitiateQIndex->m_ReqProcInfo);
		}

        /* encode and send request */
        if((TRUE == g_bSocketCommFlag) && (TRUE == bSaveRequestFlag)
                            && 
            (BACDEL_SUCCESS == eFunVal) && (FALSE == bNwLayerMsg))
        {
			#ifdef SEGMENTATION_SUPPORTED
			/* if message needs segmentation */
			if(pstReqParam->m_stAPDU.m_bSegmentedMessage)
			{
				/* Request sent succesfully wait for response */
				pstInitiateQIndex->m_eServiceState = STATE_SEGMENTED_CONFIRMATION;
			}
			else
			{
				/* reset apdu_timeout counter value */
				pstInitiateQIndex->m_ReqProcInfo.m_stProcessData.m_i32DevTimeout =
					(pstVirtualDev->m_stDevObject.m_stAPDUTimeout.m_u32Val)/CONVERT_TO_SECONDS;
				/* no need to update */
				//pstInitiateQIndex->m_eServiceState = STATE_AWAIT_RESPONSE;
			}

			#else
			
			/* reset apdu_timeout counter value */
			pstInitiateQIndex->m_ReqProcInfo.m_stProcessData.m_i32DevTimeout =
				(pstVirtualDev->m_stDevObject.
				m_stAPDUTimeout.m_u32Val)/CONVERT_TO_SECONDS;
			#endif
		}

		/* if socket communication is disabled & confirmed request */
		else if((FALSE == g_bSocketCommFlag) && (TRUE == bSaveRequestFlag))
		{
			/* fill & send response to ACK thread */
			eFunVal = Fill_Aside_Error_Response(pstInitiateQIndex, 
				ERROR_CLASS_COMMUNICATION, ERROR_CODE_OTHER, TRUE);
			if(BACDEL_SUCCESS != eFunVal)
            {
                /* debug log */
				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: \
				Initiate_Thread: Request discarded as communication disabled \r\n");
				#endif
				/* set state to free initiate Q data */
				Clear_InitiateQ_State(&pstInitiateQIndex);
            }
		}

		/* if encoding fails and confirmed request */
		else if((BACDEL_SUCCESS != eFunVal) && (TRUE == bSaveRequestFlag))
		{
			/* fill & send response to ACK thread */
			eFunVal = Fill_Aside_Error_Response(pstInitiateQIndex, 
				pstReqParam->m_stAPDU.m_u32ErrorClass, 
				pstReqParam->m_stAPDU.m_u32ErrorCode, FALSE);
			if(BACDEL_SUCCESS != eFunVal)
            {
                /* debug log */
				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: \
				Initiate_Thread: Request discarded as encoding failed \r\n");
				#endif
				/* set state to free initiate Q data */
				Clear_InitiateQ_State(&pstInitiateQIndex);
            }
		}

		/* unconfirmed request - not to be saved */
        else
        {
			/* set state to free initiate Q data */
			Clear_InitiateQ_State(&pstInitiateQIndex);
			
        }

		/* clear the in use flag for initiate thread */
		BIT_CLEAR(pstInitiateQIndex->m_u8NodeInUse, INITIATE_Q_INIT_THREAD_BIT_NO);
	}// end of while

	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Initiate_Thread: Exit \r\n");
	#endif

}


/**
*******************************************************************************
*DESCRIPTION
*   This function is called from Ack Response handler. received response parameters
*   are queued in response queue and Initiate thread triggerd to process this 
*   response. 
*   1. Find empty location in queue
*   2. Give pointer of queue
*   3. increment Qfill Index
*   4. Event for Initiate Service thread
*   5. return Continue or Error
*
*@param pstRspProcessData [in] Pointer to structure which stores response param
*
*@return    BACDEL_CONTINUE  If queue is empty and semaphore incremented
*           BACDEL_ERROR    else
*
******************************************************************************/
BACNET_RETURN_TYPE Initiate_Service_Interface(processInfo_t *pstRspProcessData)
{
	/* local variables */
	BACNET_RETURN_TYPE eRetVal = BACDEL_ERROR;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Initiate_Service_Interface: Entry \r\n");
	#endif

#ifdef SEGMENTATION_SUPPORTED
	if(pstRspProcessData->m_stProcessData.m_ePDUType == PDU_TYPE_SEGMENT_ACK ||
        (pstRspProcessData->m_stProcessData.m_ePDUType == PDU_TYPE_ABORT && 
        !pstRspProcessData->m_stProcessData.m_stAPDU.m_bServer && 
		!pstRspProcessData->m_bIsDynamic))
	{
		if(NULL == g_stReqInitiateDataQue.m_pstSegRspQueue
			[g_stReqInitiateDataQue.m_i32SegRspQFillIndex])
		{
			/* assign received response parameter to segment response queue */
			g_stReqInitiateDataQue.m_pstSegRspQueue
				[g_stReqInitiateDataQue.m_i32SegRspQFillIndex] = pstRspProcessData;

			/* set return value */
			eRetVal = BACDEL_CONTINUE;

			/* signal segmentation thread */
			if(!OSAL_Release_Sem(g_hSegmentationSemaphoreHandle, BACNET_ONE))
			{
				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: Initiate_Service_Interface: \
				Seg semaphore release error: %ld \r\n", Osal_Get_Last_Error());
				#endif
				g_stReqInitiateDataQue.m_pstSegRspQueue
					[g_stReqInitiateDataQue.m_i32SegRspQFillIndex] = NULL;
				eRetVal = BACDEL_ERROR;
			}
		}
		else
		{
			/* empty location not found */
			eRetVal = BACDEL_ERROR;
		}

		/* increment Q fill index of response queue */
	    g_stReqInitiateDataQue.m_i32SegRspQFillIndex++ ;
	    if(MAX_INITIATE_SERVICES == g_stReqInitiateDataQue.m_i32SegRspQFillIndex)
		{
			/* reset value */
		    g_stReqInitiateDataQue.m_i32SegRspQFillIndex = 0;
		}
	}
	else
	{
		/* send data to acknowledgement thread */
		if(NULL == g_stReqInitiateDataQue.m_pstRspQueue
			[g_stReqInitiateDataQue.m_i32RspQFillIndex])
		{
			/* assign received response parameter to response queue */
			g_stReqInitiateDataQue.m_pstRspQueue
				[g_stReqInitiateDataQue.m_i32RspQFillIndex] = pstRspProcessData;

			/* set return value */
			eRetVal = BACDEL_CONTINUE;

			/* signal acknowledgement thread */
			if(!OSAL_Release_Sem(g_hAckSemaphoreHandle, BACNET_ONE))
			{
				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: Initiate_Service_Interface: \
				Ack semaphore release error: %ld \r\n", Osal_Get_Last_Error());
				#endif
				g_stReqInitiateDataQue.m_pstRspQueue
					[g_stReqInitiateDataQue.m_i32RspQFillIndex] = NULL;
				eRetVal = BACDEL_ERROR;
			}
		}
		else
		{
			/* empty location not found */
			eRetVal = BACDEL_ERROR;
		}

		/* increment Q fill index of response queue */
	    g_stReqInitiateDataQue.m_i32RspQFillIndex++ ;
	    if(MAX_INITIATE_SERVICES == g_stReqInitiateDataQue.m_i32RspQFillIndex)
		{
			/* reset value */
		    g_stReqInitiateDataQue.m_i32RspQFillIndex = 0;
		}
	}
#else
    /* send data to acknowledgement thread */
	if(NULL == g_stReqInitiateDataQue.m_pstRspQueue
		[g_stReqInitiateDataQue.m_i32RspQFillIndex])
	{
		/* assign received response parameter to response queue */
		g_stReqInitiateDataQue.m_pstRspQueue
			[g_stReqInitiateDataQue.m_i32RspQFillIndex] = pstRspProcessData;

		/* set return value */
		eRetVal = BACDEL_CONTINUE;

		/* signal acknowledgement thread */
		if(!OSAL_Release_Sem(g_hAckSemaphoreHandle, BACNET_ONE))
		{
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: Initiate_Service_Interface: \
			Ack semaphore release error: %ld \r\n", Osal_Get_Last_Error());
			#endif
			g_stReqInitiateDataQue.m_pstRspQueue
				[g_stReqInitiateDataQue.m_i32RspQFillIndex] = NULL;
			eRetVal = BACDEL_ERROR;
		}
	}
	else
	{
		/* empty location not found */
		eRetVal = BACDEL_ERROR;
	}

	/* increment Q fill index of response queue */
    g_stReqInitiateDataQue.m_i32RspQFillIndex++ ;
    if(MAX_INITIATE_SERVICES == g_stReqInitiateDataQue.m_i32RspQFillIndex)
	{
		/* reset value */
	    g_stReqInitiateDataQue.m_i32RspQFillIndex = 0;
	}
#endif /* SEGMENTATION_SUPPORTED */

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Initiate_Service_Interface: Exit \r\n");
	#endif
	return eRetVal;
}

/**
*******************************************************************************
*DESCRIPTION
*   This is callback function for hInitiateTimer. It is called every 1 second.
*
*Scan entire array of "InitiateDataQue_t" for "STATE_AWAIT_RESPONSE"
*   - Decrement current timeout by elapsed time ( 1 second), 
*   - if timeout is ZERO reset timeout and increment Retry count by 1
*   - if retry count is less than MAX_RETRIES, 
*     make status "STATE_SEND_REQUEST" & release semaphore to resend request
*   - else retry is MAX_RETRIES make status "STATE_TIMEOUT"
*
******************************************************************************/
TIMER_CALLBACK Initiate_Service_Retry_Timer(void* lpParam, bool TimerOrWaitFired)
{
    //int32_t i32QIndexCnt = 0;
    uint8_t u8MaxAPDURety = 0;
    InitiateInfo_t *pstInitiateInfoQ = NULL;
    bacnetRequestData_t *pstReqParam = NULL;
    virtualDevData_t *pstVirtualDev = NULL;
    static bool bRetryFlag = false;
#ifdef SEGMENTATION_SUPPORTED
	Apdu_Segment_Data_t *pstAPDUSegmentData = NULL;
	APDUSegment_t *pstApduSegment = NULL;
	bool bEncodeSegment =  FALSE;
#endif

	/* avoid concurrent execution of multiple timer instances */
    if(true == bRetryFlag)
	{
        return NULL;
	}

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Initiate_Service_Retry_Timer:Entry\r\n");
	#endif

    /* set flag */
    bRetryFlag = true;

    /* wait for mutex */

	Osal_Wait_Mutex(m_hInitMtxLockHandle, INFINITE);
    /* Get pointer to request parameters */
    pstInitiateInfoQ = g_stReqInitiateDataQue.m_pstInitiateInfoQ;
    /* Check for "STATE_AWAIT_RESPONSE" frame of Request Initiate Queue */

    while(pstInitiateInfoQ != NULL && pstInitiateInfoQ != ((void *)0xfeeefeee))
    {
        if(STATE_AWAIT_RESPONSE == (*(INITIATE_SERVICE_STATE *)pstInitiateInfoQ))
        {
            /* Get pointer to request parameters */
            pstReqParam = &pstInitiateInfoQ->m_ReqProcInfo.m_stProcessData;

            /* Decrement current timeout by elapsed time */
            pstReqParam->m_i32DevTimeout--;

            /* Check for Timeout occurred */
            if(pstReqParam->m_i32DevTimeout <= 0)
            {
                /* Get Virtual device Pointer */
                pstVirtualDev = pstInitiateInfoQ->m_ReqProcInfo.m_pvReqDevStruct;

                /* Update Timeout value and Retry value */
                if(NULL != pstVirtualDev)
                {
                    u8MaxAPDURety = (uint8_t)pstVirtualDev->m_stDevObject.m_stNumOfAPDURetries.m_u32Val;
                    pstReqParam->m_i32DevTimeout = pstVirtualDev->m_stDevObject.
                        m_stAPDUTimeout.m_u32Val/CONVERT_TO_SECONDS;
                }
                else
                {
                    /* set default values */
                    u8MaxAPDURety = NUMBER_OF_APDU_RETRIES;
                    pstReqParam->m_i32DevTimeout = APDU_TIMEOUT/CONVERT_TO_SECONDS;
                }

                /* Increment retry count */
                pstReqParam->m_i32DevRetryCnt++;

                /* Check How many retries are done. Retries are only to be
                 * attempted if the PDU type is not unconfirmed & simpleAck */
                if((u8MaxAPDURety >= pstReqParam->m_i32DevRetryCnt) &&
                    (pstReqParam->m_ePDUType ==
                    PDU_TYPE_CONFIRMED_SERVICE_REQUEST))
                {
                    /* Resend request */
                    pstInitiateInfoQ->m_eServiceState = STATE_SEND_REQUEST;
                    
                    /* Start Initiate thread to send request */
                    if(!OSAL_Release_Sem(g_hInitiateSemaphoreHandle, BACNET_ONE))
	                {
						#ifdef DEBUG_PRINTF
		                Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: Initiate_Service_Retry_Timer: ReleaseSemaphore error: %ld \r\n", Osal_Get_Last_Error());
						#endif
                        pstInitiateInfoQ->m_eServiceState = STATE_REQUEST_SEND_FAILED;
	                }
                  }
                else
                {
					/* Make status as Timeout, don't resend request */
					pstInitiateInfoQ->m_eServiceState = STATE_TIMEOUT;
					pstInitiateInfoQ->m_ReqProcInfo.m_stProcessData.m_i32ClearIQTimer = 
						g_stStackConfigParams.m_u32ClearInitQTimeout;
					
					/* check if callback is registered */
                    if(NULL != pstInitiateInfoQ->pvCallBackFunc)
					{
						/* fill TSM timeout abort response */
						pstInitiateInfoQ->m_stIpArgs.m_stNPDUData.m_stAPDUData.m_stServiceChoice.
							m_stAbortResponse.m_eAbortReason = ABORT_REASON_TSM_TIMEOUT;
						pstInitiateInfoQ->m_stIpArgs.m_stNPDUData.m_stAPDUData.m_ePduType = 
							PDU_TYPE_ABORT;

						/* Inform to the Application(Caller) about response */
                        pstInitiateInfoQ->pvCallBackFunc(&pstInitiateInfoQ->m_stIpArgs, STATE_TIMEOUT);
					}
					/* set state to free initiate Q data */
					Clear_InitiateQ_State(&pstInitiateInfoQ);
                }
            }
        }
        if(pstInitiateInfoQ != NULL)
        {
            pstInitiateInfoQ = pstInitiateInfoQ->pstNextAddress;
        }
    }// End of for Retry Request loop

#ifdef SEGMENTATION_SUPPORTED
#ifndef RETRY_CONFIRM_REQUEST
	/* RETRY MECHANISM FOR INITIATED CONFIRMED PDU TYPE REQUEST */
	/* Get pointer to initiateQ */
    pstInitiateInfoQ = g_stReqInitiateDataQue.m_pstInitiateInfoQ;
    
    while(pstInitiateInfoQ != NULL && pstInitiateInfoQ != ((void *)0xfeeefeee))
    {
        if(STATE_SEGMENTED_CONFIRMATION == (*(INITIATE_SERVICE_STATE *)pstInitiateInfoQ))
        {
		/* Get pointr of APDU Segment data */
		pstAPDUSegmentData = pstInitiateInfoQ->m_ReqProcInfo.m_pstAPDUSegmentData;

		if((!pstInitiateInfoQ->m_u8NodeInUse) && (NULL != pstAPDUSegmentData) && 
		   (TRUE == pstAPDUSegmentData->m_uRxTxSegmentInfo.m_stTxSegmentInfo.m_bWaitForSegAck))
		{
			/* Get pointer of 1st segment */
			pstApduSegment = pstAPDUSegmentData->m_pstSegment;
			while( NULL != pstApduSegment )
			{
				if(SEGMENT_STATE_AWAIT_RESPONSE == pstApduSegment->m_eServiceState)
				{
					/* Decrement current timeout by elapsed time */
					pstApduSegment->m_i32DevSegTimeout--;
					/* Check for Timeout occurred */
					if(pstApduSegment->m_i32DevSegTimeout <= 0)
					{
                        /* Get virtual device & Update Retry value */
                        pstVirtualDev = pstInitiateInfoQ->m_ReqProcInfo.m_pvReqDevStruct;
                        if(NULL != pstVirtualDev)
                            u8MaxAPDURety = (uint8_t)pstVirtualDev->m_stDevObject.
                            m_stNumOfAPDURetries.m_u32Val;
                        else
                            /* set default values */
                            u8MaxAPDURety = NUMBER_OF_APDU_RETRIES;
						/* Increment retry count */
						pstApduSegment->m_i32DevRetryCnt++;
						/* Check How many retries are done. */
						if(u8MaxAPDURety >= pstApduSegment->m_i32DevRetryCnt)
						{
							/* Reset Timeout value and increment Retry count */
							pstApduSegment->m_i32DevSegTimeout = 
								pstAPDUSegmentData->m_u32DevSegmentTimeout;
							/* Resend request */
							pstApduSegment->m_eServiceState = SEGMENT_STATE_SEND_REQUEST;
							/* enable encode segment flag */
							bEncodeSegment = TRUE;
						}
						else
						{
							/* Make status as Timeout, don't resend request */
							pstApduSegment->m_eServiceState = SEGMENT_STATE_TIMEOUT;
							pstInitiateInfoQ->m_eServiceState = STATE_TIMEOUT;
							pstInitiateInfoQ->m_ReqProcInfo.m_stProcessData.
								m_i32ClearIQTimer = g_stStackConfigParams.m_u32ClearInitQTimeout;

							/* check if callback is registered */
							if(NULL != pstInitiateInfoQ->pvCallBackFunc)
							{
								/* fill TSM timeout abort response */
								pstInitiateInfoQ->m_stIpArgs.m_stNPDUData.m_stAPDUData.m_stServiceChoice.
									m_stAbortResponse.m_eAbortReason = ABORT_REASON_TSM_TIMEOUT;
								pstInitiateInfoQ->m_stIpArgs.m_stNPDUData.m_stAPDUData.m_ePduType = 
									PDU_TYPE_ABORT;

								#ifdef PERFORMANCE_VERIFICATION
								/* get start time */
								PV_GetCurrent_Time(&stStartDiffTime2);
								#endif

								/* Inform to the Application(Caller) about response */
								pstInitiateInfoQ->pvCallBackFunc(&pstInitiateInfoQ->m_stIpArgs, 
                                    STATE_TIMEOUT);
							}
							/* disable encode segment flag */
							bEncodeSegment = FALSE;

							/* set state to free initiate Q data */
							Clear_InitiateQ_State(&pstInitiateInfoQ);
						}
					}
				}
				else;	/* do nothing */

				/* break the loop when timeout occurs for any 1 segment */
				if(STATE_TIMEOUT == pstInitiateInfoQ->m_eServiceState || 
					STATE_ERROR == pstInitiateInfoQ->m_eServiceState)
				{
					/* no need to check remaining segments */
					break;
				}
				/* move to next segment */
				pstApduSegment = pstApduSegment->m_pstNext;
			} // end of while( NULL != pstApduSegment )
			if(bEncodeSegment)
			{
				/* reset the flag */
				bEncodeSegment = FALSE;
				/* encode & send the segments that did not receive ack */
				APDU_Packet_Encode_Handler(&pstInitiateInfoQ->m_ReqProcInfo, 
					pstAPDUSegmentData->m_i8SegAckFor, 
					pstAPDUSegmentData);
			}
		}// end of if(NULL != pstAPDUSegmentData)
        }// end of if()
		/* Move to next node in initiate queue i.e. pstInitiateInfoQ */
		if(NULL != pstInitiateInfoQ)
			pstInitiateInfoQ = pstInitiateInfoQ->pstNextAddress;
	} // end of while(pstInitiateInfoQ != NULL)
#endif /* RETRY_CONFIRM_REQUEST */
#endif /* SEGMENTATION_SUPPORTED */

    /* release mutex */
	Osal_Release_Mutex(m_hInitMtxLockHandle);
    /* reset flag */
    bRetryFlag = false;

    /* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:Initiate_Service_Retry_Timer:Exit\r\n");
	#endif

	return NULL;
}



#ifdef SEGMENTATION_SUPPORTED
/* Timer routine for segment retry mechanism, checking for segment_awaited & response_awaited
	segments & freeing the timed-out segments */
//TIMER_CALLBACK SegmentationQTimer(PVOID lpParam, BOOLEAN TimerOrWaitFired)
//TIMER_CALLBACK SegmentationQTimer_Timer(void* lpParam, bool TimerOrWaitFired)
void SegmentationQTimer_Timer(void)
{
    uint8_t u8MaxAPDURety  = 0;
    virtualDevData_t *pstVirtualDev = NULL;
	processInfo_t *pstProcessInfoQ = NULL;
	processInfo_t *pstProcessInfoQTemp = NULL;
	Apdu_Segment_Data_t *pstAPDUSegmentData = NULL;
	APDUSegment_t *pstApduSegment = NULL;
	bool bEncodeSegment =  FALSE;
    static bool bSegFlag = false;

	/* avoid concurrent execution of multiple timer instances */
    if(true == bSegFlag)
	{
        return;
	}

    /* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:SegmentationQTimer:Entry\r\n");
	#endif

    /* set flag */
    bSegFlag = true;

#ifndef RETRY_COMPLEX_ACK
	/* RETRY MECHANISM FOR COMPLEX PDU TYPE ACK's */
	/* Get pointer to processQ */
	pstProcessInfoQ = g_stComplexAckDataQue.m_pstProcessInfoQ;
    while(pstProcessInfoQ != NULL)
    {
        /* Get pointer to next node */
        pstProcessInfoQTemp = pstProcessInfoQ->m_pstNextProcessInfo;
		/* Get pointr of APDU Segment data */
		pstAPDUSegmentData = pstProcessInfoQ->m_pstAPDUSegmentData;

		if((NULL != pstAPDUSegmentData) && 
		   (TRUE == pstAPDUSegmentData->m_uRxTxSegmentInfo.m_stTxSegmentInfo.m_bWaitForSegAck))
		{
			/* Get pointer of 1st segment */
			pstApduSegment = pstAPDUSegmentData->m_pstSegment;
			/* acquire the mutex lock - Tx side */
			//Osal_Wait_Mutex(g_stComplexAckDataQue.m_hCmplxAckMtxLock, INFINITE);
			
			Osal_Wait_Mutex( m_hCmplxAckMtxLockHandle , INFINITE); //commented before
			
			while( NULL != pstApduSegment )
			{
				if(SEGMENT_STATE_AWAIT_RESPONSE == pstApduSegment->m_eServiceState)
				{
					/* Decrement current timeout by elapsed time */
					pstApduSegment->m_i32DevSegTimeout--;
					/* Check for Timeout occurred */
					if(pstApduSegment->m_i32DevSegTimeout <= 0)
					{
                        /* Get virtual device & Update Retry value */
                        pstVirtualDev = pstProcessInfoQ->m_pvReqDevStruct;
                        if(NULL != pstVirtualDev)
                            u8MaxAPDURety = (uint8_t)pstVirtualDev->m_stDevObject.
                            m_stNumOfAPDURetries.m_u32Val;
                       else
                            /* set default values */
                            u8MaxAPDURety = NUMBER_OF_APDU_RETRIES;
						/* Increment retry count */
						pstApduSegment->m_i32DevRetryCnt++;
						/* Check How many retries are done. */
						if(u8MaxAPDURety >= pstApduSegment->m_i32DevRetryCnt)
						{
							/* Reset Timeout value and increment Retry count */
							pstApduSegment->m_i32DevSegTimeout =
								pstAPDUSegmentData->m_u32DevSegmentTimeout;
							/* Resend request */
							pstApduSegment->m_eServiceState = SEGMENT_STATE_SEND_REQUEST;
							/* enable encode segment flag */
							bEncodeSegment = TRUE;
						}
						else
						{
							/* Make status as Timeout, don't resend request */
							pstApduSegment->m_eServiceState = SEGMENT_STATE_TIMEOUT;
							/* update state */
							pstProcessInfoQ->m_eState = PROC_DONE;
							/* disable encode segment flag */
							bEncodeSegment = FALSE;
					}
					}
				}
				else;	/* do nothing */
				/* move to next segment */
				pstApduSegment = pstApduSegment->m_pstNext;
			} // end of while( NULL != pstApduSegment )
			/* release the mutex lock */
			Osal_Release_Mutex( m_hCmplxAckMtxLockHandle ); //commented before
			if(bEncodeSegment)
			{
				/* reset the flag */
				bEncodeSegment = FALSE;
				/* encode & send the segments that did not receive ack */
				APDU_Packet_Encode_Handler(pstProcessInfoQ,
					pstAPDUSegmentData->m_i8SegAckFor,
					pstAPDUSegmentData);
			}
		}// end of if(NULL != pstAPDUSegmentData)
		/* Move to next node in complex ack linklist i.e. pstProcessInfoQ */
		pstProcessInfoQ = pstProcessInfoQTemp;
	} // end of while(pstProcessInfoQ != NULL)
#endif /* RETRY_COMPLEX_ACK */

#ifndef CLEAR_COMPLEX_ACK_FROM_Tx_LINKLIST
	/* MECHANISM TO CLEAR THE TIMED_OUT COMPLEX ACK's */
	/* Get pointer to processQ from Tx linklist */
    pstProcessInfoQ = g_stComplexAckDataQue.m_pstProcessInfoQ;
    /* traverse the list */
    while(pstProcessInfoQ != NULL)
    {
        /* save the next node address */
        pstProcessInfoQTemp = pstProcessInfoQ->m_pstNextProcessInfo;
		/* if state of processInfo in linklist is PROC_DONE, than free it */
		if(PROC_DONE == pstProcessInfoQ->m_eState)
		{
			/* clear the required node */
			Clear_Segmentation_Data(pstProcessInfoQ, LINK_LIST_SEGMENTATION_TRANSMIT);
			/* restore the next node address */
		}
		/* Move to next node in complex ack linklist i.e. pstProcessInfoQ */
        pstProcessInfoQ = pstProcessInfoQTemp;
	}
#endif /* CLEAR_COMPLEX_ACK_FROM_Tx_LINKLIST */

#ifndef ClEAR_AWAITED_SEGMENT_FROM_Rx_LINKLIST
	/* MECHANISM TO CLEAR THE TIMED_OUT received segments */
    /* acquire the mutex lock - Rx side */
	Osal_Wait_Mutex(m_hRxSegMtxLockHandle, INFINITE);
	/* Get pointer to processQ from Rx linklist */
	pstProcessInfoQ = g_stRxSegmentsDataQue.m_pstRxProcessInfoQ;
    /* traverse the list */
	while(pstProcessInfoQ != NULL)
	{
        /* save the next node address */
        pstProcessInfoQTemp = pstProcessInfoQ->m_pstNextProcessInfo;
		/* Get pointr of APDU Segment data */
		pstAPDUSegmentData = pstProcessInfoQ->m_pstAPDUSegmentData;
		if(NULL != pstAPDUSegmentData)
		{
			/* Get pointer of 1st segment */
			pstApduSegment = pstAPDUSegmentData->m_pstSegment;	
			while(NULL != pstApduSegment)
			{
				/* decrement segment timeout if segment is in await_segment state */
				if(SEGMENT_STATE_AWAIT_SEGMENT == pstApduSegment->m_eServiceState)
					pstApduSegment->m_i32DevSegTimeout--;
				if(0 == pstApduSegment->m_i32DevSegTimeout)
				{
					/* change the state of instance of processQ to clear it */
					pstProcessInfoQ->m_eState = PROC_DONE;
					break;
				}
				/* move to next segment */
				pstApduSegment = pstApduSegment->m_pstNext;
			}
		}
		/* move to next node */
        pstProcessInfoQ = pstProcessInfoQTemp;
	}// end of while(pstProcessInfoQ != NULL)
	/* release the mutex - Rx side */
	Osal_Release_Mutex( m_hRxSegMtxLockHandle );
	/* Clear the timed-out processInfoQ from Rx linklist */
	/* Get pointer to processQ from Rx linklist */
    pstProcessInfoQ = g_stRxSegmentsDataQue.m_pstRxProcessInfoQ;
    /* traverse the list */
	while(pstProcessInfoQ != NULL)
	{
        /* save the next node address */
        pstProcessInfoQTemp = pstProcessInfoQ->m_pstNextProcessInfo;
		/* if state of processInfo in linklist is PROC_DONE, than free it */
		if(PROC_DONE == pstProcessInfoQ->m_eState && !pstProcessInfoQ->m_u8NodeInUse)
		{
            /* clear the required node */
			Clear_Segmentation_Data(pstProcessInfoQ, LINK_LIST_SEGMENTATION_RECEIVE);
		}
		/* Move to next node in complex ack linklist i.e. pstProcessInfoQ */
		pstProcessInfoQ = pstProcessInfoQTemp;
	}//end of while(pstProcessInfoQ != NULL)
#endif /* ClEAR_AWAITED_SEGMENT_FROM_Rx_LINKLIST */

    /* reset flag */
    bSegFlag = false;

    /* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer:SegmentationQTimer:Exit\r\n");
	#endif
}
#endif /* SEGMENTATION_SUPPORTED */


/**
*
* DESCRIPTION
* Function deletes Initiate thread, VD thread and Timers for
* other stack functionalities.
*
* @param	[in]  void.
* @returns	[out] success or suitable error code.
*
*/
BACNET_RETURN_TYPE Delete_Initiate_Thread(void)
{
    /* local varaibles */

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Delete_Initiate_Thread: Entry \r\n");
	#endif


	/*************************** Initiate thread ***************************/

    /* Check the initiate thread handle */
	if(Task_Initiate_THandle != NULL)
	{
        /* set thread exit condition */
		g_bInitiateThreadExit = TRUE;

        /* Generate a signal to Initiate thread semaphore */
        if(!OSAL_Release_Sem(g_hInitiateSemaphoreHandle, BACNET_ONE))
        {
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: DeInit_ServiceLayer: "
		    "Initiate thread sem release error: %ld \r\n", Osal_Get_Last_Error());
			#endif
			return BACDEL_SEMAPHORE_ERROR;
        }

        /* Terminate & close handle of Initiate thread */
        if(!Osal_Thread_Terminate(Task_Initiate_THandle))
        {
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: Delete_Initiate_Thread: "
			"Initiate thread handle close failed \r\n");
			#endif
            return BACDEL_THREAD_ERROR;
        }
    }
	/* reset the thread handle */
    Task_Initiate_THandle = NULL;

    /* Check if valid handle to the semaphore object is created */
	if(g_hInitiateSemaphoreHandle != NULL)
	{
		/* This function closes an open object handle of semaphore */
        if(!OSAL_Close_Sem(g_hInitiateSemaphoreHandle))	
		{
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: Delete_Initiate_Thread: "
			"Initiate thread semaphore 0 handle close failed \r\n");	
			#endif
			return BACDEL_SEMAPHORE_ERROR;
		}
	}
	/* reset the semaphore handle */
	g_hInitiateSemaphoreHandle = NULL;


	/***************** Acknowledgement Thread *********************/

	/* Check the acknowledgement thread handle */
	if(AcknowledgementThreadHandle != NULL)
	{
        /* set thread exit condition */
        g_bAcknowledgementThreadExit = TRUE;

        /* Terminate & close handle of Initiate thread */
        if(!Osal_Thread_Terminate(AcknowledgementThreadHandle))
        {
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer:"
			"Acknowledgment thread handle close failed \r\n");
			#endif
            return BACDEL_THREAD_ERROR;
        }
    }
	AcknowledgementThreadHandle = NULL;

	/* close semaphore for acknowledgment thread */
	if(NULL != g_hAckSemaphoreHandle)
	{
		OSAL_Close_Sem(g_hAckSemaphoreHandle);
		g_hAckSemaphoreHandle = NULL;
	}


	/***************** Initiate Queue *********************/

	/* Free Initiate Q Nodes */
	Clear_Initiated_Request();

    /* Close handle of Invoke ID / Token ID Mutex */
    if(g_hIDMtxLockHandle)
	    Osal_Close_Mutex(g_hIDMtxLockHandle);
	g_hIDMtxLockHandle = NULL;

    /* Close handle of Initiate Queue mutex */
    if(m_hInitMtxLockHandle)
	    Osal_Close_Mutex(m_hInitMtxLockHandle);
	m_hInitMtxLockHandle = NULL;

    /* reset initiate queue structure */
	memset(&g_stReqInitiateDataQue, BACNET_ZERO, sizeof(InitiateDataQue_t));


	/***************** Initiate Q Timers *********************/

    if(!OSAL_Delete_Timer_Queue_Timer(g_Initiate_Service_Retry_TimerHandle))
    {
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: Delete_Initiate_Thread: "
		"Retry timer delete failed \r\n");	
		#endif
		return BACDEL_TIMER_ERROR;
	}
		
	g_Initiate_Service_Retry_TimerHandle = NULL;
	
  
	/***************** Segmentation Thread and Timer *********************/

#ifdef SEGMENTATION_SUPPORTED

    if(!OSAL_Delete_Timer_Queue_Timer(g_SegmentationQTimerHandle))
    {
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: Delete_Initiate_Thread: "
		"Segmentation timer delete filed \r\n");	
		#endif
        return BACDEL_TIMER_ERROR;
	}

	g_SegmentationQTimerHandle = NULL;

	/* Check the segmentation thread handle */
	if(SegmentationThreadHandle != NULL)
	{
        /* set thread exit condition */
        g_bSegmentThreadExit = TRUE;
        /* Terminate & close handle of Initiate thread */
        if(!Osal_Thread_Terminate(SegmentationThreadHandle))
        {
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: Stop_VD_Threads_Timers: "
			"VD thread handle close failed \r\n");
			#endif
            return BACDEL_THREAD_ERROR;
        }
    }
	SegmentationThreadHandle = NULL;

	/* close semaphore for segmentation thread */
	if(NULL != g_hSegmentationSemaphoreHandle)
	{
		OSAL_Close_Sem(g_hSegmentationSemaphoreHandle);
		g_hSegmentationSemaphoreHandle = NULL;
	}
#endif /* SEGMENTATION_SUPPORTED */


	/***************** Other Timers *********************/
//<123		
if(!OSAL_Delete_Timer_Queue_Timer(g_ClearInitiateQTimerHandle))
{
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: Initiate queue timer deletion: "
	"failed \r\n");
	#endif
	return BACDEL_TIMER_ERROR;
}
g_ClearInitiateQTimerHandle = NULL;	

	if(!OSAL_Delete_Timer_Queue_Timer(g_DvManagementTimerHandle))
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: Device management timer deletion:"
		"failed\r\n");
		#endif
		return BACDEL_TIMER_ERROR;
	}
	g_DvManagementTimerHandle = NULL;

	/* fucntion exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Delete_Initiate_Thread: Exit r\n");
	#endif
    return BACDEL_SUCCESS;
}

/**
*DESCRIPTION
*   This function frees input Invoke ID to use again
*/
void Free_Invoke_ID(uint8_t u8CurrInvokeID)
{
    /* Free Invoke ID, reset Array index */
    if((u8CurrInvokeID > 0) && ( u8CurrInvokeID <= INVOKE_ID_RANGE))
	{
		if(Osal_Wait_Mutex(g_hIDMtxLockHandle, INFINITE) == WAIT_OBJECT_0)
		{
			g_a8Invoke_Id[u8CurrInvokeID-1] = 0;

			/* release mutex */
			Osal_Release_Mutex(g_hIDMtxLockHandle);
			
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: \
            Free_Invoke_ID : Success \r\n");
			#endif
		}
        else
		{
			;
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: \
            Free_Invoke_ID : Failed \r\n");
			#endif
		}
	}
}

/**
*******************************************************************************
* DESCRIPTION
* This function genrates invoke Id for new request.
*
* @return u16NewInvokeId
*
******************************************************************************/
bool Generate_Invoke_ID(uint16_t *pu16InvokeID, BACNET_SERVICES_SUPPORTED eServiceSupport)
{
	/* local variable */
	bool bInvokeIdStatus = false;
    uint16_t u16IDQIndex = 0;
    bool bReStart = false;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Generate_Invoke_ID: Entry \r\n");
	#endif

	/* check input pointer */
	if(NULL == pu16InvokeID)
	{
		/* null pointer */
		#ifdef DEBUG_PRINTF
	    Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
		Generate_Invoke_ID: Null input pointer \r\n");
		#endif
		return bInvokeIdStatus;
	}

	/* set default invoke id */
	/* invoke id 0 is unused in stack */
	*pu16InvokeID = 0;

	/* check service type */
	switch(eServiceSupport)
	{
	/* unconfirmed services */
	case SERVICE_SUPPORTED_I_AM:
	case SERVICE_SUPPORTED_I_HAVE:
	case SERVICE_SUPPORTED_UNCONFIRMED_COV_NOTIFICATION:
	case SERVICE_SUPPORTED_UNCONFIRMED_EVENT_NOTIFICATION:
	case SERVICE_SUPPORTED_UNCONFIRMED_PRIVATE_TRANSFER:
	case SERVICE_SUPPORTED_UNCONFIRMED_TEXT_MESSAGE:
	case SERVICE_SUPPORTED_TIME_SYNCHRONIZATION:
	case SERVICE_SUPPORTED_UTC_TIME_SYNCHRONIZATION:
	case SERVICE_SUPPORTED_WHO_HAS:
	case SERVICE_SUPPORTED_WHO_IS:
	#if (defined BACDEL_SER_DS_WG_A && defined BACDEL_PR14)
	/* service added in PR14 i.e. ANSI/ASHRAE 2012 manual */
	case SERVICE_SUPPORTED_WRITE_GROUP:
	#endif
	{
		/* return true for unconfirmed services */
		bInvokeIdStatus = true;
	}
	break;

	/* confirmed services */
	default:
	{
		/* acquire the mutex */
		if(Osal_Wait_Mutex(g_hIDMtxLockHandle, INFINITE) != WAIT_OBJECT_0)
		{
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: \
			Generate_Invoke_ID : Mutex lock failed \r\n");
			#endif
			break;
		}

		/* Get current Invoke Id as Index for ID queue */
		u16IDQIndex = g_u16Current_Invoke_Id;

		/* Increment Index count to get next free index */
		if(u16IDQIndex >= INVOKE_ID_RANGE )
			u16IDQIndex = 0;

		/* Start search from current ID, Reset after 255 */
		for( ; u16IDQIndex < INVOKE_ID_RANGE; u16IDQIndex++ )
		{
			if( 0 == g_a8Invoke_Id[u16IDQIndex])
			{
				/* Found free ID set it as used */
				g_a8Invoke_Id[u16IDQIndex] = 1;
	            
				/* ID_Array_Range = 0-254, InvokeId_Range = 1-255 */
				g_u16Current_Invoke_Id = u16IDQIndex +1;
	            
				/* save invoke id */
				*pu16InvokeID = g_u16Current_Invoke_Id;
				bInvokeIdStatus = true;
				break;
			}

			/* Re-Start searching from 0th index, ONLY ONCE */
			if(((u16IDQIndex+1) >= INVOKE_ID_RANGE) && (false == bReStart) )
			{
				u16IDQIndex = 0;    // Reset Index at 255
				bReStart = true;
			}
		}

		/* release the mutex */
		Osal_Release_Mutex(g_hIDMtxLockHandle);
	}
	break;
	}// End of switch

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Generate_Invoke_ID: Exit \r\n");
	#endif
	/* return value */
	return bInvokeIdStatus;
}



/**
*
* DESCRIPTION
* This function genrates Token Id for new request.
*
* @returns valid or invalid token id no.
*
*/
uint32_t Generate_Token_ID()
{
    /* local varaibles */
    uint32_t u32NewTokenId = 0;

    /* function entry */
	#ifdef DEBUG_PRINTF
   	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Generate_Token_ID: Entry \r\n");
	#endif

    if(Osal_Wait_Mutex(g_hIDMtxLockHandle, INFINITE) != WAIT_OBJECT_0)
	{
        /* error acquiring mutex lock */
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: Generate_Token_ID : \
        Osal_Wait_Mutex failed. \r\n");
		#endif

        /* return invalid token id */
		return(INVALID_TOKEN_ID);
	}

    /* get the current value of token id */
    u32NewTokenId = g_u32Current_Token_Id;

    /* increment new token id */
    u32NewTokenId++;
    if(u32NewTokenId >= INVALID_TOKEN_ID)
        u32NewTokenId = 1;

    /* save the new id */
    g_u32Current_Token_Id = (uint32_t)u32NewTokenId;

	/* release the mutex */
	Osal_Release_Mutex(g_hIDMtxLockHandle);

    /* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Generate_Token_ID: Exit \r\n");
	#endif

    /* return invalid token id value */
    return(u32NewTokenId);
}


/**
*
*DESCRIPTION
*   This function copies the data to the structure to be passed to A-Side
*   Interface. Based on the data passing mechanism either a call back 
*   fucntion pointer is called or the response is stored in pstInitiateInfoQ. 
*
*@param pstRspData [in] Pointer to structure form servicelayer which has
*                       stored the received data.
*@param pstInitiateInfoQ [out] Pointer to structure which is going to take the
*                              data to A-Side Interface.
*
*@return VOID
*
*/
void Data_Exchange_A_Side(processInfo_t* pstRspData,
                          InitiateInfo_t *pstInitiateInfoQ)
{
	/* local variable */
    bacnetRequestData_t *pstReqParam = NULL;
    
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Data_Exchange_A_Side: Entry \r\n");
	#endif
	
	if(NULL == pstRspData || NULL == pstInitiateInfoQ)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: Data_Exchange_A_Side: null input pointers \r\n");
		#endif
	    return;
	}

    pstReqParam = &(pstInitiateInfoQ->m_ReqProcInfo.m_stProcessData);

    /* Fill in the IP Argument structure to pass to Application callback */
    Fill_BacnetIP_Arguments(pstInitiateInfoQ, pstRspData, 
            &(pstInitiateInfoQ->m_stIpArgs));

    /* Free Invoke ID here, Free Token ID in Clear_Initiate_Queue() */
    Free_Invoke_ID(pstReqParam->m_stAPDU.m_u8InvokeId);

    pstReqParam->m_stAPDU.m_u8InvokeId = 0;
	/* Make status STATE_RESPONSE_RECEIVED, as response received */
	pstInitiateInfoQ->m_ReqProcInfo.m_stProcessData.m_i32ClearIQTimer =
		g_stStackConfigParams.m_u32ClearInitQTimeout;
	pstInitiateInfoQ->m_eServiceState = STATE_RESPONSE_RECEIVED;

    /* If response received and callback function is registered */
    if(NULL != pstInitiateInfoQ->pvCallBackFunc &&
        ( pstInitiateInfoQ->m_eServiceState == STATE_RESPONSE_RECEIVED))
    {
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer: Data_Exchange_A_Side: "
						"Call Application CallBack Function\r\n");
		#endif

         /* Inform to the Application(Caller) about response */
        pstInitiateInfoQ->pvCallBackFunc(&(pstInitiateInfoQ->m_stIpArgs), 
            STATE_RESPONSE_RECEIVED); 

        /* Clear the Memory for request */
        Clear_InitiateQ_State(&pstInitiateInfoQ);
    }
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Data_Exchange_A_Side: Exit \r\n");
	#endif
}

/**
*
*DESCRIPTION
*   Traverse the Initiate Info Queue to find Queue index having given state, 
*   invoke id, etc.
*
*@param eFindBy [in] type of search.
*@param eStatus [in] state to be found.
*@param u8InvokeId [in] invoke id to be found.
*@param pstInitiateQ [in] base pointer for search.
*@param u32TokenID [in] token id to be found.
*@param pstRespRmDevAddr [in] address of device to be found.
*
*@return pointer of 1st initiate Q member with given state or DATA_NOT_AVALIABLE.
*
*/
InitiateInfo_t *Traverse_InitiateInfo_Queue(
    FIND_IN_INITIATE_Q eFindBy, 
    INITIATE_SERVICE_STATE eStatus, uint8_t u8InvokeId, 
    InitiateInfo_t *pstInitiateQ,
	uint32_t u32TokenID,
	BACnetAddress_t *pstRespRmDevAddr)
{
    /* local varaibles */
    InitiateInfo_t *pstInitiateInfoQ = NULL;
    InitiateInfo_t *pstReturnPtr = NULL;
    bool bFlag = false;

    /* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Traverse_InitiateInfo_Queue: Entry \r\n");
	#endif

    /* validate & update (if required) input values */
    if(FIND_STATE_NEXT_TO == eFindBy && NULL == pstInitiateQ)
        eFindBy = FIND_STATE;
    if(FIND_INVOKE_ID == eFindBy && 0 == u8InvokeId)
        return NULL;

    /* wait for mutex */
    Osal_Wait_Mutex(m_hInitMtxLockHandle, INFINITE);

    /* get 1st node */
    pstInitiateInfoQ = g_stReqInitiateDataQue.m_pstInitiateInfoQ;

    switch(eFindBy)
    {
        /* find by state */
    case FIND_STATE:
        /* traverse the list to find given state */
        while(NULL != pstInitiateInfoQ)
        {
            /* check the state */
            if(eStatus == pstInitiateInfoQ->m_eServiceState)
            {
				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer: Traverse_InitiateInfo_Queue: \
                State match found. \r\n");
				#endif
                /* return pointer */
                pstReturnPtr = pstInitiateInfoQ;
                break;
            }
            else
                /* move to next node */
                pstInitiateInfoQ = pstInitiateInfoQ->pstNextAddress;
        }
        break;

        /* find after given base pointer */
    case FIND_STATE_NEXT_TO:
        /* traverse the list to find given state after given node */
        while(NULL != pstInitiateInfoQ)
        {
            if(pstInitiateQ == pstInitiateInfoQ && !bFlag)
                bFlag = true;
            else if(eStatus == pstInitiateInfoQ->m_eServiceState)
            {
				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer: Traverse_InitiateInfo_Queue: \
                State next to match found. \r\n");
				#endif
                /* return pointer */
                pstReturnPtr = pstInitiateInfoQ;
                break;
            }
            else
                /* move to next node */
                pstInitiateInfoQ = pstInitiateInfoQ->pstNextAddress;
        }
        break;

        /* find by invoke id */
    case FIND_INVOKE_ID:
        /* traverse the list to find given invoke id */
        while(NULL != pstInitiateInfoQ)
        {
			if((u8InvokeId == pstInitiateInfoQ->m_ReqProcInfo.m_stProcessData.
				m_stAPDU.m_u8InvokeId)
				)
			{
				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer: Traverse_InitiateInfo_Queue: \
                Invoke Id match found. \r\n");
				#endif
                /* return pointer */
                pstReturnPtr = pstInitiateInfoQ;
                break;
            }
            else
                /* move to next node */
                pstInitiateInfoQ = pstInitiateInfoQ->pstNextAddress;
        }
        break;

		/* find by token id */
	case FIND_TOKEN_ID:
		/* traverse the list to find given token id */
        while(NULL != pstInitiateInfoQ)
        {
            /* check the state */
            if(u32TokenID == pstInitiateInfoQ->u32TokenID)
            {
				#ifdef DEBUG_PRINTF
                Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer: Traverse_InitiateInfo_Queue: \
                Token Id match found. \r\n");
				#endif
                /* return pointer */
                pstReturnPtr = pstInitiateInfoQ;
                break;
            }
            else
                /* move to next node */
                pstInitiateInfoQ = pstInitiateInfoQ->pstNextAddress;
        }
		break;

        /* default case - do nothing */
    default:
        break;
    }// switch ends.

    /* release the mutex */
    Osal_Release_Mutex(m_hInitMtxLockHandle);

    /* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Traverse_InitiateInfo_Queue: Exit \r\n");
	#endif
    /* return pointer */
    return pstReturnPtr;
}


/**
*
* DESCRIPTION
* Acknowledgement thread.
* This thread process all ack's except seg ack.
*
* @param  [in]  void.
* @return [out] nothing.
*
*/
//void Acknowledgement_Thread(void* lpThreadParameter)
void Acknowledgement_Thread_Task(void)
{
    /* local variables */
	DWORD dwStatus = 0;
    //virtualDevData_t* pstVirtualDev = NULL;
	InitiateInfo_t *pstInitiateQIndex = NULL;
	processInfo_t* pstRspData = NULL;
	bacnetRequestData_t *pstReqParam = NULL;
    bool bIsValidResponse = FALSE;

	/* thread entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Acknowledgement_Thread: Entry \r\n");
	#endif

	/* thread routine */
    while(1)
    {
		/* reset pointers & set default values */
		//pstVirtualDev = NULL;
		pstInitiateQIndex = NULL;
		pstRspData = NULL;
		bIsValidResponse = FALSE;

        /* wait for infinite, until the semaphore is released */
		dwStatus = OSAL_Wait_Sem(g_hAckSemaphoreHandle, INFINITE);

		/* check if the thread for correctly signalled or not */
		if(WAIT_OBJECT_0 != dwStatus) 
		{
			/* handle the error */
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL1, "BACnetStackAppLayer: \
			Acknowledgement_Thread: semaphore signal error %ld \r\n", dwStatus);
			#endif
			/* wait for next thread trigger */
			continue;
		}

		/* get pointer to response data */
        pstRspData = g_stReqInitiateDataQue.m_pstRspQueue[g_stReqInitiateDataQue.
            m_i32RspQIndex];

		/* check pointers */
		if(NULL == pstRspData)
		{
			/* handle the error */
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL1, "BACnetStackAppLayer: \
			Acknowledgement_Thread: null pointers \r\n");
			#endif
			continue;
		}

        /* find initiated request with given Invoke id */
    	pstInitiateQIndex = Traverse_InitiateInfo_Queue(
			FIND_INVOKE_ID, 0, 
            pstRspData->m_stProcessData.m_stAPDU.m_u8InvokeId, 
			NULL, 0, &pstRspData->m_stProcessData.m_stRmDvAddr);

        /* fill response and notify application */
        if(NULL != pstInitiateQIndex)
        {
			/* set the in use flag for acknowledgement thread */
			BIT_SET(pstInitiateQIndex->m_u8NodeInUse, INITIATE_Q_ACK_THREAD_BIT_NO);

            /* get pointer to request parameters */
            pstReqParam = &pstInitiateQIndex->m_ReqProcInfo.m_stProcessData;

			/* compare the remote device address */
			if(!memcmp(&pstRspData->m_stProcessData.m_stRmDvAddr, 
				&pstReqParam->m_stRmDvAddr, sizeof(BACnetAddress_t)))
			{
				/* compare service choice */
				if(PDU_TYPE_SIMPLE_ACK == pstRspData->m_stProcessData.m_ePDUType ||
					PDU_TYPE_COMPLEX_ACK == pstRspData->m_stProcessData.m_ePDUType ||
					PDU_TYPE_ERROR == pstRspData->m_stProcessData.m_ePDUType)
				{
					/* verify service choice */
					if(pstRspData->m_stProcessData.m_eServiceSupported == 
						pstReqParam->m_eServiceSupported)
					{
						/* set the flag */
						bIsValidResponse = TRUE;
					}
				}
				else if(PDU_TYPE_ABORT == pstRspData->m_stProcessData.m_ePDUType ||
					PDU_TYPE_REJECT == pstRspData->m_stProcessData.m_ePDUType)
				{
					/* abort and reject pdu do not contain service choice */
					/* set the flag */
					bIsValidResponse = TRUE;
					/* save the service type */
					pstRspData->m_stProcessData.m_eServiceSupported = 
						pstReqParam->m_eServiceSupported;
				}
			}

			if(bIsValidResponse)
			{
				/* fill data & callback to application */
				Data_Exchange_A_Side(pstRspData, pstInitiateQIndex);
			}

			/* clear the in use flag for acknowledgement thread */
			BIT_CLEAR(pstInitiateQIndex->m_u8NodeInUse, INITIATE_Q_ACK_THREAD_BIT_NO);
        }

        /* Clear Response Process Info from Process queue */
        Clear_Process_Info(pstRspData);

        /* Response Is Dynamic in case of VD Request is handled internally */
        if(TRUE == pstRspData->m_bIsDynamic)
        {
            OSAL_Free(pstRspData, __FILE__, __FUNCTION__, __LINE__);
        }
        else
        {
            /* Reintialize the parametres of the structure */
            memset(pstRspData, 0, sizeof(processInfo_t));

            /* Set State of Process Info data as PROC_INIT. Used in ProcessQueue 
               element to Reuse for received NW data, Not much useful for Initiate
               Queue as Initiate Queue uses other State param */
            pstRspData->m_eState = PROC_INIT;
        }

        /* Delete entry from response queue */
        g_stReqInitiateDataQue.m_pstRspQueue
            [g_stReqInitiateDataQue.m_i32RspQIndex] = NULL;

        /* Increment m_i32RspQIndex to point to next location */
        g_stReqInitiateDataQue.m_i32RspQIndex++;
        if(MAX_INITIATE_SERVICES ==  g_stReqInitiateDataQue.m_i32RspQIndex)
		{
            g_stReqInitiateDataQue.m_i32RspQIndex = 0;
		}

    }// end of while

	/* thread exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Acknowledgement_Thread: Exit \r\n");
	#endif
    return;
}
