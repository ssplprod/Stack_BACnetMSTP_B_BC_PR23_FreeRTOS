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
*   SoftDEL Systems Ltd.                                india@softdel.com      
*   3rd Floor, Pentagon P4,                             http://www.softdel.com  
*   Magarpatta City, Hadapsar
*   Pune - 411 028                                
*                                                                         
*
*   FILE
*   serviceReinitializeDevice_B.h
*                                                                      
*   AUTHORS                                                                     
*   Pratham N. Murkute
*                                                                         
*   DESCRIPTION                                                            
*	The B device performs reinitialization requests from the A device. 
*	The optional password field shall be supported.
*	Devices claiming conformance to DM-RD-B are only required to support 
*	the WARMSTART and COLDSTART service choices.
*                                                                         
*    
*********************************************************************************/

#ifndef REINIT_DEVICE_B_H
#define REINIT_DEVICE_B_H 
#ifdef BACDEL_SER_DM_RD_B

/* include header files */
#include "objDevice.h"

#define MINIMUM_DATA_FOR_REINIT_DEVICE_SERVICE 2

/**
*                                                                    
* DESCRIPTION                                                                          
* This function is default handler when a Reinitialize Device request is
* received.
*    
* @param pu8ServiceRequest	[in]    Data received for decoding.
* @param i32ServiceLen		[in]    Length of service request.
* @param pstProcQInfo		[out]	Pointer to the instance of processQ & will 
*									contain the decoded parameters.
*                                   
* @return	-	BACNET_RETURN_TYPE.
*	
**/
BACNET_RETURN_TYPE RD_B_Decode_Handler(
	processInfo_t *pstProcQInfo,
	uint8_t *pu8ServiceRequest,
	int32_t i32ServiceLen);

/**
*                                                                    
* DESCRIPTION                                                                          
* This function is to validate received reinitialize device request & do 
* necessary actions after successful validation.
*    
* @param pVirtualDev	[in]    virtual device data.
* @param pstProcQInfo	[in]	Pointer to the instance of processQ that 
*								contains the decoded parameters.
* @param u8ThreadNo     [in]    thread number.
*                                   
* @return	-	returns true or false.
*	
**/
bool RD_B_Request_Parser(
	virtualDevData_t *pVirtualDev,
	processInfo_t *pstProcQInfo,
	uint8_t u8ThreadNo);

/**
*                                                                    
* DESCRIPTION                                                                          
* This function re initializes the device.
* - the code in this function can be implemented as per
* client requirement.  
*    
* @param pVirtualDev [in] virtual device data.
* @param eRestartReason [in] retsart reason.
*
* @returns void.
*	
**/
void ReInitializeDevice(virtualDevData_t *pVirtualDev, BACNET_RESTART_REASON eRestartReason);

#endif /* BACDEL_SER_DM_RD_B */
#endif /* REINIT_DEVICE_B_H */
