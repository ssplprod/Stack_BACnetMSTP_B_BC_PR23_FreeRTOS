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
*   SoftDEL Systems Ltd.                         india@softdel.com      
*   3rd Floor, Pentagon P4,                      http://www.softdel.com  
*   Magarpatta City, Hadapsar
*   Pune - 411 028                                 
*                                                                         
*	FILE
*	serviceReadPropertyMultiple_A.c
*                                                            
*   AUTHORS                                                                     
*   Harshal Mangale
*                                                                         
*   DESCRIPTION 
*	The ReadPropertyMultiple service is used by a client BACnet-user to request
*	the values of one or more specified properties of one or more BACnet Objects.
*	This service allows read access to any property of any object, whether a 
*	BACnet-defined object or not.
*   This file includes all functions related to RPM-A service.   
*                                                                         
*********************************************************************************/

#ifdef BACDEL_SER_DS_RPM_A

/* header files */
#include "serviceReadPropertyMultiple_A.h"
#include "miscMiscellaneous.h"
#include "pduEncodeDecode.h"
#include "pduDataEncodeDecode.h"
#include "bacnetInitiateServiceMgmt.h"


/**
*                                                                         
* DESCRIPTION                                                                          
* This function will encode RPM-A service request.
*    
* @param pstProcQInfo    [in] Service parameter to be encoded.
* @param pu8ApduBuff [in/out] Buffer to store Encoded APDU data.
* @param i32APDULen  [in/out] Encoded APDU data length.
* 
* @return eReturnType	[out] Encoded data length.
*
*/
int32_t RPM_A_Request_Encoder(                    
    processInfo_t *pstProcQInfo,
    uint8_t **pu8ApduBuff,
    int32_t i32APDULen)
{
	/* local variables */
    int32_t i32Len = 0;
    int32_t i32CopyLen = 0;
    uint8_t u8TempBuf[MAX_APDU_LENGTH_ACCEPTED] = {0};
    bool bCopyError = false;
    Bacnet_Multiple_Data_t *pstRpmData = NULL;

	#ifdef SEGMENTATION_SUPPORTED
	uint32_t u32Count = MAX_APDU_LENGTH_ACCEPTED;
	#endif 

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	RPM_A_Request_Encoder: entry \r\n");
	#endif

    /* get data to be encode */
    pstRpmData = (Bacnet_Multiple_Data_t *)pstProcQInfo->m_stProcessData.
		m_stAPDU.m_pvServiceRequestData;
    
	/* check input pointer */
    if(NULL == pstRpmData)
    {
		#if(defined DEBUG_PRINTF && DL_0)
        Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
		RPM_A_Request_Encoder: null input pointer \r\n");
		#endif
        return 0;
    }

    while(NULL != pstRpmData)
    {
        /* Fill first Object ID in response */
        if(pstRpmData->m_stServiceData.u8FillObjId)
        {
            /* Stick this object id into the reply - if it will fit */
            i32Len = Encode_MPS_ObjId_OTag(&u8TempBuf[0], pstRpmData);

			#ifdef SEGMENTATION_SUPPORTED
			i32CopyLen = Memcopy_Dynamic_Mem_Expansion((void **)pu8ApduBuff, &u8TempBuf[0], i32APDULen,
                i32Len, &u32Count, MAX_APDU_LENGTH_ACCEPTED);
			#else
            i32CopyLen = Memcopy(*pu8ApduBuff, &u8TempBuf[0], i32APDULen,
                i32Len, MAX_APDU_LENGTH_ACCEPTED);
			#endif 
            if (i32CopyLen == 0) 
            {
                bCopyError = true;
                break; // break from while loop
            } 
            i32APDULen += i32Len;
        }

        /* Fill Property identifier and Array Index in Request */
        i32Len = Encode_MPS_property_ArrIdx(
            &u8TempBuf[0],
            pstRpmData->m_stServiceData.eObjectProperty,
            pstRpmData->m_stServiceData.u32ArrayIndex,
            pstRpmData->m_stServiceData.bArrIndxPresent);

		#ifdef SEGMENTATION_SUPPORTED
		i32CopyLen = Memcopy_Dynamic_Mem_Expansion((void **)pu8ApduBuff, &u8TempBuf[0], i32APDULen, 
			i32Len, &u32Count, MAX_APDU_LENGTH_ACCEPTED);
		#else
        i32CopyLen = Memcopy(*pu8ApduBuff, &u8TempBuf[0], i32APDULen, 
			i32Len, MAX_APDU_LENGTH_ACCEPTED);
		#endif 
        if (i32CopyLen == 0) 
        {
            bCopyError = true;
            break; // break from while loop
        }

        i32APDULen += i32Len;

        /* Append Closing tag when property list for that ObjId is over */
        /* OR it is last property of RPM-B command */
        if((pstRpmData->m_pstMDNext == NULL) || (pstRpmData->m_pstMDNext->m_stServiceData.u8FillObjId))
        {
            i32Len = Encode_Closing_Tag(&u8TempBuf[0], TAG_NO_1);
			#ifdef SEGMENTATION_SUPPORTED
			i32CopyLen = Memcopy_Dynamic_Mem_Expansion((void **)pu8ApduBuff, &u8TempBuf[0],
                i32APDULen, i32Len, &u32Count, MAX_APDU_LENGTH_ACCEPTED);
			#else
            i32CopyLen = Memcopy(*pu8ApduBuff, &u8TempBuf[0],
                i32APDULen, i32Len, MAX_APDU_LENGTH_ACCEPTED);
			#endif 
            if (i32CopyLen == 0) 
            {
                bCopyError = true;
                break; // break from while loop
            } 
            else 
            {
                i32APDULen += i32CopyLen;
            }        
        }

        /* Get pointer to Next Parameters */
        pstRpmData = pstRpmData->m_pstMDNext;

    }// End of while()

    if(bCopyError)
    {
        /* Buffer is full and no space to fill response */
        /* FIXME: Notify Application That Can't send Request */
    }

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	RPM_A_Request_Encoder: exit \r\n");
	#endif
	return i32APDULen;
}

