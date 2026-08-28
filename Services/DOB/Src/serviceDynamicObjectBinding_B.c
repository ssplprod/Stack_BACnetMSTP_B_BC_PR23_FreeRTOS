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
*                                                                        
*   SoftDEL Systems Ltd.                             india@softdel.com         
*   3rd Floor, Pentagon P4,                          http://www.softdel.com  
*   Magarpatta City, Hadapsar
*   Pune - 411 028       
*
*   File Name - serviceDynamicObjectBinding_B.c
*
*   AUTHORS                                                                     
*   Ashish V., Anagha C.    
*                                                                         
*   DESCRIPTION                                                            
*	File includes all functions required for DOB-B functionality.
*   - Who-Has request decoder.
*   - Who-Has request parser.
*   - I-Have request encoder. 
*
******************************************************************************/

#ifdef BACDEL_SER_DM_DOB_B

#include "osalFreeRTOS.h"
#include "serviceDynamicObjectBinding_B.h"
#include "pduEncodeDecode.h"
#include "bacnetDeviceMgmtInterface.h"
#include "miscMiscellaneous.h"
#include "propertyValueRead.h"
//#include "Debug.h"
#include "propertyGenricHandler.h"
#include "bacnetAPDUHandler.h"

extern DB_t SMCfg;


