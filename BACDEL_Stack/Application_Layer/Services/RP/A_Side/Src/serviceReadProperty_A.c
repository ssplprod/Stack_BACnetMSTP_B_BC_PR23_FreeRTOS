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
*	serviceReadProperty_A.c
*                                                            
*   AUTHORS                                                                     
*   Harshal Mangale
*                                                                         
*   DESCRIPTION 
*	The ReadProperty service is used by a client BACnet-user to request the 
*	value of one property of one BACnet Object. This service allows read 
*	access to any property of any object, whether a BACnet-defined object or not.
*   This file includes all functions related to RP-A service.   
*                                                                         
*********************************************************************************/

#ifdef BACDEL_SER_DS_RP_A

/* include header files */

#include "serviceReadProperty_A.h"
#include "pduServiceStructure.h"
#include "bacnetStackMgmt.h"
#include "pduEncodeDecode.h"
#include "pduDataEncodeDecode.h"
#include "bacnetInitiateServiceMgmt.h"

/**
*                                                                         
* DESCRIPTION                                                                          
* This function will encode the RP-A service request.
*    
* @param pstProcQInfo    [in] Service parameter to be encoded.
* @param pu8ApduBuff [in/out] Buffer to store Encoded APDU data.
* @param i32APDULen  [in/out] Encoded APDU data length.
*
* @return i32APDULen    [out] Encoded data length.
*
*/
int32_t RP_A_Request_Encoder(
	processInfo_t *pstProcQInfo,
    uint8_t *pu8ApduBuff,
    int32_t i32APDULen)
{
	/* local variables */
    BACNET_CONF_DATA *pstRpData = NULL;
    
	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	RP_A_Request_Encoder: entry \r\n");
	#endif

	/* get data to be encode */
    pstRpData = (BACNET_CONF_DATA *)pstProcQInfo->m_stProcessData.
		m_stAPDU.m_pvServiceRequestData;
    
	/* check input pointer */
    if(NULL == pstRpData)
    {
		#if(defined DEBUG_PRINTF && DL_0)
        Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
		RP_A_Request_Encoder: NULL Input Pointers \r\n");
		#endif
        return 0;
    }

    /* Encode Obj_ID, PropID, Array_Index */
    i32APDULen += Encode_ObjId_Prop_ArrIdx(&pu8ApduBuff[i32APDULen], pstRpData);

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	RP_A_Request_Encoder: exit \r\n");
	#endif
    return i32APDULen;
}

