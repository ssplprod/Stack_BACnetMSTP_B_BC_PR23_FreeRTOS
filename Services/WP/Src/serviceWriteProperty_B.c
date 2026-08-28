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
*   SoftDEL Systems Ltd.                             india@softdel.com         
*   3rd Floor, Pentagon P4,                          http://www.softdel.com  
*   Magarpatta City, Hadapsar
*   Pune - 411 028       
*
*   File Name
*	serviceWriteProperty_B.c
*
*   AUTHORS
*   Harshal Mangale, M. Venu, Anagha C.
*       
*   DESCRIPTION
*   The WriteProperty service is used by a client BACnet-user 
*   to modify the value of one property of a BACnet object.
*	This files includes functions for Write Property service.
*    
******************************************************************************/ 

#ifdef BACDEL_SER_DS_WP_B

/* header files */
#include "serviceWriteProperty_B.h"
#include "osalFreeRTOS.h"
//#include "Debug.h"
#include "miscMiscellaneous.h"
#include "bacnetDeviceMgmtInterface.h"
#include "pduEncodeDecode.h"
#include "pduDataEncodeDecode.h"
#include "pduError.h"
#include "propertyValueWrite.h"

/**
*
* DESCREPTION
* Function to decode WPM-B request received from network.
*
* @param pu8ServiceRequest  [in]  Data to be decoded.
* @param i32ServiceLen      [in]  Received data length.
* @param pstProcQInfo      [out]  Pointer to save WPM-B data
*
*/
BACNET_RETURN_TYPE WP_B_Decode_Handler(
	processInfo_t *pstProcQInfo,
	uint8_t *pu8ServiceRequest,
	int32_t i32ServiceLen)
{
	/* local variables */
    int32_t i32Len = 0;
    BACNET_CONF_DATA *pstWpData = NULL;
    BACNET_RETURN_TYPE eReturnType = BACDEL_SUCCESS;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	WP_B_Decode_Handler: entry \r\n");
	#endif

    if(NULL == pstProcQInfo || NULL == pu8ServiceRequest)
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
		WP_B_Decode_Handler: NULL Input Pointers \r\n");
		#endif
        return BACDEL_ERROR;
	}

    /* Allocate buffer for read property data */
    pstWpData = (BACNET_CONF_DATA *)OSAL_Malloc( sizeof(BACNET_CONF_DATA), 
		__FILE__, __FUNCTION__, __LINE__);

    if(NULL == pstWpData)
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
		WP_B_Decode_Handler: memory allocation failed \r\n");
		#endif

        /* set error parameters */
        Service_Error_Handler(BACNET_STATUS_ERROR, 
			ERROR_CLASS_RESOURCES, ERROR_CODE_OUT_OF_MEMORY, 
			pstProcQInfo, TRUE);
        return eReturnType;
    }

	/* decode WP header */
    i32Len = WP_B_Decode_Service_Request(
		pu8ServiceRequest, i32ServiceLen, pstWpData);

    /* check if data was decoded properrly */
    if(i32Len <= 0) 
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
		WP_B_Decode_Handler : Error in decoding WP-B service \r\n");
		#endif

        /* set error parameters */
        Service_Error_Handler(i32Len, pstWpData->eErrorClass,
            pstWpData->eErrorCode, pstProcQInfo, TRUE);
        /* free memory for WP service parameters */
        OSAL_Free(pstWpData,  __FILE__, __FUNCTION__, __LINE__);
        return eReturnType;
    }

    /* attach the pointer */
    pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData = pstWpData;

    /* call Object Management Layer interface */
    eReturnType = Obj_MgmtLayer_Interface(pstProcQInfo);
    
	if(BACDEL_CONTINUE != eReturnType)
    {
        /* free memory for WP service parameters */
        OSAL_Free(pstWpData,  __FILE__, __FUNCTION__, __LINE__);
        pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;
		/* Set Error for this command*/
        Service_Error_Handler(BACNET_STATUS_ABORT, 0,
            ERROR_CODE_ABORT_OUT_OF_RESOURCES, pstProcQInfo, TRUE);

		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
		WP_B_Decode_Handler: Obj_MgmtLayer_Interface returns error \r\n");
		#endif
		eReturnType = BACDEL_SUCCESS;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	WP_B_Decode_Handler: exit \r\n"); 
	#endif
    return eReturnType;
}

