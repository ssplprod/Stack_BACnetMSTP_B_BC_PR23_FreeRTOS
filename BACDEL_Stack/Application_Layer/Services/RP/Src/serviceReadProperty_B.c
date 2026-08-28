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
*   SoftDEL Systems Ltd.                            india@softdel.com         
*   3rd Floor, Pentagon P4,                         http://www.softdel.com  
*    Magarpatta City, Hadapsar
*    Pune - 411 028                                
*                                                                         
*   FILE
*   serviceReadProperty_B.c
*                                                                      
*   AUTHORS                                                                     
*   Prashant Badgujar, Harshal Mangale, M. venu
*                                                                         
*   DESCRIPTION
*   The ReadProperty service is used by a client BACnet-user to request 
*	the value of one property of one BACnet Object.
*   This file include function for RP-B service. 
*                                                                         
**********************************************************************************/

#ifdef BACDEL_SER_DS_RP_B

/* header files */
#include "osalFreeRTOS.h"
#include "serviceReadProperty_B.h"
//#include "Debug.h"
#include "miscMiscellaneous.h"
#include "bacnetAPDUHandler.h"
#include "bacnetStackMgmt.h"
#include "serviceGenericHandler.h"
#include "pduEncodeDecode.h"
#include "pduDataEncodeDecode.h"
#include "pduError.h"
#include "pduAbort.h"
#include "bacnetDeviceMgmtInterface.h"

#include "propertyClearValues.h"
#include "propertyValueRead.h"

/* to include thread pool functionality */
#ifdef THREAD_POOL
#include "bacnetThreadPoolMgmt.h"
#endif

/**
*                                                                    
* DESCRIPTION
* This function is default handler when a read property  
* request is received.
*    
* @param pstProcQInfo		 [in]	 Pointer to save decoded data.
* @param pu8ServiceRequest	 [in]    Data received for decoding.
* @param i32ServiceLen		 [in]    Length of service request.
*                                   
* @return BACNET_RETURN_TYPE [out]   Success or suitable error code.
*	
*/
BACNET_RETURN_TYPE RP_B_Decode_Handler(
    processInfo_t *pstProcQInfo,
    uint8_t *pu8ServiceRequest,
    int32_t i32ServiceLen)
{
	/* local variables */
    int32_t i32Len = 0;
    BACNET_CONF_DATA *pstRpData = NULL;
    BACNET_RETURN_TYPE eReturnType = BACDEL_SUCCESS;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: RP_B_Decode_Handler: entry \r\n");
	#endif
    
    if( pstProcQInfo != NULL && pu8ServiceRequest != NULL )
    {
        /* Allocate buffer for read property data */
        pstRpData = (BACNET_CONF_DATA *)OSAL_Malloc( sizeof(BACNET_CONF_DATA),  __FILE__, __FUNCTION__, __LINE__);

        if( pstRpData == NULL )
        {
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: RP_B_Decode_Handler : Malloc failed for RP_B param\r\n");
			#endif

            /* Set Error for this command*/
            Service_Error_Handler( BACNET_STATUS_ERROR, ERROR_CLASS_RESOURCES,
                                    ERROR_CODE_OUT_OF_MEMORY, pstProcQInfo, TRUE);

            return BACDEL_SUCCESS;
        }

        memset(pstRpData, 0, sizeof(BACNET_CONF_DATA));
    
        /* Decode the Service Request */
        i32Len = RP_B_Decode_Service_Request(pu8ServiceRequest, i32ServiceLen, 
                                            pstRpData);
        if (i32Len <= 0) 
        {
            /* Set Error for this command*/
            Service_Error_Handler( i32Len, pstRpData->eErrorClass,
                                    pstRpData->eErrorCode, pstProcQInfo, TRUE);

            /* Free memory for RP service parameters */
            if( NULL != pstRpData)
                OSAL_Free(pstRpData,  __FILE__, __FUNCTION__, __LINE__);
            
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: RP_B_Decode_Handler: Error in decoding RP-B service \r\n");
			#endif

            return BACDEL_SUCCESS;
        }
        else
        {
            /* Redirect the service data to Read property data buffer */
            pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData = pstRpData;

            /* Call Object Management Layer interface */
            eReturnType = Obj_MgmtLayer_Interface(pstProcQInfo);
            if( BACDEL_CONTINUE != eReturnType)
            {
                /* Free memory for RP service parameters */
                if(NULL != pstRpData)
                    OSAL_Free(pstRpData,  __FILE__, __FUNCTION__, __LINE__);

                pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;
				Service_Error_Handler( BACNET_STATUS_ABORT, 0, 
					ERROR_CODE_ABORT_OUT_OF_RESOURCES, pstProcQInfo, TRUE);

				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: RP_B_Decode_Handler: Obj_MgmtLayer_Interface returns Error \r\n");
				#endif

                return BACDEL_SUCCESS;
            }
        }
    }
    else
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: RP_B_Decode_Handler : NULL Input Pointers\r\n");
		#endif

        return BACDEL_ERROR;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: RP_B_Decode_Handler: exit\r\n"); 
	#endif

    return eReturnType;
}