/** 
*
* DESCRIPTION
* Decodes the WHO-HAS request.
*
* @param  pstProcQIndx		 [in]  Pointer to save decoded data.
* @param  pu8APDU			 [in]  Buffer containing data to be decoded.
* @param  i32APDULen	     [in]  Length of received request data.
*
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*                                                                      
*/
BACNET_RETURN_TYPE Who_Has_B_Decode_Handler( 
	processInfo_t *pstProcQIndx,                                      
	uint8_t *pu8APDU, 
	int32_t i32APDULen)
{
    /* Local variables */
    uint8_t u8TagNumber = 0;
    int32_t i32Len = 0;
    uint32_t u32LenValue = 0;
    uint32_t u32DecodedValue = 0;
	dob_who_has_t *pstWhoHasData = NULL;
    BACNET_RETURN_TYPE eReturnType = BACDEL_ERROR;
    int32_t i32BytesConsumed = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Who_Has_B_Decode_Handler: entry \r\n");
	#endif
    
    /* Check if data is truncated */
    if (i32APDULen <= BACNET_ZERO)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
		Who_Has_B_Decode_Handler: truncated data \r\n");
		#endif
        return BACDEL_ERROR;
	}

    /* Allocate buffer for WHO HAS service data */
	pstWhoHasData = (dob_who_has_t *)OSAL_Malloc(sizeof(dob_who_has_t),
        __FILE__, __FUNCTION__, __LINE__);
    /* Check if memeory is allocated */
    if(NULL == pstWhoHasData)
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
		Who_Has_B_Decode_Handler: malloc failed \r\n");
		#endif
        return BACDEL_ERROR;
    }

    /* Assign default values */
    pstWhoHasData->m_stDevRange.m_i32DevRangeLowLimit = BACNET_PARAMETER_ABSENT;
    pstWhoHasData->m_stDevRange.m_i32DevRangeHighLimit = BACNET_PARAMETER_ABSENT;

    /* Decode Tag to check if Device Instance Range is present */
    i32BytesConsumed = Decode_Tag_Number_And_Value(&pu8APDU[i32Len],
        &u8TagNumber, &u32LenValue);
    /* If tag is not of type Device Instance Range, Object ID or 
     * Object Name return error */
	if((u8TagNumber != TAG_NO_0) && (u8TagNumber != TAG_NO_2)
		&& (u8TagNumber != TAG_NO_3))
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
		Who_Has_B_Decode_Handler: error decoding device info tag \r\n");
		#endif
		/* free service data */
		OSAL_Free(pstWhoHasData, __FILE__, __FUNCTION__, __LINE__);
        return BACDEL_ERROR;
    }

    /* If Tag is of type Device Instance Range Low Limit */
	if(TAG_NO_0 == u8TagNumber) 
    {
        /* Extract LOW Limit */
        i32BytesConsumed = Decode_Context_Unsigned(&pu8APDU[i32Len], 
			TAG_NO_0, &u32DecodedValue);
        /* If any error occurs while extracting */
        if(i32BytesConsumed < 0)
        {
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
			Who_Has_B_Decode_Handler: error decoding low limit tag \r\n");
			#endif
			/* free service data */
			OSAL_Free(pstWhoHasData, __FILE__, __FUNCTION__, __LINE__);
            return BACDEL_ERROR;;
        }
        else
        {
           i32Len += i32BytesConsumed;
        }
        /* Check if Low Limit within maximum range */
		if(u32DecodedValue > BACNET_MAX_INSTANCE) 
		{
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
			Who_Has_B_Decode_Handler:error decoding low limit \r\n");
			#endif
			/* free service data */
			OSAL_Free(pstWhoHasData, __FILE__, __FUNCTION__, __LINE__);
			return BACDEL_ERROR;
		}
        /* Assign value to WHO has service data */
        pstWhoHasData->m_stDevRange.m_i32DevRangeLowLimit = u32DecodedValue;

        /* Extract HIGH Limit */
        i32BytesConsumed = Decode_Context_Unsigned(&pu8APDU[i32Len], 
			TAG_NO_1, &u32DecodedValue);
        /* If any error occurs while extracting */
        if(i32BytesConsumed < 0)
        {
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer:\
			Who_Has_B_Decode_Handler: error decoding high limit tag\r\n");
			#endif
			/* free service data */
			OSAL_Free(pstWhoHasData, __FILE__, __FUNCTION__, __LINE__);
            return BACDEL_ERROR;
        }
        else
        {
           i32Len += i32BytesConsumed;
        }
		/* Check if High Limit within maximum range */
		if(u32DecodedValue > BACNET_MAX_INSTANCE) 
		{
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
			Who_Has_B_Decode_Handler: error decoding low limit \r\n");
			#endif
			/* free service data */
			OSAL_Free(pstWhoHasData, __FILE__, __FUNCTION__, __LINE__);
			return BACDEL_ERROR;
		}
        /* Assign value to WHO has service data */
        pstWhoHasData->m_stDevRange.m_i32DevRangeHighLimit = u32DecodedValue;

        /* Decode next tag */
        i32BytesConsumed = Decode_Tag_Number_And_Value(&pu8APDU[i32Len],
            &u8TagNumber, &u32LenValue);
	}

    /* If Tag is of type Object Identifier */
    if(TAG_NO_2 == u8TagNumber) 
    {
        pstWhoHasData->m_u8NotificationType = TRUE;
        /* Extract Object ID */
        i32BytesConsumed = Decode_Context_Object_Id(&pu8APDU[i32Len], TAG_NO_2,
			(uint32_t *)&pstWhoHasData->m_uObject.m_stObjectId.m_eObjectType,
			&pstWhoHasData->m_uObject.m_stObjectId.m_u32ObjectInstance);
        /* If any error occurs while extracting */
        if(i32BytesConsumed < 0)
        {
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
			Who_Has_B_Decode_Handler: error decoding object id tag \r\n");
			#endif
			/* free service data */
			OSAL_Free(pstWhoHasData, __FILE__, __FUNCTION__, __LINE__);
			return BACDEL_ERROR;
        }
        else
        {
            i32Len += i32BytesConsumed;
        }
    }
    /* Search by object Name */
    else if(TAG_NO_3 == u8TagNumber) 
    {
		/* decode tag */
        Decode_Tag_Number_And_Value(&pu8APDU[i32Len], &u8TagNumber, &u32LenValue);
        if(TAG_NO_3 != u8TagNumber || u32LenValue > MAX_SUPPORTED_CHRSTR_LEN)
        {
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
			Who_Has_B_Decode_Handler: error decoding object name tag \r\n");
			#endif
			/* free service data */
			OSAL_Free(pstWhoHasData, __FILE__, __FUNCTION__, __LINE__);
			return BACDEL_ERROR;
        }
        pstWhoHasData->m_u8NotificationType = FALSE;

        /* Extarct object name */
        i32BytesConsumed = Decode_Context_Character_String(&pu8APDU[i32Len], 
			TAG_NO_3, &pstWhoHasData->m_uObject.m_stObjName);

        /* If any error occurs while extracting */
        if(i32BytesConsumed < 0)
        {
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
			Who_Has_B_Decode_Handler: error decoding object name \r\n");
			#endif
			/* free service data */
			OSAL_Free(pstWhoHasData, __FILE__, __FUNCTION__, __LINE__);
			return BACDEL_ERROR;
        }
        else
        {
            i32Len += i32BytesConsumed;
        }

		/* validate character encoding */
		if(Validate_Charstring_Encoding(pstWhoHasData->m_uObject.m_stObjName.
			m_u8Encoding,pstWhoHasData->m_uObject.m_stObjName.m_u16CodePage))
        {
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
			Who_Has_B_Decode_Handler: Character encoding not supported \r\n");
			#endif
			/* free service data */
			OSAL_Free(pstWhoHasData, __FILE__, __FUNCTION__, __LINE__);
			return BACDEL_ERROR;
        }
    }

	/* Verify if the packet is completely decoded */
    if(i32APDULen != i32Len)
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
		Who_Has_B_Decode_Handler:truncated data\r\n");
		#endif
		/* free service data */
		OSAL_Free(pstWhoHasData, __FILE__, __FUNCTION__, __LINE__);
        return BACDEL_ERROR; 
    }

	/* Redirect the service data to WHO-HAS data buffer */
    pstProcQIndx->m_stProcessData.m_stAPDU.m_pvServiceRequestData 
        = pstWhoHasData; 

    /* Pass the data to Object Management Layer */
    eReturnType = Obj_MgmtLayer_Interface(pstProcQIndx);
    /* If return type is BACDEL_ERROR free heap memory */
    if(BACDEL_CONTINUE != eReturnType)
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
		Who_Has_B_Decode_Handler:Obj_MgmtLayer_Interface returns ERROR \r\n");
		#endif

		/* free service data */
		if(NULL != pstProcQIndx->m_stProcessData.m_stAPDU.
			m_pvServiceRequestData)
		{
			OSAL_Free(pstProcQIndx->m_stProcessData.
				m_stAPDU.m_pvServiceRequestData, 
				__FILE__,__FUNCTION__,__LINE__);
			pstProcQIndx->m_stProcessData.m_stAPDU.
				m_pvServiceRequestData = NULL;
		}
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Who_Has_B_Decode_Handler: exit \r\n");
	#endif
    return eReturnType;
}


