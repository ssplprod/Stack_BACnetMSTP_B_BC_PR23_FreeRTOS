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
*	pduError.c
*                                                                      
*   AUTHORS                                                                     
*	Harshal Mangale, Ashish Verma
*                                                                         
*   DESCRIPTION                                                            
*	Functions for Encoding and Decoding of Error PDU type.
*
**********************************************************************************/

#ifndef ERROR_H
#define ERROR_H

/** header files */
#include "bacDELDef.h"
#include "pduServiceStructure.h"
#include "bacnetStackMgmt.h"

/** compile as c code */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

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
    uint8_t *pu8APDUResp);

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
	uint16_t u16PduLen);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* ERROR_H */
