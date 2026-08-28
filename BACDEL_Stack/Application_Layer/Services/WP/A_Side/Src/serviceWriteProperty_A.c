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
*   serviceWriteProperty_A.c
*
*   AUTHORS
*   Harshal Mangale, M.Venu
*
*	DESCRIPTION 
*	The WriteProperty service is used by a client BACnet-user to modify 
*	the value of a single specified property of a BACnet object. 
*	This service potentially allows write access to any property of any 
*	object, whether a BACnet-defined object or not.
*   This file includes all functions related to WP-A service.   
*
*********************************************************************************/

#ifdef BACDEL_SER_DS_WP_A

/* include header files */
#include "serviceWriteProperty_A.h"
#include "pduEncodeDecode.h"
#include "pduDataEncodeDecode.h"
#include "propertyGenricHandler.h"
#include "miscMiscellaneous.h"

/**
*                                                                         
* DESCRIPTION                                                                          
* This function will encode the WP-A service request.
*    
* @param pstProcQInfo    [in] Service parameter to be encoded.
* @param pu8ApduBuff [in/out] Buffer to store Encoded APDU data.
* @param i32APDULen  [in/out] Encoded APDU data length.
*
* @return i32APDULen    [out] Encoded data length.
*
*/
int32_t WP_A_Request_Encoder(
	processInfo_t *pstProcQInfo,
    uint8_t **pu8ApduBuff,
    int32_t i32APDULen)
{
	/* local variables */
    BACNET_CONF_DATA *pstWpData = NULL;
    
	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	WP_A_Request_Encoder: entry \r\n");
	#endif

	/* get data to be encode */
    pstWpData = (BACNET_CONF_DATA *)pstProcQInfo->m_stProcessData.
		m_stAPDU.m_pvServiceRequestData;
    
	/* check input pointer */
    if(NULL == pstWpData)
    {
		#if(defined DEBUG_PRINTF && DL_0)
        Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
		WP_A_Request_Encoder: NULL Input Pointers \r\n");
		#endif
        return 0;
    }

    /* Encode Obj_ID, PropID, Array_Index and Prop_Val */
    i32APDULen = Wp_A_Encode_Data(pu8ApduBuff, pstWpData, i32APDULen);

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	WP_A_Request_Encoder: exit \r\n");
	#endif
    return i32APDULen;
}