/**
*                                                                    
* DESCRIPTION                                                                          
* Function to encode WP-B service acknowledgement.
* Function encodes either simple ack or error ack.
*    
* @param pu8APDUResp   [in]  Buffer to save encoded data
* @param i32APDULen	   [in]  Encoded data length.
* @param pstProcQInfo  [in]  WP service data
*
* @return [out] total encoded data length
*                                                                      
*/
int32_t WP_B_Ack_Encoder(
	processInfo_t *pstProcQInfo,
	uint8_t *pu8APDUResp,
	int32_t i32APDULen)
{
	/* local variables */
    BACNET_CONF_DATA *pstWpData = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	WP_B_Ack_Encoder: entry \r\n");
	#endif

	/* get WP-B service data pointer */
    pstWpData = (BACNET_CONF_DATA *)pstProcQInfo->m_stProcessData.
		m_stAPDU.m_pvServiceRequestData;

	/* check the pointer */
    if(NULL == pstWpData)
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
		WP_B_Ack_Encoder : NULL Input Pointers \r\n");
		#endif
        return 0;
    }

    /* Check for Error while writing value in ObjMgmtLayer */
    if(pstWpData->bErrorStatus)
    {
		/* set error parameters */
		Service_Error_Handler( BACNET_STATUS_ERROR, 
			pstWpData->eErrorClass,
			pstWpData->eErrorCode, 
			pstProcQInfo, TRUE);

		/* encode error pdu */
        i32APDULen = Error_Encode_Apdu(pstProcQInfo, pu8APDUResp);
    }
    else
    {
        /* simple ack */
		;
    }
     /* free memory allocated for WP-B service */
    OSAL_Free(pstWpData,  __FILE__, __FUNCTION__, __LINE__);
    pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;
    
	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	WP_B_Ack_Encoder : exit \r\n");
	#endif
    return i32APDULen;
}

