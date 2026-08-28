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
*   FILE bacnetInitiateServiceMgmt.c
*                                                                      
*   AUTHORS                                                                             
*	 Harshal Mangale
*                                                                         
*   DESCRIPTION                                                            
*                                                                                
*   RELEASE HISTORY
*	DATE            NAME				DESCRIPTION
*   05/08/2011      Harshal Mangale     File Created
*   11/08/2011      Harshal Mangale     Implemented InitiateTherad(),
*                                       Start_Initiate_Thread(),
*                                       Initiate_Service_Interface(),
*                                       BACDEL_Generate_Aside_Request( 0, ), 
*                                       Generate_Invoke_ID().
*   18/08/2011      Harshal Mangale     Implemented Initiate_Service_Retry_Timer
*   19/08/2011      Harshal Mangale     Added WP-A Service in BACDEL_Generate_Aside_Request
*   15/09/2011      M.Venu              Added BroadcastType paramter to 
*                                       GenerateAsideRequest function to handle local 
*                                       and global broadcast requests
*   16/09/2011      M.Venu              Added BroadcastToNetwork and Forwarded-NPDU 
*                                       generate request functionalities
*   16/09/2011      Harshal M.          Added function for Traversing Initiate 
*                                       queue for Given Status
*   20/09/2011      Ashish Verma        Add functionality of processing UNICAST
*                                       request.
*   23/09/2011      Ashish Verma        Modified BACNET_CHARACTER_STRING
*                                       structure to Pr_BACnetCharStr_t
*   29/09/2011      Ashish Verma        Added fucntionality of sending I_AM. 
*   05/10/2011      Ashish Verma        Replaced Argument_List_t with 
*                                       bacnetip_arguments_t.
*   11/10/2011      Ashish Verma        Modified BACDEL_Generate_Aside_Request( 0, )
*   12/10/2011      Ashish Verma        Added fucntions Generate_Token_ID(),
*                                       BACDEL_Set_Default_Parameters().
*   13/10/2011      Ashish Verma        Modified fucntion BACDEL_Set_Default_Parameters,
*                                       added fucntion Validate_Received_Request.
*                                       modified fucntion BACDEL_Generate_Aside_Request. 
*	14/10/2011	    Prashant Badgujar   Adding Decode_A_Side_Data API.
*   18/10/2011      Prashant Badgujar   Adding Convert_AppTag_To_String, Decode_A_Side_Data,
*                                       Free_Argument_Memory APIs.                             
*
******************************************************************************/

/** header files */
#include "osalFreeRTOS.h"
#include "bacnetInitiateServiceMgmt.h"
#include "bacnetAPDUHandler.h"
#include "bacnetStackMgmt.h"
#include "bacDELDef.h"
#include "propertyGenricHandler.h"
#include "propertydef.h"
#include "pduDataEncodeDecode.h"
#include "miscMiscellaneous.h"
#include "propertyTagType.h"
#include "pduAbort.h"
#include "pduReject.h"
#include "propertyValueRead.h"
#include "propertyValueWrite.h"
#include "propertyClearValues.h"
#include "pduEncodeDecodeDT.h"


/* to include segmentation functionality */
#ifdef SEGMENTATION_SUPPORTED
#include "SegmentationGenericHandler.h"
#endif

/** stack configuration parameters */
extern BACApp_StackConfig_t g_stStackConfigParams;

extern osMutexId_t m_hInitMtxLockHandle;
extern Sem_H g_hAckSemaphoreHandle;
extern DB_t SMCfg;

/** function to convert DT error code into BACnet error codes. */
void Convert_DT_Error_To_Error_Code(
	BACNET_ERROR_CODE *peErrCode, 
	BACNET_ERROR_CLASS *peErrClass,
	int32_t i32DtErrorCode)
{
	/* check if error coded is less than zero */
	if(i32DtErrorCode >= 0)
		return;

	/* check input pointers */
	if(NULL == peErrClass || NULL == peErrCode)
		return;

	/* fil error code & class depending on Dt error code */
	switch(i32DtErrorCode)
	{
	case DT_ERR_INVALID_TAG:
	case DT_ERR_INVALID_TAG_LENGTH:
		//*peErrClass = ERROR_CLASS_PROPERTY;
		//*peErrCode = ERROR_CODE_INVALID_TAG;
		//break;

	case DT_ERR_INVALID_DATA_TYPE:
		*peErrClass = ERROR_CLASS_PROPERTY;
		*peErrCode = ERROR_CODE_INVALID_DATA_TYPE;
		break;

	case DT_ERR_DATA_TYPE_NOT_SUPPORTED:
		*peErrClass = ERROR_CLASS_PROPERTY;
		*peErrCode = ERROR_CODE_DATATYPE_NOT_SUPPORTED;
		break;

	case DT_ERR_VALUE_OUT_OF_RANGE:
		*peErrClass = ERROR_CLASS_PROPERTY;
		*peErrCode = ERROR_CODE_VALUE_OUT_OF_RANGE;
		break;

	case DT_ERR_MALLOC_FAILED:
		*peErrClass = ERROR_CLASS_RESOURCES;
		*peErrCode = ERROR_CODE_OUT_OF_MEMORY;
		break;

	case DT_ERR_INVALID_INPUTS:
	default:
		*peErrClass = ERROR_CLASS_DEVICE;
		*peErrCode = ERROR_CODE_INTERNAL_ERROR;
		break;
	}
}

/**
*******************************************************************************
*DESCRIPTION
*   This API is called by Clear_InitiateQ API to release the memory for Multiple type of 
*   requests. For e.g RPM, WPM.
*   
*@param pstMPSData [in] Pointer to second element of link list. 
*                       (Note- First element is static memory allocation).
*@param i8ServiceSupported [in] Service Choice for A-side request.

*return None.
*
******************************************************************************/
void Free_Argument_Memory(void *pstMPSData, BACNET_SERVICES_SUPPORTED  eServiceSupported)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Free_Argument_Memory: Entry \r\n");
	#endif

    /* Check the service type for traversing link list of appropriate type */
    switch(eServiceSupported)
    {
        case SERVICE_SUPPORTED_READ_PROP_MULTIPLE:
        {
            rpm_response_t   *pstRPMStart = NULL;
            rpm_response_t   *pstTmpRPMData = NULL;

            /* Get the second element of link list */
            pstRPMStart = (rpm_response_t *)pstMPSData;

            /* Traverse till the last element in the list */
            while(pstRPMStart != NULL)
            {
                /* Move to next element */
                pstTmpRPMData = pstRPMStart->m_pstRPMNextElem;

				#ifdef NEW_RP_WP_INTERFACE
				/* clear allocated memories */
				Clear_PropVal_AsPer_DataType(pstRPMStart->m_eDataType,
					&pstRPMStart->m_pvPropVal);
				#endif

                /* Release the Current element */
                OSAL_Free(pstRPMStart,  __FILE__, __FUNCTION__, __LINE__);

                /* Assign next element to currnet element */
                pstRPMStart = pstTmpRPMData;
            }
        }
        break;
        default:
        {
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL1, "BACnetStackAppLayer: Free_Argument_Memory : \
                Service is not supported\r\n");
			#endif
        }
        break;
    }
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Free_Argument_Memory: Exit \r\n");
	#endif
}


