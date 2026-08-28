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
*	pduAbort.c
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
#include "pduAbort.h"
//#include "Debug.h"

/**
*
* DESCRIPTION
* Function to convert error codes to suitable abort reasons.
* Anything not defined converts to ABORT_REASON_OTHER.
* Needs reworking if it is required to return proprietary abort reasons.
*
* @param  eErrorCode   [in]  Error code to be converted
* @return eAbortReason [out] Abort reason value
*
*/
BACNET_ABORT_REASON Abort_Convert_Error_Code(
    BACNET_ERROR_CODE eErrorCode)
{
	/* local variables */
    BACNET_ABORT_REASON eAbortReason = ABORT_REASON_OTHER;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Abort_Convert_Error_Code: Entry \r\n");
	#endif

	/* switch to error code */
    switch(eErrorCode) 
    {
        case ERROR_CODE_ABORT_BUFFER_OVERFLOW:
            eAbortReason = ABORT_REASON_BUFFER_OVERFLOW;
            break;

        case ERROR_CODE_ABORT_INVALID_APDU_IN_THIS_STATE:
            eAbortReason = ABORT_REASON_INVALID_APDU_IN_THIS_STATE;
            break;

        case ERROR_CODE_ABORT_PREEMPTED_BY_HIGHER_PRIORITY_TASK:
            eAbortReason = ABORT_REASON_PREEMPTED_BY_HIGHER_PRIORITY_TASK;
            break;

        case ERROR_CODE_ABORT_SEGMENTATION_NOT_SUPPORTED:
            eAbortReason = ABORT_REASON_SEGMENTATION_NOT_SUPPORTED;
            break;

        case ERROR_CODE_ABORT_PROPRIETARY:
            eAbortReason = FIRST_PROPRIETARY_ABORT_REASON;
            break;

		case ERROR_CODE_ABORT_APDU_TOO_LONG:
			eAbortReason = ABORT_REASON_APDU_TOO_LONG;
			break;

		case ERROR_CODE_ABORT_APPLICATION_EXCEEDED_REPLY_TIME:
			eAbortReason = ABORT_REASON_APPLICATION_EXCEEDED_REPLY_TIME;
			break;

		case ERROR_CODE_ABORT_OUT_OF_RESOURCES:
			eAbortReason = ABORT_REASON_OUT_OF_RESOURCES;
			break;

		case ERROR_CODE_ABORT_TSM_TIMEOUT:
			eAbortReason = ABORT_REASON_TSM_TIMEOUT;
			break;

		case ERROR_CODE_ABORT_WINDOW_SIZE_OUT_OF_RANGE:
			eAbortReason = ABORT_REASON_WINDOW_SIZE_OUT_OF_RANGE;
			break;

		case ERROR_CODE_ABORT_SECURITY_ERROR:
			eAbortReason = ABORT_REASON_SECURITY_ERROR;
			break;

		case ERROR_CODE_ABORT_INSUFFICIENT_SECURITY:
			eAbortReason = ABORT_REASON_INSUFFICIENT_SECURITY;
			break;

        case ERROR_CODE_ABORT_OTHER:
        default:
            eAbortReason = ABORT_REASON_OTHER;
            break;
    }// switch ends

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Abort_Convert_Error_Code: Exit \r\n");
	#endif
    return eAbortReason;
}

