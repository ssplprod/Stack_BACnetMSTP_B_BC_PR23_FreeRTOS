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
*   SoftDEL Systems Ltd.                        india@softdel.com      
*   3rd Floor, Pentagon P4,                     http://www.softdel.com  
*   Magarpatta City, Hadapsar
*   Pune - 411 028                                
*                                                                         
*   FILE
*   serviceGetEventInformation_B.c
*                                                                      
*   AUTHORS                                                                     
*   Heramb Joshi
*                                                                         
*   DESCRIPTION                                                            
*   The GetEventInformation service is used by a client BACnet-user to obtain 
*	a summary of all "active event states".
*	(a) have an Event_State property whose value is not equal to NORMAL, or
*	(b) have an Acked_Transitions property, which has at least one of the 
*	bits (TO-OFFNORMAL, TO-FAULT, TONORMAL) set to FALSE.
*	This file includes all function related to GEI-B service.
*
********************************************************************************/

#ifdef BACDEL_SER_AE_GEI_B

/* header files */
#include "serviceGetEventInformation_B.h"
#include "miscMiscellaneous.h"
#include "pduEncodeDecode.h"
#include "pduEncodeDecodeDT.h"
#include "bacnetDeviceMgmtInterface.h"
#include "propertyValueRead.h"
#include "propertyGenricHandler.h"
#include "pduAbort.h"

/**
*                                                                    
* DESCRIPTION
* This function is default handler when a GEI request is
* received from network.
*    
* @param pu8ServiceRequest	[in]    Data received for decoding.
* @param i32APDULen			[in]    Length of service request.
* @param pstProcQInfo		[out]	Pointer to the instance of processQ & will 
*									contain the decoded parameters.
*
* @return BACNET_RETURN_TYPE [out]	 success or suitable error code.
*	
*/
BACNET_RETURN_TYPE GEI_B_Decode_Handler(
	processInfo_t *pstProcQInfo,
	uint8_t *pu8ServiceRequest, 
	int32_t i32APDULen)
{  
	/* local variables */
    uint8_t u8TagNumber = 0;
    int32_t i32Len = 0;
    uint32_t u32LenValue = 0;
    Pr_BACnetObjId_t *pstLastReceiObjId = NULL;
    BACNET_RETURN_TYPE eReturnType = BACDEL_SUCCESS;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	GEI_B_Decode_Handler: entry \r\n");
	#endif

     /* check if pointer is null */
     if(NULL == pstProcQInfo)
	 {
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
		GEI_B_Decode_Handler: null input pointers \r\n");
		#endif
		return BACDEL_ERROR;
	 }

	/* check if last_Received_obj_id present */
    if(NULL != pu8ServiceRequest)
    {
		/* allocate memory */
		pstLastReceiObjId =  OSAL_Malloc(sizeof(Pr_BACnetObjId_t), 
			__FILE__, __FUNCTION__, __LINE__);
		if(NULL == pstLastReceiObjId)
		{
			#if(defined DEBUG_PRINTF && DL_0)
			Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, "APDU: \
			GEI_B_Decode_Handler: malloc failed \r\n");
			#endif
			/* set error */
			Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo,
				BACNET_STATUS_ERROR, ERROR_CLASS_RESOURCES, ERROR_CODE_OUT_OF_MEMORY);
			return BACDEL_SUCCESS;
		}
      
		/* decode the Object ID */
		i32Len += Decode_Tag_Number_And_Value(&pu8ServiceRequest[i32Len],
			&u8TagNumber, &u32LenValue);
         /* If tag is not of type process id return error */
	    if(u8TagNumber != GEI_TAG_OBJID)
        {
			#if(defined DEBUG_PRINTF && DL_1)
            Print_DebugMsg(DEBUG_LEVEL1, BACDEL_TAG_ERROR, "APDU: \
			GEI_B_Decode_Handler: u8TagNumber = %d is not valid \r\n", u8TagNumber);
			#endif
			/* free memory */
			OSAL_Free(pstLastReceiObjId, __FILE__, __FUNCTION__, __LINE__);
            /* set error */
            Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo,
                BACNET_STATUS_REJECT, BACNET_ZERO, ERROR_CODE_REJECT_INVALID_TAG);
            /* To send error response need to return error */
            return BACDEL_SUCCESS;
        }
        /* decode the object ID */
        i32Len += Decode_Object_Id(&pu8ServiceRequest[i32Len],
			(uint32_t *)&(pstLastReceiObjId->m_eObjectType),
            &(pstLastReceiObjId->m_u32ObjId));        
    }

    /* save the pointer */
    pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData = pstLastReceiObjId;

	/* check for received & decoded data length mismatch */
	if(i32Len != i32APDULen)
	{
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_DECODING_FAILED, "APDU: \
		GEI_B_Decode_Handler: Decode Error \r\n");
		#endif
		/* set error */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, BACNET_STATUS_ERROR,
			ERROR_CLASS_SERVICES, ERROR_CODE_INCONSISTENT_PARAMETERS);
		return BACDEL_SUCCESS;
	}

    /* Call Object Management Layer interface */
    eReturnType = Obj_MgmtLayer_Interface(pstProcQInfo);
    if(BACDEL_CONTINUE != eReturnType)
    {
		/* set error to send device process queue was full */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, BACNET_STATUS_ABORT, 
			BACNET_ZERO, ERROR_CODE_ABORT_OUT_OF_RESOURCES);
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, eReturnType, "APDU: \
		GEI_B_Decode_Handler: Obj_MgmtLayer_Interface returns other \r\n");
		#endif
        eReturnType = BACDEL_SUCCESS;
    }
     
	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	GEI_B_Decode_Handler: Exit \r\n");
	#endif
	return eReturnType;
}