/**
*
* DESCRIPTION
* Check if device falls within range to reply with an I-HAVE. 
* Intiate function call depending upon search of Object-Name or Object-ID. 
*
* @param  pstVirtualDev  [in]   Pointer to virtual device data structure.
* @param  pstProcQInfo   [in]   Pointer to process info queue structure.
* @param u8ThreadNo      [in]   Thread number
*
* @return bool           [out]  True/false
*
*/
bool Who_Has_B_Request_Parser( 
	virtualDevData_t *pstVirtualDev, 
	processInfo_t * pstProcQInfo,
	uint8_t u8ThreadNo)
{
    /* Local variables */
    dob_who_has_t *pstWhoHasData = NULL;
	BACNET_RETURN_TYPE eReturnType = BACDEL_SUCCESS;
	ddb_who_is_t stDevRange = {0};
	bool bRetVal = FALSE;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Who_Has_B_Request_Parser: entry \r\n");
	#endif

	/* Null pointer check for input parameters */
	if(NULL == pstProcQInfo || NULL == pstVirtualDev)
	{
		/* This should not occur ideally */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
		Who_Has_B_Request_Parser: Null input pointers \r\n");
		#endif
		return FALSE;
	}
    
    /* Extract Dynamic Object Binding structure */
    pstWhoHasData = pstProcQInfo->m_stProcessData.m_stAPDU.
        m_pvServiceRequestData;
	pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;
	if(NULL == pstWhoHasData)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
		Who_Has_B_Request_Parser: Null service data pointer \r\n");
		#endif
		return FALSE;
	}

	/* Copy range values */
	stDevRange.m_i32DevRangeHighLimit = 
		pstWhoHasData->m_stDevRange.m_i32DevRangeHighLimit;
	stDevRange.m_i32DevRangeLowLimit = 
		pstWhoHasData->m_stDevRange.m_i32DevRangeLowLimit;

	/* Check if device falls with range requested in request */
	eReturnType = Check_Device_Address_Range(&stDevRange, 
		pstVirtualDev->m_stDevObject.m_stObjectID.m_u32ObjId);

	if(BACDEL_OUT_OF_RANGE_ERROR == eReturnType)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
		Who_Has_B_Request_Parser: device range exceeds \r\n");
		#endif
		/* free service data */
		OSAL_Free(pstWhoHasData, __FILE__,__FUNCTION__,__LINE__);
		return FALSE;
	} 

	/* save service data pointer */
	pstProcQInfo->m_stProcessData.m_stAPDU.
		m_pvServiceRequestData = pstWhoHasData;

	/* search for the object within device */
	bRetVal = Who_Has_B_Search_Object(pstVirtualDev,
		pstWhoHasData->m_u8NotificationType, pstProcQInfo);

	/* free service data */
	OSAL_Free(pstWhoHasData, __FILE__,__FUNCTION__,__LINE__);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Who_Has_B_Request_Parser: exit \r\n");
	#endif
	return bRetVal;
}


