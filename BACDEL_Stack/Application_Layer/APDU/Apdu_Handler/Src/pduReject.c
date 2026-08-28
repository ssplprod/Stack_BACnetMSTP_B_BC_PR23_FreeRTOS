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
*	pduReject.c
*                                                                      
*   AUTHORS                                                                     
*	Harshal Mangale
*                                                                         
*   DESCRIPTION                                                            
*	Functions for Encoding and Decoding of Abort PDU type.
*
******************************************************************************/

/** header files */
#include "pduEncodeDecode.h"
#include "pduReject.h"
//#include "Debug.h"

/**
*
* DESCRIPTION
* Function to convert error codes to suitable reject reasons.
* Anything not defined converts to REJECT_REASON_OTHER.
* Needs reworking if it is required to return proprietary reject reasons.
*
* @param  eErrorCode    [in]  Error code to be converted
* @return eRejectReason [out] Reject reason value
*
*/
BACNET_REJECT_REASON Reject_Convert_Error_Code(
	BACNET_ERROR_CODE eErrorCode)
{
	/* local variables */
    BACNET_REJECT_REASON eRejectReason = REJECT_REASON_OTHER;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Reject_Convert_Error_Code: Entry \r\n");
	#endif

	/* switch to error code */
    switch(eErrorCode) 
    {
        case ERROR_CODE_REJECT_BUFFER_OVERFLOW:
            eRejectReason = REJECT_REASON_BUFFER_OVERFLOW;
            break;

        case ERROR_CODE_REJECT_INCONSISTENT_PARAMETERS:
            eRejectReason = REJECT_REASON_INCONSISTENT_PARAMETERS;
            break;

        case ERROR_CODE_REJECT_INVALID_PARAMETER_DATA_TYPE:
            eRejectReason = REJECT_REASON_INVALID_PARAMETER_DATA_TYPE;
            break;

        case ERROR_CODE_REJECT_INVALID_TAG:
            eRejectReason = REJECT_REASON_INVALID_TAG;
            break;

        case ERROR_CODE_REJECT_MISSING_REQUIRED_PARAMETER:
            eRejectReason = REJECT_REASON_MISSING_REQUIRED_PARAMETER;
            break;

        case ERROR_CODE_REJECT_PARAMETER_OUT_OF_RANGE:
            eRejectReason = REJECT_REASON_PARAMETER_OUT_OF_RANGE;
            break;

        case ERROR_CODE_REJECT_TOO_MANY_ARGUMENTS:
            eRejectReason = REJECT_REASON_TOO_MANY_ARGUMENTS;
            break;

        case ERROR_CODE_REJECT_UNDEFINED_ENUMERATION:
            eRejectReason = REJECT_REASON_UNDEFINED_ENUMERATION;
            break;

        case ERROR_CODE_REJECT_UNRECOGNIZED_SERVICE:
            eRejectReason = REJECT_REASON_UNRECOGNIZED_SERVICE;
            break;

        case ERROR_CODE_REJECT_PROPRIETARY:
            eRejectReason = FIRST_PROPRIETARY_REJECT_REASON;
            break;

        case ERROR_CODE_REJECT_OTHER:
        default:
            eRejectReason = REJECT_REASON_OTHER;
            break;
    }// switch ends

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Reject_Convert_Error_Code: Exit \r\n");
	#endif
    return eRejectReason;
}