/**
*Description
*   Change state of Initiate queue to STATE_ERROR to delete Entry from list
*/
void Clear_InitiateQ_State(InitiateInfo_t **dpstInitiateQ)
{
	if(NULL == dpstInitiateQ || NULL == *dpstInitiateQ)
	{
		return;
	}
    Free_Invoke_ID((*dpstInitiateQ)->m_ReqProcInfo.m_stProcessData.m_stAPDU.m_u8InvokeId);
    (*dpstInitiateQ)->m_ReqProcInfo.m_stProcessData.m_stAPDU.m_u8InvokeId = 0;

    (*dpstInitiateQ)->u32TokenID = 0;

    (*dpstInitiateQ)->m_eServiceState = STATE_ERROR;
}


/**
*
* DESCRIPTION
*   API is used to Clear Initiate Request parameters and IpArguments param,
*   after application reads response parameters.
*   
* @param pstInitiateQ [in] Initiate Q pointer to free its data.
* return Void.
*
*/
void Clear_InitiateQ_Data(InitiateInfo_t *pstInitiateQ)
{
    /* local variables */
	BACNET_SERVICES_SUPPORTED eServiceChoice = MAX_BACNET_SERVICES_SUPPORTED;

    /* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_InitiateQ_Data: Entry \r\n");
	#endif

    /* null check input pointer */
    if(NULL == pstInitiateQ)
        return;

    /* get the service type */
    eServiceChoice = pstInitiateQ->m_ReqProcInfo.m_stProcessData.m_eServiceSupported;

    /* switch as per service choice */
    switch(eServiceChoice)
    {

#ifdef BACDEL_SER_DM_DDB_A
        case SERVICE_SUPPORTED_WHO_IS:
        {
            ddb_i_am_t *pstIAM = NULL;
            ddb_i_am_t *pstIAMNxt = NULL;

            pstIAM = pstInitiateQ->m_stIpArgs.m_stNPDUData.m_stAPDUData.
                m_stServiceChoice.m_stI_AM_Request.pstNext;
            while(pstIAM != NULL)
            {
                pstIAMNxt = pstIAM->pstNext;
                /* Free the Memory allocated to the Pointer */
                OSAL_Free(pstIAM,  __FILE__, __FUNCTION__, __LINE__);
                pstIAM = pstIAMNxt;
            }
            if(pstInitiateQ->m_ReqProcInfo.m_stProcessData.m_stAPDU.
                m_pvServiceRequestData != NULL)
            {
                OSAL_Free(pstInitiateQ->m_ReqProcInfo.m_stProcessData.
                    m_stAPDU.m_pvServiceRequestData, __FILE__,
                    __FUNCTION__, __LINE__);
            }
        }
        break;
#endif /* BACDEL_SER_DM_DDB_A */

#if (defined BACDEL_SER_DS_COV_B ||defined BACDEL_SER_DS_COV_A || defined BACDEL_SER_DS_COVP_A)

        case SERVICE_SUPPORTED_UNCONFIRMED_COV_NOTIFICATION:
        case SERVICE_SUPPORTED_CONFIRMED_COV_NOTIFICATION:
        {
            ListOfBACnetCovSubs_t *pstCOVSubscribe = NULL;

            /* Extract the service structure */
            pstCOVSubscribe = pstInitiateQ->m_ReqProcInfo.m_stProcessData.
                m_stAPDU.m_pvServiceRequestData;

            /* validate that pointer is not null */
            if(pstCOVSubscribe != NULL)
            {
                /* free the subscription / notification data */
                Free_COV_Notification_Data(pstCOVSubscribe);
            }
            pstInitiateQ->m_ReqProcInfo.m_stProcessData.m_stAPDU.
                    m_pvServiceRequestData = NULL;
        }
        break;
#endif /* (defined BACDEL_SER_DS_COV_A || defined BACDEL_SER_DS_COVP_A) */

#ifdef BACDEL_SER_AE_EN_B
		case SERVICE_SUPPORTED_UNCONFIRMED_EVENT_NOTIFICATION:
        case SERVICE_SUPPORTED_CONFIRMED_EVENT_NOTIFICATION:
		{
			BacnetEnPropElem_t *pstEventNotiParam = NULL;

            /* Extract the service structure */
            pstEventNotiParam = pstInitiateQ->m_ReqProcInfo.m_stProcessData.
                m_stAPDU.m_pvServiceRequestData;

			/* validate that pointer is not null */
			if(NULL != pstEventNotiParam)
			{
				/* free service data pointer now */
				OSAL_Free(pstEventNotiParam, __FILE__, __FUNCTION__, __LINE__);
				pstInitiateQ->m_ReqProcInfo.m_stProcessData.m_stAPDU.
					m_pvServiceRequestData = NULL;
			}
        }
        break;
#endif /* EN-B */

#if (defined BACDEL_SER_DM_DCC_A || \
     defined BACDEL_SER_AE_AA_A || defined BACDEL_SER_DM_OD_A || \
	 defined BACDEL_SER_DM_TS_A || defined BACDEL_SER_DM_UTC_A || \
	 defined BACDEL_SER_DM_DDB_B || defined BACDEL_SER_DM_DOB_B || \
	 defined BACDEL_SER_DS_COV_A)

        case SERVICE_SUPPORTED_DEVICE_COMMUNICATION_CONTROL:
		case SERVICE_SUPPORTED_ACKNOWLEDGE_ALARM:
		case SERVICE_SUPPORTED_DELETE_OBJECT:
		case SERVICE_SUPPORTED_SUBSCRIBE_COV:
		case SERVICE_SUPPORTED_I_AM:
		case SERVICE_SUPPORTED_I_HAVE:
		case SERVICE_SUPPORTED_TIME_SYNCHRONIZATION:
		case SERVICE_SUPPORTED_UTC_TIME_SYNCHRONIZATION:
		{
			/* Free the Memory allocated to the service data */
			if(NULL != pstInitiateQ->m_ReqProcInfo.m_stProcessData.m_stAPDU.
                m_pvServiceRequestData)
			{
				OSAL_Free(pstInitiateQ->m_ReqProcInfo.m_stProcessData.m_stAPDU.
					m_pvServiceRequestData,  __FILE__, __FUNCTION__, __LINE__);
				pstInitiateQ->m_ReqProcInfo.m_stProcessData.m_stAPDU.
					m_pvServiceRequestData = NULL;
			}
		}
        break;
#endif /* #ifdef .... */

       /* deafult case */
        default:
        {
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL1, "BACnetStackAppLayer: Clear_InitiateQ_Data: \
            Service is not supported. \r\n");
			#endif
        }
        break;
    }// switch ends.

    /* free the memory allocated to hold RPM response */
    if(NULL != pstInitiateQ->m_pstRpmIPArgs)
	{
		/* free allocated memory */
		OSAL_Free(pstInitiateQ->m_pstRpmIPArgs, __FILE__,__FUNCTION__,__LINE__);
	}