/**
* DESCRIPTION
* Search for requested Object Name or OBject ID.
*
* @param pstVirtualDev  [in]	Pointer to virtual device data structure.
* @param bSearchElement [in]    Search for requested Object Name or OBject ID.				 
* @param  pstProcQInfo  [in]	Pointer to process info queue structure.
*
* @return bool          [out]   True/false
*
*/
bool Who_Has_B_Search_Object( 
	virtualDevData_t *pstVirtualDev, 
	bool bSearchElement, 
	processInfo_t * pstProcQInfo)
{
    /* Local variables */
    dob_who_has_t *pstWhoHas = NULL;
	dob_i_have_t  *pstIHAVEData = NULL;
    void *pvObjectPtr = NULL;
    bool bRetVal = FALSE;
	ulong32_t ulNextOffset = 0;
	uint16_t u16ObjectType = 0;
	uint32_t u32Val = 0;
	uint32_t u32ObjectInstance = 0;

    /* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Who_Has_B_Search_Object: entry \r\n");
	#endif
    
    /* Typecast the received data to access the Search Type */
    pstWhoHas = (dob_who_has_t *)pstProcQInfo->
		m_stProcessData.m_stAPDU.m_pvServiceRequestData;
	pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;

	/* Allocate buffer for service data */
	pstIHAVEData = (dob_i_have_t *)OSAL_Malloc(
		sizeof(dob_i_have_t), __FILE__, __FUNCTION__, __LINE__);
	if(NULL == pstIHAVEData)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
		Who_Has_B_Search_Object : Malloc failed \r\n");
		#endif
		return FALSE;
	}

	#if (!STACK_CUSTOMIZE_I_HAVE_B)

    /* Check if Search type is Object Identifier */
    if(bSearchElement)
    {
        /* Find given object type structure */
		pvObjectPtr = Find_Object(
			pstWhoHas->m_uObject.m_stObjectId.m_eObjectType,
			pstWhoHas->m_uObject.m_stObjectId.m_u32ObjectInstance, 
			pstVirtualDev);

        /* If Object is supported send I-Have */
        if(NULL != pvObjectPtr)
        {
			pstIHAVEData->m_eObjectType = pstWhoHas->m_uObject.
				m_stObjectId.m_eObjectType;
			pstIHAVEData->m_u32ObjectInstance = pstWhoHas->m_uObject.
				m_stObjectId.m_u32ObjectInstance;
            pstIHAVEData->m_u32DeviceInstance = pstVirtualDev->m_stDevObject.
				m_stObjectID.m_u32ObjId;

            /* Get the offset of Object Name in the Object structure */
            GET_ALLOBJ_OFFSET(																			
				pstWhoHas->m_uObject.m_stObjectId.m_eObjectType,
				m_stObjName.m_stCHARString, 
                ulNextOffset);

            memcpy(&pstIHAVEData->m_stObjName, 
				(BACnetCharStr_t *)(((ulong32_t)pvObjectPtr)+ulNextOffset), 
                sizeof(BACnetCharStr_t));

			/* To send I have response change service choice */
			pstProcQInfo->m_stProcessData.m_stAPDU.
				m_u8ServiceChoice = SERVICE_UNCONFIRMED_I_HAVE;

			/* To send I have response change service supported enum */
			pstProcQInfo->m_stProcessData.m_eServiceSupported = SERVICE_SUPPORTED_I_HAVE;

            /* make i-have service data pointer */
			pstProcQInfo->m_stProcessData.
				m_stAPDU.m_pvServiceRequestData = pstIHAVEData;

			/* set return value */
            bRetVal = TRUE;
        }
		else
		{
			/* object not found */
			OSAL_Free(pstIHAVEData, __FILE__, __FUNCTION__, __LINE__);
			/* set return value */
            bRetVal = FALSE;
		}
    }
    else
    {
		if(BACDEL_DUPLICATE_OBJECT_NAME == Check_Object_Name(
			pstVirtualDev->m_stDevObject.m_stObjectID.m_u32ObjId, 
			pstWhoHas->m_uObject.m_stObjName.m_pu8CharStr,
			&u16ObjectType, &u32ObjectInstance))
        {
			/* save i-have data */
			pstIHAVEData->m_eObjectType = u16ObjectType;
			pstIHAVEData->m_u32ObjectInstance = u32ObjectInstance;
            pstIHAVEData->m_u32DeviceInstance = pstVirtualDev->m_stDevObject.
				m_stObjectID.m_u32ObjId;			
            memcpy(&pstIHAVEData->m_stObjName, &pstWhoHas->m_uObject.
				m_stObjName, sizeof(BACnetCharStr_t));

			/* To send I have response change service choice */
            pstProcQInfo->m_stProcessData.m_stAPDU.
				m_u8ServiceChoice = SERVICE_UNCONFIRMED_I_HAVE;

			/* To send I have response change service supported enum */
			pstProcQInfo->m_stProcessData.m_eServiceSupported = SERVICE_SUPPORTED_I_HAVE;

            /* make i-have service data pointer */
            pstProcQInfo->m_stProcessData.m_stAPDU.
				m_pvServiceRequestData = pstIHAVEData;

			/* set return value */
            bRetVal = TRUE;
        }
        else
        {
			/* object not found */
			OSAL_Free(pstIHAVEData, __FILE__, __FUNCTION__, __LINE__);
			/* set return value */
            bRetVal = FALSE;
        }
    }
	#endif /* */

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Who_Has_B_Search_Object: exit \r\n");
	#endif
    return bRetVal;
}


