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
*   File Name - serviceDynamicDeviceBinding_A.c
*
*   AUTHORS
*   Prashant Badgujar, Ashish V., Anagha C.
*       
*   DESCRIPTION
*	File includes all functions required for DDB-A functionality.
*   - Who-Is request encoder.
*   - I-Am request decoder.
*   - I-Am request parser.
*    
******************************************************************************/

#ifdef BACDEL_SER_DM_DDB_A

/* include files */
#include "serviceDynamicDeviceBinding_A.h"
#include "pduEncodeDecode.h"
#include "bacnetDeviceMgmtInterface.h"
#include "bacDELDeviceConfig.h"

/* Include MSTP layer functions */
#include "datalinkMSTP.h"

/** 
*
* DESCRIPTION
* Encodes the WHO-IS Request.
*
* @param  pstProcQIndex [in]    Pointer to service data.
* @param  pu8APDUResp   [out]	Pointer to Transmit buffer.
* @param  i32APDULen    [out]   Transmit buffer fill Index.
*
* @return Number of Bytes consumed. 
*
*/
int32_t Who_Is_A_Request_Encoder(
	processInfo_t *pstProcQIndex,
	uint8_t *pu8APDUResp, 
	int32_t i32APDULen)
{
    /* Local variables */
    ddb_who_is_t *pstWhoIsData = NULL;
	
	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	Who_Is_A_Request_Encoder: entry \r\n");
	#endif
   
    /* get service data pointer */
    pstWhoIsData = (ddb_who_is_t *)pstProcQIndex->m_stProcessData.
		m_stAPDU.m_pvServiceRequestData;

	if(NULL != pstWhoIsData)
	{
		/* Encode LowLimit & HighLimit device range parameter */
		if((pstWhoIsData->m_i32DevRangeLowLimit != BACNET_PARAMETER_ABSENT) 
			&& (pstWhoIsData->m_i32DevRangeHighLimit != BACNET_PARAMETER_ABSENT))
		{
			/* encode low limit */
			i32APDULen += Encode_Context_Unsigned(&pu8APDUResp[i32APDULen], 
				TAG_NO_0, pstWhoIsData->m_i32DevRangeLowLimit);

			/* encode high limit */
			i32APDULen += Encode_Context_Unsigned(&pu8APDUResp[i32APDULen], 
				TAG_NO_1, pstWhoIsData->m_i32DevRangeHighLimit);
		}
	}
	
	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	Who_Is_A_Request_Encoder: exit \r\n");
	#endif

    /* Number of Bytes consumed */
    return i32APDULen;
}


