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
*   SoftDEL Systems Ltd.                            india@softdel.com         
*   3rd Floor, Pentagon P4,                         http://www.softdel.com  
*    Magarpatta City, Hadapsar
*    Pune - 411 028                                
*                                                                         
*   FILE
*   serviceReadProperty_B.h
*                                                                      
*   AUTHORS                                                                     
*   Prashant Badgujar, Harshal Mangale, M. venu
*                                                                         
*   DESCRIPTION
*   The ReadProperty service is used by a client BACnet-user to request 
*	the value of one property of one BACnet Object.
*   This file include function for RP-B service. 
*                                                                         
**********************************************************************************/


#ifndef RP_B_H
#define RP_B_H
#ifdef BACDEL_SER_DS_RP_B

/* header files */
#include "objDevice.h"

/**
*                                                                    
* DESCRIPTION
* Function to decoded read property service request data.
*    
* @param pu8APDUReq	[in]  Data to be decoded.
* @param i32APDULen	[in]  Length of data to be decoded.
* @param pstRpData	[out] Pointer to save Rp-B data.
*                                   
* @return [out] length of data decoded or -ve value on error.
*	
*/
int32_t RP_B_Decode_Service_Request(
    uint8_t * pu8APDUReq,
    int32_t i32APDULen,
    BACNET_CONF_DATA *pstRpData);

/**
*                                                                    
* DESCRIPTION
* This function is default handler when a read property  
* request is received.
*    
* @param pstProcQInfo		 [in]	 Pointer to save decoded data.
* @param pu8ServiceRequest	 [in]    Data received for decoding.
* @param i32ServiceLen		 [in]    Length of service request.
*                                   
* @return BACNET_RETURN_TYPE [out]   Success or suitable error code.
*	
*/
BACNET_RETURN_TYPE RP_B_Decode_Handler(
    processInfo_t *pstProcQInfo,
    uint8_t *pu8ServiceRequest,
    int32_t i32ServiceLen);

/**
*
* DESCRIPTION                                                                          
* Function to validate and execute read property service.
*    
* @param pVirtualDev	[in]  virtual device data.
* @param pstProcQInfo   [in]  rp-b service data 
* @param u8ThreadNo     [in]  thread number
*                                   
* @return [out] true or false.
*
*/
bool RP_B_Request_Parser(
	virtualDevData_t *pVirtualDev, 
	processInfo_t *pstProcQInfo,
	uint8_t u8ThreadNo);

/**
*
* DESCRIPTION
* Function to encode the RP-B acknowledgement. 
*
* @param pstProcQInfo	[in]	Pointer to service data
* @param pu8APDUResp	[in]    APDU buffer to save encoded data.
* @param i32APDULen		[in]    Length of encoded data in buffer.
*
* @return i32APDULen    [out]   Total length after encoding.
*
*/
int32_t RP_B_Ack_Encoder(
    processInfo_t *pstProcQInfo,
    uint8_t **pu8APDUResp,
    int32_t i32APDULen);

#endif /* BACDEL_SER_DS_RP_B */
#endif /* RP_B_H */