/**
*                                                                    
* DESCRIPTION                                                                          
* This function will decode the complex-ack for RP-A service request.
*    
* @param pu8ServiceRequest  [in] Buffer containing response data.
* @param i32ServiceLen      [in] Length of response data.
* @param pstProcQInfo      [out] Pointer to save decoded data.
*
* @return [out]	success or error.
*                                                                      
*/
BACNET_RETURN_TYPE RP_A_Ack_Decode_Handler(
    processInfo_t *pstProcQInfo,
    uint8_t *pu8ServiceResponse,
    int32_t i32ServiceLen)
{
	/* local variables */
    int32_t i32Len = 0;
    BACNET_CONF_DATA *pstRpData = NULL;
    BACNET_RETURN_TYPE eReturnType = BACDEL_SUCCESS;
	
	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	RP_A_Ack_Decode_Handler: entry \r\n");
	#endif
    
	/* check input pointers */
    if(NULL == pstProcQInfo || NULL == pu8ServiceResponse)
    {
		#if(defined DEBUG_PRINTF && DL_0)
        Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
		RP_A_Ack_Decode_Handler: NULL Input Pointers \r\n");
		#endif
        return BACDEL_ERROR;
	}

	do{ // decode ack

    /* Allocate buffer for read property data */
    pstRpData = (BACNET_CONF_DATA *)OSAL_Malloc(sizeof(BACNET_CONF_DATA),
		__FILE__, __FUNCTION__, __LINE__);

	/* check null pointers */
    if(NULL == pstRpData)
    {
		#if(defined DEBUG_PRINTF && DL_0)
        Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, "APDU: \
		RP_A_Ack_Decode_Handler: Malloc failed for RP-Ack parameters \r\n");
		#endif
        /* set error from client */
		Service_Error_Handler(BACNET_STATUS_ERROR,
			ERROR_CLASS_RESOURCES, ERROR_CODE_OUT_OF_MEMORY,
			pstProcQInfo, FALSE);
		break;
    }

    /* Decode the Service Request */
    i32Len = RP_A_Decode_Ack_Data(pu8ServiceResponse, 
		i32ServiceLen, pstRpData);
    if(i32Len <= 0) 
    {
        /* Free memory for RP service parameters */
		OSAL_Free(pstRpData,  __FILE__, __FUNCTION__, __LINE__);
		pstRpData = NULL;
		#if(defined DEBUG_PRINTF && DL_1)
        Print_DebugMsg(DEBUG_LEVEL1, BACDEL_INVALID_DATA_TYPE, "APDU: \
		RP_A_Ack_Decode_Handler: Rp-Ack Decoding failed \r\n");
		#endif
		/* set error from client */
		Service_Error_Handler(BACNET_STATUS_ERROR,
			ERROR_CLASS_SERVICES, ERROR_CODE_INVALID_PARAMETER_DATA_TYPE,
			pstProcQInfo, FALSE);
		break;
    }

	}while(0); // loop ends

    /* Redirect the service data to Read property data buffer */
    pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData = pstRpData;

    /* Call Object Management Layer interface */
    eReturnType = Initiate_Service_Interface(pstProcQInfo);
    if(BACDEL_CONTINUE != eReturnType )
    {
        /* Free memory for RP service parameters */
		OSAL_Free(pstRpData,  __FILE__, __FUNCTION__, __LINE__);
        pstProcQInfo->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;
		eReturnType = BACDEL_ERROR;
    }

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	RP_A_Ack_Decode_Handler: exit \r\n"); 
	#endif
    return eReturnType;
}

