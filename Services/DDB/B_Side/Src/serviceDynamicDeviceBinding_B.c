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
*   File Name - serviceDynamicDeviceBinding_B.c
*
*   AUTHORS
*   M. Venu, Ashish V., Anagha C.
*       
*   DESCRIPTION
*	File includes all functions required for DDB-B functionality.
*   - Who-Is request decoder.
*   - Who-Is request parser.
*   - I-Am request encoder.
*    
******************************************************************************/

#ifdef BACDEL_SER_DM_DDB_B

/* include files */
#include "serviceDynamicDeviceBinding_B.h"
#include "pduEncodeDecode.h"
#include "bacnetDeviceMgmtInterface.h"
#include "bacnetAPDUHandler.h"

/* to include MSTP layer functions */

#include "datalinkMSTP.h"

extern DB_t SMCfg;
/** 
*
* DESCRIPTION
* Decodes the Who-Is request.
*
* @param  pstProcQIndx  [in] Pointer to save decoded data.
* @param  pu8APDU		[in] Buffer containing data to be decoded.
* @param  i32APDULen	[in] Length of received request data.
*
* @return BACNET_RETURN_TYPE values.
*                                                                      
*/
BACNET_RETURN_TYPE Who_Is_B_Decode_Handler(
	processInfo_t *pstProcQIndx,
    uint8_t *pu8APDU, 
	int32_t i32APDULen)
{
    /* Local variables */
	ddb_who_is_t *pstWhoIsData = NULL;
    BACNET_RETURN_TYPE eReturnType = BACDEL_ERROR;
    int32_t i32BytesConsumed = 0;
	int32_t i32Len = 0;
    uint32_t u32DecodedValue = 0;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	Who_Is_B_Decode_Handler: entry \r\n");
	#endif
	
    /* Check if frame is truncated */
    if (i32APDULen < BACNET_ZERO)
	{
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_TRUNCATED_DATA_FRAME, "APDU: \
		Who_Is_B_Decode_Handler: truncated data \r\n");
		#endif
        return BACDEL_TRUNCATED_DATA_FRAME;
	}

    /* Allocate buffer for WHO IS service data */
	pstWhoIsData = (ddb_who_is_t *)OSAL_Malloc(
		sizeof(ddb_who_is_t), __FILE__, __FUNCTION__, __LINE__);
    /* Check if memeory is allocated */
    if(NULL == pstWhoIsData)
    {
		#if(defined DEBUG_PRINTF && DL_0)
        Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, "APDU: \
		Who_Is_B_Decode_Handler: malloc failed \r\n");
		#endif
        return BACDEL_MALLOC_ERROR;
    }

    /* Check if Device Instance range limit is present */
	if(i32APDULen > BACNET_ZERO)
	{
		/* Decode Device Instance Low Limit */
        i32BytesConsumed = Decode_Context_Unsigned(
			&pu8APDU[i32Len], TAG_NO_0, &u32DecodedValue);
        /* Check if there is some error during extraction */
        if(i32BytesConsumed < 0)
        {
			#if(defined DEBUG_PRINTF && DL_1)
			Print_DebugMsg(DEBUG_LEVEL1, BACDEL_TAG_ERROR, "APDU: \
			Who_Is_B_Decode_Handler: error decoding low limit \r\n");
			#endif
            /* free service data */
			OSAL_Free(pstWhoIsData, __FILE__, __FUNCTION__, __LINE__);
            return BACDEL_TAG_ERROR;
        }
        else
        {
           i32Len += i32BytesConsumed;
        }

		/* validate received value */
        if(u32DecodedValue > BACNET_MAX_INSTANCE)
		{
			/* free service data */
			OSAL_Free(pstWhoIsData, __FILE__, __FUNCTION__, __LINE__);
            return BACDEL_MAX_INSTANCE_ERROR;
		}
		/* Assign value to WHO IS service data */
		pstWhoIsData->m_i32DevRangeLowLimit = u32DecodedValue;

		/* Decode Device Instance High Limit */
        i32BytesConsumed = Decode_Context_Unsigned(
			&pu8APDU[i32Len], TAG_NO_1, &u32DecodedValue);
        /* Check if there is some error during extraction */
        if(i32BytesConsumed < 0)
        {
			#if(defined DEBUG_PRINTF && DL_1)
			Print_DebugMsg(DEBUG_LEVEL1, BACDEL_TAG_ERROR, "APDU: \
			Who_Is_B_Decode_Handler: error decoding high limit \r\n");
			#endif
            /* free service data */
			OSAL_Free(pstWhoIsData, __FILE__, __FUNCTION__, __LINE__);
            return BACDEL_TAG_ERROR;
        }
        else
        {
           i32Len += i32BytesConsumed;
        }  
		
		/* validate received value */
        if(u32DecodedValue > BACNET_MAX_INSTANCE)
		{
			/* free service data */
			OSAL_Free(pstWhoIsData, __FILE__, __FUNCTION__, __LINE__);
            return BACDEL_MAX_INSTANCE_ERROR;
			
		}
		/* Assign value to WHO IS service data */
        pstWhoIsData->m_i32DevRangeHighLimit = u32DecodedValue;
	}
    /* Device Instance range limit is not present */
	else 
	{
		/* Assign default value to WHO IS service data */
        pstWhoIsData->m_i32DevRangeLowLimit = BACNET_PARAMETER_ABSENT;
        pstWhoIsData->m_i32DevRangeHighLimit = BACNET_PARAMETER_ABSENT;
	}

	/* save service data pointer */
	pstProcQIndx->m_stProcessData.m_stAPDU.m_pvServiceRequestData 
        = pstWhoIsData;

    /* Pass the data to Object Management Layer */
    eReturnType = Obj_MgmtLayer_Interface(pstProcQIndx);
    if(BACDEL_CONTINUE != eReturnType)
    {
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, eReturnType, "APDU: \
		Who_Is_B_Decode_Handler: Obj_MgmtLayer_Interface returns ERROR \r\n");
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
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	Who_Is_B_Decode_Handler: exit \r\n");
	#endif
    return eReturnType;
}


