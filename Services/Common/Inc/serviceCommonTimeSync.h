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
*   Magarpatta City, Hadapsar
*   Pune - 411 028                                
*                                                                         
*
*   FILE
*   serviceCommonTimeSync.c
*                                                                      
*   AUTHORS                                                                     
*   Pratham M.
*                                                                         
*   DESCRIPTION                                                            
*   The B device interprets time synchronization messages from the A device &
*	shall update its local representation of time. 
*	This change shall be reflected in the Local_Time and Local_Date properties 
*	of the Device object.
*                                                                         
*    
*********************************************************************************/

#ifndef TIME_SYNC_H
#define TIME_SYNC_H

/* include header files */
#include "objDevice.h"


#if (defined BACDEL_SER_DM_TS_B || defined BACDEL_SER_DM_UTC_B)

#define MINIMUM_SERVICE_DATA_LENGTH_TS 10

/**
*                                                                    
* DESCRIPTION                                                                          
* This function is default decode handler when a Time Synchronization or 
* UTC Time Synchronization request is received.
*    
* @param pu8ServiceRequest	[in]    Data received for decoding.
* @param i32APDULen			[in]    Length of service request.
* @param pstProcQInfo		[out]	Pointer to the instance of processQ & will 
*									contain the decoded parameters.
*                                   
* @return	-	BACNET_RETURN_TYPE.
*	
*/
BACNET_RETURN_TYPE TimeSync_B_Decode_Handler(
	processInfo_t *pstProcQInfo,
	uint8_t *pu8ServiceRequest,
	int32_t i32ServiceLen);

#endif
#endif /* TIME_SYNC_H */