#ifdef SEGMENTATION_SUPPORTED
	/* if the initiated request holds any data related to segmentation then clear it */
	if(NULL != pstInitiateQ->m_ReqProcInfo.m_pstAPDUSegmentData)
	{
		/* free segmentation data */
		Clear_Segmentation_Data(&pstInitiateQ->m_ReqProcInfo, LINK_LIST_SEGMENTATION_TRANSMIT);
	}
    pstInitiateQ->m_ReqProcInfo.m_pstAPDUSegmentData = NULL;
#endif

    /* free memory allocated to initiate Q pointer */
    OSAL_Free(pstInitiateQ, __FILE__, __FUNCTION__, __LINE__);

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_InitiateQ_Data: Entry \r\n");
	#endif
    return;
}

/** clear all initiated requests */
void Clear_Initiated_Request(void)
{
    /* local varaibles */
    InitiateInfo_t* pstInitiateQ = NULL;
    void *pvTemp = NULL;
	uint32_t u32Count = 0;
	uint32_t u32MaxCount = 0;
	bool bForceDelete = FALSE;

    /* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Clear_Initiated_Request: Entry \r\n");
	#endif

    /* wait for mutex */
    Osal_Wait_Mutex(m_hInitMtxLockHandle, INFINITE);

    /* get the pointer for 1st request */
    pstInitiateQ = g_stReqInitiateDataQue.m_pstInitiateInfoQ;

    /* clear the list pointers */
    g_stReqInitiateDataQue.m_pstInitiateInfoQ = NULL;
    g_stReqInitiateDataQue.m_pstInitiateInfoQ_End_Address = NULL;

	/* reset node count to 0 */
	g_stReqInitiateDataQue.m_i32InitiateQFillIndex = 0;

    /* release mutex */
	Osal_Release_Mutex(m_hInitMtxLockHandle);

	/* get the timeout value and convert to tick counts */
	/* divide by 10 as 10 millisecond delay is used */
	u32MaxCount = g_stStackConfigParams.m_u32ClearInitReqTimeout/10;

    /* traverse Initiate queue list to remove all requests */
    while(pstInitiateQ != NULL)
    {
        /* save next pointer */
        pvTemp = pstInitiateQ->pstNextAddress;
        pstInitiateQ->pstNextAddress = NULL;

		/* reset counter */
		u32Count = 0;

		while(1)
		{
			/* check if node is in use by other threads */
			if(!pstInitiateQ->m_u8NodeInUse || bForceDelete)
			{
				/* clear initiate Q state */
				Clear_InitiateQ_State(&pstInitiateQ);
				/* clear the request data */
				Clear_InitiateQ_Data(pstInitiateQ);

				/* reset flag & break the loop */
				bForceDelete = FALSE;
				break;
			}

			/* increment count value */
			u32Count ++;
			if(u32MaxCount == u32Count)
			{
				/* 10000 ms delay exceeded, force delete & exit loop */
				bForceDelete = TRUE;
			}
			else
			{
				/* wait for some time */
				Osal_Sleep(10);
			}
		}

        /* move to next request */
        pstInitiateQ = pvTemp;
    }

    /* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Clear_Initiated_Request: Exit \r\n");
	#endif
}

/**
*******************************************************************************
*DESCRIPTION
*   This API fills in the bacnetip_arguments_t structure with Response parameters.
*
*@param pstRspQueue [in] This input parameter contains the Response packet.
*@param pstIpArguments [out] This output parameter contains Response packet information
*                            after execution of this API.
*
******************************************************************************/
void Fill_BacnetIP_Arguments(
	InitiateInfo_t *pstInitQData, 
	processInfo_t *pstRspQueue , 
	bacnetip_arguments_t *pstIpArguments)
{
    /* local variables */
	BACNET_SERVICES_SUPPORTED eServicesSupported = 0;
	BACNET_SERVICES_SUPPORTED eServiceChoice = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Fill_BacnetIP_Arguments: Entry \r\n");
	#endif

	/* check input parameters */
	if(NULL == pstInitQData || NULL == pstRspQueue || NULL == pstIpArguments)
	{
		/* null input pointers */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: Fill_BacnetIP_Arguments: \
		Null Input Pointers \r\n");
		#endif
		return;
	}
    /* Fill in the  BVLC function Type */
	pstIpArguments->m_eBvlcFunctionType = 
        pstRspQueue->m_stProcessData.m_eBVLCFunctionType;

    /* Copy the destination address information */
    memcpy(&(pstIpArguments->m_stDestBACnetAddr), 
        &(pstRspQueue->m_stProcessData.m_stRmDvAddr), sizeof(BACnetAddress_t));

    /* Fill Pdu type */
	pstIpArguments->m_stNPDUData.m_stAPDUData.m_ePduType =
        pstRspQueue->m_stProcessData.m_ePDUType;

	/* save token id */
    pstIpArguments->m_i32TokenID = pstInitQData->u32TokenID;

	/* save the actual service supported value */
	eServiceChoice = pstInitQData->m_ReqProcInfo.m_stProcessData.m_eServiceSupported;

    /* To accomodate all the Service types under single code section */
    eServicesSupported = Map_With_Services_Supported(
        pstRspQueue->m_stProcessData.m_stAPDU.m_u8ServiceChoice,
        pstRspQueue->m_stProcessData.m_ePDUType);

    /* fill data as per service choice */
    switch(eServicesSupported)
    {

#ifdef BACDEL_SER_DM_DDB_A
        case SERVICE_SUPPORTED_I_AM:
            /* This case will never get executed. */
            break;
#endif

#ifdef BACDEL_SER_AE_EN_B
		case SERVICE_SUPPORTED_UNCONFIRMED_EVENT_NOTIFICATION:
        case SERVICE_SUPPORTED_CONFIRMED_EVENT_NOTIFICATION:
			/* no data to fill as simple ack is received */
			/* free service data, it is curently freed in clear_initiateQ */
			break;
#endif

        case MAX_BACNET_SERVICES_SUPPORTED:
        {
            /* Handling for Error, Abort and Reject PDU */
			/* update the error from client or server flag */
			pstIpArguments->m_stNPDUData.m_stAPDUData.m_bErrorFromServer = 
				pstRspQueue->m_stProcessData.m_stAPDU.m_bServer;

			/* dummy if statement */
			if(0)
			{
				;//dummy code
			}

            else
            {
                pstIpArguments->m_stNPDUData.m_stAPDUData.m_eServiceSupport = 
                    MAX_BACNET_SERVICES_SUPPORTED;
                
				/* save error response */
				if(PDU_TYPE_ERROR == pstRspQueue->m_stProcessData.m_ePDUType)
				{
					/* save error class, code and first failed element */
					pstIpArguments->m_stNPDUData.m_stAPDUData.m_stServiceChoice.m_stErrorResponse.m_eErrorClass = 
						pstRspQueue->m_stProcessData.m_stAPDU.m_u32ErrorClass;
					pstIpArguments->m_stNPDUData.m_stAPDUData.m_stServiceChoice.m_stErrorResponse.m_eErrorCode = 
						pstRspQueue->m_stProcessData.m_stAPDU.m_u32ErrorCode;
					pstIpArguments->m_stNPDUData.m_stAPDUData.m_stServiceChoice.m_stErrorResponse.m_i32FirstFailedNo = 
						pstRspQueue->m_stProcessData.m_stAPDU.m_u32FirstFailedElementNo;
				}
                /* save abort response */
                else if(PDU_TYPE_ABORT == pstRspQueue->m_stProcessData.m_ePDUType)
				{
					pstIpArguments->m_stNPDUData.m_stAPDUData.m_stServiceChoice.m_stAbortResponse.m_eAbortReason =  
						pstRspQueue->m_stProcessData.m_stAPDU.m_u32ErrorCode;
				}
				/* save reject response */
                else if(PDU_TYPE_REJECT == pstRspQueue->m_stProcessData.m_ePDUType)
				{
					pstIpArguments->m_stNPDUData.m_stAPDUData.m_stServiceChoice.m_stRejectResponse.m_eRejectReason = 
						pstRspQueue->m_stProcessData.m_stAPDU.m_u32ErrorCode;
				}
                else
				{
					;/* do nothing */
				}
            }
        }
        break;

        default:
        {
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL1, "BACnetStackAppLayer: Fill_BacnetIP_Arguments : \
                Service is not supported\r\n");
			#endif
        }
        break;
    }

	/* restore the actual service supported value */
	pstIpArguments->m_stNPDUData.m_stAPDUData.m_eServiceSupport = eServiceChoice;

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Fill_BacnetIP_Arguments: Exit \r\n");
	#endif
}