/**  
*
* DESCRIPTION
* Encodes the I-HAVE response.
*
* @param  pstProcQIndex [in]  Pointer to processInfo_t structure.
*
* @param  pu8APDUResp   [out] Pointer to Transmit buffer.
* @param  i32APDULen    [out] Transmit buffer fill Index.
* @return				[out] Number of Bytes consumed. 
*
*/
int32_t I_Have_B_Request_Encoder(
	processInfo_t *pstProcQIndex,
	uint8_t *pu8APDUResp, 
	int32_t i32APDULen)
{
    /* Local variables*/
	dob_i_have_t *pstIHAVEData = NULL;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	I_Have_B_Request_Encoder: entry \r\n");
	#endif

    /* Typecast pointer to bacnetiamdata_t structure type */
    pstIHAVEData = (dob_i_have_t *)pstProcQIndex->m_stProcessData.
        m_stAPDU.m_pvServiceRequestData; 

	if(NULL != pstIHAVEData)
	{
		/* Encode the Device Instance Number & Device Object Type */
		i32APDULen += Encode_Application_Object_Id(&pu8APDUResp[i32APDULen], 
			(int32_t)OBJECT_DEVICE,
			pstIHAVEData->m_u32DeviceInstance);

		/* Encode the Object Instance Number & Object Object Type */
		i32APDULen += Encode_Application_Object_Id(&pu8APDUResp[i32APDULen], 
			(int32_t)pstIHAVEData->m_eObjectType,
			pstIHAVEData->m_u32ObjectInstance);

		/* Encode Object Name property */
		i32APDULen += Encode_Application_Character_String(&pu8APDUResp[i32APDULen], 
			&pstIHAVEData->m_stObjName);
	}
	
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	I_Have_B_Request_Encoder: exit \r\n");
	#endif
    return i32APDULen;
}
#endif /* BACDEL_SER_DM_DOB_B */