/**
*                                                                    
* DESCRIPTION                                                                          
* This function will decode the complex-ack for RPM-A service response. 
*    
* @param pu8ServiceRequest   [in] Buffer containing response data.
* @param i32ServiceLen       [in] Length of response data.
* @param pstProcQInfo       [out] Pointer to save decoded data.
*
* @return [out]	success or error.
*                                                                      
*/
BACNET_RETURN_TYPE RPM_A_Ack_Decode_Handler(
    processInfo_t *pstProcQInfo,
    uint8_t *pu8ServiceRequest,
    int32_t i32ServiceLen)
{
	/* local variables */
    int32_t i32Len = 0;
    int32_t i32DecodeLen = 0;
    Bacnet_Multiple_Data_t *pstFirstRpmData = NULL;
    Bacnet_Multiple_Data_t *pstRpmData = NULL;
    Bacnet_Multiple_Data_t *pstTmpRpmData = NULL;
    BACNET_RETURN_TYPE eReturnType = BACDEL_SUCCESS;
	bool bFlag = FALSE;
	
	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	RPM_A_Ack_Decode_Handler: entry \r\n");
	#endif

    /* check null pointers */
    if(pstProcQInfo == NULL || pu8ServiceRequest == NULL || i32ServiceLen == 0)
    {
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
		RPM_A_Ack_Decode_Handler: Null input pointers \r\n");
		#endif
        /* Return Error As no Need to send response */
        return BACDEL_ERROR;
	}

    /* default value for RPM-B response */
    pstProcQInfo->m_stProcessData.m_ePDUType = PDU_TYPE_COMPLEX_ACK;

    /* decode apdu request */
    while(i32DecodeLen < i32ServiceLen) 
    {
        /* Allocate buffer for read property multiple service data */
        /* Memory is assigned for each new Object ID present in Request */ 
        pstTmpRpmData = (Bacnet_Multiple_Data_t *)OSAL_Malloc(sizeof(Bacnet_Multiple_Data_t),
			__FILE__, __FUNCTION__, __LINE__);

		/* check null pointers */
		if(NULL == pstTmpRpmData)
		{
			#if(defined DEBUG_PRINTF && DL_0)
			Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, "APDU: \
			RPM_A_Ack_Decode_Handler: Malloc Failed \r\n");
			#endif
			/* Free Malloc Memmory */
			Free_Service_Memory(pstFirstRpmData);
			pstFirstRpmData = NULL;
			/* set error from client */
			Service_Error_Handler(BACNET_STATUS_ERROR,
				ERROR_CLASS_RESOURCES, ERROR_CODE_OUT_OF_MEMORY,
				pstProcQInfo, FALSE);
			break;
		}

		/* First time assign pointer to "pstFirstRpmData" */
		/* for next ObjId create link list */
		if(pstFirstRpmData == NULL)
		{
			pstFirstRpmData = pstTmpRpmData;
		}
		else
		{
			/* Add new Node to Last node of Link */
			pstRpmData->m_pstMDNext = pstTmpRpmData;
		}
		/* Pointer to new node to store decoded Parameters*/
		pstRpmData = pstTmpRpmData;

		/* "Fill_ObjId" Flag set to "1" indicate New Obj_Id*/
		pstRpmData->m_stServiceData.u8FillObjId = 1;

		/* Start by looking for an object ID */
		i32Len = Decode_MPS_object_id(&pu8ServiceRequest[i32DecodeLen],
			(i32ServiceLen - i32DecodeLen), pstRpmData);
		if(i32Len < 0)
		{
			/* Free Malloc Memmory */
			Free_Service_Memory(pstFirstRpmData);
			pstFirstRpmData = NULL;
			#if(defined DEBUG_PRINTF && DL_1)
			Print_DebugMsg(DEBUG_LEVEL1, BACDEL_OBJECT_TYPE_ERROR, "APDU: \
			RPM_A_Ack_Decode_Handler: invalid object ID \r\n");
			#endif
			/* set error from client */
			Service_Error_Handler(BACNET_STATUS_ERROR,
				ERROR_CLASS_SERVICES, ERROR_CODE_INVALID_PARAMETER_DATA_TYPE,
				pstProcQInfo, FALSE);
			break;
		}
		i32DecodeLen += i32Len;

		/* do each property of this object of the RPM request */
		for(;;) 
		{
			/* Fetch a property identifier and Array Index */
			i32Len = RPM_A_Ack_Decode_Object_Property(&pu8ServiceRequest[i32DecodeLen],
				(i32ServiceLen - i32DecodeLen), pstRpmData);
			if(i32Len < 0) 
			{
				/* Free Malloc Memmory */
				Free_Service_Memory(pstFirstRpmData);
				pstFirstRpmData = NULL;
				#if(defined DEBUG_PRINTF && DL_1)
				Print_DebugMsg(DEBUG_LEVEL1, BACDEL_INVALID_DATA_TYPE, "APDU: \
				RPM_A_Ack_Decode_Handler: invalid property identifier \r\n");
				#endif
				/* set error from client */
				Service_Error_Handler(BACNET_STATUS_ERROR,
					ERROR_CLASS_SERVICES, ERROR_CODE_INVALID_PARAMETER_DATA_TYPE,
					pstProcQInfo, FALSE);
				/* set flag */
				bFlag = TRUE;
				break;
			}
			i32DecodeLen += i32Len;

			/* Tag_4: opening context tag for Property Value */
			if(Decode_Is_Opening_Tag_Number(&pu8ServiceRequest[i32DecodeLen], TAG_NO_4))
			{
				/* determine the length of the data block */
				pstRpmData->m_stServiceData.i32ApplicationDataLen =
					BACApp_Verify_Data_Len(&pu8ServiceRequest[i32DecodeLen],
					i32ServiceLen - i32DecodeLen,
					pstRpmData->m_stServiceData.eObjectProperty);

				if(BACNET_STATUS_ERROR == pstRpmData->m_stServiceData.i32ApplicationDataLen)
				{
					/* Free Malloc Memmory */
					Free_Service_Memory(pstFirstRpmData);
					pstFirstRpmData = NULL;
					#if(defined DEBUG_PRINTF && DL_1)
					Print_DebugMsg(DEBUG_LEVEL1, BACDEL_TAG_ERROR,
					"APDU: RPM_A_Ack_Decode_Handler: tag length error \r\n");
					#endif
					/* set error from client */
					Service_Error_Handler(BACNET_STATUS_ERROR,
						ERROR_CLASS_SERVICES, ERROR_CODE_INVALID_PARAMETER_DATA_TYPE,
						pstProcQInfo, FALSE);
					/* set flag */
					bFlag = TRUE;
					break;
				}

				/* a tag number of 3 is not extended so only one octet increment */
				i32DecodeLen++;
				/* allocate memory */
				pstRpmData->m_stServiceData.pu8PropValueBuffer = 
					(uint8_t*) OSAL_Malloc( pstRpmData->m_stServiceData.i32ApplicationDataLen,
					__FILE__, __FUNCTION__, __LINE__);

				if(NULL == pstRpmData->m_stServiceData.pu8PropValueBuffer)
				{
					/* Free Malloc Memmory */
					Free_Service_Memory(pstFirstRpmData);
					pstFirstRpmData = NULL;
					#if(defined DEBUG_PRINTF && DL_0)
					Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, "APDU: \
					RPM_A_Ack_Decode_Handler: malloc error \r\n");
					#endif
					/* set error from client */
					Service_Error_Handler(BACNET_STATUS_ERROR,
						ERROR_CLASS_RESOURCES, ERROR_CODE_OUT_OF_MEMORY,
						pstProcQInfo, FALSE);
					/* set flag */
					bFlag = TRUE;
					break;
				}

				/* copy byte data i.e. property value byte data */
				memcpy(pstRpmData->m_stServiceData.pu8PropValueBuffer, 
					&pu8ServiceRequest[i32DecodeLen], 
					pstRpmData->m_stServiceData.i32ApplicationDataLen);

				/* add on the data length */
				i32DecodeLen += pstRpmData->m_stServiceData.i32ApplicationDataLen;

				/* decode closing tag */
				if(i32DecodeLen && Decode_Is_Closing_Tag_Number(
					&pu8ServiceRequest[i32DecodeLen], TAG_NO_4)) 
				{
					i32DecodeLen++; // tag decoded 
				}
				else
				{
					/* Free Malloc Memmory */
					Free_Service_Memory(pstFirstRpmData);
					pstFirstRpmData = NULL;
					#if(defined DEBUG_PRINTF && DL_1)
					Print_DebugMsg(DEBUG_LEVEL1, BACDEL_TAG_ERROR, "APDU: \
					RPM_A_Ack_Decode_Handler: data length error \r\n");
					#endif
					/* set error from client */
					Service_Error_Handler(BACNET_STATUS_ERROR,
						ERROR_CLASS_SERVICES, ERROR_CODE_INVALID_TAG,
						pstProcQInfo, FALSE);
					/* set flag */
					bFlag = TRUE;
					break;
				}
			} 

			/* Tag_5: opening context tag for Error Code */
			else if(Decode_Is_Opening_Tag_Number(&pu8ServiceRequest[i32DecodeLen], TAG_NO_5))
			{
				i32DecodeLen++; // tag decoded
				/* error response for property */
				pstRpmData->m_stServiceData.bErrorStatus = true;
	            /* decode error class and error code */
				i32Len = RPM_A_Ack_Decode_Property_Error(&pu8ServiceRequest[i32DecodeLen],
					i32ServiceLen - i32DecodeLen, pstRpmData);

				if(i32Len < 0) 
				{
					/* Free Malloc Memmory */
					Free_Service_Memory(pstFirstRpmData);
					pstFirstRpmData = NULL;
					#if(defined DEBUG_PRINTF && DL_1)
					Print_DebugMsg(DEBUG_LEVEL1, BACDEL_TAG_ERROR,
					"APDU: RPM_A_Ack_Decode_Handler: tag length error \r\n");
					#endif
					/* set error from client */
					Service_Error_Handler(BACNET_STATUS_ERROR,
						ERROR_CLASS_SERVICES, ERROR_CODE_INVALID_PARAMETER_DATA_TYPE,
						pstProcQInfo, FALSE);
					/* set flag */
					bFlag = TRUE;
					break;
				}
				i32DecodeLen += i32Len;

				/* decode closing tag */
				if(i32DecodeLen && Decode_Is_Closing_Tag_Number(
					&pu8ServiceRequest[i32DecodeLen], TAG_NO_5)) 
				{
					i32DecodeLen++; // tag decoded
				}
				else
				{
					/* Free Malloc Memmory */
					Free_Service_Memory(pstFirstRpmData);
					pstFirstRpmData = NULL;
					#if(defined DEBUG_PRINTF && DL_1)
					Print_DebugMsg(DEBUG_LEVEL1, BACDEL_TAG_ERROR, "APDU: \
					RPM_A_Ack_Decode_Handler: decode length error \r\n");
					#endif
					/* set error from client */
					Service_Error_Handler(BACNET_STATUS_ERROR,
						ERROR_CLASS_SERVICES, ERROR_CODE_INVALID_TAG,
						pstProcQInfo, FALSE);
					/* set flag */
					bFlag = TRUE;
					break;
				}
			}

			/* invalid case - tag is incorrect */
			else
			{
				/* Free Malloc Memmory */
				Free_Service_Memory(pstFirstRpmData);
				pstFirstRpmData = NULL;
				#if(defined DEBUG_PRINTF && DL_1)
				Print_DebugMsg(DEBUG_LEVEL1, BACDEL_TAG_ERROR, "APDU: \
				RPM_A_Ack_Decode_Handler: Invalid Tag number \r\n");
				#endif
				/* set error from client */
				Service_Error_Handler(BACNET_STATUS_ERROR,
					ERROR_CLASS_SERVICES, ERROR_CODE_INVALID_TAG,
					pstProcQInfo, FALSE);
				/* set flag */
				bFlag = TRUE;
				break;
			}

			/* Check End of property list is reached or not */
			if(Decode_Is_Closing_Tag_Number(&pu8ServiceRequest[i32DecodeLen], TAG_NO_1)) 
			{
				/* Reached end of property list so cap the result list */
				i32DecodeLen++;
				break;  /* finished with this property list */
			} 
			else 
			{
				/* Continue decoding Property identifier from command */
				/* malloc new RPMData memory and store decoded param in it */
				pstTmpRpmData = (Bacnet_Multiple_Data_t *)OSAL_Malloc(sizeof(Bacnet_Multiple_Data_t), 
					__FILE__, __FUNCTION__, __LINE__);
				if(pstTmpRpmData == NULL )
				{
					#if(defined DEBUG_PRINTF && DL_0)
					Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, "APDU: \
					RPM_A_Ack_Decode_Handler: Malloc failed \r\n");
					#endif

					/* Free Malloc Memmory */
					Free_Service_Memory(pstFirstRpmData);
					pstFirstRpmData = NULL;
					/* set error from client */
					Service_Error_Handler(BACNET_STATUS_ERROR,
						ERROR_CLASS_RESOURCES, ERROR_CODE_OUT_OF_MEMORY,
						pstProcQInfo, FALSE);
					/* set flag */
					bFlag = TRUE;
					break;
				}

				/* Get Object identifier from previous property parameters */
				pstTmpRpmData->m_stServiceData.eObjectType = 
					pstRpmData->m_stServiceData.eObjectType;
				pstTmpRpmData->m_stServiceData.u32ObjectInstance = 
					pstRpmData->m_stServiceData.u32ObjectInstance;
				 /* Flag set to "0" indicate previous Obj_Id */
				pstTmpRpmData->m_stServiceData.u8FillObjId = 0;

				/* manage the link list */
				pstRpmData->m_pstMDNext = pstTmpRpmData;
				pstRpmData = pstTmpRpmData;
			} 
		}/* End of Inner for loop */

		/* break outer loop if error in inner loop */
		if(bFlag)
		{
			break;
		}      
        
    }/* End of while Loop */

	/* Assign pointer of Decoded RPM Data to service_request_data */
    pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData = pstFirstRpmData;

    /* Call Initiate service interface */
    eReturnType = Initiate_Service_Interface(pstProcQInfo);
    if(BACDEL_CONTINUE != eReturnType)
    {
        /* Free memory for RP service parameters */
        Free_Service_Memory(pstFirstRpmData);
        pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;

		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, eReturnType, "APDU: \
		RPM_A_Ack_Decode_Handler: Response is processed \r\n");
		#endif
        eReturnType = BACDEL_ERROR;
    }              

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	RPM_A_Ack_Decode_Handler: exit \r\n");
	#endif
	return eReturnType;
}