/**
*
* DESCRIPTION
* Function to clear initiate queue nodes. 
* This function will:
* - remove all nodes with state "error" immediately.
* - remove all nodes with state "response-received" and "timeout" after
*   predefined timeout.
*
* @param  [in]  No input parameters
* @return [out] No output parameters
*
*/
void Clear_InitiateQ(void)
{
	/* local variables */
	InitiateInfo_t *pstInitiateQ = NULL;
	InitiateInfo_t *pstPreviousQ = NULL;
	InitiateInfo_t *pstNext = NULL;
	InitiateInfo_t *pstBaseNode = NULL;
	InitiateInfo_t *pstNewList = NULL;
	BACNET_SERVICES_SUPPORTED eServicesSupported = MAX_BACNET_SERVICES_SUPPORTED;
	bool bFirstNode = true;
	bool bRemoveNode = false;

	/* entry message */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: ClearIQTimer: Entry \r\n");
	#endif

	/* acquire the mutex */
    if(Osal_Wait_Mutex(m_hInitMtxLockHandle, INFINITE) != WAIT_OBJECT_0)
	{
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: \
		ClearIQTimer: Mutex lock failed \r\n");
		#endif
        return;
	}

    /* get 1st node from initiate queue */
    pstInitiateQ = g_stReqInitiateDataQue.m_pstInitiateInfoQ;

	/* traverse list 1 by 1 */
	while(NULL != pstInitiateQ)
	{
		/* reset flag to remove node */
		bRemoveNode = false;

		/* save next */
		pstNext = pstInitiateQ->pstNextAddress;

		/* check state */
		/* wait till counter becomes zero */
		if((pstInitiateQ->m_eServiceState == STATE_RESPONSE_RECEIVED) 
			|| 
			(pstInitiateQ->m_eServiceState == STATE_TIMEOUT)
			)
		{
			/* decrement count by 1 */
			pstInitiateQ->m_ReqProcInfo.m_stProcessData.m_i32ClearIQTimer--;

			/* delete node if count becomes 0 */
			if( 0 >= pstInitiateQ->m_ReqProcInfo.m_stProcessData.m_i32ClearIQTimer)
			{
				/* get service type */
				eServicesSupported = pstInitiateQ->m_ReqProcInfo.
					m_stProcessData.m_eServiceSupported;

				#if ((defined BACDEL_SER_DM_DDB_A) && (defined CALLBACK_FOR_WHO_IS_A))
				if(eServicesSupported == SERVICE_SUPPORTED_WHO_IS)
				{
					if(pstInitiateQ->pvCallBackFunc != NULL)
					{
						Copy_DDB_A_Data(pstInitiateQ);
						/* Inform to the Application(Caller) about response */
						pstInitiateQ->pvCallBackFunc(&(pstInitiateQ->
							m_stIpArgs), STATE_RESPONSE_RECEIVED);
					}
				}
				#endif /* BACDEL_SER_DM_DDB_A */

				/* remove node from list */
				bRemoveNode = true;
			}
		}
		/* remove imediately only if error state and node-in-use flag is zero */
		else if (STATE_ERROR == pstInitiateQ->m_eServiceState && 
			!pstInitiateQ->m_u8NodeInUse)
		{
			/* remove node from list */
			bRemoveNode = true;
		}
		else
		{
			; /* do nothing */
		}

		/* remove node from list */
		if(bRemoveNode)
		{
			/* reset flag */
			bRemoveNode = false;

			/* as node is deleted from queue decrement count by 1 */
			g_stReqInitiateDataQue.m_i32InitiateQFillIndex--;

			/* check the position of node in list */
			if(NULL == pstPreviousQ && NULL == pstNext)
			{
				/* only 1 node in list */
				g_stReqInitiateDataQue.m_pstInitiateInfoQ = NULL;
				g_stReqInitiateDataQue.m_pstInitiateInfoQ_End_Address = NULL;
			}
			else if(NULL == pstPreviousQ)
			{
				/* 1st node in list */
				g_stReqInitiateDataQue.m_pstInitiateInfoQ = pstNext;
			}
			else if(NULL == pstNext)
			{
				/* last node in list */
				pstPreviousQ->pstNextAddress = NULL;
				g_stReqInitiateDataQue.m_pstInitiateInfoQ_End_Address = pstPreviousQ;
			}
			else
			{
				/* middle node in list */
				pstPreviousQ->pstNextAddress = pstNext;
			}

			/* create list of nodes to be deleted */
			if(bFirstNode)
			{
				/* save base node of new list */
				bFirstNode = false;
				pstBaseNode = pstInitiateQ;
				pstNewList = pstInitiateQ;
			}
			else
			{
				/* attach at the end of new list */
				pstNewList->pstNextAddress = pstInitiateQ;
				pstNewList = pstNewList->pstNextAddress;
			}

			/* update pointers */
			pstInitiateQ->pstNextAddress = NULL;
			pstInitiateQ = pstPreviousQ;
		}

		/* update previous pointer */
		pstPreviousQ = pstInitiateQ;

		/* move to next node */
		pstInitiateQ = pstNext;
    }

	/* unlock Mutex */
	Osal_Release_Mutex(m_hInitMtxLockHandle);

	/* get the address of base node from new list */
	pstInitiateQ = pstBaseNode;
	while(NULL != pstInitiateQ)
	{
		pstBaseNode = pstInitiateQ->pstNextAddress;
		/* reset token id and free invoke id */
		pstInitiateQ->u32TokenID = 0;
		Free_Invoke_ID(pstInitiateQ->m_ReqProcInfo.m_stProcessData.m_stAPDU.m_u8InvokeId);
		pstInitiateQ->m_ReqProcInfo.m_stProcessData.m_stAPDU.m_u8InvokeId = 0;
    Clear_InitiateQ_Data(pstInitiateQ);
		/* move to next node */
		pstInitiateQ = pstBaseNode;
	}

	/* check if any data in new list */
	while(NULL != pstBaseNode)
	{	
		/* clear initiate que data */
		// TODO need to implement a loop here 
		Clear_InitiateQ_Data(pstInitiateQ);
		pstBaseNode = pstInitiateQ->pstNextAddress;
	}

    /* exit message */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: ClearIQTimer: Exit \r\n");
	#endif
	return;
}

