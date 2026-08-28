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
*                                                                        
*   SoftDEL Systems Ltd.                             india@softdel.com         
*   3rd Floor, Pentagon P4,                          http://www.softdel.com  
*   Magarpatta City, Hadapsar
*   Pune - 411 028       
*
*   File Name - serviceDynamicObjectBinding_B.h
*
*   AUTHORS                                                                     
*   Ashish V., Anagha C.    
*                                                                         
*   DESCRIPTION                                                            
*	File includes all functions required for DOB-B functionality.
*   - Who-Has request decoder.
*   - Who-Has request parser.
*   - I-Have request encoder. 
*
******************************************************************************/

#ifndef SER_DOB_B_H
#define SER_DOB_B_H
#ifdef BACDEL_SER_DM_DOB_B

//#include "Debug.h"
#include "bacnetStackMgmt.h"
#include "objDevice.h"

/** 
*
* DESCRIPTION
* Decodes the WHO-HAS request.
*
* @param  pstProcQIndx		 [in]  Pointer to save decoded data.
* @param  pu8APDU			 [in]  Buffer containing data to be decoded.
* @param  i32APDULen	     [in]  Length of received request data.
*
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*                                                                      
*/
BACNET_RETURN_TYPE Who_Has_B_Decode_Handler( 
	processInfo_t *pstProcQIndx,                                      
	uint8_t *pu8APDU, 
	int32_t i32APDULen);

/**
*
* DESCRIPTION
* Check if device falls within range to reply with an I-HAVE. 
* Intiate function call depending upon search of Object-Name or Object-ID. 
*
* @param  pstVirtualDev  [in]   Pointer to virtual device data structure.
* @param  pstProcQInfo   [in]   Pointer to process info queue structure.
* @param u8ThreadNo      [in]   Thread number
*
* @return bool           [out]  True/false
*
*/
bool Who_Has_B_Request_Parser( 
	virtualDevData_t *pstVirtualDev, 
	processInfo_t * pstProcQInfo,
	uint8_t u8ThreadNo);

/**
* DESCRIPTION
* Search for requested Object Name or OBject ID.
*
* @param pstVirtualDev  [in]	Pointer to virtual device data structure.
* @param bSearchElement [in]    Search for requested Object Name or OBject ID.				 
* @param pstProcQInfo   [in]	Pointer to process info queue structure.
*
* @return bool          [out]   True/false
*
*/
bool Who_Has_B_Search_Object( 
	virtualDevData_t *pstVirtualDev, 
	bool bSearchElement, 
	processInfo_t * pstProcQInfo);

/**  
*
* DESCRIPTION
* Encodes the I-HAVE response.
*
* @param  pstProcQIndex [in]  Pointer to processInfo_t structure.
*
* @param  pu8APDUResp   [out] Pointer to Transmit buffer.
* @param  i32APDULen    [out] Transmit buffer fill Index.
* @return				[out] Number of Bytes consumed. 
*
*/
int32_t I_Have_B_Request_Encoder( //DOB_B_I_HAVE_Handler
	processInfo_t *pstProcQIndex,
	uint8_t *pu8APDUResp, 
	int32_t i32APDULen);

#endif /* BACDEL_SER_DM_DOB_B */
#endif /* SER_DOB_B_H */