/**
*                                                                    
* DESCRIPTION
* Function to decoded read property service request data.
*    
* @param pu8APDUReq	[in]  Data to be decoded.
* @param i32APDULen	[in]  Length of data to be decoded.
* @param pstRpData	[out] Pointer to save Rp-B data.
*                                   
* @return [out] length of data decoded or -ve value on error.
*	
*/
int32_t RP_B_Decode_Service_Request(
    uint8_t * pu8APDUReq,
    int32_t i32APDULen,
    BACNET_CONF_DATA *pstRpData)
{
    int32_t i32Len = 0;
    int32_t i32DecodeLen = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: RP_B_Decode_Service_Request: Entry\r\n");
	#endif

    /* check for value pointers */
    if((NULL != pstRpData) && (NULL != pu8APDUReq) )
    {
        /* Must have at least 2 tags, an object id and a property identifier
         * of at least 1 byte in length to have any chance of parsing */
        if(i32APDULen < 7) 
        {
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: RP_B_Decode_Service_Request: missing parameter\r\n");
			#endif

            pstRpData->eErrorCode = ERROR_CODE_REJECT_MISSING_REQUIRED_PARAMETER;
            return BACNET_STATUS_REJECT;
        }

        /* Decode Object Identifier, Property Id and Array Index */
        i32DecodeLen = Decode_ObjId_Prop_ArrIdx( &pu8APDUReq[i32Len], (int16_t)(i32APDULen - i32Len),
                                            pstRpData);
        if(i32DecodeLen < 0)
		{
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: RP_B_Decode_Service_Request: Error in Decoding of ObjId, PropId, ArrIndx\r\n");
			#endif

            return i32DecodeLen;
		}

        i32Len += i32DecodeLen;

    }

    if (i32Len != i32APDULen && NULL != pstRpData) 
    {
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: RP_B_Decode_Service_Request: Invalid tag\r\n");
		#endif

        /* If something left over now, we have an invalid request */
        pstRpData->eErrorCode = ERROR_CODE_REJECT_INVALID_TAG;
        return BACNET_STATUS_REJECT;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: RP_B_Decode_Service_Request: Exit\r\n");
	#endif

    return i32Len;
}

/**
*
* DESCRIPTION
* Function to encode the RP-B acknowledgement. 
*
* @param pstProcQInfo	[in]	Pointer to service data
* @param pu8APDUResp	[in]    APDU buffer to save encoded data.
* @param i32APDULen		[in]    Length of encoded data in buffer.
*
* @return i32APDULen    [out]   Total length after encoding.
*
*/
int32_t RP_B_Ack_Encoder(
    processInfo_t *pstProcQInfo,
    uint8_t **pu8APDUResp,
    int32_t i32APDULen)
{
    /* Local variables */
    BACNET_CONF_DATA *pstRpData = NULL;
    uint32_t u32CopyLen = 0;
    /* this array size is linkes with input_buffer_size parameter of BACApp_Encode_Data_Type */
    uint8_t u8TempBuf[MAX_APDU_BUFFER] = { 0 }; 
    bool bCopyError = false;
    #ifdef SEGMENTATION_SUPPORTED
	uint32_t u32BufferSize = MAX_APDU_BUFFER;
    #endif 
    virtualDevData_t *pVirtualDev = NULL;
    
	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: RP_B_Ack_Encoder: Entry\r\n");
	#endif

    pstRpData = (BACNET_CONF_DATA *)pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData;
    pVirtualDev = pstProcQInfo->m_pvReqDevStruct;

    if( pstRpData == NULL || pVirtualDev == NULL)
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: RP_B_Ack_Encoder : NULL Input Pointers\r\n");
		#endif
        return 0;
    }

    /* Check for Error while reading value in ObjMgmtLayer */
    if(pstRpData->bErrorStatus)
    {
         Service_Error_Handler( BACNET_STATUS_ERROR, pstRpData->eErrorClass,
                        pstRpData->eErrorCode, pstProcQInfo, TRUE);

        i32APDULen = Error_Encode_Apdu(pstProcQInfo, *pu8APDUResp);
    }
    else
    {
        /* Encode Obj_ID, PropID, PropIndex, AppOpening _Tag */
        i32APDULen += Encode_ObjId_Prop_ArrIdx(&(*pu8APDUResp)[i32APDULen], pstRpData);

        /* Encode Opening Tag for Value */ 
        i32APDULen += Encode_Opening_Tag(&(*pu8APDUResp)[i32APDULen], TAG_NO_3);

        /* Encode Appliaction data according to data type */
        if(pstRpData->pvReadPropValue )
        {
            /* Function call which returns encoded application data string*/
            /*  (TAG + property value)*/	
            pstRpData->i32ApplicationDataLen = 																		
            BACApp_Encode_Data_Type(&u8TempBuf[0], 
                                    MAX_APDU_BUFFER,
                                    pstRpData->pvReadPropValue,
                                    pstRpData->eData_Type,
                                    pstRpData->u32ArrayIndex,
                                    0,
                                    pVirtualDev->m_stDevObject.
                                    m_stSegmentationSupport.m_eSegmentationSupport,
                                    pstRpData->bArrIndxPresent,
                                    pVirtualDev->m_stDevObject.
                                    m_stMaxAPDULenAccepted.m_u16Val, NULL, NULL, 0);
        
            /* If data type is not supported*/
            if(pstRpData->i32ApplicationDataLen == DATA_TYPE_NOT_SUPPORTED)
            {
                pstRpData->eErrorClass = ERROR_CLASS_PROPERTY;
                pstRpData->eErrorCode = ERROR_CODE_DATATYPE_NOT_SUPPORTED;
                Service_Error_Handler( BACNET_STATUS_ERROR, pstRpData->eErrorClass,
                            pstRpData->eErrorCode, pstProcQInfo, TRUE);

                i32APDULen = Error_Encode_Apdu(pstProcQInfo, *pu8APDUResp);
            }
            else if(pstRpData->i32ApplicationDataLen == SEGMENTATION_NOT_SUPPORTED)
            {
                pstRpData->eErrorClass = ERROR_CLASS_PROPERTY;
                pstRpData->eErrorCode = ERROR_CODE_ABORT_SEGMENTATION_NOT_SUPPORTED;
                Service_Error_Handler( BACNET_STATUS_ABORT, pstRpData->eErrorClass,
                            pstRpData->eErrorCode, pstProcQInfo, TRUE);

                i32APDULen = Abort_Encode_Apdu(pstProcQInfo, *pu8APDUResp);
            }
			else if(pstRpData->i32ApplicationDataLen == APDU_DATA_BUFFER_SIZE_EXCEEDED)
			{
				pstRpData->eErrorClass = ERROR_CLASS_PROPERTY;
                pstRpData->eErrorCode = ERROR_CODE_ABORT_BUFFER_OVERFLOW;
                Service_Error_Handler( BACNET_STATUS_ABORT, pstRpData->eErrorClass,
                            pstRpData->eErrorCode, pstProcQInfo, TRUE);

                i32APDULen = Abort_Encode_Apdu(pstProcQInfo, *pu8APDUResp);
			}
            else
            {
                /* Copy the encoded data */
#ifdef SEGMENTATION_SUPPORTED
            	u32CopyLen = Memcopy_Dynamic_Mem_Expansion((void **)pu8APDUResp, &u8TempBuf[0], i32APDULen,
			                    pstRpData->i32ApplicationDataLen, &u32BufferSize, 
                                MAX_APDU_LENGTH_ACCEPTED);
#else
            	u32CopyLen = Memcopy(*pu8APDUResp, &u8TempBuf[0], i32APDULen,
                                pstRpData->i32ApplicationDataLen, MAX_APDU_LENGTH_ACCEPTED);
#endif 
                if (u32CopyLen == 0 && pstRpData->i32ApplicationDataLen)
                    bCopyError = true;

                /* Get the total encoded data length */
                i32APDULen += pstRpData->i32ApplicationDataLen;

                /* Encode the application closing tag */
                i32APDULen += Encode_Closing_Tag(&(*pu8APDUResp)[i32APDULen], TAG_NO_3);
            }
        }
		else
			/* Encode the application closing tag */
			i32APDULen += Encode_Closing_Tag(&(*pu8APDUResp)[i32APDULen], TAG_NO_3);
    }

	/* check if error in endocing response */
    if(bCopyError)
    {
        /* Error occured during memcopy */
        pstRpData->eErrorClass = ERROR_CLASS_RESOURCES;
		#ifdef SEGMENTATION_SUPPORTED
        pstRpData->eErrorCode = ERROR_CODE_ABORT_BUFFER_OVERFLOW;
		#else
		pstRpData->eErrorCode = ERROR_CODE_ABORT_SEGMENTATION_NOT_SUPPORTED;
		#endif
        Service_Error_Handler( BACNET_STATUS_ABORT, pstRpData->eErrorClass,
                    pstRpData->eErrorCode, pstProcQInfo, TRUE);
        /* Encode abort APDU */
        i32APDULen = Abort_Encode_Apdu(pstProcQInfo, *pu8APDUResp);
    }

     /* Free malloc memory for RP-B service */
    if( NULL != pstRpData )
	{
		/* free memory allocated to copy data */
		Clear_Duplicate_Prop_Values(pstRpData->eData_Type, &pstRpData->pvReadPropValue);

        OSAL_Free(pstRpData,  __FILE__, __FUNCTION__, __LINE__);		
	}

    pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: RP_B_Ack_Encoder: Exit\r\n");
	#endif
    return i32APDULen;
}

