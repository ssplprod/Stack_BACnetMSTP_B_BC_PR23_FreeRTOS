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
*   File Name
*	serviceWritePropertyMultiple_B.h
*
*   AUTHORS
*   Harshal Mangale, M. Venu
*       
*   DESCRIPTION
*   The WritePropertyMultiple service is used by a client BACnet-user 
*   to modify the value of one or more specified properties of a 
*	BACnet object.
*	This files includes functions for Write Property Multiple service.
*    
******************************************************************************/


#ifndef WPM_B_H
#define WPM_B_H
#ifdef BACDEL_SER_DS_WPM_B

/* header file */
#include "objDevice.h"

/**
*
* DESCREPTION
* Function to decode WPM-B request received from network.
*
* @param pu8ServiceRequest  [in]  Data to be decoded.
* @param i32ServiceLen      [in]  Received data length.
* @param pstProcQInfo      [out]  Pointer to save WPM-B data
*
* @return [out] success or error
*
*/
BACNET_RETURN_TYPE WPM_B_Decode_Handler(
	processInfo_t *pstProcQInfo,
	uint8_t *pu8ServiceRequest,
	int32_t i32ServiceLen);
    
/**
*
* DESCRIPTION
* Function to encode and send WPM error ack.
*    
* @param pu8APDUResp   [in]  Buffer to save encoded data
* @param i32APDULen	   [in]  Encoded data length.
* @param pstProcQInfo  [in]  WPM service data
*
* @return [out] total encoded data length
*                                                                      
*/
int32_t WPM_B_Ack_Encoder(                    
	processInfo_t *pstProcQInfo,
	uint8_t *pu8APDUResp,
	int32_t i32APDULen);

/**
*
* DESCRIPTION
* Function to validate and execute WPM-B service.
*
* @param pstVirtualDev [in] Virtual device data.
* @param pstProcQInfo  [in] WPM-B service data. 
* @param u8ThreadNo    [in] Thread number
*
* @return TRUE or FALSE 
*
*/
bool WPM_B_Request_Parser(
	virtualDevData_t *pstVirtualDev, 
	processInfo_t *pstProcQIndex,
	uint8_t u8ThreadNo);

#endif /* BACDEL_SER_DS_WPM_B */
#endif /* WPM_B_H */