/**
*
* DESCRIPTION
* Function to convert reject reasons to suitable error codes.
* Anything not defined converts to ERROR_CODE_REJECT_OTHER.
* Needs reworking if it is required to return proprietary error codes.
*
* @param  eRejectReason [in]  Reject reason to be converted
* @return eErrorCode    [out] Error code value
*
*/
BACNET_ERROR_CODE Reject_Convert_RejectReason_To_ErrorCode(
    BACNET_REJECT_REASON eRejectReason)
{
	/* local variables */
    BACNET_ERROR_CODE eErrorCode = ERROR_CODE_REJECT_OTHER;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Reject_Convert_RejectReason_To_ErrorCode: Entry \r\n");
	#endif

	/* switch to reject reason */
    switch(eRejectReason) 
    {
        case REJECT_REASON_BUFFER_OVERFLOW:
            eErrorCode = ERROR_CODE_REJECT_BUFFER_OVERFLOW;
            break;

        case REJECT_REASON_INCONSISTENT_PARAMETERS:
            eErrorCode = ERROR_CODE_REJECT_INCONSISTENT_PARAMETERS;
            break;

        case REJECT_REASON_INVALID_PARAMETER_DATA_TYPE:
            eErrorCode = ERROR_CODE_REJECT_INVALID_PARAMETER_DATA_TYPE;
            break;

        case REJECT_REASON_INVALID_TAG:
            eErrorCode = ERROR_CODE_REJECT_INVALID_TAG;
            break;

        case REJECT_REASON_MISSING_REQUIRED_PARAMETER:
            eErrorCode = ERROR_CODE_REJECT_MISSING_REQUIRED_PARAMETER;
            break;

        case REJECT_REASON_PARAMETER_OUT_OF_RANGE:
            eErrorCode = ERROR_CODE_REJECT_PARAMETER_OUT_OF_RANGE;
            break;

        case REJECT_REASON_TOO_MANY_ARGUMENTS:
            eErrorCode = ERROR_CODE_REJECT_TOO_MANY_ARGUMENTS;
            break;

        case REJECT_REASON_UNDEFINED_ENUMERATION:
            eErrorCode = ERROR_CODE_REJECT_UNDEFINED_ENUMERATION;
            break;

        case REJECT_REASON_UNRECOGNIZED_SERVICE:
            eErrorCode = ERROR_CODE_REJECT_UNRECOGNIZED_SERVICE;
            break;

        case FIRST_PROPRIETARY_REJECT_REASON:
            eErrorCode = ERROR_CODE_REJECT_PROPRIETARY;
            break;

        case REJECT_REASON_OTHER:
        default:
			eErrorCode = ERROR_CODE_REJECT_OTHER;
            break;
    }// switch ends

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Reject_Convert_RejectReason_To_ErrorCode: Exit \r\n");
	#endif
    return eErrorCode;
}

/**
*
* DESCRIPTION
* Function to encode Reject PDU.
*
* @param  pstProcQInfo [in]  Contains reject PDU data to be encoded
* @param  pu8APDUResp  [in]  Buffer to save encoded data
*
* @return [out] Total no of bytes encoded
*
*/
int32_t Reject_Encode_Apdu(
	processInfo_t *pstProcQInfo, 
	uint8_t *pu8APDUResp)
{
	/* local variables */
    int32_t i32APDULen = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Reject_Encode_Apdu: Entry \r\n");
	#endif

	/* check input pointers */
    if(pu8APDUResp && pstProcQInfo) 
    {
		/* PDU type */
		pu8APDUResp[0] = PDU_TYPE_REJECT;

		/* Invoke ID */
        pu8APDUResp[1] = pstProcQInfo->m_stProcessData.m_stAPDU.m_u8InvokeId;

		/* Reject Reason */
        pu8APDUResp[2] = (uint8_t)pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode;

		/* Abort PDU is always 3 bytes */
        i32APDULen = 3;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Reject_Encode_Apdu: Exit \r\n");
	#endif
    return i32APDULen;
}

/**
*
* DESCRIPTION
* Function to decode Reject PDU.
*
* @param pu8APDUReq		 [in]     Apdu data to decode
* @param u32APDULen		 [in]     Length of received data
* @param pu8InvokeId	 [in/out] Invoke Id received
* @param pu8RejectReason [in/out] Reject reason received
*
* @return [out] Total length of decoded data
*
*/
int32_t Reject_Decode_Apdu(
	uint8_t *pu8APDUReq, 
	uint32_t u32APDULen,
	uint8_t *pu8InvokeId,
    uint8_t *pu8RejectReason)
{
	/* local variables */
    int32_t i32Len = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Reject_Decode_Apdu: Entry \r\n");
	#endif

	/* check input parameters */
	if(u32APDULen > 0 && pu8APDUReq && pu8InvokeId && pu8RejectReason) 
    {
		/* invoke id */
        *pu8InvokeId = pu8APDUReq[i32Len++];

		/* reject reason */
        *pu8RejectReason = pu8APDUReq[i32Len++];
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Reject_Decode_Apdu: Exit \r\n");
	#endif
    return i32Len;
}

/************************** end of pduReject.c file ***************************/