/** 
*
* DESCRIPTION:
* Function to decode received WP-B service request data.
*
* @param pu8APDUReq  [in]    WP-B data to be decoded
* @param i32APDULen  [in]    Length of received WP-B data
* @param pstWpData   [out]   Structure to store WP-B parameters.
*
* @return i32Len [out] Total decoded length of received request 
*
*/
int32_t WP_B_Decode_Service_Request(
    uint8_t *pu8APDUReq,
    int32_t i32APDULen,
    BACNET_CONF_DATA * pstWpData)
{
	/* local variables */
    int32_t i32Len = 0;
    int32_t i32DecodeLen = 0;
    uint32_t u32LenValueType = 0;
    uint32_t u32UnsignedValue = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	WP_B_Decode_Service_Request: entry \r\n"); 
	#endif

    /* check for value pointers */
    if(i32APDULen && (NULL != pstWpData) && (NULL != pu8APDUReq))
	{
        /* Decode Object Identifier, Property Id and Array Index */
        i32DecodeLen = Decode_ObjId_Prop_ArrIdx( 
			&pu8APDUReq[i32Len], (int16_t)i32APDULen, pstWpData);
        if(i32DecodeLen < 0)
		{
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
			WP_B_Decode_Service_Request: Error in Decoding \
			ObjId, PropId, ArrIndx \r\n"); 
			#endif
            return i32DecodeLen;
		}
        else
		{
			/* add decoded data length */
            i32Len += i32DecodeLen;
		}

       
		/* Tag 3: opening context tag */
        if(!Decode_Is_Opening_Tag_Number(&pu8APDUReq[i32Len], TAG_NO_3))
        {
            pstWpData->eErrorCode = ERROR_CODE_REJECT_INVALID_TAG;

			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
			WP_B_Decode_Service_Request: invalid opening tag \r\n"); 
			#endif

            return BACNET_STATUS_REJECT;
        }        

        /* Appln Tag: Property Value */
        /* determine the length of the data block */
        pstWpData->i32ApplicationDataLen =
            BACApp_Verify_Data_Len(&pu8APDUReq[i32Len], 
			(i32APDULen - i32Len),
            pstWpData->eObjectProperty);

        if(BACNET_STATUS_ERROR == pstWpData->i32ApplicationDataLen)
        {
            pstWpData->eErrorCode = ERROR_CODE_REJECT_INVALID_TAG;

			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
			WP_B_Decode_Service_Request: invalid property value data \r\n"); 
			#endif

            return BACNET_STATUS_REJECT;
        }

        /* a tag number of 3 is not extended so only one octet increment */
        i32Len++;
        
        pstWpData->pu8PropValueBuffer = 
            (uint8_t*) OSAL_Malloc( (pstWpData->i32ApplicationDataLen+1), 
			__FILE__, __FUNCTION__, __LINE__);

        if(NULL == pstWpData->pu8PropValueBuffer)
        {
            pstWpData->eErrorCode = ERROR_CODE_OUT_OF_MEMORY;

			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
			WP_B_Decode_Service_Request: memory allocation failed \r\n"); 
			#endif

            return BACNET_STATUS_ERROR;
        }

		/* copy data */
        memcpy(pstWpData->pu8PropValueBuffer, &pu8APDUReq[i32Len], 
			pstWpData->i32ApplicationDataLen);

        /* add on the data length */
        i32Len += pstWpData->i32ApplicationDataLen;
        
        
        /* Tag 3: Closing context tag */
        if(!Decode_Is_Closing_Tag_Number(&pu8APDUReq[i32Len++], TAG_NO_3))
        {
            pstWpData->eErrorCode = ERROR_CODE_REJECT_INVALID_TAG;

			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
			WP_B_Decode_Service_Request: invalid closing tag \r\n"); 
			#endif

            return BACNET_STATUS_REJECT;
        }

		/* Tag 4: optional Priority - assumed MAX if not explicitly set */
        pstWpData->u8Priority = BACNET_MAX_PRIORITY;
        if(i32Len < i32APDULen) 
        {
			/* decode priority */
            if(Decode_Context_Tag(&pu8APDUReq[i32Len], TAG_NO_4, 
											&u32LenValueType))
            {
                i32Len++ ; // tag decoded
                i32Len += Decode_Unsigned(&pu8APDUReq[i32Len], 
					u32LenValueType, &u32UnsignedValue);
                if ((u32UnsignedValue >= BACNET_MIN_PRIORITY) &&
                    (u32UnsignedValue <= BACNET_MAX_PRIORITY)) 
                {
                    pstWpData->u8Priority = (uint8_t)u32UnsignedValue;
                }
                else
                {
                    pstWpData->eErrorCode = 
							ERROR_CODE_REJECT_PARAMETER_OUT_OF_RANGE;

					#ifdef DEBUG_PRINTF
					Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
					WP_B_Decode_Service_Request: \
					priority value out of range \r\n"); 
					#endif

                    return BACNET_STATUS_REJECT;
                }
            }
            else
            {
                pstWpData->eErrorCode = ERROR_CODE_REJECT_INVALID_TAG;

				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
				WP_B_Decode_Service_Request: invalid priority tag \r\n"); 
				#endif

                return BACNET_STATUS_REJECT;
            }
        }
    }

	/* verify that complete data was decoded */
    if (i32Len != i32APDULen && NULL != pstWpData) 
    {
        /* If something left over now, we have an invalid request */
        pstWpData->eErrorCode = ERROR_CODE_REJECT_INVALID_TAG;

		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
		WP_B_Decode_Service_Request: invalid tag \r\n"); 
		#endif
        i32Len = BACNET_STATUS_REJECT;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	WP_B_Decode_Service_Request: exit \r\n"); 
	#endif
    return i32Len;
}

#endif /* BACDEL_SER_DS_WP_B */