/**
*
*DESCRIPTION
*   This is 1sec. Timer call back fucntion. 
*
*@param lpParam [in] This is not used.
*@param TimerOrWaitFired [in] <TBD>
*
*@return VOID.
*/
void  ClearInitiateQTimer(void)
{
    /* local variables */
	static bool bCIQueTimerFlag = false;

	/* avoid concurrent execution of multiple timer instances */
	if(true == bCIQueTimerFlag)
	{
		/* return from function */
		return;

	}
	/* set the flag */
	bCIQueTimerFlag = true;

	/* clear initiate queue nodes */
	Clear_InitiateQ();

	/* reset the flag */
	bCIQueTimerFlag = false;

}

/**
*
*DESCRIPTION
*   This function validates the received request.
*
*@param pstBACnetArgs [in] Pointer to structure with received parameters.
*
*@return BACNET_RETURN_TYPE
*
*/
BACNET_RETURN_TYPE Validate_Received_Request(bacnetip_arguments_t *pstBACnetArgs)
{
	/* local variables */
	BACNET_RETURN_TYPE eRetVal = BACDEL_INITIATOR_SUCCESS;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Validate_Received_Request: Entry \r\n");
	#endif

    if(NULL == pstBACnetArgs)
        return BACDEL_ERROR;

    switch(pstBACnetArgs->m_stNPDUData.m_stAPDUData.m_eServiceSupport)
    {

#ifdef BACDEL_SER_DM_DDB_A
    case SERVICE_SUPPORTED_WHO_IS:
    {
		/* Check for Network layer massage : Who-is router to network */
		if(TRUE == pstBACnetArgs->m_stNPDUData.m_bIsNwLayerMsg)
		{
			break;
		}

		/* check destination type */
        if(pstBACnetArgs->m_stNPDUData.m_eDestinationType != DESTINATION_IS_GLOBAL_BROADCAST &&
            pstBACnetArgs->m_stNPDUData.m_eDestinationType != DESTINATION_IS_LOCAL_BROADCAST &&
            pstBACnetArgs->m_stNPDUData.m_eDestinationType != DESTINATION_IS_UNICAST &&
			pstBACnetArgs->m_stNPDUData.m_eDestinationType != DESTINATION_IS_REMOTE_BROADCAST)
        {
            eRetVal = BACDEL_DESTINATION_INVALID;
			break;
        }
		
		/* check If Low Limit & High Limit value are within range */
        if((pstBACnetArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
            m_stWHO_IS_Request.m_i32DevRangeHighLimit > BACNET_MAX_INSTANCE)
            ||(pstBACnetArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
            m_stWHO_IS_Request.m_i32DevRangeLowLimit > BACNET_MAX_INSTANCE) 
			||
			(pstBACnetArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
            m_stWHO_IS_Request.m_i32DevRangeHighLimit < BACNET_PARAMETER_ABSENT)
            ||(pstBACnetArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
            m_stWHO_IS_Request.m_i32DevRangeLowLimit < BACNET_PARAMETER_ABSENT))
        {
            eRetVal = BACDEL_INITIATOR_MAX_INSTANCE_ERROR;
			break;
        }
		/* low limit should be less than high limit */
        else if(pstBACnetArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
            m_stWHO_IS_Request.m_i32DevRangeLowLimit > pstBACnetArgs->m_stNPDUData.
            m_stAPDUData.m_stServiceChoice.m_stWHO_IS_Request.m_i32DevRangeHighLimit)
        {
            eRetVal = BACDEL_INITIATOR_NOT_PROPER_RANGE_ERROR;
			break;
        }
		/* check if both parameters are present or not */
        else if((pstBACnetArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
            m_stWHO_IS_Request.m_i32DevRangeLowLimit != pstBACnetArgs->m_stNPDUData.
            m_stAPDUData.m_stServiceChoice.m_stWHO_IS_Request.m_i32DevRangeHighLimit)
            && 
            (pstBACnetArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
            m_stWHO_IS_Request.m_i32DevRangeLowLimit == BACNET_NEGATIVE_ONE ||
            pstBACnetArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
            m_stWHO_IS_Request.m_i32DevRangeHighLimit == BACNET_NEGATIVE_ONE))
        {
            eRetVal = BACDEL_INITIATOR_NOT_PROPER_RANGE_ERROR;
			break;
        }
    }
    break;
#endif


#if (defined BACDEL_SER_DS_COV_A || defined BACDEL_SER_DS_COV_B)
    case SERVICE_SUPPORTED_CONFIRMED_COV_NOTIFICATION:
	case SERVICE_SUPPORTED_UNCONFIRMED_COV_NOTIFICATION:
	{

	}
    break;
#endif

#ifdef BACDEL_SER_DM_DDB_B
    case SERVICE_SUPPORTED_I_AM:
	{
		/* Check for Network layer massage : Who-is router to network */
		if(TRUE == pstBACnetArgs->m_stNPDUData.m_bIsNwLayerMsg)
		{
			break;
		}

		/* Validate Destination Type */
        if(pstBACnetArgs->m_stNPDUData.m_eDestinationType != DESTINATION_IS_GLOBAL_BROADCAST &&
            pstBACnetArgs->m_stNPDUData.m_eDestinationType != DESTINATION_IS_LOCAL_BROADCAST &&
            pstBACnetArgs->m_stNPDUData.m_eDestinationType != DESTINATION_IS_UNICAST &&
			pstBACnetArgs->m_stNPDUData.m_eDestinationType != DESTINATION_IS_REMOTE_BROADCAST)
        {
            eRetVal = BACDEL_DESTINATION_INVALID;
			break;
        }

		/* check device id */
		if (pstBACnetArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
			m_stI_AM_Request.m_u32DeviceInstance >= BACNET_MAX_INSTANCE)
        {
			eRetVal = BACDEL_INITIATOR_MAX_INSTANCE_ERROR;
			break;
		}

		/* check Max APDU length */
		if (pstBACnetArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
			m_stI_AM_Request.m_u16MaxAPDUlengthAccepted > MAX_APDU_LENGTH_ACCEPTED)
        {
            eRetVal = BACDEL_INITIATOR_NOT_PROPER_RANGE_ERROR;
			break;
        }	

		/* check segmentation supported */
		if (pstBACnetArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
			m_stI_AM_Request.m_eSegmentationSupported >= MAX_BACNET_SEGMENTATION ||
			pstBACnetArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
			m_stI_AM_Request.m_eSegmentationSupported < 0)
        {
            eRetVal = BACDEL_INITIATOR_NOT_PROPER_RANGE_ERROR;
			break;
        }	

		/* check vendor id */
		if (pstBACnetArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
			m_stI_AM_Request.m_u32VendorID > UINT16_MAX)
        {
            eRetVal = BACDEL_INITIATOR_NOT_PROPER_RANGE_ERROR;
			break;
        }	
	}
    break;
#endif

#ifdef BACDEL_SER_DM_DOB_B
    case SERVICE_SUPPORTED_I_HAVE:
	{

        /* Validate Destination Type */
        if(pstBACnetArgs->m_stNPDUData.m_eDestinationType != DESTINATION_IS_GLOBAL_BROADCAST &&
            pstBACnetArgs->m_stNPDUData.m_eDestinationType != DESTINATION_IS_LOCAL_BROADCAST &&
            pstBACnetArgs->m_stNPDUData.m_eDestinationType != DESTINATION_IS_UNICAST &&
			pstBACnetArgs->m_stNPDUData.m_eDestinationType != DESTINATION_IS_REMOTE_BROADCAST)
        {
            eRetVal = BACDEL_DESTINATION_INVALID;
			break;
        }

		/* check device id */
		if (pstBACnetArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
			m_stI_HAVE_Request.m_u32DeviceInstance >= BACNET_MAX_INSTANCE)
        {
            eRetVal = BACDEL_INITIATOR_MAX_INSTANCE_ERROR;
			break;
        }	

		/* check object type */
		if (pstBACnetArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
			m_stI_HAVE_Request.m_eObjectType >= MAX_BACNET_OBJECT_TYPE ||
			pstBACnetArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
			m_stI_HAVE_Request.m_eObjectType < 0)
        {
            eRetVal = BACDEL_INITIATOR_MAX_OBJECT_TYPE_ERROR;
			break;
        }	

		/* check object id */
		if (pstBACnetArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
			m_stI_HAVE_Request.m_u32ObjectInstance >= BACNET_MAX_INSTANCE)
        {
            eRetVal = BACDEL_INITIATOR_MAX_INSTANCE_ERROR;
			break;
        }

		/* check object name */
		{
			uint32_t u32StrLen = 0;
			u32StrLen = pstBACnetArgs->m_stNPDUData.m_stAPDUData.
				m_stServiceChoice.m_stI_HAVE_Request.m_stObjName.m_u32StrLen;
			if(u32StrLen > MAX_CHARACTER_STRING_BYTES)
			{
				eRetVal = BACDEL_INITIATOR_MAX_CHARACTER_LENGTH_ERROR;
				break;
			}
		}
	}
    break;
#endif


#if (defined BACDEL_SER_AE_EN_A || defined BACDEL_SER_AE_EN_B)
    case SERVICE_SUPPORTED_CONFIRMED_EVENT_NOTIFICATION:
	case SERVICE_SUPPORTED_UNCONFIRMED_EVENT_NOTIFICATION:
	{

	}
    break;
#endif
#ifdef BACDEL_PR23
#ifdef BACDEL_SER_DM_DDA_A
	case SERVICE_SUPPORTED_YOU_ARE:
	{
		#ifdef DISPLAY_PARAMETERS
		/* display service data */
		You_Are_A_Display_Data(&pstBACnetArgs->m_stNPDUData.m_stAPDUData.
			m_stServiceChoice.m_stYOU_ARE_Request);
		#endif

		if(pstBACnetArgs->m_stNPDUData.m_eDestinationType != DESTINATION_IS_GLOBAL_BROADCAST &&
			pstBACnetArgs->m_stNPDUData.m_eDestinationType != DESTINATION_IS_LOCAL_BROADCAST &&
			pstBACnetArgs->m_stNPDUData.m_eDestinationType != DESTINATION_IS_UNICAST &&
			pstBACnetArgs->m_stNPDUData.m_eDestinationType != DESTINATION_IS_REMOTE_BROADCAST)
		{
			eRetVal = BACDEL_DESTINATION_INVALID;
			break;
		}

		/* check vendor id */
		if(pstBACnetArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
			m_stYOU_ARE_Request.m_u16VendorId > UINT16_MAX)
		{
			eRetVal = BACDEL_INITIATOR_NOT_PROPER_RANGE_ERROR;
			break;
		}

		/* check model name and serial number */
		{
			uint32_t u32StrLen = 0;
			/* model name */
			u32StrLen = pstBACnetArgs->m_stNPDUData.m_stAPDUData.
				m_stServiceChoice.m_stYOU_ARE_Request.m_stModelName.m_u32StrLen;
			if(u32StrLen > MAX_CHARACTER_STRING_BYTES)
			{
				eRetVal = BACDEL_INITIATOR_MAX_CHARACTER_LENGTH_ERROR;
				break;
			}
			/* serial number */
			u32StrLen = pstBACnetArgs->m_stNPDUData.m_stAPDUData.
				m_stServiceChoice.m_stYOU_ARE_Request.m_stSerialNumber.m_u32StrLen;
			if(u32StrLen > MAX_CHARACTER_STRING_BYTES)
			{
				eRetVal = BACDEL_INITIATOR_MAX_CHARACTER_LENGTH_ERROR;
				break;
			}
		}

		/* check as per the user choice */
		/* device-identifier */
		if(TRUE == pstBACnetArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
			m_stYOU_ARE_Request.m_bDeviceIdentifierPresent)
		{
			/* check device type */
			if(pstBACnetArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
				m_stYOU_ARE_Request.m_eDeviceType != OBJECT_DEVICE)
			{
				eRetVal = ERROR_CODE_INCONSISTENT_PARAMETERS;
				break;
			}
			/* check device instance */
			else if(pstBACnetArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
				m_stYOU_ARE_Request.m_u32DeviceId > BACNET_MAX_INSTANCE)
			{
				eRetVal = BACDEL_INITIATOR_MAX_INSTANCE_ERROR;
				break;
			}
		}

		/* check as per the user choice */
		/* device-mac-address */
		if(TRUE == pstBACnetArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
			m_stYOU_ARE_Request.m_bDeviceMACAddPresent)
		{
			/* check mac length */
			if(MAX_MAC_LEN < pstBACnetArgs->m_stNPDUData.m_stAPDUData.m_stServiceChoice.
				m_stYOU_ARE_Request.m_stDeviceMACAddrs.m_u32OctetCount)
			{
				eRetVal = BACDEL_INVALID_MAC_ADDRESS;
				break;
			}
		}
	}
	break;
#endif /* BACDEL_SER_DM_DDA_A */
#endif // BACDEL_PR23
    default:
    {
        eRetVal = BACDEL_INITIATOR_UNKNOWN_SERVICE_CHOICE;
		break;
    }
    }//end of switch

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Validate_Received_Request: Exit \r\n");
	#endif
    return eRetVal;
}