/** 
*
* DESCRIPTION
* Decodes the I-AM request.
*
* @param  pstProcQIndx  [in] Pointer to save decoded data.
* @param  pu8APDU		[in] Buffer containing data to be decoded.
* @param  i32APDULen	[in] Length of received request data.
*
* @return BACNET_RETURN_TYPE values.
*                                                                      
*/
BACNET_RETURN_TYPE I_Am_A_Decode_Handler(
	processInfo_t *pstProcQIndx,
	uint8_t *pu8APDU, 
	int32_t i32APDULen)
{
    /* Local variables */
	ddb_i_am_t *pstIAMData = NULL;
    uint8_t u8TagNumber = 0;
    int32_t i32Len = 0;
    uint32_t u32LenValue = 0;
    uint32_t u32DecodedValue = 0;
    BACNET_RETURN_TYPE eReturnType = BACDEL_ERROR;
	uint32_t u32ObjectType = 0;
	uint32_t u32Value = 0;
	
	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	I_Am_A_Decode_Handler: entry \r\n");
	#endif

    /* check if data is truncated */
    if(i32APDULen <= I_AM_MIN_DATA_LENGTH)
	{
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_TRUNCATED_DATA_FRAME, "APDU: \
		I_Am_A_Decode_Handler: truncated data received \r\n");
		#endif
        return BACDEL_ERROR;
	}

    /* Allocate buffer for I-AM request data */
	pstIAMData = (ddb_i_am_t *)OSAL_Malloc(sizeof(ddb_i_am_t),
        __FILE__, __FUNCTION__, __LINE__);
    /* Check if memeory is allocated */
    if(NULL == pstIAMData)
    {
		#if(defined DEBUG_PRINTF && DL_0)
        Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, "APDU: \
		I_Am_A_Decode_Handler: malloc failed \r\n");
		#endif
        return BACDEL_ERROR;
    }
	
    /* Decode Device Object Identifier tag */
    i32Len += Decode_Tag_Number_And_Value(&pu8APDU[i32Len], &u8TagNumber,
        &u32DecodedValue);
    /* Check decoded tag is Object ID type */
    if (BACNET_APPLICATION_TAG_OBJECT_ID != u8TagNumber)
	{
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_TAG_ERROR, "APDU: \
		DDB_A_I_AM_Handler: invalid device tag \r\n");
		#endif
        /* Call the clean up function */
		OSAL_Free(pstIAMData, __FILE__,__FUNCTION__,__LINE__);
        return BACDEL_TAG_ERROR;
	}
    /* Decode the Device object Identifier parameters */
    i32Len += Decode_Object_Id(&pu8APDU[i32Len],
        &u32ObjectType,
        &pstIAMData->m_u32DeviceInstance);

    if(OBJECT_DEVICE != u32ObjectType)
    {
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_OBJECT_TYPE_ERROR, "APDU: \
		I_Am_A_Decode_Handler: invalid object type \r\n");
		#endif
        /* Call the clean up function */
        OSAL_Free(pstIAMData, __FILE__,__FUNCTION__,__LINE__);
        return BACDEL_OBJECT_TYPE_ERROR;
    }

    /* Decode Maximum APDU length accepted tag */
    i32Len += Decode_Tag_Number_And_Value(&pu8APDU[i32Len], &u8TagNumber,
        &u32LenValue);
    /* Check decoded tag is Unsigned int type */
    if (BACNET_APPLICATION_TAG_UNSIGNED_INT != u8TagNumber)
	{
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_TAG_ERROR, "APDU:\
		I_Am_A_Decode_Handler: invalid apdu length tag \r\n");
		#endif
        /* Call the clean up function */
        OSAL_Free(pstIAMData, __FILE__,__FUNCTION__,__LINE__);
        return BACDEL_TAG_ERROR;
	}
    /* Decode Maximum APDU length accepted */ 
	i32Len += Decode_Unsigned(&pu8APDU[i32Len], u32LenValue, 
		&u32Value);
    pstIAMData->m_u16MaxAPDUlengthAccepted = (uint16_t)u32Value;

    /* Decode Segmentation Supported tag */
    i32Len += Decode_Tag_Number_And_Value(&pu8APDU[i32Len], &u8TagNumber,
        &u32LenValue); 
    /* Check decoded tag is enumerated type */
    if (BACNET_APPLICATION_TAG_ENUMERATED != u8TagNumber)
	{
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_TAG_ERROR, "APDU: \
		I_Am_A_Decode_Handler: invalid segmentation tag \r\n");
		#endif
        /* Call the clean up function */
        OSAL_Free(pstIAMData, __FILE__,__FUNCTION__,__LINE__);
        return BACDEL_TAG_ERROR;
	}
    /* Decode Segmentation Supported property */
    i32Len += Decode_Enumerated(&pu8APDU[i32Len], u32LenValue,
        &u32Value); 
	pstIAMData->m_eSegmentationSupported = (uint8_t)u32Value;

	if(pstIAMData->m_eSegmentationSupported >= MAX_BACNET_SEGMENTATION)
	{
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_OUT_OF_RANGE_ERROR, "APDU: \
		I_Am_A_Decode_Handler: invalid segmentation support \r\n");
		#endif
        /* Call the clean up function */
        OSAL_Free(pstIAMData, __FILE__,__FUNCTION__,__LINE__);
        return BACDEL_OUT_OF_RANGE_ERROR;
	}

    /* Decode Vendor ID tag */
    i32Len += Decode_Tag_Number_And_Value(&pu8APDU[i32Len], &u8TagNumber,
        &u32LenValue);
    /* Check decoded tag is unsigned int type */
    if (BACNET_APPLICATION_TAG_UNSIGNED_INT != u8TagNumber)
	{
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_TAG_ERROR, "APDU:\
		I_Am_A_Decode_Handler: invalid vendor id tag \r\n");
		#endif
        /* Call the clean up function */
        OSAL_Free(pstIAMData, __FILE__,__FUNCTION__,__LINE__);
        return BACDEL_TAG_ERROR;
	}
    /* Decode vendor Identifier */ 
    i32Len += Decode_Unsigned(&pu8APDU[i32Len], u32LenValue, 
        &pstIAMData->m_u32VendorID);      	

	/* save service data pointer */
	pstProcQIndx->m_stProcessData.m_stAPDU.m_pvServiceRequestData
        = pstIAMData;

	#if 1
    /* Pass the data to Object Management Layer */
    eReturnType = Obj_MgmtLayer_Interface(pstProcQIndx);
	if(BACDEL_CONTINUE != eReturnType)
    {
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, eReturnType, "APDU: \
		I_Am_A_Decode_Handler: Object Mgmt interface returns ERROR \r\n");
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
	#endif

	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	I_Am_A_Decode_Handler: exit \r\n");
	#endif
    return eReturnType; 
}


