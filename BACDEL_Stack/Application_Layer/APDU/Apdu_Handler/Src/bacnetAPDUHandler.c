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
*   SoftDEL Systems Ltd.						india@softdel.com         
*   3rd Floor, Pentagon P4,						http://www.softdel.com  
*   Magarpatta City, Hadapsar
*   Pune - 411 028                                
*                                                                         
*   FILE
*	bacnetAPDUHandler.c
*                                                                      
*   AUTHORS                                                                     
*	Harshal Mangale, Ashish Verma
*                                                                         
*   DESCRIPTION                                                            
*	Functions for Encoding and Decoding of APDU layer header.
*
******************************************************************************/

/** header files */
#include "datalinkMSTP.h"
#include "pduEncodeDecode.h"
#include "pduError.h"
#include "pduReject.h"
#include "pduAbort.h"
#include "bacnetAPDUHandler.h"
#include "pduServiceStructure.h"
#include "bacnetNPDUHandler.h"
#include "propertyValidations.h"
#include <string.h>
/* To include A-side functionality */
#ifdef INITIATE_SERVICE_ENABLED
#include "bacnetInitiateServiceMgmt.h"
#endif

/* To Include Segmentation Support */
#ifdef SEGMENTATION_SUPPORTED
#include "SegmentationGenericHandler.h"
#endif

/* To include RP-B Service */
#ifdef BACDEL_SER_DS_RP_B
#include "serviceReadProperty_B.h"
#endif /* BACDEL_SER_DS_RP_B */

/* To include WP-B Service */
#ifdef BACDEL_SER_DS_WP_B
#include "serviceWriteProperty_B.h"
#endif /* BACDEL_SER_DS_WP_B */

/* To include RP-A Service */
#ifdef BACDEL_SER_DS_RP_A
#include "serviceReadProperty_A.h"
#endif /* BACDEL_SER_DS_RP_A */

/* To include WP-A Service */
#ifdef BACDEL_SER_DS_WP_A
#include "serviceWriteProperty_A.h"
#endif /* BACDEL_SER_DS_WP_A */

/* To include RPM-B Service */
#ifdef BACDEL_SER_DS_RPM_B
#include "serviceReadPropertyMultiple_B.h"
#endif /* BACDEL_SER_DS_RPM_B */

/* To include WPM-B Service */
#ifdef BACDEL_SER_DS_WPM_B
#include "serviceWritePropertyMultiple_B.h"
#endif /* BACDEL_SER_DS_WPM_B */

/* To include RPM-A Service */
#ifdef BACDEL_SER_DS_RPM_A
#include "serviceReadPropertyMultiple_A.h"
#endif /* BACDEL_SER_DS_RPM_A */

/* To include DDB-B Service */
#ifdef BACDEL_SER_DM_DDB_B
#include "serviceDynamicDeviceBinding_B.h"
#endif /* BACDEL_SER_DM_DDB_B */

/* To include DDB-A Service */
#ifdef BACDEL_SER_DM_DDB_A
#include "serviceDynamicDeviceBinding_A.h"
#endif /* BACDEL_SER_DM_DDB_A */

/* To include DOB-B Service */
#ifdef BACDEL_SER_DM_DOB_B
#include "serviceDynamicObjectBinding_B.h"
#endif /* BACDEL_SER_DM_DOB_B */

/* To include DOB-B Service */
#ifdef BACDEL_SER_DS_COV_B
#include "serviceChangeOfValue_B.h"
#endif /* BACDEL_SER_DS_COV_B */

/* To Include DCC-B Service */
#ifdef BACDEL_SER_DM_DCC_B
#include "serviceDeviceCommControl.h"
#endif /* BACDEL_SER_DM_DCC_B */


/* To include EN-B Service */
#if (defined BACDEL_SER_AE_EN_B && defined BACDEL_OBJ_NC)
#include "serviceEventNotification_B.h"
#endif /* BACDEL_SER_AE_EN_B */

/* To include GEI-B Service */
#ifdef BACDEL_SER_AE_GEI_B
#include "serviceGetEventInformation_B.h"
#endif /* BACDEL_SER_AE_GEI_B */


/* Confirmed Request Function Handlers */
/* If they are not set, they are handled by a reject message */
static Confirmed_Function_t afpConfirmedFunction[MAX_BACNET_CONFIRMED_SERVICE];

/* Allow the APDU handler to automatically reject */
static Confirmed_Function_t fpUnrecognizedServiceHandler;

/* Unconfirmed Request Function Handlers */
/* If they are not set, they are not handled */
static Unconfirmed_Function_t afpUnconfirmedFunction[MAX_BACNET_UNCONFIRMED_SERVICE];

/* Confirmed ACK Function Handlers */
static Confirmed_ACK_Function_t afpConfirmedACKFunction[MAX_BACNET_CONFIRMED_SERVICE];

/* Error ACK Function Handlers */
static Error_Function_t afpErrorFunction[MAX_BACNET_CONFIRMED_SERVICE];

/* Abort ACK Function Handlers */
static Abort_Function_t fpAbortFunction;

/* Reject ACK Function Handlers */
static Reject_Function_t fpRejectFunction;

/** Process Queue */
extern processDataQue_t g_stProcDataQ;

extern osMutexId_t m_hProcessDataQMtxLockHandle;

extern DB_t SMCfg;
/** 
*
* DESCRIPTION
* Function to set confirmed service request handler
*
* @param eServiceChoice  [in]  Confirmed service type
* @param fpFunction	     [in]  Function pointer for respective service
*
* @return [out] void
*
*/
void APDU_Set_Confirmed_Handler(
    BACNET_CONFIRMED_SERVICE eServiceChoice,
    Confirmed_Function_t fpFunction)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	APDU_Set_Confirmed_Handler: Entry \r\n");
	#endif

	/* check if valid service */
    if(eServiceChoice < MAX_BACNET_CONFIRMED_SERVICE)
	{
		/* save function pointer */
        afpConfirmedFunction[eServiceChoice] = fpFunction;
	}
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	APDU_Set_Confirmed_Handler: Exit \r\n");
	#endif
}

/** 
*
* DESCRIPTION
* Function to set un-recognized service request handler
*
* @param fpFunction  [in]  Function pointer for handler
* @return [out] void
*
*/
void APDU_Set_Unrecognized_Service_Handler(
    Confirmed_Function_t fpFunction)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	APDU_Set_Unrecognized_Service_Handler: Entry \r\n");
	#endif

	/* save function pointer */
    fpUnrecognizedServiceHandler = fpFunction;

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	APDU_Set_Unrecognized_Service_Handler: Exit \r\n");
	#endif
}

/** 
*
* DESCRIPTION
* Function to set un-confirmed service request handler
*
* @param eServiceChoice  [in]  Un-confirmed service type
* @param fpFunction	     [in]  Function pointer for respective service
*
* @return [out] void
*
*/
void APDU_Set_Unconfirmed_Handler(
    BACNET_UNCONFIRMED_SERVICE eServiceChoice,
    Unconfirmed_Function_t fpFunction)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	APDU_Set_Unconfirmed_Handler: Entry \r\n");
	#endif

	/* check if valid service */
    if(eServiceChoice < MAX_BACNET_UNCONFIRMED_SERVICE)
	{
		/* save function pointer */
        afpUnconfirmedFunction[eServiceChoice] = fpFunction;
	}
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	APDU_Set_Unconfirmed_Handler: Exit \r\n");
	#endif
}

/** 
*
* DESCRIPTION
* Function to set simple ACK handler
*
* @param eServiceChoice  [in]  Confirmed service type
* @param fpFunction	     [in]  Function pointer for respective service
*
* @return [out] void
*
*/
void APDU_Set_Confirmed_Simple_Ack_Handler(
    BACNET_CONFIRMED_SERVICE eServiceChoice,
    Confirmed_Simple_ACK_Function_t fpFunction)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	APDU_Set_Confirmed_Simple_Ack_Handler: Entry \r\n");
	#endif

	/* switch to service type */
    switch(eServiceChoice) 
    {
			/* Alarm and Event Services */
        case SERVICE_CONFIRMED_ACKNOWLEDGE_ALARM:
        case SERVICE_CONFIRMED_COV_NOTIFICATION:
        case SERVICE_CONFIRMED_EVENT_NOTIFICATION:
        case SERVICE_CONFIRMED_SUBSCRIBE_COV:
        case SERVICE_CONFIRMED_SUBSCRIBE_COV_PROPERTY:
        case SERVICE_CONFIRMED_LIFE_SAFETY_OPERATION:
            /* Object Access Services */
        case SERVICE_CONFIRMED_ADD_LIST_ELEMENT:
        case SERVICE_CONFIRMED_REMOVE_LIST_ELEMENT:
        case SERVICE_CONFIRMED_DELETE_OBJECT:
        case SERVICE_CONFIRMED_WRITE_PROPERTY:
        case SERVICE_CONFIRMED_WRITE_PROP_MULTIPLE:
            /* Remote Device Management Services */
        case SERVICE_CONFIRMED_DEVICE_COMMUNICATION_CONTROL:
        case SERVICE_CONFIRMED_TEXT_MESSAGE:
        case SERVICE_CONFIRMED_REINITIALIZE_DEVICE:
            /* Virtual Terminal Services */
        case SERVICE_CONFIRMED_VT_CLOSE:
            /* Security Services */
        case SERVICE_CONFIRMED_REQUEST_KEY:
		{
			/* save function pointer */
            afpConfirmedACKFunction[eServiceChoice] =
                (Confirmed_ACK_Function_t) fpFunction;
		}
        break;

		/* default case */
        default:
        break;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	APDU_Set_Confirmed_Simple_Ack_Handler: Exit \r\n");
	#endif
}

/** 
*
* DESCRIPTION
* Function to set complex ACK handler
*
* @param eServiceChoice  [in]  Confirmed service type
* @param fpFunction	     [in]  Function pointer for respective service
*
* @return [out] void
*
*/
void APDU_Set_Confirmed_Ack_Handler(
    BACNET_CONFIRMED_SERVICE eServiceChoice,
    Confirmed_ACK_Function_t fpFunction)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	APDU_Set_Confirmed_Ack_Handler: Entry \r\n");
	#endif

	/* switch to service type */
    switch(eServiceChoice) 
    {
			/* Alarm and Event Services */
        case SERVICE_CONFIRMED_GET_ALARM_SUMMARY:
        case SERVICE_CONFIRMED_GET_ENROLLMENT_SUMMARY:
        case SERVICE_CONFIRMED_GET_EVENT_INFORMATION:
            /* File Access Services */
        case SERVICE_CONFIRMED_ATOMIC_READ_FILE:
        case SERVICE_CONFIRMED_ATOMIC_WRITE_FILE:
            /* Object Access Services */
        case SERVICE_CONFIRMED_CREATE_OBJECT:
        case SERVICE_CONFIRMED_READ_PROPERTY:
        case SERVICE_CONFIRMED_READ_PROP_CONDITIONAL:
        case SERVICE_CONFIRMED_READ_PROP_MULTIPLE:
        case SERVICE_CONFIRMED_READ_RANGE:
            /* Remote Device Management Services */
        case SERVICE_CONFIRMED_PRIVATE_TRANSFER:
            /* Virtual Terminal Services */
        case SERVICE_CONFIRMED_VT_OPEN:
        case SERVICE_CONFIRMED_VT_DATA:
            /* Security Services */
        case SERVICE_CONFIRMED_AUTHENTICATE:
		{
			/* save function pointer */
            afpConfirmedACKFunction[eServiceChoice] = fpFunction;
		}
        break;

		/* default case */
        default:
        break;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	APDU_Set_Confirmed_Ack_Handler: Exit \r\n");
	#endif
}