/**
*
* DESCRIPTION                                                                          
* Function to validate and execute read property service.
*    
* @param pVirtualDev	[in]  virtual device data.
* @param pstProcQInfo   [in]  rp-b service data 
* @param u8ThreadNo     [in]  thread number
*                                   
* @return [out] true or false.
*
*/
bool RP_B_Request_Parser(
	virtualDevData_t *pVirtualDev, 
	processInfo_t *pstProcQInfo,
	uint8_t u8ThreadNo)
{
	/* local variables */
	BACNET_CONF_DATA *pstRpData = NULL;	
	rp_request_t stRpRequest = {0};
	BACnetAddress_t stRmtDevAddr = {0};
	BACNET_CALLBACK_CONFIG_TYPE eCallbackConfig = CALLBACK_CONFIG_NOT_REQUIRED;
	BACNET_RETURN_TYPE eReturnType = BACDEL_SUCCESS;
	uint32_t u32ErrorCode = 0;
	BACNET_ERROR_CLASS eErrorClass = MAX_BACNET_ERROR_CLASS;
	BACNET_PDU_TYPE ePDUType = PDU_TYPE_CONFIRMED_SERVICE_REQUEST;	
	uint32_t u32CallbackId = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	RP_B_Request_Parser: Entry \r\n");     
	#endif

	/* check input pointers */
	if(NULL == pVirtualDev || NULL == pstProcQInfo)
	{
		/* This should not occur ideally */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
		RP_B_Request_Parser: Null input pointers \r\n");
		#endif
		if(NULL != pstProcQInfo)
		{
			/* set error code */
			pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode = 
				ERROR_CODE_INTERNAL_ERROR;
			pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorClass = 
				ERROR_CLASS_DEVICE;
			pstProcQInfo->m_stProcessData.m_ePDUType = 
				PDU_TYPE_ERROR;
		}
		return TRUE;
	}

	/* default response pdu type */
	pstProcQInfo->m_stProcessData.m_ePDUType = PDU_TYPE_COMPLEX_ACK;

	/* get Rp service data pointer */
	pstRpData = pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData;
	if(NULL == pstRpData)
	{
		/* property validation fails */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
		RP_B_Request_Parser: Rp property validation failed \r\n");
		#endif

		/* set error parameters */
		Exit_Routine_To_Error_Of_Confirmed_Request(pstProcQInfo, 
			BACNET_STATUS_ERROR, ERROR_CLASS_RESOURCES, 
			ERROR_CODE_OTHER);
		return TRUE;
	}

	/* validate RP/RPM request for property validations */
	if(!Validate_ObjId_PropId_ArrayIndx(pstRpData, pVirtualDev))		
	{
		/* property validation fails */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
		RP_B_Request_Parser: Rp property validation failed \r\n");
		#endif
		/* Note that error code is set in validation function itself */
		return TRUE;
	}

	#if (CALLBACK_BEFORE_EXECUTION == CALLBACK_CONFIG_READ_PROPERTY)
	if(NULL == afpApplicationFunctionCb[APP_CB_READ_PROPERTY] ||
		pstRpData->bIsApplicationReq)
	{
		/* if callback is not registered for RP service */
		/* or if call from VD thread i.e. application */
		#ifdef DEBUG_PRINT
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
		RP_B_Request_Parser: Callback not registered \r\n");
		#endif
	}
	else
	{
		/* check if callback is required for property */
		eCallbackConfig = GetPropertyRpCallback(pstRpData->eObjectType,					
			pstRpData->eObjectProperty);		

		if(CALLBACK_CONFIG_REQUIRED == eCallbackConfig)
		{
			/* copy data to service request for callback */
			stRpRequest.m_eObjectType = pstRpData->eObjectType;
			stRpRequest.m_u32ObjectInstance = pstRpData->u32ObjectInstance;
			stRpRequest.m_eObjectProperty = pstRpData->eObjectProperty;
			stRpRequest.m_u32ArrayIndex = pstRpData->u32ArrayIndex;
			stRpRequest.m_u8ArrayIndexPresent = (uint8_t)pstRpData->bArrIndxPresent;

			/* copy address */
			memcpy(&stRmtDevAddr, &pstProcQInfo->m_stProcessData.m_stRmDvAddr, 
				sizeof(BACnetAddress_t));

			/* generate a new callback id */
			u32CallbackId = Generate_Callback_ID();

			/* call callback function */
			eReturnType = afpApplicationFunctionCb[APP_CB_READ_PROPERTY](
				pVirtualDev->m_stDevObject.m_stObjectID.m_u32ObjId,
				u32CallbackId,
				SERVICE_SUPPORTED_READ_PROPERTY,
				&eErrorClass,
				&u32ErrorCode,
				&ePDUType,
				&stRpRequest,
				NULL,
				NULL,
				&stRmtDevAddr,
				NULL);

			/* check callback returm value */
			if(BACDEL_SUCCESS != eReturnType)
			{				
				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
				RP_B_Request_Parser: callback returns error \r\n");
				#endif	

				/* fill error parameters */
				Set_Error_Parameters_Of_Confirmed_Req(pstProcQInfo, 
					ePDUType, eErrorClass, u32ErrorCode);
				return TRUE;
			}
		}
	}
	#endif /* CALLBACK_BEFORE_EXECUTION */

	/* get the property value for encoding */
	Read_All_Property_Value(pstRpData, pVirtualDev);  

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	RP_B_Request_Parser: Exit \r\n");     
	#endif
	return TRUE;
}

#endif /* BACDEL_SER_DS_RP_B */
