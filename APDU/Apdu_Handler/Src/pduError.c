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
*	pduError.c
*                                                                      
*   AUTHORS                                                                     
*	Harshal Mangale, Ashish Verma
*                                                                         
*   DESCRIPTION                                                            
*	Functions for Encoding and Decoding of Error PDU type.
*
******************************************************************************/

/** header files */
#include "pduEncodeDecode.h"
#include "pduError.h"

/* WPM-B service */
#ifdef BACDEL_SER_DS_WPM_B
#include "serviceWritePropertyMultiple_B.h"
#endif 

/**
*
* DESCRIPTION
* Function to encode Error PDU.
*
* @param  pstProcQInfo [in]  Contains error PDU data to be encoded
* @param  pu8APDUResp  [in]  Buffer to save encoded data
*
* @return [out] Total no of bytes encoded
*
*/
int32_t Error_Encode_Apdu(
	processInfo_t *pstProcQInfo, 
    uint8_t *pu8APDUResp)
{
	/* local variables */
    int32_t i32APDULen = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Error_Encode_Apdu: Entry \r\n");
	#endif

	/* check input pointers */
    if(pu8APDUResp && pstProcQInfo) 
	{
		/* Error PDU type */
        pu8APDUResp[0] = PDU_TYPE_ERROR;
		/* Invoke Id */
        pu8APDUResp[1] = pstProcQInfo->m_stProcessData.m_stAPDU.m_u8InvokeId;
		/* Service Choice */
        pu8APDUResp[2] = pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice;
        i32APDULen = 3;

		/* encode service specific parameters */
		switch(pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice)
		{
			#ifdef BACDEL_SER_DS_WPM_B
            case SERVICE_CONFIRMED_WRITE_PROP_MULTIPLE:
			{
				/* opening tag */
				i32APDULen += Encode_Opening_Tag(&pu8APDUResp[i32APDULen], TAG_NO_0);
				/* error class */
				i32APDULen += Encode_Application_Enumerated(&pu8APDUResp[i32APDULen], 
					pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorClass);
				/* error code */
				i32APDULen += Encode_Application_Enumerated(&pu8APDUResp[i32APDULen], 
					pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode);
				/* closing tag */
				i32APDULen += Encode_Closing_Tag(&pu8APDUResp[i32APDULen], TAG_NO_0);
                break;
			}
			#endif /* WPM-B */

			default:
			{
				/* error class */
				i32APDULen += Encode_Application_Enumerated(&pu8APDUResp[i32APDULen], 
					pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorClass);
				/* error code */
				i32APDULen += Encode_Application_Enumerated(&pu8APDUResp[i32APDULen], 
					pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode);
				break;
			}
		}
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Error_Encode_Apdu: Exit \r\n");
	#endif
    return i32APDULen;
}

/**
*
* DESCRIPTION
* Function to decode Error PDU.
*
* @param pu8APDUReq		[in]  Apdu data to decode
* @param u16PduLen		[in]  Length of received data
* @param pstProcQInfo   [in/out] Process Q node to save decoded data
*
* @return [out] Total length of decoded data
*
*/
int32_t Error_Decode_Apdu(
	processInfo_t *pstProcQInfo, 
	uint8_t *pu8APDUReq, 
	uint16_t u16PduLen)
{
	/* local variables */
    uint8_t u8TagNumber = 0;
    uint32_t u32LenValueType = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Error_Decode_Apdu: Entry \r\n");
	#endif

	/* switch as per service type */
	switch(pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice) 
    {
		case SERVICE_CONFIRMED_WRITE_PROP_MULTIPLE:
		case SERVICE_CONFIRMED_CREATE_OBJECT:
		case SERVICE_CONFIRMED_ADD_LIST_ELEMENT:
		case SERVICE_CONFIRMED_REMOVE_LIST_ELEMENT:
		case SERVICE_CONFIRMED_PRIVATE_TRANSFER:
		{
			/* skip over opening tag 0 */
			if (Decode_Is_Opening_Tag_Number(&pu8APDUReq[u16PduLen], TAG_NO_0)) 
			{
				u16PduLen++;  /* a tag number of 0 is not extended so only one octet */
				break;
			}
			else return 0;
		}

		default:
		{
			// do nothing 
			break;
		}
    }

    /* decode tag */
    u16PduLen += (uint16_t)Decode_Tag_Number_And_Value(&pu8APDUReq[u16PduLen], 
		&u8TagNumber, &u32LenValueType);
	/* validate that the tag is enumerated */
    if(u8TagNumber != BACNET_APPLICATION_TAG_ENUMERATED)
		return 0;
	/* decode error class */
    u16PduLen += (uint16_t)Decode_Enumerated(&pu8APDUReq[u16PduLen], u32LenValueType, 
        &pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorClass);
    
    /* decode tag */
    u16PduLen += (uint16_t)Decode_Tag_Number_And_Value(&pu8APDUReq[u16PduLen], 
        &u8TagNumber, &u32LenValueType);
    /* validate that the tag is enumerated */
	if(u8TagNumber != BACNET_APPLICATION_TAG_ENUMERATED)
		return 0;
	/* decode error code */
    u16PduLen += (uint16_t)Decode_Enumerated(&pu8APDUReq[u16PduLen], u32LenValueType, 
        &pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode);

	/* switch as per service type */
	switch (pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice) 
    {
		case SERVICE_CONFIRMED_WRITE_PROP_MULTIPLE:
		case SERVICE_CONFIRMED_CREATE_OBJECT:
		case SERVICE_CONFIRMED_ADD_LIST_ELEMENT:
		case SERVICE_CONFIRMED_REMOVE_LIST_ELEMENT:
		case SERVICE_CONFIRMED_PRIVATE_TRANSFER:
		{
			/* skip over opening tag 0 */
			if (Decode_Is_Closing_Tag_Number(&pu8APDUReq[u16PduLen], TAG_NO_0)) 
			{
				u16PduLen++;  /* a tag number of 0 is not extended so only one octet */
				break;
			}
			else return 0;
		}

		default:
		{
			// do nothing 
			break;
		}
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Error_Decode_Apdu: Exit \r\n");
	#endif
	return u16PduLen;
}

/************************** end of pduError.c file ***************************/
