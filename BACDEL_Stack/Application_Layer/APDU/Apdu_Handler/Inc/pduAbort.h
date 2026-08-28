/**********************************************************************************
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
*	pduAbort.h
*                                                                      
*   AUTHORS                                                                     
*	Harshal Mangale
*                                                                         
*   DESCRIPTION                                                            
*	Functions for Encoding and Decoding of Abort PDU type.
*
**********************************************************************************/

#ifndef ABORT_H
#define ABORT_H

/** header files */
#include "bacDELDef.h"
#include "pduServiceStructure.h"

/** compile as c code */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
*
* DESCRIPTION
* Function to convert error codes to suitable abort reasons.
* Anything not defined converts to ABORT_REASON_OTHER.
* Needs reworking if it is required to return proprietary abort codes.
*
* @param  eErrorCode   [in]  Error code to be converted
* @return eAbortReason [out] Abort reason value
*
*/
BACNET_ABORT_REASON Abort_Convert_Error_Code(
    BACNET_ERROR_CODE eErrorCode);

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
    BACNET_ABORT_REASON eAbortReason);

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
    uint8_t *pu8APDUResp);

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
    uint8_t *pu8AbortReason);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* ABORT_H */