/**
*
* DESCRIPTION
* Function to convert abort reasons to suitable error codes.
* Anything not defined converts to ERROR_CODE_ABORT_OTHER.
* Needs reworking if it is required to return proprietary error codes.
*
* @param  eAbortReason [in]  Abort reason to be converted
* @return eErrorCode   [out] Error code value
*
*/
BACNET_ERROR_CODE Abort_Convert_AbortReason_To_ErrorCode(
    BACNET_ABORT_REASON eAbortReason)
{
	/* local variables */
    BACNET_ERROR_CODE eErrorCode = ERROR_CODE_ABORT_OTHER;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
    Abort_Convert_AbortReason_To_ErrorCode: Entry \r\n");
	#endif

	/* switch to abort reason */
    switch(eAbortReason) 
    {
        case ABORT_REASON_BUFFER_OVERFLOW:
            eErrorCode = ERROR_CODE_ABORT_BUFFER_OVERFLOW;
            break;

        case ABORT_REASON_INVALID_APDU_IN_THIS_STATE:
            eErrorCode = ERROR_CODE_ABORT_INVALID_APDU_IN_THIS_STATE;
            break;

        case ABORT_REASON_PREEMPTED_BY_HIGHER_PRIORITY_TASK:
            eErrorCode = ERROR_CODE_ABORT_PREEMPTED_BY_HIGHER_PRIORITY_TASK;
            break;

        case ABORT_REASON_SEGMENTATION_NOT_SUPPORTED:
            eErrorCode = ERROR_CODE_ABORT_SEGMENTATION_NOT_SUPPORTED;
            break;

		case ABORT_REASON_SECURITY_ERROR:
            eErrorCode = ERROR_CODE_ABORT_SECURITY_ERROR;
			break;

		case ABORT_REASON_INSUFFICIENT_SECURITY:
            eErrorCode = ERROR_CODE_ABORT_INSUFFICIENT_SECURITY;
			break;

		case ABORT_REASON_WINDOW_SIZE_OUT_OF_RANGE:
            eErrorCode = ERROR_CODE_ABORT_WINDOW_SIZE_OUT_OF_RANGE;
			break;

		case ABORT_REASON_APPLICATION_EXCEEDED_REPLY_TIME:
            eErrorCode = ERROR_CODE_ABORT_APPLICATION_EXCEEDED_REPLY_TIME;
			break;

		case ABORT_REASON_OUT_OF_RESOURCES:
            eErrorCode = ERROR_CODE_ABORT_OUT_OF_RESOURCES;
			break;

		case ABORT_REASON_TSM_TIMEOUT:
            eErrorCode = ERROR_CODE_ABORT_TSM_TIMEOUT;
			break;

		case ABORT_REASON_APDU_TOO_LONG:
            eErrorCode = ERROR_CODE_ABORT_APDU_TOO_LONG;
			break;

        case FIRST_PROPRIETARY_ABORT_REASON:
            eErrorCode = ERROR_CODE_ABORT_PROPRIETARY;
            break;

        case ABORT_REASON_OTHER:
        default:
            eErrorCode = ERROR_CODE_ABORT_OTHER;
            break;
    }// switch ends

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
    Abort_Convert_AbortReason_To_ErrorCode: Exit \r\n");
	#endif
    return eErrorCode;
}

/**
*
* DESCRIPTION
* Function to encode Abort PDU.
*
* @param  pstProcQInfo [in]  Contains abort PDU data to be encoded
* @param  pu8APDUResp  [in]  Buffer to save encoded data
*
* @return [out] Total no of bytes encoded
*
*/
int32_t Abort_Encode_Apdu(
    processInfo_t *pstProcQInfo,
    uint8_t *pu8APDUResp)
{
	/* local variables */
    int32_t i32APDULen = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Abort_Encode_Apdu: Entry \r\n");
	#endif

	/* check input pointers */
    if(pu8APDUResp && pstProcQInfo) 
    {
		/* PDU type and Server flag */
		pu8APDUResp[0] = PDU_TYPE_ABORT;
        if (pstProcQInfo->m_stProcessData.m_stAPDU.m_bServer)
		{
			/* update client or server flag */
            pu8APDUResp[0] = PDU_TYPE_ABORT | 1;
		}

		/* Invoke ID */
        pu8APDUResp[1] = pstProcQInfo->m_stProcessData.m_stAPDU.m_u8InvokeId;

		/* Abort Reason */
        pu8APDUResp[2] = (uint8_t)pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode;

		/* Abort PDU is always 3 bytes */
        i32APDULen = 3;
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Abort_Encode_Apdu: Exit \r\n");
	#endif
    return i32APDULen;
}

/**
*
* DESCRIPTION
* Function to decode Abort PDU.
*
* @param pu8APDUReq		[in]     Apdu data to decode
* @param u32APDULen		[in]     Length of received data
* @param pu8InvokeId	[in/out] Invoke Id received
* @param pu8AbortReason [in/out] Abort reason received
*
* @return [out] Total length of decoded data
*
*/
int32_t Abort_Decode_Apdu(
    uint8_t *pu8APDUReq,
    uint32_t u32APDULen,
    uint8_t *pu8InvokeId,
    uint8_t *pu8AbortReason)
{
	/* local variables */
    int32_t i32Len = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Abort_Decode_Apdu: Entry \r\n");
	#endif

	/* check input parameters */
    if(u32APDULen > 0 && pu8APDUReq && pu8InvokeId && pu8AbortReason) 
	{
		/* invoke id */
        *pu8InvokeId = pu8APDUReq[i32Len++];

		/* abort reason */
        *pu8AbortReason = pu8APDUReq[i32Len++];
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
	Abort_Decode_Apdu: Exit \r\n");
	#endif
    return i32Len;
}

/************************** end of pduAbort.c file ***************************/