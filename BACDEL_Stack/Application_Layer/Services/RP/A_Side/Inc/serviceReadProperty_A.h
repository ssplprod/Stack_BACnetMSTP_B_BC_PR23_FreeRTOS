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
*	serviceReadProperty_A.h
*                                                            
*   AUTHORS                                                                     
*   Harshal Mangale
*                                                                         
*   DESCRIPTION 
*	The ReadProperty service is used by a client BACnet-user to request the 
*	value of one property of one BACnet Object. This service allows read 
*	access to any property of any object, whether a BACnet-defined object or not.
*   This file includes all functions related to RP-A service.   
*                                                                         
*********************************************************************************/

#ifndef READPROPERTY_A_H
#define READPROPERTY_A_H
#ifdef BACDEL_SER_DS_RP_A

/** header files */
#include "bacDELDef.h"
#include "bacDELApi.h"
#include "pduServiceStructure.h"
#include "bacnetAPDUHandler.h"

/**
*                                                                         
* DESCRIPTION                                                                          
* This function will encode the RP-A service request. 
*    
* @param pstProcQInfo    [in] Service parameter to be encoded.
* @param pu8ApduBuff [in/out] Buffer to store Encoded APDU data.
* @param i32APDULen  [in/out] Encoded APDU data length.
*
* @return i32APDULen    [out] Encoded data length.
*
*/
int32_t RP_A_Request_Encoder(
	processInfo_t *pstProcQInfo,
    uint8_t *pu8ApduBuff,
    int32_t i32APDULen);

/**
*                                                                    
* DESCRIPTION                                                                          
* This function will decode the complex-ack for RP-A service request.
*    
* @param pu8ServiceRequest  [in] Buffer containing response data.
* @param i32ServiceLen      [in] Length of response data.
* @param pstProcQInfo      [out] Pointer to save decoded data.
*
* @return [out]	success or error.
*                                                                      
*/
BACNET_RETURN_TYPE RP_A_Ack_Decode_Handler(
    processInfo_t *pstProcQInfo,
    uint8_t *pu8ServiceResponse,
    int32_t i32ServiceLen);

/** 
*
* DESCRIPTION
* Decode the Read-Property complex ack data.
* The property value data is saved in 'pu8PropValueBuffer' buffer 
* to be decoded later.
*
* @param pu8APDUReq  [in]  Ack data to be decoded.
* @param i32APDULen  [in]  Length of data to be decoded.
* @param pstRpData  [out]  Pointer to save decoded data.
*
* @return [out]	Number of decoded bytes. -ve value on decode failure.
*
*/
int32_t RP_A_Decode_Ack_Data(
    uint8_t *pu8APDUReq,
    int32_t i32APDULen,
    BACNET_CONF_DATA *pstRpData);

#ifdef DISPLAY_PARAMETERS
/**
*                                                                         
* DESCRIPTION                                                                          
* This function will print the validated values in stack for RP-A service.
*    
* @param pstRpReq [in] RP Service parameter to be display.
* @return nothing.
*
*/
void RP_A_Display_Data(rp_request_t *pstRpReq);
#endif

#endif /* */
#endif /* READPROPERTY_A_H */