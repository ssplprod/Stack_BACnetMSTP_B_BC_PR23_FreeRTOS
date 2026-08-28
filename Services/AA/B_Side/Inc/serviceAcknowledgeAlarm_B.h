/******************************************************************************
*
*            Copyright (c) by SoftDEL Systems Pvt. Ltd.               
*
*  This software is copyrighted by and is the sole property of 
*  SoftDEL Systems Pvt. Ltd.
*  All rights, title, ownership, or other interests         
*  in the software remain the property of  SoftDEL Systems Pvt. Ltd.
*  This software may only be used in accordance with the corresponding        
*  license agreement.  Any unauthorized use, duplication, transmission,  
*  distribution, or disclosure of this software is expressly forbidden.  
*
*  This Copyright notice may not be removed or modified without prior    
*  Written consent of SoftDEL Systems Pvt. Ltd.
*
*  SoftDEL Systems Pvt. Ltd. reserves the right to modify this software
*  Without notice.
*
*  SoftDEL Systems Pvt. Ltd.                      info@softdel.com         
*  3rd Floor, Pentagon P4,                        http://www.softdel.com   
*  Magarpatta City, Hadapsar                                                
*  Pune - 411 028
*
*  FILE
*  serviceAcknowledgeAlarm_B.h
*
*  AUTHORS                                                                     
*  Pratham N. Murkute
*
*  DESCRIPTION
*  B-side service processes acknowledgments of previously transmitted  
*  alarm/event notifications. 
* 
*********************************************************************************/

#ifndef SER_AA_B_H
#define SER_AA_B_H
#if (defined BACDEL_SER_AE_AA_B && defined BACDEL_OBJ_NC)

/** include header files */
#include "objDevice.h"
#include "serviceEventNotification_B.h"

/** macros */
#define MINIMUM_SERVICE_DATA_LENGTH_AA 20

/**
*                                                                    
* DESCRIPTION                                                                          
* This function is default handler when a Acknowledge Alarm request is
* received.
*    
* @param pu8ServiceRequest		[in]    Data received for decoding.
* @param i32ServiceLen			[in]    Length of service request.
* @param pstProcQInfo			[out]	Pointer to the instance of processQ & will 
*										contain the decoded parameters.
*                                   
* @return BACNET_RETURN_TYPE	[out]	success or suitable error code.
*	
*/
BACNET_RETURN_TYPE AckAlarm_B_Decode_Handler(
	processInfo_t *pstProcQInfo,
	uint8_t *pu8ServiceRequest,
	int32_t i32ServiceLen);

/**
*                                                                    
* DESCRIPTION                                                                          
* This function is to validate received acknowledge alarm request & do 
* necessary actions after successful validation.
*    
* @param pVirtualDev	[in]    virtual device data.
* @param pstProcQInfo	[in]	Pointer to service data.
* @param u8ThreadNo     [in]    thread number
*                                   
* @return				[out]	true or false.
*	
*/
bool AckAlarm_B_Request_Parser(
	virtualDevData_t *pVirtualDev, 
	processInfo_t *pstProcQInfo, 
	uint8_t u8ThreadNo);

/**
*                                                                    
* DESCRIPTION                                                                          
* Api to clear saved notification when it is either acknowledged or the 
* device to which this notification belongs is deleted.
*    
* @param u32DevID			[in] device id.
* @param bIsDeleteDevice	[in] True if api is called from delete device api. 
* @param pvInstance			[in] pointer to be deleted from the list.
*                                   
* @returns Void.
*	
*/
void AA_B_Clear_Saved_Event_Notification(
	uint32_t u32DevID, 
	bool bIsDeleteDevice,
	void *pvInstance);

#endif /* BACDEL_SER_AE_AA_B */
#endif /* SER_AA_B_H */