/**  
*
* DESCRIPTION
* Encodes the I-AM request.
*
* @param  pstProcQIndex	[in]	Pointer to service data.
* @param  pu8APDUResp	[out]   Pointer to Transmit buffer.
* @param  i32APDULen    [out]   Number of bytes encoded.
*
* @return i32APDULen    [out]   total bytes encoded. 
*
*/
int32_t I_Am_B_Request_Encoder(
   processInfo_t *pstProcQIndex,
   uint8_t *pu8APDUResp,
   int32_t i32APDULen)
{
    /* Local variables */
	ddb_i_am_t *pstIAMData = NULL;
	
	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	I_Am_B_Request_Encoder: entry \r\n");
	#endif
    
    /* get service data pointer */
    pstIAMData = (ddb_i_am_t *)pstProcQIndex->m_stProcessData.
        m_stAPDU.m_pvServiceRequestData;

	if(NULL == pstIAMData)
	{
		#if(defined DEBUG_PRINTF && DL_0)
        Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
		I_Am_B_Request_Encoder: Null service data pointer \r\n");
		#endif
		return 0;
	}

	/* Encode the Device Instance Number & Device Object Type */
	i32APDULen += Encode_Application_Object_Id(
		&pu8APDUResp[i32APDULen], 
		(int32_t)OBJECT_DEVICE,
		pstIAMData->m_u32DeviceInstance);

	/* Encode the Maximum APDU Length accepted */
	i32APDULen += Encode_Application_Unsigned(
		&pu8APDUResp[i32APDULen],
		pstIAMData->m_u16MaxAPDUlengthAccepted);

	/* Encode Segmentation property */
	i32APDULen += Encode_Application_Enumerated(
		&pu8APDUResp[i32APDULen],
		pstIAMData->m_eSegmentationSupported);

	/* Encode Vendor ID */
	i32APDULen += Encode_Application_Unsigned(
		&pu8APDUResp[i32APDULen],
		pstIAMData->m_u32VendorID);
	
	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	I_Am_B_Request_Encoder: exit \r\n");
	#endif

    /* Number of Bytes consumed */
    return i32APDULen;
}