/** 
*
* DESCRIPTION
* Function to set error ACK handler
*
* @param eServiceChoice  [in]  Confirmed service type
* @param fpFunction	     [in]  Function pointer for respective service
*
* @return [out] void
*
*/
void APDU_Set_Error_Handler(
    BACNET_CONFIRMED_SERVICE eServiceChoice,
    Error_Function_t fpFunction)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	APDU_Set_Error_Handler: Entry \r\n");
	#endif

	/* check if valid service */
    if(eServiceChoice < MAX_BACNET_CONFIRMED_SERVICE)
	{
		/* save function pointer */
        afpErrorFunction[eServiceChoice] = fpFunction;
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	APDU_Set_Error_Handler: Exit \r\n");
	#endif
}

/** 
*
* DESCRIPTION
* Function to set abort ACK handler
*
* @param fpFunction [in]  Function pointer for handler
* @return [out] void
*
*/
void APDU_Set_Abort_Handler(
    Abort_Function_t fpFunction)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	APDU_Set_Abort_Handler: Entry \r\n");
	#endif

	/* save function pointer */
    fpAbortFunction = fpFunction;

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	APDU_Set_Abort_Handler: Exit \r\n");
	#endif
}

/** 
*
* DESCRIPTION
* Function to set reject ACK handler
*
* @param fpFunction [in]  Function pointer for handler
* @return [out] void
*
*/
void APDU_Set_Reject_Handler(
    Reject_Function_t fpFunction)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	APDU_Set_Reject_Handler: Entry \r\n");
	#endif

	/* save function pointer */
    fpRejectFunction = fpFunction;

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	APDU_Set_Reject_Handler: Exit \r\n");
	#endif
}

