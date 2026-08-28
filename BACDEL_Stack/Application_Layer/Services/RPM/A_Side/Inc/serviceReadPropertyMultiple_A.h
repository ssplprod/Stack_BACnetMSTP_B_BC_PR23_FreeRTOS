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
*	serviceReadPropertyMultiple_A.h
*                                                            
*   AUTHORS                                                                     
*   Harshal Mangale
*                                                                         
*   DESCRIPTION 
*	The ReadPropertyMultiple service is used by a client BACnet-user to request
*	the values of one or more specified properties of one or more BACnet Objects.
*	This service allows read access to any property of any object, whether a 
*	BACnet-defined object or not.
*   This file includes all functions related to RPM-A service.   
*                                                                         
*********************************************************************************/

#ifndef RPM_A_H
#define RPM_A_H
#ifdef BACDEL_SER_DS_RPM_A

/* header files */
#include "objDevice.h"

/**
*                                                                         
* DESCRIPTION                                                                          
* This function will encode RPM-A service request.
*    
* @param pstProcQInfo    [in] Service parameter to be encoded.
* @param pu8ApduBuff [in/out] Buffer to store Encoded APDU data.
* @param i32APDULen  [in/out] Encoded APDU data length.
* 
* @return eReturnType	[out] Encoded data length.
*
*/
int32_t RPM_A_Request_Encoder(                    
    processInfo_t *pstProcQInfo,
    uint8_t **pu8ApduBuff,
    int32_t i32APDULen);

/**
*                                                                    
* DESCRIPTION                                                                          
* This function will decode the complex-ack for RPM-A service response. 
*    
* @param pu8ServiceRequest   [in] Buffer containing response data.
* @param i32ServiceLen       [in] Length of response data.
* @param pstProcQInfo       [out] Pointer to save decoded data.
*
* @return [out]	success or error.
*                                                                      
*/
BACNET_RETURN_TYPE RPM_A_Ack_Decode_Handler(
    processInfo_t *pstProcQInfo,
    uint8_t *pu8ServiceRequest,
    int32_t i32ServiceLen);

/** 
*
* DESCRIPTION:
* Decode the object property portion of the RPM service response.
*
* @param pu8APDUReq  [in]    Received RPM-B request
* @param u32APDULen  [in]    Length of received RPM-b request
* @param pstRpmData  [out]   Structure to store parameters of RPM request
*
* @return i32Len     [out]   Total decoded length of received request
* BACNET_STATUS_REJECT If tags are wrong or missing / incomplete
*
*/
int32_t RPM_A_Ack_Decode_Object_Property(
   uint8_t *pu8APDUReq,
   uint32_t u32APDULen,
   Bacnet_Multiple_Data_t *pstRpmData);

/** 
*
* DESCRIPTION
* Decode the object property error code.
*
* @param pu8APDUReq  [in]    Received RPM-B request
* @param u32APDULen  [in]    Length of received RPM-b request
* @param pstRpmData  [out]   Structure to store parameters of RPM request
*
* @return i32Len     [out]   Total decoded length of received request
* BACNET_STATUS_REJECT If tags are wrong or missing / incomplete
*
*/
int32_t RPM_A_Ack_Decode_Property_Error(
    uint8_t *pu8APDUReq,
    uint32_t u32APDULen,
    Bacnet_Multiple_Data_t *pstRpmData);

#ifdef DISPLAY_PARAMETERS
/**
*                                                                         
* DESCRIPTION                                                                          
* This function will print the validated values in stack for RPM-A service.
*    
* @param pstRpmReq [in] RPM Service parameter to be display.
* @return nothing.
*
*/
void RPM_A_Display_Data(rpm_request_t *pstRpmReq);
#endif

#endif
#endif /* RPM_A_H */