/** 
*
* DESCRIPTION
* Function to Encode WP-A service request.
*
* @param pu8APDUResp  [in/out]  APDU buffer to store encoded data.
* @param u32APDULen   [in/out]  Encoded APDU data length.
* @param pstWpData		  [in]  Write property request data.
*
* @return u32APDULen     [out]	Total Encoded length of request.
*
*/
int32_t Wp_A_Encode_Data(
    uint8_t **pu8ApduBuff,
    BACNET_CONF_DATA *pstWpData, 
    uint32_t u32APDULen)
{
    /* local variables */
	#if (defined CONSOLE_APPLICATION || defined OLD_RP_WP_INTERFACE)
    BACNET_PROPERTY_VALUE stPropertyValue = {0};
	#endif
    int32_t i32Len = 0;
    uint32_t u32CopyLen = 0;
    //bool bCopyError = false;
    uint8_t u8TempBuf[MAX_APDU_BUFFER] = {0};

	#ifdef CONSOLE_APPLICATION
    BACNET_APPLICATION_TAG ePropertyTag = 0;
    bool bStatus = false;
	#endif

	#ifdef SEGMENTATION_SUPPORTED
	uint32_t u32Count = MAX_APDU_LENGTH_ACCEPTED;
	#endif 

    /* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	Wp_A_Encode_Data: entry \r\n");
	#endif
	
    if(NULL != pu8ApduBuff && NULL != *pu8ApduBuff) 
    {
        /* Encode Obj_ID, PropID, Array_Index */
        i32Len = Encode_ObjId_Prop_ArrIdx(&u8TempBuf[0], pstWpData);

        /* Encode opening Tag for property value */
        i32Len += Encode_Opening_Tag(&u8TempBuf[i32Len], TAG_NO_3);

		#ifdef SEGMENTATION_SUPPORTED
        u32CopyLen = Memcopy_Dynamic_Mem_Expansion((void **)pu8ApduBuff, &u8TempBuf[0],
        		u32APDULen, i32Len, &u32Count, MAX_APDU_LENGTH_ACCEPTED);
		#else
		u32CopyLen = Memcopy(*pu8ApduBuff, &u8TempBuf[0], u32APDULen,
            i32Len, MAX_APDU_LENGTH_ACCEPTED);
		#endif 
        if (u32CopyLen == 0 && i32Len != 0) 
        {
            //bCopyError = true;
            /* Buffer is full and no space to fill response */
            /* FIXME: Notify Application That Can't send Request */
        } 
        u32APDULen += i32Len;

		#ifdef CONSOLE_APPLICATION
        /* Encode property value (App Tag + Value) */
        GetPropertyDataType(pstWpData->eObjectType, pstWpData->eObjectProperty, &ePropertyTag );
        
        /* Verify if the value recived is not NULL */
        if((memcmp((pstWpData->pu8PropValueBuffer), "NULL", 4) == (int)NULL) ||
            (memcmp((pstWpData->pu8PropValueBuffer), "null", 4) == (int)NULL))
        {
            ePropertyTag = BACNET_APPLICATION_TAG_NULL;   
        }
        /* Extract the value from the respective structure */
        bStatus = ConvertInStr_To_AppTag(ePropertyTag,
            (int8_t *)pstWpData->pu8PropValueBuffer, &stPropertyValue);
        if(!bStatus) 
        {
			;
            /* FIXME: show the expected entry format for the tag */
			#if(defined DEBUG_PRINTF && DL_2)
            Print_DebugMsg(DEBUG_LEVEL2, BACDEL_INFORMATIVE_MESSAGE, "APDU: \
			Wp_A_Encode_Data: unable to parse the tag value \r\n");
			#endif
        }
		#elif defined OLD_RP_WP_INTERFACE
        /* Copy Data if received as Structure */
        memcpy(&stPropertyValue, pstWpData->pvReadPropValue, sizeof(BACNET_PROPERTY_VALUE));
		#endif

		#ifdef OLD_RP_WP_INTERFACE
        /* Encode Property Value */
        i32Len = BACApp_Encode_data(&u8TempBuf[0], &stPropertyValue, pstWpData->eObjectProperty);
		#elif defined NEW_RP_WP_INTERFACE
		/* Encode Property Value */
		i32Len = BACApp_Encode_Data_Type(&u8TempBuf[0], sizeof(u8TempBuf), 
			pstWpData->pvReadPropValue, pstWpData->eData_Type, 0, 0, 
			SEGMENTATION_BOTH, false, MAX_APDU_LENGTH_ACCEPTED, NULL, NULL, 0);
		#endif

		/* check encoded data length */
		if(i32Len < 0)
		{
			/* error encoding property value */
			/* FIXME: Notify Application That Can't send Request */
			i32Len = 0; // make length 0 to continue encoding closing tag
		}

		#ifdef SEGMENTATION_SUPPORTED
        u32CopyLen = Memcopy_Dynamic_Mem_Expansion((void **)pu8ApduBuff, &u8TempBuf[0],
        	u32APDULen, i32Len, &u32Count, MAX_APDU_LENGTH_ACCEPTED);
		#else
		u32CopyLen = Memcopy(*pu8ApduBuff, &u8TempBuf[0], u32APDULen,
            i32Len, MAX_APDU_LENGTH_ACCEPTED);
		#endif 
        if(u32CopyLen == 0 && i32Len != 0) 
        {
            //bCopyError = true;
            /* Buffer is full and no space to fill response */
            /* FIXME: Notify Application That Can't send Request */
        } 
        u32APDULen += i32Len;

        /* Encode Closing Tag for property value */
        i32Len = Encode_Closing_Tag(&u8TempBuf[0], TAG_NO_3);

        /* optional priority - 0 if not set, 1..16 if set */
        if (pstWpData->u8Priority != BACNET_NO_PRIORITY)
        {
            i32Len += Encode_Context_Unsigned(&u8TempBuf[i32Len], 
                TAG_NO_4, pstWpData->u8Priority);
        }

		#ifdef SEGMENTATION_SUPPORTED
        u32CopyLen = Memcopy_Dynamic_Mem_Expansion((void **)pu8ApduBuff, &u8TempBuf[0], 
        	u32APDULen, i32Len, &u32Count, MAX_APDU_LENGTH_ACCEPTED);
		#else
		u32CopyLen = Memcopy(*pu8ApduBuff, &u8TempBuf[0], u32APDULen,
            i32Len, MAX_APDU_LENGTH_ACCEPTED);
		#endif 
        if(u32CopyLen == 0 && i32Len != 0) 
        {
            //bCopyError = true;
            /* Buffer is full and no space to fill response */
            /* FIXME: Notify Application That Can't send Request */
        } 
        u32APDULen += i32Len;

        /* Free malloced memory if any */
        //Clear_Property_Value(pstWpData->pvReadPropValue);
        //OSAL_Free(pstWpData->pvReadPropValue, __FILE__,__FUNCTION__,__LINE__);
        //pstWpData->pvReadPropValue = NULL;
    }

    /* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
	Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	Wp_A_Encode_Data: exit \r\n");
	#endif

	/* return the encoded length */
    return u32APDULen;
}

#ifdef DISPLAY_PARAMETERS
/**
*                                                                         
* DESCRIPTION                                                                          
* This function will print the validated values in stack for WP-A service.
*    
* @param pstWpReq    [in] WP Service parameter to be display.
* @return nothing.
*
*/
void Wp_A_Display_Data(wp_request_t *pstWpReq)
{
	/* function entry */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_ENTRY_MESSAGE, "APDU: \
	Wp_A_Display_Data: entry \r\n");
	#endif
  
	/* validate input pointer */ 
	if(NULL == pstWpReq)
	{
		/* null input pointers */
		#if(defined DEBUG_PRINTF && DL_0)
		Print_DebugMsg(DEBUG_LEVEL0, BACDEL_INVALID_INPUT_PARAMETER, "APDU: \
		Wp_A_Display_Data: Null input pointer \r\n");
		#endif
		return;
	}

     /* Display values */
	printf("\n ----------------- Stack WP-A ---------------- \n");

	printf("\n Object type = %u", pstWpReq->m_eObjectType);
	printf("\n Object instance = %u", pstWpReq->m_u32ObjectInstance);
	printf("\n Property Id = %u", pstWpReq->m_eObjectProperty);
	printf("\n Priority = %u", pstWpReq->m_u32Priority);
	printf("\n Array index present = %u", pstWpReq->m_u8ArrayIndexPresent);
	printf("\n Array index = %u", pstWpReq->m_u32ArrayIndex);
	#ifdef NEW_RP_WP_INTERFACE
	printf("\n Data Type = %u", pstWpReq->m_eDataType);
	#endif

	/* display of data completed */
	printf("\r\n");

	/* function exit */
	#if(defined DEBUG_PRINTF && DL_3)
    Print_DebugMsg(DEBUG_LEVEL3, BACDEL_EXIT_MESSAGE, "APDU: \
	Wp_A_Display_Data: exit \r\n");
	#endif
    return;
}
#endif /* */

#endif /* BACDEL_SER_DS_WP_A */
