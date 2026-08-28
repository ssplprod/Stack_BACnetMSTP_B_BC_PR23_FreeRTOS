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
*   SoftDEL Systems Ltd.                           india@softdel.com      
*   3rd Floor, Pentagon P4,                        http://www.softdel.com  
*   Magarpatta City, Hadapsar
*   Pune - 411 028                                
*                                                                         
*
*   FILE
*   serviceTimeSynchronization_B.h
*                                                                      
*   AUTHORS                                                                     
*   Pratham M., Sarvesh J.
*                                                                         
*   DESCRIPTION                                                            
*   The B device interprets time synchronization messages from the A device &
*	shall update its local representation of time. 
*	This change shall be reflected in the Local_Time and Local_Date properties 
*	of the Device object.
*                                                                         
*********************************************************************************/

#ifndef SER_TS_B_H
#define SER_TS_B_H 
#ifdef BACDEL_SER_DM_TS_B

/* include header files */
#include "serviceCommonTimeSync.h"

/**
*                                                                    
* DESCRIPTION                                                                          
* This function updates the local_date & local_time properties
* of the device object.
*    
* @param pVirtualDev	[in]    virtual device data.
* @param pstProcQInfo	[in]	Pointer to the instance of processQ that 
*								contains the decoded parameters.
* @param u8ThreadNo     [in]    thread number
*                                   
* @return [out]	false as this is unconfirmed service.
*	
*/
bool TimeSync_B_Request_Parser(
	virtualDevData_t *pVirtualDev,
	processInfo_t *pstProcQInfo,
	uint8_t u8ThreadNo);

				 
#endif /* BACDEL_SER_DM_TS_B */
#endif /* SER_TS_B_H */