/**
*                                                                    
* DESCRIPTION                                                                          
* This function will execute GEI-B req and fill all Active-Events data.
*    
* @param pVirtualDev	[in]  virtual device data.
* @param pstProcQInfo	[in]  Pointer to the service data.
*                                   
* @return true or false.
*	
*/
bool GEI_B_Request_Parser(
	virtualDevData_t *pVirtualDev,
	processInfo_t *pProcQIndex)
{
	/* local variables */
	event_info_response_t *pstGetEventInfoACK = NULL;
	event_summary_t *pstBaseNode = NULL;
	event_summary_t *pstPrevNode = NULL;
    int32_t i32GetEvtInfo = 0xc0; 
    Pr_BACnetObjId_t *pstLastReceiObjId = NULL;
	Pr_BACnetObjId_t *pstBackupReceiObjId = NULL;
    void *pvobjstruct = NULL;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	GEI_B_Request_Parser: entry \r\n");
	#endif

	/* check input pointers */
	if(NULL == pVirtualDev || NULL == pProcQIndex)
	{
		/* function entry */
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
		GEI_B_Request_Parser: null input pointers \r\n");
		#endif
		return FALSE;
	}

	/* set default ack PDU type */
    pProcQIndex->m_stProcessData.m_ePDUType = PDU_TYPE_COMPLEX_ACK;

    /* check if service data is present or not */
    if(NULL != pProcQIndex->m_stProcessData.m_stAPDU.m_pvServiceRequestData)
    {
		/* get service data */
        pstLastReceiObjId = pProcQIndex->m_stProcessData.m_stAPDU.m_pvServiceRequestData;
		pstBackupReceiObjId = pstLastReceiObjId;
        /* set the value */
        //i32MatchOccur = OBJ_NOT_ACTIVE_STATE;
    }

	/* allocate memory for response data */
    pstGetEventInfoACK = OSAL_Malloc(sizeof(event_info_response_t),  
		__FILE__, __FUNCTION__, __LINE__);
    /* if return null pointer */
    if(NULL == pstGetEventInfoACK)
	{
		/* function entry */
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, "APDU: \
		GEI_B_Request_Parser: malloc failed \r\n");
		#endif
		/* set error parameters */
		Exit_Routine_To_Error_Of_Confirmed_Request(pProcQIndex, 
			BACNET_STATUS_ERROR, ERROR_CLASS_RESOURCES, 
			ERROR_CODE_OUT_OF_MEMORY);
		return TRUE;
	}

	if(NULL == pstLastReceiObjId || pstLastReceiObjId->m_eObjectType == OBJECT_ANALOG_INPUT)
    {   /*check for Analog input*/
#ifdef BACDEL_OBJ_AI
        pvobjstruct = (analoginput_t *)(pVirtualDev->m_stSupportedObjects.m_pstAnalogInput); 
        /*check for all object of same type*/
        while(NULL != ((analoginput_t * )pvobjstruct))
        {
			#ifdef BACDEL_PR14
			/* check if event detection enable property value is true, then only go for below process */
			if(TRUE == ((analoginput_t * )pvobjstruct)->m_stEventDetectionEnable.m_bVal)
			#endif
			/* match obj id with last_Received_obj_id if present */
            if((NULL == pstLastReceiObjId || (pstLastReceiObjId->m_u32ObjId == ((analoginput_t * )pvobjstruct)->m_stObjectID.m_u32ObjId)))
            {/*if last_Received_obj_id present and match found, then find next object that would be successor of this obj*/
                if(NULL != pstLastReceiObjId)
                {   /* obj match with last_Received_obj_id*/
                    //i32MatchOccur = OBJ_ACTIVE_STATE;
                    /*find successor of last_Received_obj_id */
                    pstLastReceiObjId = NULL;
                }
                else
                {
                    /*check if Event State is not equal Normal and Ack_transition which has at least one of its bit False*/
                    i32GetEvtInfo = (((analoginput_t * )pvobjstruct)->m_i32GetSummary);
                    if((BIT_VALUE_CHECK(i32GetEvtInfo,7)) || (BIT_CHECK(i32GetEvtInfo,6)))
                    {
                        /*Fill ACK Para as object is active event State*/
                         GEI_B_Fill_Event_Information_Data(
							OBJECT_ANALOG_INPUT,&((analoginput_t * )pvobjstruct)->m_stObjectID,
                            pVirtualDev,pstGetEventInfoACK, &pstBaseNode, &pstPrevNode, pvobjstruct);
                
                        if(NULL == pstPrevNode)
                        {
							/* free allocated service data */														
							pProcQIndex->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;
							if(NULL != pstBackupReceiObjId)
							{
								OSAL_Free(pstBackupReceiObjId,  __FILE__, __FUNCTION__, __LINE__);
							}
							/* free all Active Event State */
							GEI_Free_Ack_Data(pstGetEventInfoACK);
							/* Update Error/Reject/Abort response for this request */
							Service_Error_Handler(BACNET_STATUS_ERROR, ERROR_CLASS_RESOURCES,
                                ERROR_CODE_OUT_OF_MEMORY, pProcQIndex, TRUE);  
                            return TRUE;
                        }
                    }
                }
            }
        pvobjstruct = ((analoginput_t * )pvobjstruct)->pstNext;
        }/*while*/
#endif
        pstLastReceiObjId = NULL;        
    }

    if(NULL == pstLastReceiObjId || pstLastReceiObjId->m_eObjectType == OBJECT_ANALOG_OUTPUT)
    {   /*check for Analog Output*/
#ifdef BACDEL_OBJ_AO
        pvobjstruct = (analogoutput_t *)(pVirtualDev->m_stSupportedObjects.m_pstAnalogOutput); 
        /*check for all object of same type*/
        while(NULL != ((analogoutput_t * )pvobjstruct))
        {
			#ifdef BACDEL_PR14
			/* check if event detection enable property value is true, then only go for below process */
			if(TRUE == ((analogoutput_t * )pvobjstruct)->m_stEventDetectionEnable.m_bVal)
			#endif
			/* match obj id with last_Received_obj_id if present */
            if((NULL == pstLastReceiObjId || (pstLastReceiObjId->m_u32ObjId == ((analogoutput_t * )pvobjstruct)->m_stObjectID.m_u32ObjId)))
            {/*if last_Received_obj_id present and match found, then find next object that would be successor of this obj*/
                if(NULL != pstLastReceiObjId)
                {
                    /*find successor of last_Received_obj_id */
                    pstLastReceiObjId = NULL;
                }
                else
                {
                    /*check if Event State is not equal Normal and Ack_transition which has at least one of its bit False*/
                    i32GetEvtInfo = (((analogoutput_t * )pvobjstruct)->m_i32GetSummary);
                    if((BIT_VALUE_CHECK(i32GetEvtInfo,7)) || (BIT_CHECK(i32GetEvtInfo,6)))
                    {
                        /*Fill ACK Para as object is active event State*/
                        GEI_B_Fill_Event_Information_Data(
							OBJECT_ANALOG_OUTPUT,&((analogoutput_t * )pvobjstruct)->m_stObjectID,
                            pVirtualDev,pstGetEventInfoACK, &pstBaseNode, &pstPrevNode, pvobjstruct);
                
                        if(NULL == pstPrevNode)
                        {
							/* free allocated service data */														
							pProcQIndex->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;
							if(NULL != pstBackupReceiObjId)
							{
								OSAL_Free(pstBackupReceiObjId,  __FILE__, __FUNCTION__, __LINE__);
							}
                            /* free all Active Event State */
							GEI_Free_Ack_Data(pstGetEventInfoACK);
							/* Update Error/Reject/Abort response for this request */
							Service_Error_Handler(BACNET_STATUS_ERROR, ERROR_CLASS_RESOURCES,
                                ERROR_CODE_OUT_OF_MEMORY, pProcQIndex, TRUE);  
                            return TRUE;
                        }
                    }
                }
            }
        pvobjstruct = ((analogoutput_t * )pvobjstruct)->pstNext;
        }/*while*/
#endif
        pstLastReceiObjId = NULL;        
    }

    if(NULL == pstLastReceiObjId || pstLastReceiObjId->m_eObjectType == OBJECT_ANALOG_VALUE)
    {   /*check for Analog Value*/
#ifdef BACDEL_OBJ_AV
        pvobjstruct = (analogvalue_t *)(pVirtualDev->m_stSupportedObjects.m_pstAnalogValue); 
        /*check for all object of same type*/
        while(NULL != ((analogvalue_t * )pvobjstruct))
        {
			#ifdef BACDEL_PR14
			/* check if event detection enable property value is true, then only go for below process */
			if(TRUE == ((analogvalue_t * )pvobjstruct)->m_stEventDetectionEnable.m_bVal)
			#endif
			/* match obj id with last_Received_obj_id if present */
            if((NULL == pstLastReceiObjId || (pstLastReceiObjId->m_u32ObjId == ((analogvalue_t * )pvobjstruct)->m_stObjectID.m_u32ObjId)))
            {/*if last_Received_obj_id present and match found, then find next object that would be successor of this obj*/
                if(NULL != pstLastReceiObjId)
                {   /* obj match with last_Received_obj_id*/
                    //i32MatchOccur = OBJ_ACTIVE_STATE;
                    /*find successor of last_Received_obj_id */
                    pstLastReceiObjId = NULL;
                }
                else
                {
                    /*check if Event State is not equal Normal and Ack_transition which has at least one of its bit False*/
                    i32GetEvtInfo = (((analogvalue_t * )pvobjstruct)->m_i32GetSummary);
                    if((BIT_VALUE_CHECK(i32GetEvtInfo,7)) || (BIT_CHECK(i32GetEvtInfo,6)))
                    {
                        /*Fill ACK Para as object is active event State*/
                       GEI_B_Fill_Event_Information_Data(
							OBJECT_ANALOG_VALUE,&((analogvalue_t * )pvobjstruct)->m_stObjectID,
                            pVirtualDev,pstGetEventInfoACK, &pstBaseNode, &pstPrevNode, pvobjstruct);
                
                        if(NULL == pstPrevNode)
                        {
							/* free allocated service data */														
							pProcQIndex->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;
							if(NULL != pstBackupReceiObjId)
							{
								OSAL_Free(pstBackupReceiObjId,  __FILE__, __FUNCTION__, __LINE__);
							}
                            /* free all Active Event State */
							GEI_Free_Ack_Data(pstGetEventInfoACK);
							/* Update Error/Reject/Abort response for this request */
							Service_Error_Handler(BACNET_STATUS_ERROR, ERROR_CLASS_RESOURCES,
                                ERROR_CODE_OUT_OF_MEMORY, pProcQIndex, TRUE);  
                            return TRUE;
                        }
                    }
                }
            }
        pvobjstruct = ((analogvalue_t * )pvobjstruct)->pstNext;
        }/*while*/
#endif
        pstLastReceiObjId = NULL;        
    }

    if(NULL == pstLastReceiObjId || pstLastReceiObjId->m_eObjectType == OBJECT_BINARY_INPUT)
    {   /*check for Binary Input*/
#ifdef BACDEL_OBJ_BI
        pvobjstruct = (BinaryInput_t *)(pVirtualDev->m_stSupportedObjects.m_pstBinaryInput); 
        /*check for all object of same type*/
        while(NULL != ((BinaryInput_t * )pvobjstruct))
        {
			#ifdef BACDEL_PR14
			/* check if event detection enable property value is true, then only go for below process */
			if(TRUE == ((BinaryInput_t * )pvobjstruct)->m_stEventDetectionEnable.m_bVal)
			#endif
			/* match obj id with last_Received_obj_id if present */
            if((NULL == pstLastReceiObjId || (pstLastReceiObjId->m_u32ObjId == ((BinaryInput_t * )pvobjstruct)->m_stObjectID.m_u32ObjId)))
            {/*if last_Received_obj_id present and match found, then find next object that would be successor of this obj*/
                if(NULL != pstLastReceiObjId)
                {   /* obj match with last_Received_obj_id*/
                    //i32MatchOccur = OBJ_ACTIVE_STATE;
                    /*find successor of last_Received_obj_id */
                    pstLastReceiObjId = NULL;
                }
                else
                {
                    /*check if Event State is not equal Normal and Ack_transition which has at least one of its bit False*/
                    i32GetEvtInfo = (((BinaryInput_t * )pvobjstruct)->m_i32GetSummary);
                    if((BIT_VALUE_CHECK(i32GetEvtInfo,7)) || (BIT_CHECK(i32GetEvtInfo,6)))
                    {
                        /*Fill ACK Para as object is active event State*/
                        GEI_B_Fill_Event_Information_Data(
							OBJECT_BINARY_INPUT,&((BinaryInput_t * )pvobjstruct)->m_stObjectID,
                            pVirtualDev,pstGetEventInfoACK, &pstBaseNode, &pstPrevNode, pvobjstruct);
                
                        if(NULL == pstPrevNode)
                        {
							/* free allocated service data */														
							pProcQIndex->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;
							if(NULL != pstBackupReceiObjId)
							{
								OSAL_Free(pstBackupReceiObjId,  __FILE__, __FUNCTION__, __LINE__);
							}
                            /* free all Active Event State */
							GEI_Free_Ack_Data(pstGetEventInfoACK);
							/* Update Error/Reject/Abort response for this request */
							Service_Error_Handler(BACNET_STATUS_ERROR, ERROR_CLASS_RESOURCES,
                                ERROR_CODE_OUT_OF_MEMORY, pProcQIndex, TRUE);  
                            return TRUE;
                        }
                    }
                }
            }
        pvobjstruct = ((BinaryInput_t * )pvobjstruct)->pstNext;
        }/*while*/
#endif
        pstLastReceiObjId = NULL;        
    }

    if(NULL == pstLastReceiObjId || pstLastReceiObjId->m_eObjectType == OBJECT_BINARY_OUTPUT)
    {   /*check for Binary Output*/
#ifdef BACDEL_OBJ_BO
        pvobjstruct = (BinaryOutput_t *)(pVirtualDev->m_stSupportedObjects.m_pstBinaryOutput); 
        /*check for all object of same type*/
        while(NULL != ((BinaryOutput_t * )pvobjstruct))
        {
			#ifdef BACDEL_PR14
			/* check if event detection enable property value is true, then only go for below process */
			if(TRUE == ((BinaryOutput_t * )pvobjstruct)->m_stEventDetectionEnable.m_bVal)
			#endif
			/* match obj id with last_Received_obj_id if present */
            if((NULL == pstLastReceiObjId || (pstLastReceiObjId->m_u32ObjId == ((BinaryOutput_t * )pvobjstruct)->m_stObjectID.m_u32ObjId)))
            {/*if last_Received_obj_id present and match found, then find next object that would be successor of this obj*/
                if(NULL != pstLastReceiObjId)
                {
                    /*find successor of last_Received_obj_id */
                    pstLastReceiObjId = NULL;
                }
                else
                {
                    /*check if Event State is not equal Normal and Ack_transition which has at least one of its bit False*/
                    i32GetEvtInfo = (((BinaryOutput_t * )pvobjstruct)->m_i32GetSummary);
                    if((BIT_VALUE_CHECK(i32GetEvtInfo,7)) || (BIT_CHECK(i32GetEvtInfo,6)))
                    {
                        /*Fill ACK Para as object is active event State*/
                        GEI_B_Fill_Event_Information_Data(
							OBJECT_BINARY_OUTPUT,&((BinaryOutput_t * )pvobjstruct)->m_stObjectID,
                            pVirtualDev,pstGetEventInfoACK, &pstBaseNode, &pstPrevNode, pvobjstruct);
                
                        if(NULL == pstPrevNode)
                        {
							/* free allocated service data */														
							pProcQIndex->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;
							if(NULL != pstBackupReceiObjId)
							{
								OSAL_Free(pstBackupReceiObjId,  __FILE__, __FUNCTION__, __LINE__);
							}
                            /* free all Active Event State */
							GEI_Free_Ack_Data(pstGetEventInfoACK);
							/* Update Error/Reject/Abort response for this request */
							Service_Error_Handler(BACNET_STATUS_ERROR, ERROR_CLASS_RESOURCES,
                                ERROR_CODE_OUT_OF_MEMORY, pProcQIndex, TRUE);  
                            return TRUE;
                        }
                    }
                }
            }
        pvobjstruct = ((BinaryOutput_t * )pvobjstruct)->pstNext;
        }/*while*/
#endif
        pstLastReceiObjId = NULL;        
    }

    if(NULL == pstLastReceiObjId || pstLastReceiObjId->m_eObjectType == OBJECT_BINARY_VALUE)
    {   /*check for Binary value */
#ifdef BACDEL_OBJ_BV
        pvobjstruct = (BinaryValue_t *)(pVirtualDev->m_stSupportedObjects.m_pstBinaryValue); 
        /*check for all object of same type*/
        while(NULL != ((BinaryValue_t * )pvobjstruct))
        {
			#ifdef BACDEL_PR14
			/* check if event detection enable property value is true, then only go for below process */
			if(TRUE == ((BinaryValue_t * )pvobjstruct)->m_stEventDetectionEnable.m_bVal)
			#endif
			/* match obj id with last_Received_obj_id if present */
            if((NULL == pstLastReceiObjId || (pstLastReceiObjId->m_u32ObjId == ((BinaryValue_t * )pvobjstruct)->m_stObjectID.m_u32ObjId)))
            {/*if last_Received_obj_id present and match found, then find next object that would be successor of this obj*/
                if(NULL != pstLastReceiObjId)
                {   /* obj match with last_Received_obj_id*/
                    //i32MatchOccur = OBJ_ACTIVE_STATE;
                    /*find successor of last_Received_obj_id */
                    pstLastReceiObjId = NULL;
                }
                else
                {
                    /*check if Event State is not equal Normal and Ack_transition which has at least one of its bit False*/
                    i32GetEvtInfo = (((BinaryValue_t * )pvobjstruct)->m_i32GetSummary);
                    if((BIT_VALUE_CHECK(i32GetEvtInfo,7)) || (BIT_CHECK(i32GetEvtInfo,6)))
                    {
                        /*Fill ACK Para as object is active event State*/
                        GEI_B_Fill_Event_Information_Data(
							OBJECT_BINARY_VALUE,&((BinaryValue_t * )pvobjstruct)->m_stObjectID,
                            pVirtualDev,pstGetEventInfoACK, &pstBaseNode, &pstPrevNode, pvobjstruct);
                
                        if(NULL == pstPrevNode)
                        {
							/* free allocated service data */														
							pProcQIndex->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;
							if(NULL != pstBackupReceiObjId)
							{
								OSAL_Free(pstBackupReceiObjId,  __FILE__, __FUNCTION__, __LINE__);
							}
                            /* free all Active Event State */
							GEI_Free_Ack_Data(pstGetEventInfoACK);
							/* Update Error/Reject/Abort response for this request */
							Service_Error_Handler(BACNET_STATUS_ERROR, ERROR_CLASS_RESOURCES,
                                ERROR_CODE_OUT_OF_MEMORY, pProcQIndex, TRUE);  
                            return TRUE;
                        }
                    }
                }
            }
        pvobjstruct = ((BinaryValue_t * )pvobjstruct)->pstNext;
        }/*while*/
#endif
        pstLastReceiObjId = NULL;        
    }

    /*  Object Calendar */
    if(NULL == pstLastReceiObjId || pstLastReceiObjId->m_eObjectType == OBJECT_CALENDAR)
        pstLastReceiObjId = NULL;  
    /* Device Object */
    if(NULL == pstLastReceiObjId || pstLastReceiObjId->m_eObjectType == OBJECT_DEVICE)
        pstLastReceiObjId = NULL;    

    /* Event Enrollment*/
    if(NULL == pstLastReceiObjId || pstLastReceiObjId->m_eObjectType == OBJECT_EVENT_ENROLLMENT)
    {
		/*check for event enrollment */
        pstLastReceiObjId = NULL;        
    }

	/* file object */
    if(NULL == pstLastReceiObjId || pstLastReceiObjId->m_eObjectType == OBJECT_FILE)
        pstLastReceiObjId = NULL;

    if(NULL == pstLastReceiObjId || pstLastReceiObjId->m_eObjectType == OBJECT_LOOP)
    {
		/*check for loop input*/
		pstLastReceiObjId = NULL;
    }    

    if(NULL == pstLastReceiObjId || pstLastReceiObjId->m_eObjectType == OBJECT_MULTI_STATE_INPUT)
    {   /*check for multistate input*/
#ifdef BACDEL_OBJ_MSI
        pvobjstruct = (MultistateInput_t *)(pVirtualDev->m_stSupportedObjects.m_pstMSInput); 
        /*check for all object of same type*/
        while(NULL != ((MultistateInput_t * )pvobjstruct))
        {
			#ifdef BACDEL_PR14
			/* check if event detection enable property value is true, then only go for below process */
			if(TRUE == ((MultistateInput_t * )pvobjstruct)->m_stEventDetectionEnable.m_bVal)
			#endif
			/* match obj id with last_Received_obj_id if present */
            if((NULL == pstLastReceiObjId || (pstLastReceiObjId->m_u32ObjId == ((MultistateInput_t * )pvobjstruct)->m_stObjectID.m_u32ObjId)))
            {/*if last_Received_obj_id present and match found, then find next object that would be successor of this obj*/
                if(NULL != pstLastReceiObjId)
                {   /* obj match with last_Received_obj_id*/
                    //i32MatchOccur = OBJ_ACTIVE_STATE;
                    /*find successor of last_Received_obj_id */
                    pstLastReceiObjId = NULL;
                }
                else
                {
                    /*check if Event State is not equal Normal and Ack_transition which has at least one of its bit False*/
                    i32GetEvtInfo = (((MultistateInput_t * )pvobjstruct)->m_i32GetSummary);
                    if((BIT_VALUE_CHECK(i32GetEvtInfo,7)) || (BIT_CHECK(i32GetEvtInfo,6)))
                    {
                        /*Fill ACK Para as object is active event State*/
                        GEI_B_Fill_Event_Information_Data(
							OBJECT_MULTI_STATE_INPUT,&((MultistateInput_t * )pvobjstruct)->m_stObjectID,
                            pVirtualDev,pstGetEventInfoACK, &pstBaseNode, &pstPrevNode, pvobjstruct);
                
                        if(NULL == pstPrevNode)
                        {
							/* free allocated service data */														
							pProcQIndex->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;
							if(NULL != pstBackupReceiObjId)
							{
								OSAL_Free(pstBackupReceiObjId,  __FILE__, __FUNCTION__, __LINE__);
							}
                            /* free all Active Event State */
							GEI_Free_Ack_Data(pstGetEventInfoACK);
							/* Update Error/Reject/Abort response for this request */
							Service_Error_Handler(BACNET_STATUS_ERROR, ERROR_CLASS_RESOURCES,
                                ERROR_CODE_OUT_OF_MEMORY, pProcQIndex, TRUE);  
                            return TRUE;
                        }
                    }
                }
            }
        pvobjstruct = ((MultistateInput_t * )pvobjstruct)->pstNext;
        }/*while*/
#endif
        pstLastReceiObjId = NULL;        
    }


    if(NULL == pstLastReceiObjId || pstLastReceiObjId->m_eObjectType == OBJECT_MULTI_STATE_OUTPUT)
    {   /*check for multistate output*/
#ifdef BACDEL_OBJ_MSO
        pvobjstruct = (MultistateOutput_t *)(pVirtualDev->m_stSupportedObjects.m_pstMSOutput); 
        /*check for all object of same type*/
        while(NULL != ((MultistateOutput_t * )pvobjstruct))
        {
			#ifdef BACDEL_PR14
			/* check if event detection enable property value is true, then only go for below process */
			if(TRUE == ((MultistateOutput_t * )pvobjstruct)->m_stEventDetectionEnable.m_bVal)
			#endif
			/* match obj id with last_Received_obj_id if present */
            if((NULL == pstLastReceiObjId || (pstLastReceiObjId->m_u32ObjId == ((MultistateOutput_t * )pvobjstruct)->m_stObjectID.m_u32ObjId)))
            {/*if last_Received_obj_id present and match found, then find next object that would be successor of this obj*/
                if(NULL != pstLastReceiObjId)
                {   /* obj match with last_Received_obj_id*/
                    //i32MatchOccur = OBJ_ACTIVE_STATE;
                    /*find successor of last_Received_obj_id */
                    pstLastReceiObjId = NULL;
                }
                else
                {
                    /*check if Event State is not equal Normal and Ack_transition which has at least one of its bit False*/
                    i32GetEvtInfo = (((MultistateOutput_t * )pvobjstruct)->m_i32GetSummary);
                    if((BIT_VALUE_CHECK(i32GetEvtInfo,7)) || (BIT_CHECK(i32GetEvtInfo,6)))
                    {
                        /*Fill ACK Para as object is active event State*/
                        GEI_B_Fill_Event_Information_Data(
							OBJECT_MULTI_STATE_OUTPUT,&((MultistateOutput_t * )pvobjstruct)->m_stObjectID,
                            pVirtualDev,pstGetEventInfoACK, &pstBaseNode, &pstPrevNode, pvobjstruct);
                
                        if(NULL == pstPrevNode)
                        {
							/* free allocated service data */														
							pProcQIndex->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;
							if(NULL != pstBackupReceiObjId)
							{
								OSAL_Free(pstBackupReceiObjId,  __FILE__, __FUNCTION__, __LINE__);
							}
                            /* free all Active Event State */
							GEI_Free_Ack_Data(pstGetEventInfoACK);
							/* Update Error/Reject/Abort response for this request */
							Service_Error_Handler(BACNET_STATUS_ERROR, ERROR_CLASS_RESOURCES,
                                ERROR_CODE_OUT_OF_MEMORY, pProcQIndex, TRUE);  
                            return TRUE;
                        }
                    }
                }
            }
        pvobjstruct = ((MultistateOutput_t * )pvobjstruct)->pstNext;
        }/*while*/
#endif
        pstLastReceiObjId = NULL;        
    }

	/* Notification class object */
    if(NULL == pstLastReceiObjId || pstLastReceiObjId->m_eObjectType == OBJECT_NOTIFICATION_CLASS)
        pstLastReceiObjId = NULL;

	/* schedule object */
    if(NULL == pstLastReceiObjId || pstLastReceiObjId->m_eObjectType == OBJECT_SCHEDULE)
    {
	    /* check for schedule */
#if (defined BACDEL_OBJ_SDL && defined BACDEL_PR14)
        pvobjstruct = (Schedule_t *)(pVirtualDev->m_stSupportedObjects.m_pstSchedule); 
        /* check for all object of same type */
        while(NULL != ((Schedule_t *)pvobjstruct))
        {
			#ifdef BACDEL_PR14
			/* check if event detection enable property value is true, then only go for below process */
			if(TRUE == ((Schedule_t *)pvobjstruct)->m_stEventDetectionEnable.m_bVal)
			#endif
			/* match obj id with last_Received_obj_id if present */
            if((NULL == pstLastReceiObjId || (pstLastReceiObjId->m_u32ObjId == ((Schedule_t *)pvobjstruct)->m_stObjectID.m_u32ObjId)))
            {
				/* if last_Received_obj_id present and match found, then find next object that would be successor of this obj */
                if(NULL != pstLastReceiObjId)
                {
					/* obj match with last_Received_obj_id */
                    //i32MatchOccur = OBJ_ACTIVE_STATE;
                    /* find successor of last_Received_obj_id */
                    pstLastReceiObjId = NULL;
                }
                else
                {
                    /* check if Event State is not equal Normal and Ack_transition which has at least one of its bit False */
                    i32GetEvtInfo = (((Schedule_t *)pvobjstruct)->m_i32GetSummary);
                    if((BIT_VALUE_CHECK(i32GetEvtInfo,7)) || (BIT_CHECK(i32GetEvtInfo,6)))
                    {
                        /* Fill ACK Para as object is active event State */
                        GEI_B_Fill_Event_Information_Data(
							OBJECT_SCHEDULE, &((Schedule_t *)pvobjstruct)->m_stObjectID,
                            pVirtualDev, pstGetEventInfoACK, &pstBaseNode, &pstPrevNode, pvobjstruct);
 
						/* validate pointer */
                        if(NULL == pstPrevNode)
                        {
							/* free allocated service data */														
							pProcQIndex->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;
							if(NULL != pstBackupReceiObjId)
							{
								OSAL_Free(pstBackupReceiObjId, __FILE__, __FUNCTION__, __LINE__);
							}
                            /* free all Active Event State */
							GEI_Free_Ack_Data(pstGetEventInfoACK);
							/* Update Error/Reject/Abort response for this request */
							Service_Error_Handler(BACNET_STATUS_ERROR, ERROR_CLASS_RESOURCES,
                                ERROR_CODE_OUT_OF_MEMORY, pProcQIndex, TRUE);  
                            return TRUE;
                        }
                    }
                }
            }
		/* move to next node */
        pvobjstruct = ((Schedule_t *)pvobjstruct)->pstNext;
        }/*while*/ 
#endif /* SDL */
        pstLastReceiObjId = NULL;
	}


    if(NULL == pstLastReceiObjId || pstLastReceiObjId->m_eObjectType == OBJECT_MULTI_STATE_VALUE)
    {
		/* check for multistate value */
#ifdef BACDEL_OBJ_MSV
        pvobjstruct = (MultistateValue_t *)(pVirtualDev->m_stSupportedObjects.m_pstMSValue); 
        /*check for all object of same type*/
        while(NULL != ((MultistateValue_t * )pvobjstruct))
        {
			#ifdef BACDEL_PR14
			/* check if event detection enable property value is true, then only go for below process */
			if(TRUE == ((MultistateValue_t * )pvobjstruct)->m_stEventDetectionEnable.m_bVal)
			#endif
			/* match obj id with last_Received_obj_id if present */
            if((NULL == pstLastReceiObjId || (pstLastReceiObjId->m_u32ObjId == ((MultistateValue_t * )pvobjstruct)->m_stObjectID.m_u32ObjId)))
            {/*if last_Received_obj_id present and match found, then find next object that would be successor of this obj*/
                if(NULL != pstLastReceiObjId)
                {   /* obj match with last_Received_obj_id*/
                    //i32MatchOccur = OBJ_ACTIVE_STATE;
                    /*find successor of last_Received_obj_id */
                    pstLastReceiObjId = NULL;
                }
                else
                {
                    /*check if Event State is not equal Normal and Ack_transition which has at least one of its bit False*/
                    i32GetEvtInfo = (((MultistateValue_t * )pvobjstruct)->m_i32GetSummary);
                    if((BIT_VALUE_CHECK(i32GetEvtInfo,7)) || (BIT_CHECK(i32GetEvtInfo,6)))
                    {
                        /*Fill ACK Para as object is active event State*/
                        GEI_B_Fill_Event_Information_Data(
							OBJECT_MULTI_STATE_VALUE,&((MultistateValue_t * )pvobjstruct)->m_stObjectID,
                            pVirtualDev,pstGetEventInfoACK, &pstBaseNode, &pstPrevNode, pvobjstruct);
                
                        if(NULL == pstPrevNode)
                        {
							/* free allocated service data */														
							pProcQIndex->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;
							if(NULL != pstBackupReceiObjId)
							{
								OSAL_Free(pstBackupReceiObjId,  __FILE__, __FUNCTION__, __LINE__);
							}
                            /* free all Active Event State */
							GEI_Free_Ack_Data(pstGetEventInfoACK);
							/* Update Error/Reject/Abort response for this request */
							Service_Error_Handler(BACNET_STATUS_ERROR, ERROR_CLASS_RESOURCES,
                                ERROR_CODE_OUT_OF_MEMORY, pProcQIndex, TRUE);  
                            return TRUE;
                        }
                    }
                }
            }
        pvobjstruct = ((MultistateValue_t * )pvobjstruct)->pstNext;
        }/*while*/
#endif
        pstLastReceiObjId = NULL;        
    }



    /* if last_Received_obj_id is present but match obj not in Active Event State */
    if(pstLastReceiObjId != NULL  &&  pstGetEventInfoACK->m_pstEventSummaryList == NULL)
	{
		/* set error */
		Exit_Routine_To_Error_Of_Confirmed_Request(pProcQIndex,
			BACNET_STATUS_ERROR, ERROR_CLASS_OBJECT,
			ERROR_CODE_UNKNOWN_OBJECT);
	}

	/* free memory allocated while decoding */
	if(NULL != pstBackupReceiObjId)
	{
		OSAL_Free(pstBackupReceiObjId,  __FILE__, __FUNCTION__, __LINE__);
	}

    /* save response data pointer */
    pProcQIndex->m_stProcessData.m_stAPDU.m_pvServiceRequestData = pstGetEventInfoACK;

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	GEI_B_Request_Parser: Exit \r\n");
	#endif
    return TRUE;
}