/** 
*
* DESCRIPTION:
* Decode the object property portion of the RPM service response.
*
* @param pu8APDUReq  [in]    Received RPM-B request
* @param u32APDULen  [in]    Length of received RPM-b request
* @param pstRpmData  [out]   Structure to store parameters of RPM request
*
* @return i32Len     [out]   Total decoded length of received request
* BACNET_STATUS_REJECT If tags are wrong or missing / incomplete
*
*/
int32_t RPM_A_Ack_Decode_Object_Property(
   uint8_t *pu8APDUReq,
   uint32_t u32APDULen,
   Bacnet_Multiple_Data_t *pstRpmData)
{
	/* local variables */
    int32_t i32Len = 0;
    int32_t i32OptionLen = 0;
    uint8_t  u8TagNumber = 0;
    uint32_t u32LenValueType = 0;
    uint32_t u32Property = 0;      
    uint32_t u32ArrayValue = 0;
	
	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	RPM_A_Ack_Decode_Object_Property: entry \r\n");
	#endif

    /* check for valid pointers */
    if(pu8APDUReq && u32APDULen && pstRpmData) 
	{
        /* Tag 0: propertyIdentifier */
        if(!Decode_Context_Tag(&pu8APDUReq[i32Len++], TAG_NO_2, &u32LenValueType))
		{
            pstRpmData->m_stServiceData.eErrorCode = ERROR_CODE_REJECT_INVALID_TAG;

			/* invalid tag error */
			#if(defined DEBUG_PRINTF && DL_1)
			Print_DebugMsg(DEBUG_LEVEL1, BACDEL_TAG_ERROR, "APDU: \
			RPM_A_Ack_Decode_Object_Property: invalid tag \r\n");
			#endif
            return BACNET_STATUS_REJECT;
        }

        /* Should be at least the uint32_t value + 1 tag left */
        if((i32Len + u32LenValueType) >= u32APDULen) 
		{
            pstRpmData->m_stServiceData.eErrorCode = 
                ERROR_CODE_REJECT_MISSING_REQUIRED_PARAMETER;

			#if(defined DEBUG_PRINTF && DL_1)
			Print_DebugMsg(DEBUG_LEVEL1, BACDEL_MISSING_PARAMETER,
			"APDU: RPM_A_Ack_Decode_Object_Property: parameter missing \r\n");
			#endif
            return BACNET_STATUS_REJECT;
        }
        i32Len += Decode_Enumerated(&pu8APDUReq[i32Len], u32LenValueType, &u32Property);

        pstRpmData->m_stServiceData.eObjectProperty = (BACNET_PROPERTY_ID)u32Property;
        
		/* Assume most probable outcome */
        pstRpmData->m_stServiceData.u32ArrayIndex = BACNET_ARRAY_ALL;
        pstRpmData->m_stServiceData.bArrIndxPresent = ARRAY_INDEX_ABSENT;
        /* Tag 1: Optional propertyArrayIndex */
        if(IS_CONTEXT_SPECIFIC(pu8APDUReq[i32Len]) && !IS_CLOSING_TAG(pu8APDUReq
            [i32Len]))
		{
        	i32OptionLen = Decode_Tag_Number_And_Value(&pu8APDUReq[i32Len], &u8TagNumber,
                &u32LenValueType);
            if(u8TagNumber == TAG_NO_3) 
			{
            	i32Len += i32OptionLen;
                /* Should be at least the uint32_t array index + 1 tag left */
                if((i32Len + u32LenValueType) >= u32APDULen) 
                {
                    pstRpmData->m_stServiceData.eErrorCode =
                        ERROR_CODE_REJECT_MISSING_REQUIRED_PARAMETER;

					#if(defined DEBUG_PRINTF && DL_1)
					Print_DebugMsg(DEBUG_LEVEL1, BACDEL_MISSING_PARAMETER,
					"APDU: RPM_A_Ack_Decode_Object_Property: parameter missing \r\n");
					#endif
                    return BACNET_STATUS_REJECT;
                }
                i32Len += Decode_Unsigned(&pu8APDUReq[i32Len], u32LenValueType, &u32ArrayValue);
                pstRpmData->m_stServiceData.u32ArrayIndex = u32ArrayValue;
                pstRpmData->m_stServiceData.bArrIndxPresent = ARRAY_INDEX_PRESENT;
            }
        }
    }

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	RPM_A_Ack_Decode_Object_Property: exit \r\n");
	#endif
    return i32Len;
}

