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
*	serviceWriteProperty_B.h
*
*   AUTHORS
*   Harshal Mangale, M. Venu, Anagha C.
*       
*   DESCRIPTION
*   The WriteProperty service is used by a client BACnet-user 
*   to modify the value of one property of a BACnet object.
*	This files includes functions for Write Property service.
*    
******************************************************************************/ 


#ifndef WP_B_H
#define WP_B_H
#ifdef BACDEL_SER_DS_WP_B

/* header files */
#include "objDevice.h"
#include "pduServiceStructure.h"

/**
*
* DESCREPTION
* Function to decode WPM-B request received from network.
*
* @param pu8ServiceRequest  [in]  Data to be decoded.
* @param i32ServiceLen      [in]  Received data length.
* @param pstProcQInfo      [out]  Pointer to save WPM-B data
*
*/
BACNET_RETURN_TYPE WP_B_Decode_Handler(
	processInfo_t *pstProcQInfo,
	uint8_t *pu8ServiceRequest,
	int32_t i32ServiceLen);

/**
*                                                                    
* DESCRIPTION                                                                          
* Function to encode WP-B service acknowledgement.
* Function encodes either simple ack or error ack.
*    
* @param pu8APDUResp   [in]  Buffer to save encoded data
* @param i32APDULen	   [in]  Encoded data length.
* @param pstProcQInfo  [in]  WP service data
*
* @return [out] total encoded data length
*                                                                      
*/
int32_t WP_B_Ack_Encoder(
	processInfo_t *pstProcQInfo,
	uint8_t *pu8APDUResp,
	int32_t i32APDULen);

/** 
*
* DESCRIPTION:
* Function to decode received WP-B service request data.
*
* @param pu8APDUReq  [in]    WP-B data to be decoded
* @param i32APDULen  [in]    Length of received WP-B data
* @param pstWpData   [out]   Structure to store WP-B parameters.
*
* @return i32Len [out] Total decoded length of received request 
*
*/
int32_t WP_B_Decode_Service_Request(
    uint8_t *pu8APDUReq,
    int32_t i32APDULen,
    BACNET_CONF_DATA * pstWpData);

#endif /* BACDEL_SER_DS_WP_B */
#endif /* WP_B_H */