/** 
*
* DESCRIPTION
* Function to decode fixed parameters of Apdu of confirm service request.
*
* @param u8ReqAPDU			[in]      Apdu request data
* @param u16APDULen			[in]      Apdu request data length
* @param pProcQIndex		[in/out]  Process queue node to fill decoded data
* @param pu8ServiceRequest  [in/out]  Returns address variable part of Apdu
* @param pi16ServiceReqLen	[in/out]  Remaining length of apdu data to decode
*
* @return u16PduLen [out] Length of decoded apdu request data
*
*/
uint16_t APDU_Decode_Confirmed_Service_Request(
    processInfo_t *pstProcQInfo,
    uint8_t *u8ReqAPDU,
    uint16_t u16APDULen,
    uint8_t **pu8ServiceRequest,
    int16_t *pi16ServiceReqLen)
{
	/* local variables */
    uint16_t u16PduLen = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	APDU_Decode_Confirmed_Service_Request: Entry \r\n");
	#endif
    
	/* decode APDU header */
    pstProcQInfo->m_stProcessData.m_stAPDU.m_bSegmentedMessage = (u8ReqAPDU[0] & BIT3) ? true : false;
    pstProcQInfo->m_stProcessData.m_stAPDU.m_bMoreFollows = (u8ReqAPDU[0] & BIT2) ? true : false;
    pstProcQInfo->m_stProcessData.m_stAPDU.m_bSegmentedResponseAccepted = (u8ReqAPDU[0] & BIT1) ? true : false;
    pstProcQInfo->m_stProcessData.m_stAPDU.m_i32MaxSegs = Decode_Max_Segs(u8ReqAPDU[1]);
    pstProcQInfo->m_stProcessData.m_stAPDU.m_i32MaxResp = Decode_Max_Apdu(u8ReqAPDU[1]);
    pstProcQInfo->m_stProcessData.m_stAPDU.m_u8InvokeId = u8ReqAPDU[2];

	/* three bytes decoded */
    u16PduLen = 3;

	/* check if segmented APDU */
    if(pstProcQInfo->m_stProcessData.m_stAPDU.m_bSegmentedMessage) 
    {
		/* decode segmentation related data */
        pstProcQInfo->m_stProcessData.m_stAPDU.m_u8SequenceNumber = u8ReqAPDU[u16PduLen++];

        pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ProposedWindowNumber = u8ReqAPDU[u16PduLen++];

		/* check for max segments accepted value */
		if(0 == pstProcQInfo->m_stProcessData.m_stAPDU.m_i32MaxSegs)
		{
			/* as per BTL implementation guidelined v42 clause 5.5 */
			pstProcQInfo->m_stProcessData.m_stAPDU.m_i32MaxSegs = 2;
		}
    }

	/* save service choice */
    pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice = u8ReqAPDU[u16PduLen++];

	/* calculate variable APDU part data length */
    *pi16ServiceReqLen = u16APDULen - u16PduLen;

	/* save address of variable part of APDU data */
    if(*pi16ServiceReqLen != 0)
	{
		/* save address */
        *pu8ServiceRequest = &u8ReqAPDU[u16PduLen];
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	APDU_Decode_Confirmed_Service_Request: Exit \r\n");
	#endif
    return u16PduLen;
}

/**                                                                         
*
* DESCRIPTION                                                           
* Function to check if duplicate requests is received from client devices.
* Criteria for duplicate request:
* 1. Invoke id 
* 2. Service type
* 3. Client device address
* in received request should match.
*
* @param 		[in]   No parameter (Void)
* @return bool  [out]  True if duplicate request else False.
*
*/
bool APDU_Check_If_Duplicate_Request(void)
{
	/* local variables */
	uint32_t u32Cnt = 0;
	uint32_t u32TestReqCnt = 0;
	bool bRetVal = FALSE;
	bool bExitFlag = FALSE;
	processInfo_t *pstProcQIndexNew = NULL;
	processInfo_t *pstProcQIndex = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	APDU_Check_If_Duplicate_Request: entry \r\n");
	#endif

	/* acquire the mutex */
    Osal_Wait_Mutex(m_hProcessDataQMtxLockHandle, INFINITE);	

	/* get the index number of queue in which data was filled */
	if(0 == g_stProcDataQ.m_u32QFillIndex)
	{
		u32TestReqCnt = MAX_PACKET-1;		
	}
	else
	{
		/* get the last fill index value */
		u32TestReqCnt = g_stProcDataQ.m_u32QFillIndex - 1;
	}

	/* get the pointer to new process Q index */
	pstProcQIndexNew = &g_stProcDataQ.m_stProcessQueue[u32TestReqCnt];

	/* get the index number of queue from which checking should be done */
	if(0 == u32TestReqCnt)
	{
		u32Cnt = MAX_PACKET-1;		
	}
	else
	{
		/* get the last fill index value */
		u32Cnt = u32TestReqCnt - 1;
	}

	/* verify new node against all other nodes in queue */
	for( ; u32Cnt < MAX_PACKET; )
	{
		/* break the loop if new packet index is equal to current queue index */
		if(u32Cnt == u32TestReqCnt)
		{
			break;
		}

		/* get the pointer to process Q index */
		pstProcQIndex = &g_stProcDataQ.m_stProcessQueue[u32Cnt];

		/* scan all queue nodes other that those empty */
		if(PROC_CONTINUE == pstProcQIndex->m_eState 
			|| 
			PROC_DONE == pstProcQIndex->m_eState)			
		{
			/* compare address, invoke id and service type */
			if((pstProcQIndexNew->m_stProcessData.m_stAPDU.m_u8InvokeId ==
				pstProcQIndex->m_stProcessData.m_stAPDU.m_u8InvokeId) 
				&&
			   (pstProcQIndexNew->m_stProcessData.m_eServiceSupported ==
				pstProcQIndex->m_stProcessData.m_eServiceSupported) 
				&&
			   (!memcmp(&pstProcQIndexNew->m_stProcessData.m_stRmDvAddr,
				&pstProcQIndex->m_stProcessData.m_stRmDvAddr,
				sizeof(BACnetAddress_t)))
				&&
				(!memcmp(&pstProcQIndexNew->m_stProcessData.m_stIUTAddr,
				&pstProcQIndex->m_stProcessData.m_stIUTAddr,
				sizeof(BACnetAddress_t))))			
			{
				/* duplicate request received */
				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer: \
				APDU_Check_If_Duplicate_Request: Duplicate request received \r\n");
				#endif

				/* break the loop and return true */
				bRetVal = TRUE;
				break;
			}
		}

		/* manage the current count */
		if(0 == u32Cnt && !bExitFlag)
		{
			/* rotate the loop */
			u32Cnt = MAX_PACKET-1;
			bExitFlag = TRUE;
		}
		else if(0 == u32Cnt && bExitFlag)
		{
			/* ideally should not enter this case */
			break;
		}
		else
		{
			/* decrement count by one */
			u32Cnt--;
		}			
	}// for loop ends

    /* release mutex */
	Osal_Release_Mutex(m_hProcessDataQMtxLockHandle);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	APDU_Check_If_Duplicate_Request: exit \r\n");
	#endif
	return bRetVal;
}

/**
*                                                                    
* DESCRIPTION:
* Function to decode the APDU header.
* This function will call appropriate service handler after decoding APDU header.
* Decoded data will be filled in process queue node.
*
* @param pu8APDUReq		[in]      APDU data to be decoded
* @param u16APDULen		[in]      APDU data length
* @param pstProcQInfo	[in/out]  Pointer to process queue node
*
* @return  [out]  BACNET_RETURN_TYPE enumerations
* This function is called from process (Px) thread:
* - BACDEL_SUCCESS will trigger Tx thread to send response
* - BACDEL_CONTINUE will do nothing
* - BACDEL_ERROR or any other value will free Px-Q node
*
*/
BACNET_RETURN_TYPE APDU_Decode_Handler(
    processInfo_t *pstProcQInfo,
    uint8_t *pu8APDUReq,
    uint16_t u16APDULen)
{
    /* local variables */
    BACNET_RETURN_TYPE eReturnValue = BACDEL_SUCCESS;
	BACNET_PDU_TYPE ePduType = 0;
    uint8_t *pu8ServiceRequest = NULL;
    int16_t i16ServiceReqLen = 0;
    uint16_t u16PduLen = 0;
    virtualDevData_t* pstDeviceStruct = NULL;
	bool bRetVal = FALSE;

	#ifdef SEGMENTATION_SUPPORTED
	bool bSegmentationFlag = FALSE;
	processInfo_t *pstOutProcQInfo = NULL;
	#endif

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: APDU_Decode_Handler: Entry \r\n");
	#endif

	#ifdef SUPPORT_MULTIPLE_DEVICE
    /* Assign Device Object structure for Processing request */
    if(NULL != pstProcQInfo->m_pvReqDevStruct)
	{
		/* use pure IP or virtual device */
        pstDeviceStruct = (virtualDevData_t *)pstProcQInfo->m_pvReqDevStruct;
	}
    else
	#endif
	{
		/* use pure IP device */
        pstDeviceStruct = gstHostDevice.m_pstDeviceStruct;
	}

	/* check input pointers */
	if(pstProcQInfo == NULL || pu8APDUReq == NULL || pstDeviceStruct == NULL)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: \
		APDU_Decode_Handler: Null input pointers \r\n");
		#endif
		return BACDEL_ERROR;
	}

	/* get Pdu type from request */
    pstProcQInfo->m_stProcessData.m_ePDUType = pu8APDUReq[0] & 0xF0;
	ePduType = pu8APDUReq[0] & 0xF0;

	/* validate received packet data length */
	if(u16APDULen > pstDeviceStruct->m_stDevObject.m_stMaxAPDULenAccepted.m_u16Val)
	{
		/* received packet size exceeds device max_apdu_length_accepted */
		pstProcQInfo->m_stProcessData.m_ePDUType = PDU_TYPE_REJECT;
		pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorClass = ERROR_CLASS_RESOURCES;
		pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode = 
			REJECT_REASON_BUFFER_OVERFLOW;

		/* debug log */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: \
		APDU_Decode_Handler: Length of received packet is more than Max APDU length \r\n");
		#endif

		if(PDU_TYPE_CONFIRMED_SERVICE_REQUEST == ePduType)
        {
            /* save invoke id & reject confirmed request */
            pstProcQInfo->m_stProcessData.m_stAPDU.m_u8InvokeId = pu8APDUReq[2];
			return BACDEL_SUCCESS;
        }
        else if(PDU_TYPE_COMPLEX_ACK == ePduType)
        {
			/* save invoke id & reject complex ack */
            pstProcQInfo->m_stProcessData.m_stAPDU.m_u8InvokeId = pu8APDUReq[1];
            /* reply only if segmented message */
            if((pu8APDUReq[0] & BIT3) ? true : false)
			{
				/* send success to send back reply */
			    return BACDEL_SUCCESS;
			}
            else 
			{
				/* send error to discard packet */
				return BACDEL_ERROR;
			}
        }
		else
		{
            /* just discard this apdu */
			return BACDEL_ERROR;
		}
	}

    /* PDU Type */
    switch (pstProcQInfo->m_stProcessData.m_ePDUType) 
    {
		/* Confirmed Request */
        case PDU_TYPE_CONFIRMED_SERVICE_REQUEST:
		{
            /* Decode confirmed service request upto fixed parmaeters */
            u16PduLen =																																						
                APDU_Decode_Confirmed_Service_Request(pstProcQInfo, &pu8APDUReq[0],
                u16APDULen,  &pu8ServiceRequest, &i16ServiceReqLen);

			/* debug log */
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: APDU_Decode_Handler: \
			Confirm request APDU header decoded \r\n");
			#endif

			/* map the service supported */
			pstProcQInfo->m_stProcessData.m_eServiceSupported = 
				Map_With_Services_Supported
					(pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice,
					pstProcQInfo->m_stProcessData.m_ePDUType);


			#ifdef BACDEL_SER_DM_DCC_B           
			/* if DCC is disabled, only DCC and RD can be processed */
            if((DCC_B_Get_Status(&pstProcQInfo->m_stProcessData.m_stIUTAddr) == COMMUNICATION_DISABLE) 
				&&
               (pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice
					!= SERVICE_CONFIRMED_DEVICE_COMMUNICATION_CONTROL) 
				&&
			   (pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice
					!= SERVICE_CONFIRMED_REINITIALIZE_DEVICE))
            {
				/* do not process the request */
				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: APDU_Decode_Handler: \
				Device communication is disabled \r\n");
				#endif
                return BACDEL_ERROR;
            }
			#endif /* BACDEL_SER_DM_DCC_B */

			/* Detect and discard duplicate request */
			bRetVal = APDU_Check_If_Duplicate_Request();
			if(bRetVal)
			{
				/* duplicate request received, send error to disacrd packet */
				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: APDU_Decode_Handler: \
				Duplicate request from client \r\n");
				#endif
				return BACDEL_ERROR;
			}

			#ifdef SEGMENTATION_SUPPORTED
			/* check if segmented request */
			if(pstProcQInfo->m_stProcessData.m_stAPDU.m_bSegmentedMessage)
			{
				/* check if segmentation is supported by the device */
				if(pstDeviceStruct->m_stDevObject.
					m_stSegmentationSupport.m_eSegmentationSupport == SEGMENTATION_RECEIVE || 
					pstDeviceStruct->m_stDevObject.
					m_stSegmentationSupport.m_eSegmentationSupport == SEGMENTATION_BOTH)
				{
					/* receiving of segmented messages is supported, receive segments */
					if(!pstProcQInfo->m_stProcessData.m_stAPDU.m_u8SequenceNumber)
					{
						/* clause 5.4.5.1 */
						if(0 == pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ProposedWindowNumber ||
							pstProcQInfo->m_stProcessData.m_stAPDU.
							m_u8ProposedWindowNumber > MAX_SEGMENT_WINDOW_SIZE)
						{
							/* proposed window size is incorrect, send abort message */
							pstProcQInfo->m_stProcessData.m_ePDUType = PDU_TYPE_ABORT;
							pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode = 
								ABORT_REASON_WINDOW_SIZE_OUT_OF_RANGE;
							pstProcQInfo->m_stProcessData.m_stAPDU.m_bServer = TRUE;
							#ifdef DEBUG_PRINTF
							Print_DebugMsg(DEBUG_LEVEL1, "BACnetStackAppLayer: APDU_Decode_Handler: \
							Invalid proposed window size received \r\n");
							#endif
							return BACDEL_SUCCESS;							
						}

						/* this is 1st segment, so allocate memory to save segmentation related data 
							& initialize all variables to their default values */
						eReturnValue = Create_Segmentation_Node(pstProcQInfo);
						if(BACDEL_ERROR == eReturnValue)
						{
							/* unexpected error */
							#ifdef DEBUG_PRINTF
							Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: APDU_Decode_Handler: \
							Create_Segmentation_Node returns error \r\n");
							#endif
							return BACDEL_ERROR;
						}
						else if(BACDEL_MALLOC_ERROR == eReturnValue)
						{
							/* memory allocation failed, send error */
							pstProcQInfo->m_stProcessData.m_ePDUType = PDU_TYPE_ABORT;
							pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorClass = ERROR_CLASS_RESOURCES;
							pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode = ABORT_REASON_OUT_OF_RESOURCES;
                            pstProcQInfo->m_stProcessData.m_stAPDU.m_bServer = TRUE;
							#ifdef DEBUG_PRINTF
							Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: APDU_Decode_Handler: \
							Create_Segmentation_Node returns malloc error \r\n");
							#endif
							return BACDEL_SUCCESS;
						}
					}
					/* decode the received segment */
					eReturnValue = APDU_Packet_Decode_Handler
						(pstProcQInfo, pu8ServiceRequest, i16ServiceReqLen, &pstOutProcQInfo);
					/* APDU_Packet_Decode_Handler returns 2 values on successful decoding, viz:
						1. BACDEL_CONTINUE = no need to send segACK.
						2. BACDEL_SUCCESS = send segACK or abort if max_seg_accepted exceeds.
						3. BACDEL_OTHER = last packet received, send segACK & go for actual decoding.
						4. BACDEL_ERROR = no need to send segACK.
						if there is no need to send segACK, then memory in process queue must be freed,
						so return BACDEL_ERROR to free this memory */
					if(BACDEL_ERROR == eReturnValue || BACDEL_CONTINUE == eReturnValue)
					{
						/* discard the packet */
						#ifdef DEBUG_PRINTF
						Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: APDU_Decode_Handler: \
						No need to send Segment ACK \r\n");
						#endif
						return BACDEL_ERROR;
					}
					else if(BACDEL_SUCCESS == eReturnValue)
					{
						/* send seg ack */
						#ifdef DEBUG_PRINTF
						Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: APDU_Decode_Handler: \
						Send Segment ACK \r\n");
						#endif
						return BACDEL_SUCCESS;
					}
					else if(BACDEL_OTHER == eReturnValue)
					{
						/* all segments are recived, send data for decoding */
						bSegmentationFlag = TRUE;
					}
				}
				else
				{
					/* receiving of segmented messages is not supported, send abort message */
					pstProcQInfo->m_stProcessData.m_ePDUType = PDU_TYPE_ABORT;
					pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode = 
                        ABORT_REASON_SEGMENTATION_NOT_SUPPORTED;
                    pstProcQInfo->m_stProcessData.m_stAPDU.m_bServer = TRUE;
					#ifdef DEBUG_PRINTF
                    Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: APDU_Decode_Handler: \
					Device cannot decode segmented message \r\n");
					#endif
					return BACDEL_SUCCESS;
					//break;
				}
			}
			#else
            /* Check for Segmentation support */
            if(pstProcQInfo->m_stProcessData.m_stAPDU.m_bSegmentedMessage )
            {
				/* segmentation not supported, send abort */
                pstProcQInfo->m_stProcessData.m_ePDUType = PDU_TYPE_ABORT;
                pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode = 
                    ABORT_REASON_SEGMENTATION_NOT_SUPPORTED;
                pstProcQInfo->m_stProcessData.m_stAPDU.m_bServer = TRUE;
                break;
            }
			#endif /* SEGMENTATION_SUPPORTED */

            /* Check for service support and call respective service handler */
            if((pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice < MAX_BACNET_CONFIRMED_SERVICE) 
                    && 
			   (afpConfirmedFunction[pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice])
			   &&
			   (Check_Service_Support(pstDeviceStruct->m_stDevObject.m_stObjectID.m_u32ObjId,
			   pstProcQInfo->m_stProcessData.m_eServiceSupported))) 
            {
				#ifdef SEGMENTATION_SUPPORTED
				/* check if segmented request */
				if(TRUE == bSegmentationFlag && NULL != pstOutProcQInfo)
				{
					/* call service decode handler */
					eReturnValue = 
					(afpConfirmedFunction[pstOutProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice])
							(pstOutProcQInfo, 
							pstOutProcQInfo->m_pstAPDUSegmentData->m_pu8ActualApduData,
							pstOutProcQInfo->m_pstAPDUSegmentData->m_u32ActualDataSize);

					/* Free the memory allocated to save segmentation related data, in the linklist */
					if(NULL != pstOutProcQInfo->m_pstNextProcessInfo)
					{
						/* free segmentation data */
						Clear_Segmentation_Data_State(pstOutProcQInfo->m_pstNextProcessInfo,
							LINK_LIST_SEGMENTATION_RECEIVE);
						pstOutProcQInfo->m_pstNextProcessInfo = NULL;
					}
					if(BACDEL_ERROR == eReturnValue)
					{
						/* After sending the Error frame set all parameters at 
						 * that location to zero */
						memset(pstOutProcQInfo, 0, sizeof(processInfo_t));
						/* Make the process queue location avaliable */
						pstOutProcQInfo->m_eState = PROC_INIT;
					}
					/* If Decoding is success activate Transmit Thread */
					else if(BACDEL_SUCCESS == eReturnValue)
					{
						/* Modify the status of Process queue slot to indicated 
						 * processing is complete */
						pstOutProcQInfo->m_eState = PROC_DONE;
						/* trigger transmit thread to send seg ack */
						
						if(!OSAL_Release_Sem(g_hTxSemaphoreHandle, BACNET_ONE))
						{
							#ifdef DEBUG_PRINTF
							Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: \
							APDU_Decode_Handler: Release semaphore error: %ld \r\n", 
							Osal_Get_Last_Error());
							#endif
							/* mark process Q node as free */
							memset(pstOutProcQInfo, 0, sizeof(processInfo_t));
							pstOutProcQInfo->m_eState = PROC_INIT;
						}
						//vTaskDelay(100/portTICK_PERIOD_MS);
					}
					/* return success to send the final segACK */
					eReturnValue = BACDEL_SUCCESS;
				}
				else
				{
					/* call service decode handler */
					eReturnValue = 
                    (afpConfirmedFunction[pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice])
                            (pstProcQInfo, pu8ServiceRequest,i16ServiceReqLen);
				}
				#else
				/* call service decode handler */
                eReturnValue = 
					afpConfirmedFunction[pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice]
						(pstProcQInfo, pu8ServiceRequest,i16ServiceReqLen);
				#endif /* SEGMENTATION_SUPPORTED */
            }
            else
            {
				#ifdef SEGMENTATION_SUPPORTED
				/* Free the memory allocated to save segmentation related data, in the linklist */
				if(NULL != pstOutProcQInfo)
				{
					if(NULL != pstOutProcQInfo->m_pstNextProcessInfo)
					{
						/* free segmentation data */
						Clear_Segmentation_Data_State(pstOutProcQInfo->m_pstNextProcessInfo,
							LINK_LIST_SEGMENTATION_RECEIVE);
						pstOutProcQInfo->m_pstNextProcessInfo = NULL;
					}
                    /* Make the process queue location avaliable */
                    memset(pstOutProcQInfo, 0, sizeof(processInfo_t));
					pstOutProcQInfo->m_eState = PROC_INIT;
				}
				#endif
				/* service not supported, send reject */
                pstProcQInfo->m_stProcessData.m_ePDUType = PDU_TYPE_REJECT;
                pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode = REJECT_REASON_UNRECOGNIZED_SERVICE;
				#ifdef SEGMENTATION_SUPPORTED
				/* return success to send the Reject reason */
				eReturnValue = BACDEL_SUCCESS;
				#endif
            }
            break;
		}

		/* Un-Confirmed Request */
        case PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST:
		{
			/* Decode Unconfirmed service request */
            pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice = pu8APDUReq[1];
            pu8ServiceRequest = &pu8APDUReq[2];
            i16ServiceReqLen = u16APDULen - 2;

			/* debug log */
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: APDU_Decode_Handler: \
			Un-Confirm request APDU header decoded \r\n");
			#endif

			/* map the service supported */
			pstProcQInfo->m_stProcessData.m_eServiceSupported =                
				Map_With_Services_Supported
					(pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice,
					pstProcQInfo->m_stProcessData.m_ePDUType);

			#ifdef BACDEL_SER_DM_DCC_B
			/* check DCC status */
			/* if DCC is disabled, discard all unconfirm requests */
			if((DCC_B_Get_Status(&pstProcQInfo->m_stProcessData.m_stIUTAddr) == 
				COMMUNICATION_DISABLE))
			{
				/* discard request */
				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: APDU_Decode_Handler: \
				Device communication is disabled. \r\n");
				#endif
                return BACDEL_ERROR;
			}
			#endif /* BACDEL_SER_DM_DCC_B */



            /* Check service support and call respective service handler */
            if((pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice < MAX_BACNET_UNCONFIRMED_SERVICE) 
				&& 
			   (afpUnconfirmedFunction[pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice])
			   &&
			   (Check_Service_Support(pstDeviceStruct->m_stDevObject.m_stObjectID.m_u32ObjId,
			   pstProcQInfo->m_stProcessData.m_eServiceSupported)))
            {
				/* call service decode handler */
                eReturnValue = 
					afpUnconfirmedFunction[pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice] 
                        (pstProcQInfo, pu8ServiceRequest,i16ServiceReqLen);
            }
            else
			{
				/* service not supported, discard packet */
                eReturnValue = BACDEL_ERROR;
			}
            break;
		}

		/* Simple Ack */
        case PDU_TYPE_SIMPLE_ACK:
		{
             /* decode simple ack */
            pstProcQInfo->m_stProcessData.m_stAPDU.m_u8InvokeId = pu8APDUReq[1];
            pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice = pu8APDUReq[2];

			/* debug log */
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: APDU_Decode_Handler: \
			Simple Ack decoded \r\n");
			#endif

			/* check service choice */
            switch (pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice) 
            {
					/* Alarm and Event Services */
                case SERVICE_CONFIRMED_ACKNOWLEDGE_ALARM:
                case SERVICE_CONFIRMED_COV_NOTIFICATION:
                case SERVICE_CONFIRMED_EVENT_NOTIFICATION:
                case SERVICE_CONFIRMED_SUBSCRIBE_COV:
                case SERVICE_CONFIRMED_SUBSCRIBE_COV_PROPERTY:
                case SERVICE_CONFIRMED_LIFE_SAFETY_OPERATION:
                    /* Object Access Services */
                case SERVICE_CONFIRMED_ADD_LIST_ELEMENT:
                case SERVICE_CONFIRMED_REMOVE_LIST_ELEMENT:
                case SERVICE_CONFIRMED_DELETE_OBJECT:
                case SERVICE_CONFIRMED_WRITE_PROPERTY:
                case SERVICE_CONFIRMED_WRITE_PROP_MULTIPLE:
                    /* Remote Device Management Services */
                case SERVICE_CONFIRMED_DEVICE_COMMUNICATION_CONTROL:
                case SERVICE_CONFIRMED_REINITIALIZE_DEVICE:
                case SERVICE_CONFIRMED_TEXT_MESSAGE:
                    /* Virtual Terminal Services */
                case SERVICE_CONFIRMED_VT_CLOSE:
                    /* Security Services */
                case SERVICE_CONFIRMED_REQUEST_KEY:
				{
					/* map the service supported */																											
					pstProcQInfo->m_stProcessData.m_eServiceSupported = 								//	<@>
						Map_With_Services_Supported
							(pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice,
							pstProcQInfo->m_stProcessData.m_ePDUType);

                    /* pass the ack data to acknowledgement thread */
                    eReturnValue = Initiate_Service_Interface(pstProcQInfo);
                    break;
				}

				/* invalid simple ack received */
                default:
                    break;
            } // inner switch

			/* if failure in sending data to acknowledgement thread, discard packet */
            if(BACDEL_CONTINUE != eReturnValue)
			{
				/* debug log */
				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: APDU_Decode_Handler: \
				Simple Ack discarded \r\n");
				#endif
				/* set value to discard packet */
				eReturnValue = BACDEL_ERROR;
			}
            break;
		}

		/* Complex Ack */
        case PDU_TYPE_COMPLEX_ACK:
		{
            /* decode complex ack */
            pstProcQInfo->m_stProcessData.m_stAPDU.m_bSegmentedMessage = (pu8APDUReq[0] & BIT3) ? true : false;
            pstProcQInfo->m_stProcessData.m_stAPDU.m_bMoreFollows = (pu8APDUReq[0] & BIT2) ? true : false;
            pstProcQInfo->m_stProcessData.m_stAPDU.m_u8InvokeId = pu8APDUReq[1];
            u16PduLen = 2;

			/* check if segmented complex ack */
            if (pstProcQInfo->m_stProcessData.m_stAPDU.m_bSegmentedMessage) 
            {
				/* decode sequence no and proposed window size */
                pstProcQInfo->m_stProcessData.m_stAPDU.m_u8SequenceNumber = pu8APDUReq[u16PduLen++];
                pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ProposedWindowNumber = pu8APDUReq[u16PduLen++];
            }

			/* decode service choice */
            pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice = pu8APDUReq[u16PduLen++];
            pu8ServiceRequest = &pu8APDUReq[u16PduLen];
            i16ServiceReqLen = u16APDULen - u16PduLen;

			/* debug log */
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: APDU_Decode_Handler: \
			Complex Ack header decoded \r\n");
			#endif

			#ifdef SEGMENTATION_SUPPORTED
			/* if segmented then, receive all segments one by one */
			if(pstProcQInfo->m_stProcessData.m_stAPDU.m_bSegmentedMessage)
			{
				/* check if device can receive segments */
				if(pstDeviceStruct->m_stDevObject.
					m_stSegmentationSupport.m_eSegmentationSupport == SEGMENTATION_RECEIVE || 
					pstDeviceStruct->m_stDevObject.
					m_stSegmentationSupport.m_eSegmentationSupport == SEGMENTATION_BOTH)
				{
					/* receiving of segmented messages is supported, receive segments */
					if(!pstProcQInfo->m_stProcessData.m_stAPDU.m_u8SequenceNumber)
					{
						/* this is 1st segment, so allocate memory to save segmentation related data 
							& initialize all variables to their default values */
						eReturnValue = Create_Segmentation_Node(pstProcQInfo);
						if(BACDEL_ERROR == eReturnValue)
						{
							#ifdef DEBUG_PRINTF
							Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: APDU_Decode_Handler: \
							Saving 1st segmented complex ack failed \r\n");
							#endif
							return BACDEL_ERROR;
						}
						else if(BACDEL_MALLOC_ERROR == eReturnValue)
						{
							/* memory allocation failed, send error */
							pstProcQInfo->m_stProcessData.m_ePDUType = PDU_TYPE_ABORT;
							pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorClass = ERROR_CLASS_RESOURCES;
							pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode = 
								ABORT_REASON_OUT_OF_RESOURCES;
                            pstProcQInfo->m_stProcessData.m_stAPDU.m_bServer = FALSE;
							#ifdef DEBUG_PRINTF
							Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: APDU_Decode_Handler: \
							Memory allocation failed while saving 1st segmented complex ack \r\n");
							#endif
							return BACDEL_SUCCESS;
						}
					}
					/* decode the received segment */
					eReturnValue = APDU_Packet_Decode_Handler
						(pstProcQInfo, pu8ServiceRequest, i16ServiceReqLen, &pstOutProcQInfo);
					/* APDU_Packet_Decode_Handler returns 2 values on successful decoding, viz:
						1. BACDEL_CONTINUE = no need to send segACK.
						2. BACDEL_SUCCESS = send segACK or abort if max_seg_accepted exceeds.
						3. BACDEL_OTHER = last packet received, send segACK & go for actual decoding.
						4. BACDEL_ERROR = no need to send segACK.
						if there is no need to send segACK, then memory in process queue must be freed,
						so return BACDEL_ERROR to free this memory */
					if(BACDEL_ERROR == eReturnValue || BACDEL_CONTINUE == eReturnValue)
					{
						/* do not send seg ack */
						#ifdef DEBUG_PRINTF
						Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: APDU_Decode_Handler: \
						No need to send segment ack from client \r\n");
						#endif
						return BACDEL_ERROR;
					}
					else if(BACDEL_SUCCESS == eReturnValue)
					{
						/* send seg ack */
						#ifdef DEBUG_PRINTF
						Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: APDU_Decode_Handler: \
						Send segment ack from client \r\n");
						#endif
						return BACDEL_SUCCESS;
					}
					else if(BACDEL_OTHER == eReturnValue)
					{
						/* last segment received */
						bSegmentationFlag = TRUE;
					}
				}
				else
				{
					/* receiving of segmented messages is not supported, send abort message */
					pstProcQInfo->m_stProcessData.m_ePDUType = PDU_TYPE_ABORT;
					pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode = 
                        ABORT_REASON_SEGMENTATION_NOT_SUPPORTED;
                    pstProcQInfo->m_stProcessData.m_stAPDU.m_bServer = FALSE;
					#ifdef DEBUG_PRINTF
					Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: APDU_Decode_Handler: \
					Segmented complex ack can't be received \r\n");
					#endif
					return BACDEL_ERROR; //BACDEL_SUCCESS;
				}
			}
			#else
            /* check for segmentation support */
            if(pstProcQInfo->m_stProcessData.m_stAPDU.m_bSegmentedMessage)
            {
				/* return abort */
                pstProcQInfo->m_stProcessData.m_ePDUType = PDU_TYPE_ABORT;
                pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode = 
                    ABORT_REASON_SEGMENTATION_NOT_SUPPORTED;
                pstProcQInfo->m_stProcessData.m_stAPDU.m_bServer = FALSE;
                break;
            }
			#endif /* SEGMENTATION_SUPPORTED */

			/* check service choice */
            switch (pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice) 
            {
					/* Alarm and Event Services */
                case SERVICE_CONFIRMED_GET_ALARM_SUMMARY:
                case SERVICE_CONFIRMED_GET_ENROLLMENT_SUMMARY:
                case SERVICE_CONFIRMED_GET_EVENT_INFORMATION:
                    /* File Access Services */
                case SERVICE_CONFIRMED_ATOMIC_READ_FILE:
                case SERVICE_CONFIRMED_ATOMIC_WRITE_FILE:
                    /* Object Access Services */
                case SERVICE_CONFIRMED_CREATE_OBJECT:
                case SERVICE_CONFIRMED_READ_PROPERTY:
                case SERVICE_CONFIRMED_READ_PROP_CONDITIONAL:
                case SERVICE_CONFIRMED_READ_PROP_MULTIPLE:
                case SERVICE_CONFIRMED_READ_RANGE:
                case SERVICE_CONFIRMED_PRIVATE_TRANSFER:
                    /* Virtual Terminal Services */
                case SERVICE_CONFIRMED_VT_OPEN:
                case SERVICE_CONFIRMED_VT_DATA:
                    /* Security Services */
                case SERVICE_CONFIRMED_AUTHENTICATE:
				{
                    /* check service support and call respective service handler */
                    if(NULL != afpConfirmedACKFunction[pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice]) 
                    {
						#ifdef SEGMENTATION_SUPPORTED
						/* check if valid data */
						if(TRUE == bSegmentationFlag && NULL != pstOutProcQInfo)
						{
							/* call complex ack decoder as per service */
							eReturnValue = (afpConfirmedACKFunction
								[pstOutProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice])
								(pstOutProcQInfo, 
								pstOutProcQInfo->m_pstAPDUSegmentData->m_pu8ActualApduData,
								pstOutProcQInfo->m_pstAPDUSegmentData->m_u32ActualDataSize);
							/* free the memory allocated to save segmentation related data */
							if(NULL != pstOutProcQInfo->m_pstNextProcessInfo)
							{
								Clear_Segmentation_Data_State(pstOutProcQInfo->m_pstNextProcessInfo,
									LINK_LIST_SEGMENTATION_RECEIVE);
								pstOutProcQInfo->m_pstNextProcessInfo = NULL;
							}
							if(BACDEL_ERROR == eReturnValue)
							{
								/* After sending the Error frame set all parameters at 
								 * that location to zero */
								memset(pstOutProcQInfo, 0, sizeof(processInfo_t));
								/* Make the process queue location avaliable */
								pstOutProcQInfo->m_eState = PROC_INIT;
							}
							/* Decoding success, initiate thread activated in response mode */
							else if(BACDEL_CONTINUE == eReturnValue)
							{
								;
							}
							/* return success to send the final segACK */
							eReturnValue = BACDEL_SUCCESS;
						}
						else
						{
							/* call complex ack decoder as per service */
							eReturnValue = (afpConfirmedACKFunction
								[pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice])
                                (pstProcQInfo, pu8ServiceRequest,i16ServiceReqLen);
						}
						#else
						/* call complex ack decoder as per service */
                        eReturnValue = (afpConfirmedACKFunction
							[pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice])
                            (pstProcQInfo, pu8ServiceRequest,i16ServiceReqLen);
						#endif /* SEGMENTATION_SUPPORTED */
                    }
                    else
                    {
						#ifdef SEGMENTATION_SUPPORTED
						/* free the memory allocated to save segmentation related data */
						if(NULL != pstOutProcQInfo)	
						{
							if(NULL != pstOutProcQInfo->m_pstNextProcessInfo)
							{
								Clear_Segmentation_Data_State(pstOutProcQInfo->m_pstNextProcessInfo,
									LINK_LIST_SEGMENTATION_RECEIVE);
								pstOutProcQInfo->m_pstNextProcessInfo = NULL;
							}
                            /* make the process queue location avaliable */
                            memset(pstOutProcQInfo, 0, sizeof(processInfo_t));
							pstOutProcQInfo->m_eState = PROC_INIT;
						}
						#endif /* SEGMENTATION_SUPPORTED */
						/* discard packet */
						eReturnValue = BACDEL_ERROR;
                    }
				}break;

				/* invalid complex ack received, discard packet */
                default:
                    eReturnValue = BACDEL_ERROR;
                    break;
            } // inner switch
            break;
		}

		/* Segment Ack */
        case PDU_TYPE_SEGMENT_ACK:
		{

			#ifdef SEGMENTATION_SUPPORTED
			/* Decode segment ack as per clause 20.1.6 */
			pstProcQInfo->m_stProcessData.m_stAPDU.m_bNAK = (pu8APDUReq[0] & BIT1) ? true : false;
            pstProcQInfo->m_stProcessData.m_stAPDU.m_bServer = (pu8APDUReq[0] & BIT0) ? true : false;
            pstProcQInfo->m_stProcessData.m_stAPDU.m_u8InvokeId = pu8APDUReq[1];
            pstProcQInfo->m_stProcessData.m_stAPDU.m_u8SequenceNumber = pu8APDUReq[2];
            pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ProposedWindowNumber = pu8APDUReq[3];
			u16PduLen = 4;


			/* check if sequence no is 0 */
			if(!pstProcQInfo->m_stProcessData.m_stAPDU.m_u8SequenceNumber)// || (prev_count==pstProcQInfo->m_stProcessData.m_stAPDU.m_u8SequenceNumber))
			{
				/* clause 5.4 */
				if(0 == pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ProposedWindowNumber ||
					pstProcQInfo->m_stProcessData.m_stAPDU.
					m_u8ProposedWindowNumber > MAX_SEGMENT_WINDOW_SIZE)
				{
					/* window size is incorrect, send abort message */
					pstProcQInfo->m_stProcessData.m_ePDUType = PDU_TYPE_ABORT;
					pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode = 
						ABORT_REASON_WINDOW_SIZE_OUT_OF_RANGE;
					pstProcQInfo->m_stProcessData.m_stAPDU.m_bServer = 
						!pstProcQInfo->m_stProcessData.m_stAPDU.m_bServer;
					#ifdef DEBUG_PRINTF
					Print_DebugMsg(DEBUG_LEVEL1, "BACnetStackAppLayer: APDU_Decode_Handler: \
					Invalid actual window size received \r\n");
					#endif
					return BACDEL_SUCCESS;
				}
			}

			/* debug log */
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: APDU_Decode_Handler: \
			Segment Ack decoded \r\n");
			#endif

			/* pass the seg ack data to segmentation thread */
			eReturnValue = Initiate_Service_Interface(pstProcQInfo);  //<@>
            if(BACDEL_CONTINUE != eReturnValue)
            {
                /* debug log */
				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: APDU_Decode_Handler: \
				Segment Ack discarded \r\n");
				#endif
				/* set value to discard packet */
				eReturnValue = BACDEL_ERROR;
            }
			#else

            /* not supporting segmentation, discard seg ack */
            eReturnValue = BACDEL_ERROR;
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: APDU_Decode_Handler: \
			No segmentation support, segment ack discarded \r\n");
			#endif
			#endif /* SEGMENTATION_SUPPORTED */
            break;
		}

		/* Error Ack */
        case PDU_TYPE_ERROR:
		{
			/* decode error PDU header */
            pstProcQInfo->m_stProcessData.m_stAPDU.m_u8InvokeId = pu8APDUReq[1];
            pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice = pu8APDUReq[2];
            u16PduLen = 3;

			/* debug log */
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: APDU_Decode_Handler: \
			Error Ack header decoded \r\n");
			#endif

			/* error is always received from server device */
			pstProcQInfo->m_stProcessData.m_stAPDU.m_bServer = TRUE;

            pu8ServiceRequest = &pu8APDUReq[u16PduLen];
            i16ServiceReqLen = u16APDULen - u16PduLen;

            /* check service support and call respective service handler */
            if(pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice < MAX_BACNET_CONFIRMED_SERVICE) 
            {
                if(afpErrorFunction[pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice])
                {
					/* call error ack decoder as per service */
                    eReturnValue = afpErrorFunction[pstProcQInfo->m_stProcessData.
						m_stAPDU.m_u8ServiceChoice] 
                        (pstProcQInfo, pu8ServiceRequest,i16ServiceReqLen);
                }
            }
			else
			{
				/* invalid service choice */
				/* set value to discard packet */
				eReturnValue = BACDEL_ERROR;
			}

            /* notify application about error response */
            if(eReturnValue == BACDEL_SUCCESS)
            {
				/* pass the error ack data to acknowledgement thread */
                eReturnValue = Initiate_Service_Interface(pstProcQInfo);  //<@>
                if(BACDEL_CONTINUE != eReturnValue)
                {
                    /* debug log */
					#ifdef DEBUG_PRINTF
					Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: APDU_Decode_Handler: \
					Error Ack discarded \r\n");
					#endif
					/* set value to discard packet */
					eReturnValue = BACDEL_ERROR;
                }
            }
            break;
		}

		/* Reject Ack */
        case PDU_TYPE_REJECT:
		{
			/* decode reject PDU header */
            pstProcQInfo->m_stProcessData.m_stAPDU.m_u8InvokeId = pu8APDUReq[1];
            pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode = pu8APDUReq[2];

			/* debug log */
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: APDU_Decode_Handler: \
			Reject Ack decoded \r\n");
			#endif

			/* reject is always received from server device */
			pstProcQInfo->m_stProcessData.m_stAPDU.m_bServer = TRUE;

            /* check service support and call respective service handler */
            if(NULL != fpRejectFunction)
			{
				/* call reject ack decoder as per service */
                eReturnValue = fpRejectFunction(&pstProcQInfo->m_stProcessData.m_stRmDvAddr, 
					pstProcQInfo->m_stProcessData.m_stAPDU.m_u8InvokeId, 
					(uint8_t)pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode);
			}

            /* notify application about reject response */
            if(eReturnValue == BACDEL_SUCCESS)
            {
				/* pass the error ack data to acknowledgement thread */
                eReturnValue = Initiate_Service_Interface(pstProcQInfo);		//<@>
				if(BACDEL_CONTINUE != eReturnValue)
                {
                    /* debug log */
					#ifdef DEBUG_PRINTF
					Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: APDU_Decode_Handler: \
					Reject Ack discarded \r\n");
					#endif
					/* set value to discard packet */
					eReturnValue = BACDEL_ERROR;
                }
            }
            break;
		}

		/* Abort Ack */
        case PDU_TYPE_ABORT:
		{
			/* decode abort PDU header */
            pstProcQInfo->m_stProcessData.m_stAPDU.m_bServer = pu8APDUReq[0] & 0x01;
            pstProcQInfo->m_stProcessData.m_stAPDU.m_u8InvokeId = pu8APDUReq[1];
            pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode = pu8APDUReq[2];

			/* debug log */
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: APDU_Decode_Handler: \
			Abort Ack decoded \r\n");
			#endif

            /* check service support and call respective service handler */
            if(NULL != fpAbortFunction)
			{
				/* call abort ack decoder as per service */
                eReturnValue = fpAbortFunction(&pstProcQInfo->m_stProcessData.m_stRmDvAddr, 
					pstProcQInfo->m_stProcessData.m_stAPDU.m_u8InvokeId,
					(uint8_t)pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode,
					pstProcQInfo->m_stProcessData.m_stAPDU.m_bServer);
			}
                            
            /* notify application about abort response */
            if(eReturnValue == BACDEL_SUCCESS)
            {
				/* pass the error ack data to acknowledgement or segmentation thread */
                eReturnValue = Initiate_Service_Interface(pstProcQInfo);   //<@>
				if(BACDEL_CONTINUE != eReturnValue)
                {
                    /* debug log */
					#ifdef DEBUG_PRINTF
					Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: APDU_Decode_Handler: \
					Abort Ack discarded \r\n");
					#endif
					/* set value to discard packet */
					eReturnValue = BACDEL_ERROR;
                }
            }
            break;
		}

        default:
		{
			/* debug log */
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1, "BACnetStackAppLayer: APDU_Decode_Handler: \
			Invalid APDU type \r\n");
			#endif
			/* set value to discard packet */
            eReturnValue = BACDEL_ERROR;
            break;
		}
    }// switch(Pdu-Type)

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: APDU_Decode_Handler: Exit \r\n");
	#endif
    return eReturnValue;
}