/** 
*
* DESCRIPTION
* Decode the Read-Property complex ack data.
* The property value data is saved in 'pu8PropValueBuffer' buffer 
* to be decoded later.
*
* @param pu8APDUReq  [in]  Ack data to be decoded.
* @param i32APDULen  [in]  Length of data to be decoded.
* @param pstRpData  [out]  Pointer to save decoded data.
*
* @return [out]	Number of decoded bytes. -ve value on decode failure.
*
*/
int32_t RP_A_Decode_Ack_Data(
    uint8_t *pu8APDUReq,
    int32_t i32APDULen,
    BACNET_CONF_DATA *pstRpData)
{
	/* local variables */
    int32_t i32DecodeLen = 0;
    int32_t i32Len = 0;        
    uint32_t u32Property = 0;      
	
	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	RP_A_Decode_Ack_Data: entry \r\n");
	#endif

    /* Decode Object Identifier, Property Id and Array Index */
    i32DecodeLen = Decode_ObjId_Prop_ArrIdx(&pu8APDUReq[i32Len], 
		(i32APDULen - i32Len), pstRpData);

    if(i32DecodeLen < 0)
	{
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_DECODING_FAILED, "APDU: \
		RP_A_Decode_Ack_Data: invalid decode length with error code = %d\r\n",
		pstRpData->eErrorCode);
		#endif
        return i32DecodeLen;
	}
    i32Len += i32DecodeLen;

    /* opening context tag */
    if(!Decode_Is_Opening_Tag_Number(&pu8APDUReq[i32Len], TAG_NO_3))
    {
		/* invalid tag error */
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_TAG_ERROR, "APDU: \
		RP_A_Decode_Ack_Data: invalid opening tag number \r\n");
		#endif
        return BACNET_STATUS_ERROR;
    }

    /* determine the length of the data block i.e. property value data */
    pstRpData->i32ApplicationDataLen = BACApp_Verify_Data_Len(&pu8APDUReq[i32Len], 
		(i32APDULen - i32Len), (BACNET_PROPERTY_ID)u32Property);
    
    if(pstRpData->i32ApplicationDataLen < 0)
    {
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_TAG_ERROR, "APDU: \
		RP_A_Decode_Ack_Data: invalid tag \r\n");
		#endif
        return BACNET_STATUS_ERROR;
    }

    /* a tag number of 3E is not extended so only one octet increment */
    i32Len++;

	/* allocate memory to save data */
    pstRpData->pu8PropValueBuffer = (uint8_t *)OSAL_Malloc(
		(pstRpData->i32ApplicationDataLen + 1), __FILE__, __FUNCTION__, __LINE__);

    if(NULL == pstRpData->pu8PropValueBuffer)
	{
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_MALLOC_ERROR, "APDU: \
		RP_A_Decode_Ack_Data: malloc failed \r\n");
		#endif
        return BACNET_STATUS_ERROR;
	}

	/* copy data */
    memcpy(pstRpData->pu8PropValueBuffer, &pu8APDUReq[i32Len], 
		pstRpData->i32ApplicationDataLen);

    /* add on the data length */
    i32Len += pstRpData->i32ApplicationDataLen;
        
    /* closing context tag */
    if(!Decode_Is_Closing_Tag_Number(&pu8APDUReq[i32Len++], TAG_NO_3))
    {
		/* free allocated memory */
		OSAL_Free(pstRpData->pu8PropValueBuffer, __FILE__, __FUNCTION__, __LINE__);
		pstRpData->pu8PropValueBuffer = NULL;

		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_TAG_ERROR, "APDU: \
		RP_A_Decode_Ack_Data: invalid closing tag number \r\n");
		#endif
        return BACNET_STATUS_ERROR;
    }

    if(i32Len != i32APDULen) 
    {
		/* free allocated memory */
		OSAL_Free(pstRpData->pu8PropValueBuffer, __FILE__, __FUNCTION__, __LINE__);
		pstRpData->pu8PropValueBuffer = NULL;

		/* inconsistent data received */
		#if(defined DEBUG_PRINTF && DL_1)
		Print_DebugMsg(DEBUG_LEVEL1, BACDEL_DECODING_FAILED, "APDU: \
		RP_A_Decode_Ack_Data: inconsistent data received \r\n");
		#endif
        return BACNET_STATUS_ERROR;
    }

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	RP_A_Decode_Ack_Data: exit \r\n");
	#endif
	return i32Len;
}

#ifdef DISPLAY_PARAMETERS
/**
*                                                                         
* DESCRIPTION                                                                          
* This function will print the validated values in stack for RP-A service.
*    
* @param pstRpReq [in] RP Service parameter to be display.
* @return nothing.
*
*/
void RP_A_Display_Data(rp_request_t *pstRpReq)
{
	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	RP_A_Display_Data: entry \r\n");
	#endif

	/* check inpur pointer */
	if(NULL == pstRpReq)
	{
		/* null input pointers */
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
		RP_A_Display_Data: null input pointer \r\n");
		#endif
		return;
	}

    /* Display values */
	printf("\n ----------------- Stack RP-A ---------------- \n");

	printf("\n Object type = %u", pstRpReq->m_eObjectType);
	printf("\n Object instance = %u", pstRpReq->m_u32ObjectInstance);
	printf("\n Property Id = %u", pstRpReq->m_eObjectProperty);
	printf("\n Array index present = %u", pstRpReq->m_u8ArrayIndexPresent);
	printf("\n Array index = %u", pstRpReq->m_u32ArrayIndex);

	/* display of data completed */
	printf("\r\n");

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	RP_A_Display_Data: exit \r\n");
	#endif
    return;
}
#endif /* */

#endif /* BACDEL_SER_DS_RP_A */