/**
*
* DESCRIPTION
* Function to encode the GEI acknowledgement. 
*
* @param pu8APDUResp	[in]    APDU buffer to save encoded data.
* @param i32APDULen		[in]    Length of encoded data in buffer.
* @param pstProcQInfo	[in]	Pointer to service data.
*
* @return i32APDULen	[out]	total length after encoding.
*
*/
int32_t GEI_B_Ack_Encoder(
	processInfo_t *pstProcQInfo,
	uint8_t **pu8APDUResp,
	int32_t i32APDULen)
{  
	/* local variables */
	event_info_response_t *pstEncodeEventInfoACK = NULL;
	event_summary_t *pstEventSummary = NULL; 
	uint8_t u8Count = 0;
	int32_t i32MaxResp = 0;
	uint32_t u32BufSize = MAX_APDU_LENGTH_ACCEPTED;
	uint8_t u8TempBuf[MAX_APDU_LENGTH_ACCEPTED] = {0};
	bool bCopyError = FALSE;
	bool bMoreEvents = FALSE;
	bool bSegmentedRespAccepted = FALSE;
	uint32_t u32Len = 0;
	uint32_t u32CopyLen = 0;	

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	GEI_B_Ack_Encoder: Entry \r\n");
	#endif

	/* check input pointers */
	if(NULL == pstProcQInfo || NULL == pu8APDUResp || NULL == *pu8APDUResp)
	{
		/* This should not occur ideally */
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
		GEI_B_Ack_Encoder: Null input pointers \r\n");
		#endif
		return 0;
	}

    /* get service response data pointer */
    pstEncodeEventInfoACK = pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData;
	if(NULL != pstEncodeEventInfoACK)
	{
		/* get pointer to 1st event summary */
		pstEventSummary = pstEncodeEventInfoACK->m_pstEventSummaryList;
		/* set to null */
		pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;
	}

	/* get max APDU length size & segmentation support of client device */
    i32MaxResp = pstProcQInfo->m_stProcessData.m_stAPDU.m_i32MaxResp;
	bSegmentedRespAccepted = pstProcQInfo->m_stProcessData.m_stAPDU.m_bSegmentedResponseAccepted;

    /* Encode opening tag 0E */
    i32APDULen += Encode_Opening_Tag(&(*pu8APDUResp)[i32APDULen], TAG_NO_0);

    /* Encode all Active Event States */ 
	while( NULL != pstEventSummary)
    {
		/* clear the local buffer */
		u32Len = 0;
		memset(u8TempBuf, 0, sizeof(u8TempBuf));

        /* Encode object ID */
        u32Len += Encode_Context_Object_Id(&u8TempBuf[u32Len], TAG_NO_0, 
			(int32_t)pstEventSummary->m_eObjectType, 
			pstEventSummary->m_u32ObjectInstance);

        /* Encode Event State */
        u32Len += Encode_Context_Enumerated(&u8TempBuf[u32Len], TAG_NO_1,
            pstEventSummary->m_eEventState);

        /* Encode Ack_Transition */
        u32Len += Encode_Context_BITSTRING(&u8TempBuf[u32Len], TAG_NO_2,
            &pstEventSummary->m_stAckTransition);

        /* Encode opening tag 3 for 3 Event time stamp */
        u32Len += Encode_Opening_Tag(&u8TempBuf[u32Len], TAG_NO_3);
        /* Encode time stamp array */
		for(u8Count = 0; u8Count < BACNET_ARRAY_OF_THREE; u8Count++)
		{
			u32Len += DT_Encode_TimeStamp(&u8TempBuf[u32Len],
				&pstEventSummary->m_stTimeStamp[u8Count].m_stTimeStamp,
				pstEventSummary->m_stTimeStamp[u8Count].m_eTimeStampType);
		}
        /* Encode closing tag 3 */
        u32Len += Encode_Closing_Tag(&u8TempBuf[u32Len], TAG_NO_3);

        /* Encode Notify Type */
        u32Len += Encode_Context_Enumerated(&u8TempBuf[u32Len], TAG_NO_4,
            pstEventSummary->m_eNotifyType);

        /* Encode Event Enable */
        u32Len += Encode_Context_BITSTRING(&u8TempBuf[u32Len], TAG_NO_5,
            &pstEventSummary->m_stEventEnable);

        /* Encode opening tag6 for Event priorty */
        u32Len += Encode_Opening_Tag(&u8TempBuf[u32Len], TAG_NO_6);
        /* Encode priorty 1 */
        u32Len += Encode_Application_Unsigned(&u8TempBuf[u32Len], 
			pstEventSummary->m_EventPriority[0]);
        /* Encode priorty 2 */
        u32Len += Encode_Application_Unsigned(&u8TempBuf[u32Len], 
			pstEventSummary->m_EventPriority[1]);
        /* Encode priorty 3 */
        u32Len += Encode_Application_Unsigned(&u8TempBuf[u32Len], 
			pstEventSummary->m_EventPriority[2]);
        /* Encode closing tag6 */ 
        u32Len += Encode_Closing_Tag(&u8TempBuf[u32Len], TAG_NO_6);

		/* copy data in actual buffer */
		#ifdef SEGMENTATION_SUPPORTED
		u32CopyLen = Memcopy_Dynamic_Mem_Expansion((void **)pu8APDUResp, &u8TempBuf[0],
			i32APDULen, u32Len, &u32BufSize, MAX_APDU_LENGTH_ACCEPTED);		
		#else
		u32CopyLen = Memcopy(*pu8APDUResp, &u8TempBuf[0], i32APDULen,
			u32Len, MAX_APDU_LENGTH_ACCEPTED);
		#endif

		/* verify that data was copied properly */
		if (u32CopyLen == 0 && u32Len != 0)
		{
			/* break from while loop */
			bCopyError = true;
			break;		
		}

		/* Note: 3 bytes are for encoding closing tag 0F & more events */
		if(((i32APDULen + u32Len + 3) > (uint32_t)i32MaxResp) && !bSegmentedRespAccepted)
		{
			/* segmentation not supported by client */
			/* set more events and send the response */
			bMoreEvents = TRUE;
			break;
		}

		/* add to total encoded length */
		i32APDULen += u32Len;

        /* move to next node */
		pstEventSummary = pstEventSummary->m_pstNextSummary;
    }

	/* Encode Closing tag 0F */
	i32APDULen += Encode_Closing_Tag(&(*pu8APDUResp)[i32APDULen], TAG_NO_0);

	/* check if more-events is to be encoded */
	if(bMoreEvents)
	{
		/* encode more events as TRUE */
		i32APDULen += Encode_Context_Boolean(&(*pu8APDUResp)[i32APDULen], 
			TAG_NO_1, TRUE);	 
	}
	else
	{
		/* encode more events as FALSE */
		i32APDULen += Encode_Context_Boolean(&(*pu8APDUResp)[i32APDULen], 
			TAG_NO_1, FALSE);
	}

	/* check if error in endocing response */
	if(bCopyError)
	{
		/* Set Error for this command */
		#ifdef SEGMENTATION_SUPPORTED
		Service_Error_Handler(BACNET_STATUS_ABORT, ERROR_CLASS_RESOURCES,
			ERROR_CODE_ABORT_BUFFER_OVERFLOW, pstProcQInfo, TRUE);
		#else
		Service_Error_Handler(BACNET_STATUS_ABORT, ERROR_CLASS_RESOURCES,
			ERROR_CODE_ABORT_SEGMENTATION_NOT_SUPPORTED, pstProcQInfo, TRUE);
		#endif
		/* encode abort pdu */
		i32APDULen = Abort_Encode_Apdu(pstProcQInfo, *pu8APDUResp);
	}

	/* free all Active Event State */
	GEI_Free_Ack_Data(pstEncodeEventInfoACK);

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	GEI_B_Ack_Encoder: Exit \r\n");
	#endif
	/* return encoded APDU length */
	return i32APDULen;
}