/**
*                                                                    
* DESCRIPTION:
* Function to encode the APDU data, header plus variable data.
* This function will call appropriate service encode handler after encoding APDU header.
*
* @param pstProcQInfo	[in]  Pointer to process queue node
* @return  [out]  BACNET_RETURN_TYPE enumerations
* BACDEL_SUCCESS on success else any other error code
*
*/
BACNET_RETURN_TYPE APDU_Encode_Handler(processInfo_t *pstProcQInfo)
{
    /* local variables */
    BACNET_RETURN_TYPE eReturnValue = BACDEL_SUCCESS;
    virtualDevData_t* pstDeviceStruct = NULL;
    BACNET_COMMUNICATION_STATE eDvCommState = COMMUNICATION_DISABLE;
	BACNET_ERROR_CODE eErrorCode = MAX_BACNET_ERROR_CODE;
	BACNET_ERROR_CODE eErrorClass = MAX_BACNET_ERROR_CLASS;

    /* FIXME: Take care incase of APDU Encoding length is more than assigned APDU length */
	#ifdef SEGMENTATION_SUPPORTED
	uint8_t *au8ApduResp = NULL;
	#else
   uint8_t au8Apdu[MAX_APDU_LENGTH_ACCEPTED] = {0};

	uint8_t *au8ApduResp = NULL;
	#endif
    int32_t i32APDULen = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: APDU_Encode_Handler: Entry \r\n");
	#endif
	
	#ifdef SUPPORT_MULTIPLE_DEVICE
    /* Assign Device Object structure for Processing request */
    if(NULL != pstProcQInfo->m_pvReqDevStruct)
	{
		/* use pure IP or virtual device */
        pstDeviceStruct = (virtualDevData_t *)pstProcQInfo->m_pvReqDevStruct;
	}
    else
	#endif
	{
		/* use pure IP device */
        pstDeviceStruct = gstHostDevice.m_pstDeviceStruct;
	}

	#ifdef SEGMENTATION_SUPPORTED
	/* allocate memory */
	//au8ApduResp = (uint8_t *)OSAL_Malloc(MAX_APDU_LENGTH_ACCEPTED,
		//__FILE__, __FUNCTION__, __LINE__);
	/*As Realloc is not possible in Free RTOS directly allocating 
	the memory at a time*/
	au8ApduResp = (uint8_t *)OSAL_Malloc(MAX_APDU_BUFFER,
		__FILE__, __FUNCTION__, __LINE__);	
	if(NULL == au8ApduResp)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0, "BACnetStackAppLayer: \
		APDU_Encode_Handler: Malloc Error \r\n");
		#endif
		return BACDEL_MALLOC_ERROR;
	}
	#else
    /* pointer to apdu buffer for encoding */
	au8ApduResp = au8Apdu;
	#endif /* SEGMENTATION_SUPPORTED */

	#ifdef BACDEL_SER_DM_DCC_B
    /* get that current status of device communication control */
    eDvCommState = DCC_B_Get_Status(&pstProcQInfo->m_stProcessData.m_stIUTAddr);

	/* if dcc is disable then only response to dcc-b and rd-b will be encoded */
    /* if dcc is disable-initiation then pdu other than confirm and unconfirm will be encodeed */
	/* if dcc is disable-initiation then I-Am can be sent */
    if(((eDvCommState == COMMUNICATION_DISABLE) && 
		(pstProcQInfo->m_stProcessData.m_ePDUType !=
        PDU_TYPE_SIMPLE_ACK) &&
        (pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice != 
        SERVICE_CONFIRMED_DEVICE_COMMUNICATION_CONTROL) &&
		(pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice != 
        SERVICE_CONFIRMED_REINITIALIZE_DEVICE))
		||
        ((eDvCommState == COMMUNICATION_DISABLE_INITIATION) && 
		((pstProcQInfo->m_stProcessData.m_ePDUType ==
        PDU_TYPE_CONFIRMED_SERVICE_REQUEST) ||
        (pstProcQInfo->m_stProcessData.m_ePDUType ==
        PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST)) &&
        (pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice != 
        SERVICE_UNCONFIRMED_I_AM)))
    {
		/* Send abort message to application */
		/* Set Error for this command */
		Service_Error_Handler(BACNET_STATUS_ERROR, ERROR_CLASS_DEVICE,
			ERROR_CODE_COMMUNICATION_DISABLED, pstProcQInfo, FALSE);
		#ifdef SEGMENTATION_SUPPORTED
		/* free memory allocated to APDU data buffer */
		OSAL_Free(au8ApduResp, __FILE__, __FUNCTION__, __LINE__);
		#endif
		/* communication disabled */
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1, "BACnetStackAppLayer: APDU_Encode_Handler: \
		Device commmunication is disabled by DCC service \r\n");
		#endif
        return BACDEL_ERROR;
    }
	#endif /* BACDEL_SER_DM_DCC_B */

	/* check if this is network layer message, if yes pass directly to npdu encoder */
	/* call npdu encoder function */
	#ifdef SEGMENTATION_SUPPORTED
    if(pstProcQInfo->m_stProcessData.m_stNPDU.network_layer_message)
	{
		/* send data to NPDU layer */
        eReturnValue = NPDU_Encode_Handler(pstProcQInfo, au8ApduResp, (uint16_t)i32APDULen);
		/* free memory allocated to APDU data buffer */
		OSAL_Free(au8ApduResp, __FILE__, __FUNCTION__, __LINE__);
		return eReturnValue;
	}
	#else
	/* call npdu encoder function if network layer message only */
    if(pstProcQInfo->m_stProcessData.m_stNPDU.network_layer_message)
	{
		/* send data to NPDU layer */
        return(NPDU_Encode_Handler(pstProcQInfo, &au8ApduResp[0], i32APDULen));
	}
	#endif /* SEGMENTATION_SUPPORTED */

	/* PDU type */
    switch (pstProcQInfo->m_stProcessData.m_ePDUType)
	{
		/* Confirm Request */
        case PDU_TYPE_CONFIRMED_SERVICE_REQUEST:
		{
            /* Encode PDU_TYPE, SEGMENTATION_SUPPORT, MORE_FOLLOWS */
            au8ApduResp[i32APDULen++] = pstProcQInfo->m_stProcessData.m_ePDUType;
            if(pstProcQInfo->m_stProcessData.m_stAPDU.m_bSegmentedMessage)
                au8ApduResp[0] |= BIT3;
            if(pstProcQInfo->m_stProcessData.m_stAPDU.m_bMoreFollows)
                au8ApduResp[0] |= BIT2;
            if(pstProcQInfo->m_stProcessData.m_stAPDU.m_bSegmentedResponseAccepted)
                au8ApduResp[0] |= BIT1;

            /* Encode MAX_Segs,  MAX_Resp */
            au8ApduResp[i32APDULen++] = Encode_Max_Segs_Max_Apdu(
                pstProcQInfo->m_stProcessData.m_stAPDU.m_i32MaxSegs, 
                pstProcQInfo->m_stProcessData.m_stAPDU.m_i32MaxResp);

            /* Encode INVOKE_ID */
            au8ApduResp[i32APDULen++] = pstProcQInfo->m_stProcessData.m_stAPDU.m_u8InvokeId;

            /* Encode Sequence_Number, Proposed_window_size */
            if(pstProcQInfo->m_stProcessData.m_stAPDU.m_bSegmentedMessage) 
            {
                au8ApduResp[i32APDULen++] = pstProcQInfo->m_stProcessData.m_stAPDU.m_u8SequenceNumber;
                au8ApduResp[i32APDULen++] = pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ProposedWindowNumber;
            }    

            /* Encode SERVICE_CHOICE */
            au8ApduResp[i32APDULen++] = pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice;

            /* Call Service Encode Handler */
            switch(pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice)
            {

			#ifdef BACDEL_SER_DS_COV_B
            case SERVICE_CONFIRMED_COV_NOTIFICATION:
                i32APDULen = COVB_Notify_Encoder(pstProcQInfo, au8ApduResp, i32APDULen);
                break;
			#endif /* BACDEL_SER_DS_COV_B */

			#ifdef  BACDEL_SER_DS_RP_A
			case SERVICE_CONFIRMED_READ_PROPERTY:
				i32APDULen = RP_A_Request_Encoder(pstProcQInfo, au8ApduResp, i32APDULen );
				break;
			#endif /* BACDEL_SER_DS_RP_A */

			#ifdef BACDEL_SER_DS_WP_A
			case SERVICE_CONFIRMED_WRITE_PROPERTY:
				i32APDULen = WP_A_Request_Encoder(pstProcQInfo, &au8ApduResp, i32APDULen );
				break;
			#endif /* BACDEL_SER_DS_WP_A */

			#ifdef BACDEL_SER_DS_RPM_A
			case SERVICE_CONFIRMED_READ_PROP_MULTIPLE:
				i32APDULen = RPM_A_Request_Encoder(pstProcQInfo, &au8ApduResp, i32APDULen );
				break;
			#endif /* BACDEL_SER_DS_RPM_A */

			#ifdef BACDEL_SER_DS_WPM_A
			case SERVICE_CONFIRMED_WRITE_PROP_MULTIPLE:
			   i32APDULen = WPM_A_Request_Encoder(pstProcQInfo, &au8ApduResp, i32APDULen );
			break;
			#endif /* BACDEL_SER_DS_WPM_A */


			#if (defined BACDEL_SER_AE_EN_B && defined BACDEL_OBJ_NC)
			case SERVICE_CONFIRMED_EVENT_NOTIFICATION:
				i32APDULen = Event_Notification_Encoder(pstProcQInfo, au8ApduResp, i32APDULen);
				break;
			#endif /* BACDEL_SER_AE_EN_B */


            default :
                /* FIXME: Mechanism to Notify user that service can't be initiate */
                break;
            }

			/* debug log */
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: APDU_Encode_Handler: \
			Confirmed request encoded \r\n");
			#endif
            break;
		}

		/* Un-Confirm Request */
        case PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST:
		{
            /* Encode PDU Type */
            au8ApduResp[i32APDULen++] = pstProcQInfo->m_stProcessData.
                m_ePDUType;
            
            /* Encode SERVICE_CHOICE */
            au8ApduResp[i32APDULen++] = pstProcQInfo->m_stProcessData.
                m_stAPDU.m_u8ServiceChoice;

            /* Call Service Encode Handler */
            switch(pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice)
            {
            #ifdef BACDEL_SER_DM_DDB_A
            case SERVICE_UNCONFIRMED_WHO_IS:
                i32APDULen = Who_Is_A_Request_Encoder(pstProcQInfo,au8ApduResp,
                    i32APDULen);
                break;
            #endif /* BACDEL_SER_DM_DDB_A */

            #ifdef BACDEL_SER_DM_DDB_B
            case SERVICE_UNCONFIRMED_I_AM:
                i32APDULen = I_Am_B_Request_Encoder(pstProcQInfo,au8ApduResp,
                    i32APDULen);
                break;
            #endif /* BACDEL_SER_DM_DDB_B */

            #ifdef BACDEL_SER_DM_DOB_B
            case SERVICE_UNCONFIRMED_I_HAVE:
                i32APDULen = I_Have_B_Request_Encoder(pstProcQInfo,au8ApduResp,
                    i32APDULen);
                break;
            #endif /* BACDEL_SER_DM_DOB_B */

            #ifdef BACDEL_SER_DS_COV_B
            case SERVICE_UNCONFIRMED_COV_NOTIFICATION:
                i32APDULen = COVB_Notify_Encoder(pstProcQInfo, au8ApduResp, i32APDULen);
                break;
            #endif /* BACDEL_SER_DS_COV_B */

			#ifdef BACDEL_PR23
			#ifdef BACDEL_SER_DM_DDA_A
			case SERVICE_UNCONFIRMED_YOU_ARE:
				i32APDULen = You_Are_A_Request_Encoder(pstProcQInfo, au8ApduResp,
					i32APDULen);
				break;
			#endif /* BACDEL_SER_DM_DDA_A */
			#endif /* BACDEL_PR23 */

#if (defined BACDEL_SER_AE_EN_B && defined BACDEL_OBJ_NC)
			case SERVICE_UNCONFIRMED_EVENT_NOTIFICATION:
				i32APDULen = Event_Notification_Encoder(pstProcQInfo, au8ApduResp, i32APDULen);
				break;
#endif /* BACDEL_SER_AE_EN_B */

            default :
                break;
            }

			/* if MAX APDU length of device is less, abort message */
			// FIXME : notify application that message is aborted
			if(i32APDULen > pstDeviceStruct->m_stDevObject.m_stMaxAPDULenAccepted.m_u16Val)
			{
				#ifdef SEGMENTATION_SUPPORTED
				/* free memory allocated to APDU data buffer */
				OSAL_Free(au8ApduResp, __FILE__, __FUNCTION__, __LINE__);
				#endif
				return BACDEL_ERROR;
			}

			/* debug log */
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: APDU_Encode_Handler: \
			Unconfirmed request encoded \r\n");
			#endif
            break;
		}
        
		/* Simple Ack */
        case PDU_TYPE_SIMPLE_ACK:
		{
            /* Call Simple ACk handler */
            i32APDULen = Encode_Simple_Ack(pstProcQInfo, au8ApduResp );

			/* call service specific handler */
            switch(pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice)
            {
			#ifdef BACDEL_SER_DS_WP_B
            case SERVICE_CONFIRMED_WRITE_PROPERTY:
                i32APDULen = WP_B_Ack_Encoder(pstProcQInfo, au8ApduResp, i32APDULen );
                break;
			#endif /* WP-B */

			#ifdef BACDEL_SER_DS_WPM_B
            case SERVICE_CONFIRMED_WRITE_PROP_MULTIPLE:
                i32APDULen = WPM_B_Ack_Encoder(pstProcQInfo, au8ApduResp, i32APDULen );
                break;
			#endif /* WPM-B */

            default :
                break;
            }

            /* Reset DER bit in NPDU FALSE, As no reply is expected */
            pstProcQInfo->m_stProcessData.m_stNPDU.data_expecting_reply = false;

			/* debug log */
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: APDU_Encode_Handler: \
			Simple Ack encoded \r\n");
			#endif
            break;
		}

		/* Complex Ack */
        case PDU_TYPE_COMPLEX_ACK:
		{            
            /* Encode PDU_TYPE, SEGMENTATION_SUPPORT, MORE_FOLLOWS */
            au8ApduResp[i32APDULen++] = pstProcQInfo->m_stProcessData.m_ePDUType;
            if(pstProcQInfo->m_stProcessData.m_stAPDU.m_bSegmentedMessage)
                au8ApduResp[0] |= BIT3;
            if(pstProcQInfo->m_stProcessData.m_stAPDU.m_bMoreFollows)
                au8ApduResp[0] |= BIT2;
            
			/* Encode INVOKE_ID */
            au8ApduResp[i32APDULen++] = pstProcQInfo->m_stProcessData.m_stAPDU.m_u8InvokeId;
            
			/* Encode Sequence_Number, Proposed_window_size */
            if (pstProcQInfo->m_stProcessData.m_stAPDU.m_bSegmentedMessage) 
            {
                au8ApduResp[i32APDULen++] = pstProcQInfo->m_stProcessData.m_stAPDU.m_u8SequenceNumber;
                au8ApduResp[i32APDULen++] = pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ProposedWindowNumber;
            }    

            /* Encode SERVICE_CHOICE */
            au8ApduResp[i32APDULen++] = pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice;
            
             /* Call Complex-ACK Service Encode Handler */
            switch(pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice)
            {
			#ifdef BACDEL_SER_DS_RP_B
            case SERVICE_CONFIRMED_READ_PROPERTY:
                i32APDULen = RP_B_Ack_Encoder(pstProcQInfo, &au8ApduResp, i32APDULen );
                break;
			#endif /* RP-B */

			#ifdef BACDEL_SER_DS_RPM_B
            case SERVICE_CONFIRMED_READ_PROP_MULTIPLE:
                i32APDULen = RPM_B_Ack_Encoder(pstProcQInfo, &au8ApduResp, i32APDULen );
				break;
			#endif /* RPM-B */

			#ifdef BACDEL_SER_AE_GEI_B
            case SERVICE_CONFIRMED_GET_EVENT_INFORMATION:
                i32APDULen = GEI_B_Ack_Encoder(pstProcQInfo, &au8ApduResp, i32APDULen );
                break;
			#endif /* GEI-B */

            default :
                break;
            }

			#ifdef SEGMENTATION_SUPPORTED
			/* check is done below at the end of this function */
			#else
            /* check if response length is within max_apdu_length acceppted in request */
            if(pstProcQInfo->m_stProcessData.m_stAPDU.m_i32MaxResp < i32APDULen)
            {
                /* set error for this command */
                Service_Error_Handler( BACNET_STATUS_ABORT, 0,
					ERROR_CODE_ABORT_SEGMENTATION_NOT_SUPPORTED, pstProcQInfo, TRUE);
				/* encode abort packet */
                i32APDULen = Abort_Encode_Apdu(pstProcQInfo, au8ApduResp);
            }
			#endif /* SEGMENTATION_SUPPORTED */

            /* Reset DER bit in NPDU FALSE, As no reply is expected */
            pstProcQInfo->m_stProcessData.m_stNPDU.data_expecting_reply = false;

			/* debug log */
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: APDU_Encode_Handler: \
			Complex Ack encoded \r\n");
			#endif
            break;
		}
        
		/* Segment Ack */
        case PDU_TYPE_SEGMENT_ACK:
		{            
            /* Encode PDU_TYPE, NAK, Server */
            au8ApduResp[i32APDULen++] = pstProcQInfo->m_stProcessData.m_ePDUType;
            if(pstProcQInfo->m_stProcessData.m_stAPDU.m_bServer)
                au8ApduResp[0] |= BIT0;
            if(pstProcQInfo->m_stProcessData.m_stAPDU.m_bNAK)
                au8ApduResp[0] |= BIT1;
            
            /* Encode INVOKE_ID */
            au8ApduResp[i32APDULen++] = pstProcQInfo->m_stProcessData.m_stAPDU.m_u8InvokeId;

            /* Encode Sequence_Number, Actual Window size */
            au8ApduResp[i32APDULen++] = pstProcQInfo->m_stProcessData.m_stAPDU.m_u8SequenceNumber;
            au8ApduResp[i32APDULen++] = pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ProposedWindowNumber;

			/* Reset DER bit in NPDU FALSE, As no reply is expected */
            pstProcQInfo->m_stProcessData.m_stNPDU.data_expecting_reply = false;

			/* debug log */
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: APDU_Encode_Handler: \
			Segment Ack encoded \r\n");
			#endif
            break;
		}
        
		/* Error Ack */
        case PDU_TYPE_ERROR:
		{
             /* Call Error Encoder */
            i32APDULen = Error_Encode_Apdu(pstProcQInfo, au8ApduResp);

			/* if MAX APDU length of device is less, abort message */
			// FIXME : notify application that message is aborted
			if(i32APDULen > pstDeviceStruct->m_stDevObject.m_stMaxAPDULenAccepted.m_u16Val)
			{
				/* Call Abort Encoder */
				Service_Error_Handler(BACNET_STATUS_ABORT, 0, ERROR_CODE_ABORT_BUFFER_OVERFLOW, 
					pstProcQInfo, TRUE);
				i32APDULen = Abort_Encode_Apdu(pstProcQInfo, au8ApduResp);
			}

            /* Reset DER bit in NPDU FALSE, As no reply is expected */
            pstProcQInfo->m_stProcessData.m_stNPDU.data_expecting_reply = false;

			/* debug log */
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: APDU_Encode_Handler: \
			Error Ack encoded \r\n");
			#endif
            break;
		}

		/* Reject Ack */
        case PDU_TYPE_REJECT:
		{
            /* Call Reject Encoder */
            i32APDULen = Reject_Encode_Apdu(pstProcQInfo, au8ApduResp);        

            /* Reset DER bit in NPDU FALSE, As no reply is expected */
            pstProcQInfo->m_stProcessData.m_stNPDU.data_expecting_reply = false;

			/* debug log */
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: APDU_Encode_Handler: \
			Reject Ack encoded \r\n");
			#endif
            break;
		}

		/* Abort Ack */
        case PDU_TYPE_ABORT:
		{
            /* Call Abort Encoder */
            i32APDULen = Abort_Encode_Apdu(pstProcQInfo, au8ApduResp);

            /* Reset DER bit in NPDU FALSE, As no reply is expected */
            pstProcQInfo->m_stProcessData.m_stNPDU.data_expecting_reply = false;

			/* debug log */
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackAppLayer: APDU_Encode_Handler: \
			Abort Ack encoded \r\n");
			#endif
            break;
		}

		/* Default */
        default:
		{
			/* invalid PDU type */
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL1, "BACnetStackAppLayer: APDU_Encode_Handler: \
			PDU_TYPE not supported \r\n");
			#endif
            break;
		}
    }

	/* create packets here - handle segmentation */
	#ifdef SEGMENTATION_SUPPORTED
	/* confirm request */
	if(PDU_TYPE_CONFIRMED_SERVICE_REQUEST == (BACNET_PDU_TYPE)pstProcQInfo->m_stProcessData.m_ePDUType)
	{
		/* check if encoded data length of either client or server is exceeded */
		if( (i32APDULen > 0) &&
			((pstProcQInfo->m_stProcessData.m_stDstInfo.m_u16MaxAPDULenAccepted < (uint16_t)i32APDULen) ||
			(pstDeviceStruct->m_stDevObject.m_stMaxAPDULenAccepted.m_u16Val < (uint16_t)i32APDULen)) )
		{
			/* check if both client & server support segmentation */
			if(	(TRUE == pstProcQInfo->m_stProcessData.m_stDstInfo.bDstSegSupport) && 
				(pstDeviceStruct->m_stDevObject.
				m_stSegmentationSupport.m_eSegmentationSupport == SEGMENTATION_TRANSMIT || 
				pstDeviceStruct->m_stDevObject.
				m_stSegmentationSupport.m_eSegmentationSupport == SEGMENTATION_BOTH))
			{
				/* select mac APDU length from smallest of the two lengths */
				if(	pstDeviceStruct->m_stDevObject.m_stMaxAPDULenAccepted.m_u16Val <
					pstProcQInfo->m_stProcessData.m_stDstInfo.m_u16MaxAPDULenAccepted )
				{
					/* select the smallest packet size */
					pstProcQInfo->m_stProcessData.m_stDstInfo.m_u16MaxAPDULenAccepted = 
						pstDeviceStruct->m_stDevObject.m_stMaxAPDULenAccepted.m_u16Val;
				}
				/* add data to segmentation queue */
				/* create packets for segmentation */
				eReturnValue = Create_APDU_Packets(pstProcQInfo, au8ApduResp, (uint16_t)i32APDULen);
				if(BACDEL_SUCCESS != eReturnValue)
				{
					/* set error code */
					if(BACDEL_LOCAL_BUFFER_EXCEEDED == eReturnValue)
					{
						/* local buffer value exceeded */
						eErrorClass = ERROR_CLASS_RESOURCES;
						eErrorCode = ERROR_CODE_ABORT_BUFFER_OVERFLOW;
					}
					else if(BACDEL_MALLOC_ERROR == eReturnValue)
					{
						/* memory allocation failed */
						eErrorClass = ERROR_CLASS_RESOURCES;
						eErrorCode = ERROR_CODE_OUT_OF_MEMORY;
					}
					else
					{
						/* internal stack error */
						eErrorClass = ERROR_CLASS_DEVICE;
						eErrorCode = ERROR_CODE_INTERNAL_ERROR;
					}
					/* Send abort message to application */
					/* Set Error for this command */
					Service_Error_Handler(BACNET_STATUS_ERROR, eErrorClass,
						eErrorCode, pstProcQInfo, FALSE);
					/* free memory allocated to APDU data buffer */
					OSAL_Free(au8ApduResp, __FILE__, __FUNCTION__, __LINE__);
					#ifdef DEBUG_PRINTF
					Print_DebugMsg(DEBUG_LEVEL1, "BACnetStackAppLayer: APDU_Encode_Handler: \
					APDU_Packet_Encode_Handler returns error \r\n");
					#endif
					return eReturnValue;
				}
				else
				{
					/* encode 1st packet or segment & send */
					/* note that all others segments are sent from segmentation thread */
					eReturnValue = APDU_Packet_Encode_Handler(pstProcQInfo, 
						pstProcQInfo->m_pstAPDUSegmentData->m_i8SegAckFor,
						pstProcQInfo->m_pstAPDUSegmentData);
					/* free memory allocated to APDU data buffer */
					OSAL_Free(au8ApduResp, __FILE__, __FUNCTION__, __LINE__);
					return eReturnValue;
				}
			}
			else
			{
				/* send abort message to application */
				/* set error for this command */
				Service_Error_Handler( BACNET_STATUS_ABORT, 0,
					ERROR_CODE_ABORT_APDU_TOO_LONG, pstProcQInfo, FALSE);
				/* free memory allocated to APDU data buffer */
				OSAL_Free(au8ApduResp, __FILE__, __FUNCTION__, __LINE__);
				return BACDEL_ERROR;
			}
		}
	}
	/* complex ack */
	else if(PDU_TYPE_COMPLEX_ACK == (BACNET_PDU_TYPE)pstProcQInfo->m_stProcessData.m_ePDUType)
	{
		/* check if encoded data length of either client or server is exceeded */
        if( (i32APDULen > 0) &&
			((pstProcQInfo->m_stProcessData.m_stAPDU.m_i32MaxResp < i32APDULen) ||
			(pstDeviceStruct->m_stDevObject.m_stMaxAPDULenAccepted.m_u16Val < (uint16_t)i32APDULen)) )
        {
			/* check if both client & server support segmentation */
			if(pstProcQInfo->m_stProcessData.m_stAPDU.m_bSegmentedResponseAccepted &&
				(pstDeviceStruct->m_stDevObject.
				m_stSegmentationSupport.m_eSegmentationSupport == SEGMENTATION_TRANSMIT || 
				pstDeviceStruct->m_stDevObject.
				m_stSegmentationSupport.m_eSegmentationSupport == SEGMENTATION_BOTH))
			{
				/* select mac APDU length from smallest of the two lengths */
				if(	pstDeviceStruct->m_stDevObject.m_stMaxAPDULenAccepted.m_u16Val <
					pstProcQInfo->m_stProcessData.m_stAPDU.m_i32MaxResp )
				{
					/* select the smallest packet size */
					pstProcQInfo->m_stProcessData.m_stAPDU.m_i32MaxResp = 
						pstDeviceStruct->m_stDevObject.m_stMaxAPDULenAccepted.m_u16Val;
				}
				/* add data to segmentation queue */
				/* create packets for segmentation */
				eReturnValue = Create_APDU_Packets(pstProcQInfo, au8ApduResp, (uint16_t)i32APDULen);
				if(BACDEL_SUCCESS != eReturnValue)
				{
					/* set error code */
					if(BACDEL_LOCAL_BUFFER_EXCEEDED == eReturnValue)
					{
						/* local buffer value exceeded */
						/* save pdu type */
						pstProcQInfo->m_stProcessData.m_ePDUType = PDU_TYPE_ABORT;
						/* save abort reason */
						pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode = 
							ABORT_REASON_BUFFER_OVERFLOW;
						pstProcQInfo->m_stProcessData.m_stAPDU.m_bServer = TRUE;
						/* encode abort packet */
						i32APDULen = Abort_Encode_Apdu(pstProcQInfo, au8ApduResp);
					}
					else if(BACDEL_MALLOC_ERROR == eReturnValue)
					{
						/* memory allocation failed */
						/* save pdu type */
						pstProcQInfo->m_stProcessData.m_ePDUType = PDU_TYPE_ABORT;
						/* save abort reason */
						pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode = 
							ABORT_REASON_OUT_OF_RESOURCES;
						pstProcQInfo->m_stProcessData.m_stAPDU.m_bServer = TRUE;
						/* encode abort packet */
						i32APDULen = Abort_Encode_Apdu(pstProcQInfo, au8ApduResp);
					}
					else
					{
						/* internal stack error */
						eErrorClass = ERROR_CLASS_DEVICE;
						eErrorCode = ERROR_CODE_INTERNAL_ERROR;
						/* send error- ack */
						Service_Error_Handler(BACNET_STATUS_ERROR, eErrorClass,
							eErrorCode, pstProcQInfo, TRUE);
						/* encode error packet */
						i32APDULen = Error_Encode_Apdu(pstProcQInfo, au8ApduResp);
					}
				}
				else
				{
					/* encode 1st packet or segment & send */
					/* note that all others segments are sent from segmentation thread */
					//if(SMCfg.g_ui8ComType==ect_BACnet_TP /*BACnetConfigData.m_eBACStack == BACNET_STACK_IP*/)
				    {
					   // if(g_bSocketCommFlag == TRUE)
						{
							g_stMstpVariables.TokenRequiredSegmentation=TRUE;
						}

				    }
					eReturnValue = APDU_Packet_Encode_Handler(pstProcQInfo, 
						pstProcQInfo->m_pstAPDUSegmentData->m_i8SegAckFor,
						pstProcQInfo->m_pstAPDUSegmentData);
					/* free memory allocated to APDU data buffer */
					OSAL_Free(au8ApduResp, __FILE__, __FUNCTION__, __LINE__);
					return eReturnValue;
				}
			}
			else
			{
				/* send abort message */
				/* set error for this command */
                Service_Error_Handler(BACNET_STATUS_ABORT, 0,
                    ERROR_CODE_ABORT_SEGMENTATION_NOT_SUPPORTED, pstProcQInfo,
                    TRUE);
				/* encode abort packet */
                i32APDULen = Abort_Encode_Apdu(pstProcQInfo, au8ApduResp);
			}
		}
	}

	/* call npdu encoder function */
    /* pass index of tx queue, apdu, apdu_len */
    if(i32APDULen > 0)
	{
		/* pass data to NPDU layer */
        eReturnValue = NPDU_Encode_Handler(pstProcQInfo, au8ApduResp, (uint16_t)i32APDULen);
		/* free memory allocated to APDU data buffer */
		OSAL_Free(au8ApduResp, __FILE__, __FUNCTION__, __LINE__);
		return eReturnValue;
	}
	/* free memory allocated to APDU data buffer */
	OSAL_Free(au8ApduResp, __FILE__, __FUNCTION__, __LINE__);
	#else
    /* Call NPDU Encoder function*/
    /* Pass Index of Tx Queue, APDU, APDU_LEN */
    if(i32APDULen > 0)
	{
		/* pass data to NPDU layer */
        return(NPDU_Encode_Handler(pstProcQInfo, &au8ApduResp[0], i32APDULen));
	}
	#endif /* SEGMENTATION_SUPPORTED */

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: APDU_Encode_Handler: Exit \r\n");
	#endif
    /* if u16apdulen is zero means request is not sent, so return error  */
    return BACDEL_ERROR;
}


/************************** end of bacnetAPDUHandler.c file ***************************/