/** 
*
* DESCRIPTION
* Decode the object property error code.
*
* @param pu8APDUReq  [in]    Received RPM-B request
* @param u32APDULen  [in]    Length of received RPM-b request
* @param pstRpmData  [out]   Structure to store parameters of RPM request
*
* @return i32Len     [out]   Total decoded length of received request
* BACNET_STATUS_REJECT If tags are wrong or missing / incomplete
*
*/
int32_t RPM_A_Ack_Decode_Property_Error(
    uint8_t *pu8APDUReq,
    uint32_t u32APDULen,
    Bacnet_Multiple_Data_t *pstRpmData)
{
	/* local variables */
    int32_t i32Len = 0;
    uint8_t  u8TagNumber = 0;
    uint32_t u32LenValueType = 0;
    uint32_t u32ErrorVal = 0;

	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	RPM_A_Ack_Decode_Property_Error: entry \r\n");
	#endif

    /* check for valid pointers */
    if(pu8APDUReq && u32APDULen && pstRpmData) 
	{
        /* decode the class and code sequence */
    	i32Len = Decode_Tag_Number_And_Value(&pu8APDUReq[i32Len], &u8TagNumber, 
            &u32LenValueType);
        
        /* FIXME: we could validate that the tag is enumerated... */
        if(u8TagNumber == BACNET_APPLICATION_TAG_ENUMERATED &&
            (i32Len + u32LenValueType) < u32APDULen ) 
		{
        	i32Len += Decode_Enumerated(&pu8APDUReq[i32Len], u32LenValueType, 
                &u32ErrorVal);
            pstRpmData->m_stServiceData.eErrorClass = u32ErrorVal;
        }
        else
		{
			#if(defined DEBUG_PRINTF && DL_1)
			Print_DebugMsg(DEBUG_LEVEL1, BACDEL_TAG_ERROR, "APDU: \
			RPM_A_Ack_Decode_Property_Error: Invalid Tag \r\n");
			#endif
            return BACNET_STATUS_REJECT;
		}

        i32Len += Decode_Tag_Number_And_Value(&pu8APDUReq[i32Len], &u8TagNumber, 
            &u32LenValueType);
        
        /* FIXME: we could validate that the tag is enumerated... */
        if(u8TagNumber == BACNET_APPLICATION_TAG_ENUMERATED &&
            (i32Len + u32LenValueType) < u32APDULen ) 
		{
        	i32Len += Decode_Enumerated(&pu8APDUReq[i32Len], u32LenValueType,
                &u32ErrorVal);
            pstRpmData->m_stServiceData.eErrorCode = u32ErrorVal;
        }
        else
		{
			#if(defined DEBUG_PRINTF && DL_1)
			Print_DebugMsg(DEBUG_LEVEL1, BACDEL_TAG_ERROR, "APDU: \
			RPM_A_Ack_Decode_Property_Error: Invalid Tag \r\n");
			#endif
            return BACNET_STATUS_REJECT;
		}
    }

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	RPM_A_Ack_Decode_Property_Error: exit \r\n");
	#endif
    return i32Len;
}

