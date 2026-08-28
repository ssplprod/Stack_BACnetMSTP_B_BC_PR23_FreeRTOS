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
*   SoftDEL Systems Ltd.                     		india@softdel.com         
*   3rd Floor, Pentagon P4,							http://www.softdel.com  
*	Magarpatta City, Hadapsar
*	Pune - 411 028   							 
*                                                                         
*                                                                         
*   FILE
*	BACnet_DeviceCommunicationControl.h
*                                                                      
*   AUTHORS                                                                             
*	Shruti K.
*                                                                         
*   DESCRIPTION                                                            
*	Defines the fucntions used to implement the device communication control
*   service. 
*
*********************************************************************************/

#ifndef DCC_B_H
#define DCC_B_H
#ifdef BACDEL_SER_DM_DCC_B

/* header files */
#include "objDevice.h"

#define DCC_MIN_PACKET_LEN 2
#define DCC_TAG_ERROR -1
#define DCC_NULL_VALUE 0
#define DCC_TIME_PERIOD_TAG 0
#define DCC_STATUS_TAG 1
#define DCC_PASSWORD_TAG 2


/**
*                                                                    
* DESCRIPTION                                                                          
* This API will decode the DCC service request and depending upon the
* result either generates a error code or SimpleAck.
*    
* @param pu8ServiceRequest   [in]    The contents of service request.
* @param i32ServiceLen       [in]    Length of service request.
* @param pstProcQInfo        [out]   This will contain pointer to data of type
*                                    bacnetRequestData_t.
* @return BACNET_RETURN_TYPE [out] success or suitable error code.
*                                                                      
*/
BACNET_RETURN_TYPE DCC_B_Decode_Handler(
	processInfo_t *pstProcQInfo,
    uint8_t *pu8ServiceRequest,
	int32_t i32ServiceLen);

/**
*
* DESCRIPTION
* Parse the request received & based on the status return Error packet
* or a simple acknowledge.
*
* @param pstVirtualDev [in] Pointer to Virtual Device struct.
* @param pstProcQInfo  [in] process Q data 
* @param u8ThreadNo    [in] thread number
*
* @return TRUE/FALSE 
*
*/
BOOL DCC_B_Request_Parser(
	virtualDevData_t *pstVirtualDev, 
	processInfo_t * pstProcQInfo,
	uint8_t u8ThreadNo);

/**
*
* DESCRIPTION
* Provides the current device communication status of the requested device.
*
* @param BACnetAddress_t				[in] Bacnet address.
* @return BACNET_COMMUNICATION_STATE    [out] Value indicates if the current state is
*											  ENABLE/DISABLE/DISABLE_INITIATION.
*
*/
BACNET_COMMUNICATION_STATE DCC_B_Get_Status(
	BACnetAddress_t *pstBACnetAddress);

/**
*
* Description
* Api to update the device communication state & then call the callback 
* function to notify application about changed state 
*
* @param pstInDCCReq	[in] dcc request data
* @param pstRmtDevAddr  [in] remote device address
* @param pstVirtualDev  [in] virtual device pointer
* @param pstProcQInfo   [in] process Q data
*
* @returns void
*
*/
void DCC_B_Update_State_And_Inform_Application(
	virtualDevData_t *pstVirtualDev, 
	dcc_request_t *pstInDCCReq,
	BACnetAddress_t *pstRmtDevAddr);
	
/**
*
* DESCRIPTION
* This function will validate the received password against the
* stored password.
*
* @param m_stPassword [in] Received Password.
* @return TRUE / FALSE
*
*/
BOOL DCC_B_Validate_Password(
	virtualDevData_t *pstVirtualDev,
	BACnetCharStr_t *pstPassword);	

#endif /* BACDEL_SER_DM_DCC_B */
#endif /* DCC_B_H */