/**
*                                                                         
* DESCRIPTION                                                                          
* This function will fill active events data 
*      
* @param eObjectType		[in]  Object type
* @param u32InObjID			[in]  Object ID
* @param pVirtualDev		[in]  Pointer to Virtual Device.
* @param pstGetEventInfoACK [in]  Pointer to 1st active event data
*
* @return [out] pointer to 1st node in list.
*
*/
void GEI_B_Fill_Event_Information_Data(
	BACNET_OBJECT_TYPE eObjectType,
	Pr_BACnetObjId_t *pstInObjID, 
	virtualDevData_t *pVirtualDev, 
	event_info_response_t *pstGetEventInfoACK,
	event_summary_t **pstBaseNode,
	event_summary_t **pstPrevNode,
	void *pvObject)
{
	/* local variables */
    int8_t i8count = 0;
    Pr_BACnetNotifyPriority_t stPriority = {0};
    ulong32_t ul32OffsetAddr = 0;
    ulong32_t ul32BaseAddr = 0;
    uint32_t i32ObjNotificationClass = 0;
	#ifdef BACDEL_OBJ_NC
    Notificationclass_t	*pstNotificationClass = NULL;
	#endif
	event_summary_t *pstTempNode = NULL;
	Pr_BACnetEventTimeStamp_t *pstEventTimeStamp = NULL;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	GEI_B_Fill_Event_Information_Data: Entry \r\n");
	#endif

	/* allocate memory */
    pstTempNode = OSAL_Malloc(sizeof(event_summary_t),  
		__FILE__, __FUNCTION__, __LINE__);
    /* if return null pointer */
    if(NULL == pstTempNode)
	{
		/* set pointer to null o indicate error */
		*pstPrevNode = NULL;
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, "APDU: \
		GEI_B_Fill_Event_Information_Data: malloc failed \r\n");
		#endif
		return;
	}

    /* check if its first node */ 
    if(NULL == *pstBaseNode && NULL == *pstPrevNode )
    {
		/* 1st node in list */
		*pstBaseNode = pstTempNode;
		*pstPrevNode = *pstBaseNode;
		/* save the base pointer */
		pstGetEventInfoACK->m_pstEventSummaryList = pstTempNode;
    }
    else
    {
		/* save node to next of previous */
		(*pstPrevNode)->m_pstNextSummary = pstTempNode; 
		*pstPrevNode = pstTempNode;  

		/* set More-Events bit of previous active event state to true */
        pstGetEventInfoACK->m_bMoreEvent = TRUE; 
    }

    /* get base address of object */
    ul32BaseAddr = (ulong32_t)pvObject;

    /* object id */
    GET_NCPROP_OFFSET(eObjectType, m_stNotificationClass.m_u32Val, ul32OffsetAddr);
    memcpy(&(i32ObjNotificationClass), (void *)(ul32BaseAddr+ul32OffsetAddr),
        sizeof(uint32_t));

	#ifdef BACDEL_OBJ_NC
	/* check all NC objects */
    pstNotificationClass = ((virtualDevData_t *)pVirtualDev)->m_stSupportedObjects.
		m_pstNotificationClass;
    while(NULL != pstNotificationClass)
    {
		/* match notification class property value */
        if(pstNotificationClass->m_stNotificationClass.m_u32Val == i32ObjNotificationClass)
        {  
			for(i8count = 0; i8count < BACNET_ARRAY_OF_THREE; i8count++)
            {
				/* copy value */
                stPriority.m_u32Value[i8count] = 
					pstNotificationClass->m_stPriority.m_u32Value[i8count];
            }
        }
		/* move to next NC object */
        pstNotificationClass = pstNotificationClass->pstNext;
    }
	#endif
    
    /* object type */
	pstTempNode->m_eObjectType = eObjectType;

	/* object instance */
	pstTempNode->m_u32ObjectInstance = pstInObjID->m_u32ObjId;
    
    /* Event State */
    GET_EVENTSTATE_OFFSET(eObjectType, m_stEventState.m_eEventState, ul32OffsetAddr);
	memcpy((void *)&(pstTempNode->m_eEventState), 
		(void *)(ul32BaseAddr+ul32OffsetAddr), sizeof(BACNET_EVENT_STATE));

    /* ACK Transition */
    GET_INTRINSIC_OFFSET(eObjectType, m_stAckedTransitions.m_stBitStr, ul32OffsetAddr);
    memcpy((void *)&(pstTempNode->m_stAckTransition), 
		(void *)(ul32BaseAddr+ul32OffsetAddr), sizeof(BACnetBitStr_t));

    /* Event Time Stamp */
    GET_INTRINSIC_OFFSET(eObjectType, m_stEventTimeStamps, ul32OffsetAddr);
	pstEventTimeStamp = (Pr_BACnetEventTimeStamp_t *)(ul32BaseAddr+ul32OffsetAddr);
	for(i8count = 0; i8count < BACNET_ARRAY_OF_THREE; i8count++)
    {
		pstTempNode->m_stTimeStamp[i8count].m_eTimeStampType = 
			pstEventTimeStamp->m_eTimeStampType[i8count];	
		memcpy(&pstTempNode->m_stTimeStamp[i8count].m_stTimeStamp, 
			&pstEventTimeStamp->m_uEvntStamp[i8count], sizeof(timeStamp_u));
	}

    /* Notify Type */
    GET_INTRINSIC_OFFSET(eObjectType, m_stNotifyType.m_eNotifyType, ul32OffsetAddr);
	memcpy((void *)&(pstTempNode->m_eNotifyType), 
		(void *)(ul32BaseAddr+ul32OffsetAddr), sizeof(BACNET_NOTIFY_TYPE));

    /* Event Enable */
    GET_INTRINSIC_OFFSET(eObjectType, m_stEventEnable.m_stBitStr, ul32OffsetAddr);
    memcpy((void *)&(pstTempNode->m_stEventEnable), 
		(void *)(ul32BaseAddr+ul32OffsetAddr), sizeof(BACnetBitStr_t));

    /* Priority */
	for(i8count = 0; i8count < BACNET_ARRAY_OF_THREE; i8count++)
	{
		pstTempNode->m_EventPriority[i8count] = stPriority.m_u32Value[i8count];
	}

    /* set more events to false as this is last active state object */
    pstGetEventInfoACK->m_bMoreEvent = FALSE;

    /* set next ptr to NULL */
    pstTempNode->m_pstNextSummary = NULL;

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	GEI_B_Fill_Event_Information_Data: Exit \r\n");
	#endif
    /* return addres of 1st active event data */ 
    return;
}

#endif /* BACDEL_SER_AE_GEI_B */