#ifdef DISPLAY_PARAMETERS
/**
*                                                                         
* DESCRIPTION                                                                          
* This function will print the validated values in stack for RPM-A service.
*    
* @param pstRpmReq [in] RPM Service parameter to be display.
* @return nothing.
*
*/
void RPM_A_Display_Data(rpm_request_t *pstRpmReq)
{
	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	RPM_A_Display_Data: entry \r\n");
	#endif

	/* check inpur pointer */
	if(NULL == pstRpmReq)
	{
		/* null input pointer */
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
		RPM_A_Display_Data: null input pointer \r\n");
		#endif
		return;
	}

    /* Display values */
	printf("\n ----------------- Stack RPM-A ---------------- \n");

	while(pstRpmReq != NULL)
	{
		printf("\n Object type = %u", pstRpmReq->m_eObjectType);
		printf("\n Object instance = %u", pstRpmReq->m_u32ObjectInstance);
		printf("\n Property Id = %u", pstRpmReq->m_eObjectProperty);
		printf("\n Array index present = %u", pstRpmReq->m_u8ArrayIndexPresent);
		printf("\n Array index = %u", pstRpmReq->m_u32ArrayIndex);
		pstRpmReq = pstRpmReq->m_pstRPMNextElem;
	}

	/* display of data completed */
	printf("\r\n");

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	RPM_A_Display_Data: exit \r\n");
	#endif
    return;
}
#endif /* */

#endif /* BACDEL_SER_DS_RPM_A */