/** 
*
* DESCRIPTION
* Executes Who-Is request.
* 
* @param  pstVirtualDev [in/out] virtual device data.
* @param  pstProcQInfo  [in/out] who-is service data.
* @param  u8ThreadNo    [in]     Thread number
*
* @return bool          [out]    True/false
*
*/
bool Who_Is_B_Request_Parser(
   virtualDevData_t* pstVirtualDev, 
   processInfo_t * pstProcQInfo,
   uint8_t u8ThreadNo)
{
    /* Local variables */
    BACNET_RETURN_TYPE eReturnType = BACDEL_SUCCESS;
    ddb_who_is_t *pstWhoIsData = NULL;
	ddb_who_is_t *pstTempWhoIsData = NULL;
	bool bRetVal = FALSE;
	
	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	Who_Is_B_Request_Parser: entry \r\n");
	#endif
    
	/* Null pointer check for input parameters */
	if(NULL == pstProcQInfo || NULL == pstVirtualDev)
	{
		/* This should not occur ideally */
		#if(defined DEBUG_PRINTF && DL_0)
        Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
		Who_Is_B_Request_Parser: Null input pointers \r\n");
		#endif
		return FALSE;
	}

    /* Extract the Device Address Range Parameter */
    pstWhoIsData = (ddb_who_is_t *)pstProcQInfo->
        m_stProcessData.m_stAPDU.m_pvServiceRequestData;
	pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;

	if(NULL == pstWhoIsData)
	{
		/* This should not occur ideally */
		#if(defined DEBUG_PRINTF && DL_0)
        Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
		Who_Is_B_Request_Parser: Null service data pointers \r\n");
		#endif
		return FALSE;
	}

	/* Check if device falls with range requested in request */
	eReturnType = Check_Device_Address_Range(pstWhoIsData,
		pstVirtualDev->m_stDevObject.m_stObjectID.m_u32ObjId);	

	if(BACDEL_OUT_OF_RANGE_ERROR == eReturnType)
	{
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_OUT_OF_RANGE_ERROR, "APDU: \
		Who_Is_B_Request_Parser: device range exceeds \r\n");
		#endif
		/* free service data */
		OSAL_Free(pstWhoIsData, __FILE__,__FUNCTION__,__LINE__);
		return FALSE;		
	}

	/* To reallocate the void pointer with different dyanmic structure, 
	 * free the earlier allocated heap memory.
	 */
	OSAL_Free(pstWhoIsData,  __FILE__, __FUNCTION__, __LINE__);

	#if ((!STACK_CUSTOMIZE_I_AM_B) || (STACK_DEBUG))

	/* Intiate to send I-AM response */
	/* Fill the I_AM structure with required device data */
	if(!I_Am_B_Fill_Structure(pstVirtualDev, pstProcQInfo))
	{
		/* return true to send I-Am */
		bRetVal = TRUE;
	}
	/* If some error occurs */
	else
	{
		#if(defined DEBUG_PRINTF && DL_2)
		Print_DebugMsg(DEBUG_LEVEL2, BACDEL_INFORMATIVE_MESSAGE, "APDU: \
		Who_Is_B_Request_Parser: error sending i-am \r\n");
		#endif			
		/* free service data */
		OSAL_Free(pstTempWhoIsData,__FILE__,__FUNCTION__,__LINE__);
		return bRetVal;
	}

	#endif /* STACK_CUSTOMIZE_I_AM_B */

	/* funcion exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	Who_Is_B_Request_Parser: exit \r\n");
	#endif
	return bRetVal;
}

#if ((!STACK_CUSTOMIZE_I_AM_B) || (STACK_DEBUG))
/**
*
* DESCRIPTION
* Fills the I_AM structure with required device data.
* 
* @param  pstVirtualDev  [in/out] virtual device data
* @param  pstProcQInfo   [in/out] service data pointer
*
* @return [out] Success or suitable error code.
*
*/
BACNET_RETURN_TYPE I_Am_B_Fill_Structure(
   virtualDevData_t* pstVirtualDev, 
   processInfo_t * pstProcQInfo)
{
    /* Local variables */
    ddb_i_am_t *pstIAMData = NULL;
	uint32_t u32Val = 0;

    /* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	I_Am_B_Fill_Structure: entry \r\n");
	#endif

    /* Update the Service Choice to I_AM */
	pstProcQInfo->m_stProcessData.m_eServiceSupported = SERVICE_SUPPORTED_I_AM;
    pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice = 
		SERVICE_UNCONFIRMED_I_AM;

    /* Allocate buffer for WHO IS service data */
	pstIAMData = (ddb_i_am_t *)OSAL_Malloc(sizeof(ddb_i_am_t),
        __FILE__, __FUNCTION__, __LINE__);
    if(pstIAMData == NULL )
    {
		#if(defined DEBUG_PRINTF && DL_0)
        Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, "APDU: \
		I_Am_B_Fill_Structure: malloc failed \r\n");
		#endif
        return BACDEL_MALLOC_ERROR;
    }

    /* attach the service data to I-AM data pointer */
	pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData = pstIAMData;

    /* MAX APDU Length Accepted */
    pstIAMData->m_u16MaxAPDUlengthAccepted = 
		pstVirtualDev->m_stDevObject.m_stMaxAPDULenAccepted.m_u16Val;

    /* Device Object ID */
	pstIAMData->m_u32DeviceInstance = 
		pstVirtualDev->m_stDevObject.m_stObjectID.m_u32ObjId;

    /* Segmentation supported */
    pstIAMData->m_eSegmentationSupported = pstVirtualDev->m_stDevObject.
		m_stSegmentationSupport.m_eSegmentationSupport;

    /* Device Vendor ID */
	pstIAMData->m_u32VendorID = 
		pstVirtualDev->m_stDevObject.m_stVendorId.m_u16Val;

   //if(SMCfg.g_ui8ComType==ect_BACnet_TP/*BACnetConfigData.m_eBACStack == BACNET_STACK_MSTP*/)
    {
	/* Set broadcast MSTP address */
	MSTP_GetBroadcastMacId(DESTINATION_IS_GLOBAL_BROADCAST,
		&pstProcQInfo->m_stProcessData.m_stRmDvAddr);
    }

    /* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	I_Am_B_Fill_Structure: exit \r\n");
	#endif
    return BACDEL_SUCCESS;
}
#endif /* ((!STACK_CUSTOMIZE_I_AM_B) || (STACK_DEBUG)) */
#endif /* BACDEL_SER_DM_DDB_B */
