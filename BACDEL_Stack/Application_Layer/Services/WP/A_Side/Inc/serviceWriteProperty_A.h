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
*   SoftDEL Systems Ltd.                         india@softdel.com      
*   3rd Floor, Pentagon P4,                      http://www.softdel.com  
*   Magarpatta City, Hadapsar
*   Pune - 411 028                                 
*             
*	FILE
*   serviceWriteProperty_A.c
*
*   AUTHORS
*   Harshal Mangale, M.Venu
*
*	DESCRIPTION 
*	The WriteProperty service is used by a client BACnet-user to modify 
*	the value of a single specified property of a BACnet object. 
*	This service potentially allows write access to any property of any 
*	object, whether a BACnet-defined object or not.
*   This file includes all functions related to WP-A service.   
*
*********************************************************************************/

#ifndef WRITEPROPERTY_A_H
#define WRITEPROPERTY_A_H
#ifdef BACDEL_SER_DS_WP_A

/* include header files */
#include "objDevice.h"

/**
*                                                                         
* DESCRIPTION                                                                          
* This API will Encode the WP-A service request and call the  
* Npdu encoder.
*    
* @param pstProcQInfo    [in]    Service parameters to encode.
* @param pu8APDUResp     [out]   APDU buffer to store encoded data.
* @param i32APDULen      [in]    Encoded APDU data length.
*
* @return i32APDULen	 [out]	 encoded data length.
*
*/
int32_t WP_A_Request_Encoder(
	processInfo_t *pstProcQInfo,
    uint8_t **pu8APDUResp,
    int32_t i32APDULen);
    
/** 
*
* DESCRIPTION
* Function to Encode WP-A service request.
*
* @param pu8APDUResp  [in/out]  APDU buffer to store encoded data.
* @param u32APDULen   [in/out]  Encoded APDU data length.
* @param pstWpData		  [in]  Write property request data.
*
* @return u32APDULen     [out]	Total Encoded length of request.
*
*/
int32_t Wp_A_Encode_Data(
    uint8_t **pu8ApduBuff,
    BACNET_CONF_DATA *pstWpData, 
    uint32_t u32APDULen);

#ifdef DISPLAY_PARAMETERS
/**
*                                                                         
* DESCRIPTION                                                                          
* This function will print the validated values in stack for WP-A service.
*    
* @param pstWpReq    [in] WP Service parameter to be display.
* @return nothing.
*
*/
void Wp_A_Display_Data(wp_request_t *pstWpReq);
#endif

#endif /* */
#endif /* WRITEPROPERTY_A_H */