#ifdef BACDEL_SER_DM_DDB_A
/**
*
*DESCRIPTION
*   Response to WHO-IS request i.e. I-AM are copied from the Device Address
*   Binding structure to the provided pointer.
*
*@param pstInitiateQ [in/out] Pointer to structure of Pr_ListOfBACnetAddrBinding_t. Copy
*                             I-AM responses to this structure
*
*@return VOID
*
*/
#if (CALLBACK_FOR_WHO_IS_A)
void Copy_DDB_A_Data(InitiateInfo_t* pstInitiateQ)
{
	/* local variables */
    ddb_who_is_t *pstWHOIS = NULL;
    BACnetAddrBinding_t *pstAddBind = NULL;
    ddb_i_am_t *pstIAM = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Copy_DDB_A_Data: Entry \r\n");
	#endif

	/* check input pointer */
	if(NULL == pstInitiateQ)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: Copy_DDB_A_Data: null input pointers \r\n");
		#endif
		return ;
	}

    /* Extract the base address of Device Address Binding array. */
	/* acquire the mutex */
	Osal_Wait_Mutex(g_hDynDevAddrBindMtxHandle, INFINITE);
	/* get base node */
    pstAddBind = &g_stDevAddBinding.m_stAddBinding;

    /* Extract the request for which the response is to be sent. */
    pstWHOIS = pstInitiateQ->m_ReqProcInfo.m_stProcessData.m_stAPDU.
        m_pvServiceRequestData;
    /* Pointer to the location where I-AM response has to be stored */
    pstIAM = &(pstInitiateQ->m_stIpArgs.m_stNPDUData.m_stAPDUData.
        m_stServiceChoice.m_stI_AM_Request);
    /* Update the token ID */
    pstInitiateQ->m_stIpArgs.m_i32TokenID = pstInitiateQ->u32TokenID;

    /* Check for Device range limits. */
    if((pstWHOIS->m_i32DevRangeLowLimit != DATA_NOT_AVALIABLE) &&
        (pstWHOIS->m_i32DevRangeHighLimit != DATA_NOT_AVALIABLE))
    {
        /* Traverse the list to exclude the responses with Device
        *  id lower than Device range low limit & greater than
		*  high limit mentioned in the request.
        */
        while(NULL != pstAddBind)
        {
			/* check if id is within range limits */
			if((pstAddBind->m_u32ObjId >= (uint32_t)pstWHOIS->m_i32DevRangeLowLimit) &&
			   (pstAddBind->m_u32ObjId <= (uint32_t)pstWHOIS->m_i32DevRangeHighLimit))
			{
				/* Copy the data. */
				pstIAM->m_u16MaxAPDUlengthAccepted = pstAddBind->m_u16MaxAPDULenAccepted;
				pstIAM->m_u32DeviceInstance = pstAddBind->m_u32ObjId;
				pstIAM->m_eSegmentationSupported = pstAddBind->m_eSegmentationSupport;
				pstIAM->m_u32VendorID = pstAddBind->m_u16VendorId;
			}
			/* Move to the next node */
			pstAddBind = pstAddBind->pstNext;
			/* reached end of linklist then break */
			if(NULL == pstAddBind)
				break;
			else if((pstAddBind->m_u32ObjId >= (uint32_t)pstWHOIS->m_i32DevRangeLowLimit) &&
			   (pstAddBind->m_u32ObjId <= (uint32_t)pstWHOIS->m_i32DevRangeHighLimit))
			{
				/* Allocate memory to store the next response. */
                pstIAM->pstNext = OSAL_Malloc(sizeof(ddb_i_am_t),
                                              __FILE__, __FUNCTION__, __LINE__);
                if(pstIAM->pstNext == NULL)
                {
					;
					#ifdef DEBUG_PRINTF
                    Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: Copy_DDB_A_Data:"
                        "Malloc Failed\r\n");
					#endif
                }
                pstIAM = pstIAM->pstNext;
			}
        }//end of while loop.
    }
    else
    {
        /* If Device range limits are not present. */
        while(NULL != pstAddBind)
        {
            /* Copy the data. */
			pstIAM->m_u16MaxAPDUlengthAccepted = pstAddBind->m_u16MaxAPDULenAccepted;
			pstIAM->m_u32DeviceInstance = pstAddBind->m_u32ObjId;
			pstIAM->m_eSegmentationSupported = pstAddBind->m_eSegmentationSupport;
			pstIAM->m_u32VendorID = pstAddBind->m_u16VendorId;
            /* Move to the next node */
            pstAddBind = pstAddBind->pstNext;
            /* reached end of linklist then break */
            if(NULL == pstAddBind)
                break;
            else
            {
                /* Allocate memory to store the next response. */
                pstIAM->pstNext = OSAL_Malloc(sizeof(ddb_i_am_t),
                                              __FILE__, __FUNCTION__, __LINE__);
                if(pstIAM->pstNext == NULL)
                {
					;
					#ifdef DEBUG_PRINTF
                    Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: Copy_DDB_A_Data:"
                        "Malloc Failed\r\n");
					#endif
                 }
                pstIAM = pstIAM->pstNext;
            }
        }
    }

	/* release the mutex */
	Osal_Release_Mutex(g_hDynDevAddrBindMtxHandle);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Copy_DDB_A_Data: Exit \r\n");
	#endif
}
#endif
#endif /* BACDEL_SER_DM_DDB_A */

