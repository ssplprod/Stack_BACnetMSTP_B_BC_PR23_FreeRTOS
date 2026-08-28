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
*   File Name - serviceDynamicDeviceBinding_A.h
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

#ifndef SER_DDB_A_H
#define SER_DDB_A_H
#ifdef BACDEL_SER_DM_DDB_A

/* header files */
#include "bacnetStackMgmt.h"
#include "objDevice.h"

/** macros */
#define I_AM_MIN_DATA_LENGTH 10


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
	int32_t i32APDULen);

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
	int32_t i32APDULen);


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
	uint8_t u8ThreadNo);
#endif /* BACDEL_SER_DM_DDB_A */
#endif /* SER_DDB_A_H */
