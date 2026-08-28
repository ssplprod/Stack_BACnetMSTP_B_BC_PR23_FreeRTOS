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
*   SoftDEL Systems Ltd.                        india@softdel.com      
*   3rd Floor, Pentagon P4,                     http://www.softdel.com  
*   Magarpatta City, Hadapsar
*   Pune - 411 028                                
*                                                                         
*   FILE
*   serviceGetEventInformation_B.h
*                                                                      
*   AUTHORS                                                                     
*   Heramb Joshi
*                                                                         
*   DESCRIPTION                                                            
*   The GetEventInformation service is used by a client BACnet-user to obtain 
*	a summary of all "active event states".
*	(a) have an Event_State property whose value is not equal to NORMAL, or
*	(b) have an Acked_Transitions property, which has at least one of the 
*	bits (TO-OFFNORMAL, TO-FAULT, TONORMAL) set to FALSE.
*	This file includes all function related to GEI-B service.
*
********************************************************************************/

#ifndef GEI_B_H
#define GEI_B_H
#ifdef BACDEL_SER_AE_GEI_B

/* header files */
#include "objDevice.h"

/* macros */
#define GEI_TAG_OBJID 0
#define OBJ_NOT_ACTIVE_STATE 1
#define OBJ_ACTIVE_STATE 2
#define MAX_SIZE_GEI_ACK 70
#define MORE_EVENTS_MAX_APDU 1

/**
*                                                                    
* DESCRIPTION
* This function is default handler when a GEI request is
* received from network.
*    
* @param pu8ServiceRequest	[in]    Data received for decoding.
* @param i32APDULen			[in]    Length of service request.
* @param pstProcQInfo		[out]	Pointer to the instance of processQ & will 
*									contain the decoded parameters.
*
* @return BACNET_RETURN_TYPE [out]	 success or suitable error code.
*	
*/
BACNET_RETURN_TYPE GEI_B_Decode_Handler(
	processInfo_t *pstProcQInfo,
	uint8_t *pu8ServiceRequest, 
	int32_t i32APDULen);

/**
*                                                                    
* DESCRIPTION                                                                          
* This function will execute GEI-B req and fill all Active-Events data.
*    
* @param pVirtualDev	[in]  virtual device data.
* @param pstProcQInfo	[in]  Pointer to the service data.
*                                   
* @return true or false.
*	
*/
bool GEI_B_Request_Parser(
	virtualDevData_t *pVirtualDev,
	processInfo_t *pProcQIndex);

/**
*                                                                         
* DESCRIPTION                                                                          
* This function will fill active events data 
*      
* @param eObjectType		[in]  Object type
* @param u32InObjID			[in]  Object ID
* @param pVirtualDev		[in]  Pointer to Virtual Device.
* @param pstGetEventInfoACK [in]  Pointer to 1st active event data
*
* @return [out] pointer to 1st node in list.
*
*/
void GEI_B_Fill_Event_Information_Data(
	BACNET_OBJECT_TYPE eObjectType,
	Pr_BACnetObjId_t *pstInObjID, 
	virtualDevData_t *pVirtualDev, 
	event_info_response_t *pstGetEventInfoACK,
	event_summary_t **pstBaseNode,
	event_summary_t **pstPrevNode,
	void *pvObject);


/**
*
* DESCRIPTION
* Function to encode the GEI acknowledgement. 
*
* @param pu8APDUResp	[in]    APDU buffer to save encoded data.
* @param i32APDULen		[in]    Length of encoded data in buffer.
* @param pstProcQInfo	[in]	Pointer to service data.
*
* @return i32APDULen	[out]	total length after encoding.
*
*/
int32_t GEI_B_Ack_Encoder(
	processInfo_t *pstProcQInfo,
	uint8_t **pu8APDUResp,
	int32_t i32APDULen);

 #endif/* BACDEL_SER_AE_GEI_B */
#endif /* GEI_B_H */