/** 
*
* DESCRIPTION
* Executes I-AM request & stores in Device Address Binding structure.
* 
* @param pstVirtualDev  [in] virtual device data.
* @param pstProcQInfo   [in] I-am service data.
* @param u8ThreadNo		[in] thread pool thread no.
*
* @return bool          [out]    True/false
*
*/
bool I_AM_A_Request_Parser(
	virtualDevData_t *pstVirtualDev, 
	processInfo_t * pstProcQInfo,
	uint8_t u8ThreadNo)
{
  /* 
   * Follow the steps below:-
   * 1) Create the Device_Address_Binding array of BACnetAddressBinding 
   *    structure. The elements of BACnetAddressBinding will be Status, 
   *    Device Identifier, Network Number, MAC Length & MAC Address.
   *    Keep the array length as 255. 
   * 2) When a I-AM packet is received it will search for an device id match
   *    in Device_Address_Binding linklist.
   * 3) If match is found then the old data is replaced with new data.
   * 4) If no match is found, a new node is added to linklist.
   */  
    
    /* Local Variables */
	BACnetAddrBinding_t *pstAddBindData = NULL;
	BACnetAddrBinding_t *pstPrevious = NULL;
    ddb_i_am_t *pstIAMDataBuf = NULL;
	ddb_i_am_t *pstTempIamData = NULL;

#if (CALLBACK_AFTER_EXECUTION == CALLBACK_CONFIG_I_AM)
	BACnetAddrBinding_t *pstAddBindCbData = NULL;
#endif
	uint32_t u32ErrCode = MAX_BACNET_ERROR_CODE;
	BACNET_ERROR_CLASS eErrClass = MAX_BACNET_ERROR_CLASS;
	BACnetAddress_t stRmtDvAddr = {0};

	//struct in_addr sin_addr = {0};

	
	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	I_AM_A_Request_Parser: entry \r\n");
	#endif

	/* Null pointer check for input parameters */
	if(NULL == pstProcQInfo || NULL == pstVirtualDev)
	{
		/* This should not occur ideally */
		#if(defined DEBUG_PRINTF && DL_0)
        Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
		I_AM_A_Request_Parser: Null input pointers \r\n");
		#endif
		return FALSE;
	}

    /* Assign the base address of BACnet Address Binding Structure */
    /* Assign the base address of BACnet I-AM Structure */
    pstIAMDataBuf = (ddb_i_am_t *)pstProcQInfo->m_stProcessData.m_stAPDU.
        m_pvServiceRequestData;
	pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;
    if(NULL == pstIAMDataBuf)
    {
		#if(defined DEBUG_PRINTF && DL_0)
        Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
        I_AM_A_Request_Parser: null service data pointer. \r\n");
		#endif
        return FALSE;
    }

	/* if not 1st host device, do not execute i-am */
	/* this is because we only have 1 global dynamic binding list
	   shared between all devices */
	if(pstVirtualDev != gstHostDevice.m_pstDeviceStruct)
	{
		/* free service data */
        OSAL_Free(pstIAMDataBuf, __FILE__,__FUNCTION__,__LINE__);
		return FALSE;
	}

	/* copy remote device address */
	memcpy(&stRmtDvAddr, &pstProcQInfo->m_stProcessData.m_stRmDvAddr, 
		sizeof(BACnetAddress_t));

	/* check if max count is not exceeded for device address binding list */
    /* if max count is exceeded but device to be added is virtual device 
       (i.e. device on which stack is running, then add it.) */
    if(g_stDevAddBinding.m_u32Count >= g_stStackMaxLimits.m_u32MaxDynamicAddrBind)
    {
    	if(stRmtDvAddr.u8IpAddrs[0] == MSTP_GetMacId(NULL)	)
    	{
    		; // do nothing
    	}
        else
        {
            /* list is full */
			#if(defined DEBUG_PRINTF && DL_0)
			Print_DebugMsg(DEBUG_LEVEL0, BACDEL_QUEUE_FULL, "APDU: \
            I_AM_A_Request_Parser: address binding list is full. \r\n");
			#endif
			/* free service data */
            OSAL_Free(pstIAMDataBuf, __FILE__,__FUNCTION__,__LINE__);
            return FALSE;
        }
    }

	#if (CALLBACK_AFTER_EXECUTION == CALLBACK_CONFIG_I_AM ) || \
		(CALLBACK_BEFORE_EXECUTION == CALLBACK_CONFIG_I_AM)

	/* Allocate buffer for service data */
	pstTempIamData = (ddb_i_am_t *)OSAL_Malloc(
		sizeof(ddb_i_am_t), __FILE__, __FUNCTION__, __LINE__);

	if(NULL == pstTempIamData)
	{
		#if(defined DEBUG_PRINTF && DL_0)
        Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, "APDU: \
		I_AM_A_Request_Parser: Malloc failed \r\n");
		#endif
		/* free service data */
        OSAL_Free(pstIAMDataBuf, __FILE__,__FUNCTION__,__LINE__);
		return FALSE;
	}

	/* copy i-am parameters */
	memcpy(pstTempIamData, pstIAMDataBuf, sizeof(ddb_i_am_t));

	#endif

	/* acquire the mutex */
	Osal_Wait_Mutex(g_hDynDevAddrBindMtxHandle, INFINITE);

	/* Assign the base address of Address Binding data */
	pstAddBindData = &g_stDevAddBinding.m_stAddBinding;

    /* Check if any entry is avaliable for the device */
    while(NULL != pstAddBindData)
    {
		/* compare device id and address both */
        if(pstAddBindData->m_u32ObjId == pstIAMDataBuf->m_u32DeviceInstance 
			#if (!STACK_CUSTOMIZE_I_AM_A)
			&& !memcmp(&stRmtDvAddr, &pstAddBindData->m_stAddress, 
			sizeof(BACnetAddress_t))
			#endif
			)
        {
			/* device entry already exists */
            break;
        }
		/* save previous node */
		pstPrevious = pstAddBindData;
        /* move to next node */
		pstAddBindData = pstAddBindData->pstNext;
    }

    /* If entry is not avaliable add the new device */
    if(NULL == pstAddBindData)
    {
		/* allocate memory to save new data */
		pstAddBindData = OSAL_Malloc
			(sizeof(BACnetAddrBinding_t), __FILE__, __FUNCTION__, __LINE__);
		if(NULL == pstAddBindData)
		{
			/* release the mutex */
			Osal_Release_Mutex(g_hDynDevAddrBindMtxHandle);

			/* malloc failed */
			#if(defined DEBUG_PRINTF && DL_0)
			Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, "APDU: \
			I_AM_A_Request_Parser: malloc error \r\n");
			#endif

            /* free allocated service data */
			OSAL_Free(pstTempIamData,__FILE__,__FUNCTION__,__LINE__);
			OSAL_Free(pstIAMDataBuf,__FILE__,__FUNCTION__,__LINE__);
			return FALSE;
		}
		/* make next pointer null */
		pstAddBindData->pstNext = NULL;

		/* increment count */
        g_stDevAddBinding.m_u32Count++;

		{
			pstPrevious->pstNext = pstAddBindData;
		}
    }
	#if (!STACK_CONFIG_EXECUTE_DUPLICATE_I_AM)
	else
	{
		/* release the mutex */
		Osal_Release_Mutex(g_hDynDevAddrBindMtxHandle);
		/* do not overwrite existing entry */
		/* free allocated servcie data */
		OSAL_Free(pstTempIamData, __FILE__, __FUNCTION__, __LINE__);
		OSAL_Free(pstIAMDataBuf, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}
	#endif

    /* copy the data to the old/new node */
	pstAddBindData->m_eObjectType = OBJECT_DEVICE;
	pstAddBindData->m_u32ObjId = 
		pstIAMDataBuf->m_u32DeviceInstance;
    memcpy(&(pstAddBindData->m_stAddress), &stRmtDvAddr, 
		sizeof(BACnetAddress_t));
    pstAddBindData->m_u16MaxAPDULenAccepted = 
        pstIAMDataBuf->m_u16MaxAPDUlengthAccepted;
	pstAddBindData->m_eSegmentationSupport = 
		pstIAMDataBuf->m_eSegmentationSupported;
    pstAddBindData->m_u16VendorId = (uint16_t)pstIAMDataBuf->m_u32VendorID;

	#if (CALLBACK_AFTER_EXECUTION == CALLBACK_CONFIG_I_AM)

	/* allocate memory to copy data */
	pstAddBindCbData = OSAL_Malloc(sizeof(BACnetAddrBinding_t), __FILE__, __FUNCTION__, __LINE__);
	if(NULL == pstAddBindCbData)
	{
		;// log error and do nothing
		/* malloc failed */
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, 
		"APDU: I_AM_A_Request_Parser: malloc error \r\n");
		#endif
	}
	else
	{
		/* copy data to send it to application in callback */
		memcpy(pstAddBindCbData, pstAddBindData, sizeof(BACnetAddrBinding_t));
		pstAddBindCbData->pstNext = NULL;
	}

	#endif /* CALLBACK_AFTER_EXECUTION */

	/* release the mutex */
	Osal_Release_Mutex(g_hDynDevAddrBindMtxHandle);

	#if (CALLBACK_AFTER_EXECUTION == CALLBACK_CONFIG_I_AM)

	/* If callback function is registered, then pass the data. */
	if(NULL == afpApplicationFunctionCb[APP_CB_I_AM])
	{
		/* free allocated servcie data */
		OSAL_Free(pstTempIamData,__FILE__,__FUNCTION__,__LINE__);
		OSAL_Free(pstIAMDataBuf,__FILE__,__FUNCTION__,__LINE__);
		OSAL_Free(pstAddBindCbData,__FILE__,__FUNCTION__,__LINE__);
		return FALSE;
	}

	/* add node to callback notification queue */
	u32ErrCode = CbNotifyQue_AddNode(
			&stRmtDvAddr, NULL, 0,
			SERVICE_SUPPORTED_WHO_HAS, pstTempIamData,
			pstVirtualDev, &eErrClass);

	/* if error received while adding node to queue */
	if(MAX_BACNET_ERROR_CODE != u32ErrCode)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_CALLBACK_NOTIFICATION_QUEUE_FULL, "APDU: \
		I_AM_A_Request_Parser: Error in adding node to notification\
		queue Errorcode = %d \r\n", u32ErrCode);
		#endif
		/* free allocated servcie data */
		OSAL_Free(pstTempIamData,__FILE__,__FUNCTION__,__LINE__);
		OSAL_Free(pstIAMDataBuf,__FILE__,__FUNCTION__,__LINE__);
		OSAL_Free(pstAddBindCbData, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}

	#endif /* CALLBACK_AFTER_EXECUTION */

	/* free service data */
	//Exit_Routine_To_Error_Of_Unconfirmed_Request(pstProcQInfo);
	OSAL_Free(pstIAMDataBuf,__FILE__,__FUNCTION__,__LINE__);

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	I_AM_A_Request_Parser: exit \r\n");
	#endif
    return FALSE;
}
#endif /* BACDEL_SER_DM_DDB_A */