/***
*DESCRIPTION
*   Function fills the initiator response structure as per the error value.
*
*@param eReturnVal [in]		Type of error returned by previous function.
*@param eReturnVal [in]		Instance of InitiateQ member.
*@param pstInitRet [out]	Initiator response structure.
*
*@return - returns nothing.
*
***/
void Fill_Initiator_Response(BACNET_RETURN_TYPE eReturnVal,
							initiator_response_t *pstInitRet,
							InitiateInfo_t **pstInitiateInfoQ)
{
	/* function entry */ 
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
				Fill_Initiator_Response: entry \r\n");
	#endif

	/* Clear the InitiateQ */
	if(NULL != pstInitiateInfoQ)
	{
		/* as error response is returned immediately, no need to hav callback */
		if(NULL != *pstInitiateInfoQ)
		{
			/* un-register the callback */
			(*pstInitiateInfoQ)->pvCallBackFunc = NULL;
		}
		/* change the state to error */
		Clear_InitiateQ_State(pstInitiateInfoQ);
	}

	/* fill initiator error */
	pstInitRet->m_eRetType = BACDEL_INITIATOR_ERROR;
	pstInitRet->m_eErrorCode = eReturnVal;
	pstInitRet->m_i32TokenID = -1;

	/* switch to error return type */
	switch(eReturnVal)
	{
		default:
			/* Default case */
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
				Fill_Initiator_Response: return type error code = %d \r\n", eReturnVal);
			#endif
			break;
	}//end of switch

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
				Fill_Initiator_Response: exit \r\n");
	#endif
}

