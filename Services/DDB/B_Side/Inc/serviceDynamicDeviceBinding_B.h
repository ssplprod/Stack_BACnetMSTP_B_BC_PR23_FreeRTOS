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
*   File Name - serviceDynamicDeviceBinding_B.h
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

#ifndef SER_DDB_B_H
#define SER_DDB_B_H
#ifdef BACDEL_SER_DM_DDB_B

/* header files */
#include "bacnetStackMgmt.h"
#include "objDevice.h"


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
	int32_t i32APDULen);

/**  
*
* DESCRIPTION
* Encodes the I-AM request.
*
* @param  pstProcQIndex	[in]	Pointer to service data.
* @param  pu8APDUResp	[out]   Pointer to Transmit buffer. 								encoded from this structure.
* @param  i32APDULen    [out]   Number of bytes encoded.
*
* @return i32APDULen    [out]   total bytes encoded. 
*
*/
int32_t I_Am_B_Request_Encoder(
   processInfo_t *pstProcQIndex,
   uint8_t *pu8APDUResp,
   int32_t i32APDULen);

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
   uint8_t u8ThreadNo);

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
   processInfo_t * pstProcQInfo);
#endif

#endif /* BACDEL_SER_DM_DDB_B */
#endif /* SER_DDB_B_H */