/* function to fill broadcast message parameters */
BACNET_RETURN_TYPE Fill_Destination_Parameter(
	bacnetRequestData_t *pstReqParam, 
	bacnetip_arguments_t *pstServiceArgs)
{
	/* Local variables */
	uint16_t u16PortNum = 0, u16TempPortNum = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Fill_Destination_Parameter: entry \r\n");
	#endif

	/* check input pointer */
	if(NULL == pstReqParam || NULL == pstServiceArgs)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
		Fill_Destination_Parameter: Null input pointers \r\n");
		#endif
		return BACDEL_ERROR;
	}

	/* If Global Broadcast */
    if((pstServiceArgs->m_stNPDUData.m_eDestinationType == 
			DESTINATION_IS_LOCAL_BROADCAST) || 
        (pstServiceArgs->m_stNPDUData.m_eDestinationType == 
			DESTINATION_IS_GLOBAL_BROADCAST) ||
        (pstServiceArgs->m_stNPDUData.m_eDestinationType == 
            DESTINATION_IS_REMOTE_BROADCAST))
    {

        /* Fill proper destination type */
        if(pstServiceArgs->m_stNPDUData.m_eDestinationType == 
			DESTINATION_IS_LOCAL_BROADCAST) 
        {
			;//pstReqParam->m_stRmDvAddr.u16net = BACNET_LOCAL_BROADCAST_NETWORK_NO;
        }
        else if(pstServiceArgs->m_stNPDUData.m_eDestinationType == 
			DESTINATION_IS_GLOBAL_BROADCAST)
        {
			;//pstReqParam->m_stRmDvAddr.u16net = BACNET_GLOBAL_BROADCAST_NETWORK_NO;
        }
        else if(pstServiceArgs->m_stNPDUData.m_eDestinationType == 
			DESTINATION_IS_REMOTE_BROADCAST)
		{
			;//pstReqParam->m_stRmDvAddr.net = ;
	    }
    }
    /* If Unicast */
    else if(pstServiceArgs->m_stNPDUData.m_eDestinationType == 
		DESTINATION_IS_UNICAST)
    {
		/* update bvlc to unicast */
        pstReqParam->m_stNPDU.eDestinationType = DESTINATION_IS_UNICAST;
        pstReqParam->m_eBVLCFunctionType = BVLC_ORIGINAL_UNICAST_NPDU;
		// TODO - set frame type 
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Fill_Destination_Parameter: exit \r\n");
	#endif

	return BACDEL_SUCCESS;
}

/**                                                                         
*
* DESCRIPTION                                                           
* Function to copy destination address parameter as per destination type 
*
* @param eDestType				 [in]  Type of destination.
* @param bDestTypeFlag			 [in]  Flag to check request generated by
*									   Device ID or Device address. 
* @param u32DestDevId			 [in]  Destination Device ID. 
* @param pstDestinationAdd		 [in]  Destination Device IP address.
*
* @param pstReturnDestAddr		 [out] Return Destination Device IP address.
* @return BACNET_RETURN_TYPE     [out] success or suitable error code.
*
*/

BACNET_RETURN_TYPE Copy_Destination_Parameter(
	DESTINATION_TYPE eDestType,
	bool bDestTypeFlag,
	uint32_t u32DestDevId,
	BACnetAddress_t *pstDestinationAdd,
	BACnetAddress_t *pstReturnDestAddr)
{
	/* Local variables */
	 BACNET_RETURN_TYPE eReturnVal = BACDEL_SUCCESS;
	 uint16_t u16PortNum = DEFAULT_PORT_NUMBER;
	 uint8_t u8Count = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Copy_Destination_Parameter: entry \r\n");
	#endif

	/* copy destination addreess */
	switch(eDestType)
	{
		case DESTINATION_IS_UNICAST:
		{
			if(TRUE == bDestTypeFlag)
			{
				/* find address of destination device ID */
				eReturnVal = BACDEL_Get_Device_ID_Or_Address(
					&u32DestDevId, pstReturnDestAddr, TRUE);
				if(BACDEL_SUCCESS != eReturnVal)
				{
					eReturnVal = BACDEL_DESTINATION_NOT_FOUND;     
				}
			}
			else if(NULL != pstDestinationAdd)
			{

			}
			else
			{
				eReturnVal = BACDEL_INVALID_INPUT_PARAMETER;
			}
		}
		break;

		case DESTINATION_IS_REMOTE_BROADCAST:
		{
			if(NULL != pstDestinationAdd)
			{
				/* copy dnet */
				pstReturnDestAddr->u16net = pstDestinationAdd->u16net;
				/* reset dadr */
				pstReturnDestAddr->u8dlen = 0;
			}
			else
			{
				eReturnVal = BACDEL_INVALID_INPUT_PARAMETER;
			}
		}
		break;

		case DESTINATION_IS_LOCAL_BROADCAST:
		case DESTINATION_IS_GLOBAL_BROADCAST:
		{

		}
		break;


		default:
		{
			eReturnVal = BACDEL_DESTINATION_INVALID;
		}
		break;
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAPI: \
    Copy_Destination_Parameter: Exit \r\n");
	#endif
	return eReturnVal;
}

/************************ end of file **************************/
