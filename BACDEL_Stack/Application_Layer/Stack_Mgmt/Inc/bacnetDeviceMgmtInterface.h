/**********************************************************************************
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
*   SoftDEL Systems Ltd.						india@softdel.com         
*   3rd Floor, Pentagon P4,						http://www.softdel.com  
*   Magarpatta City, Hadapsar
*   Pune - 411 028                                
*                                                                         
*   FILE
*	bacnetDeviceMgmtInterface.h
*                                                                      
*   AUTHORS                                                                     
*	Surendra Oturkar, Harshal Mangale, M.Venu, Ashish Verma
*                                                                         
*   DESCRIPTION                                                            
*	Functions for passing request data to individual devices created within stack.
*	Pure IP device actsas the virtual router to send the request data to virtual 
*	devices.
*	In case a broadcast message is received, the data is passed to all devices 
*	within the stack.
*
**********************************************************************************/

#ifndef DEV_MGMT_H
#define DEV_MGMT_H

/** header files */
#include "bacnetStackMgmt.h"

/**
*
* DESCRIPTION
* Process Thread uses this Interface. 
* Process thread will call this interface to pass received request data 
* to respective device thread.
*                                                                             
* @param pstReqProcInfo [in] Received request data.
* @return [out] BACDEL_SUCCESS on success else error
*
*/
BACNET_RETURN_TYPE Obj_MgmtLayer_Interface(processInfo_t *pstReqProcInfo);

/**
*
* DESCRIPTION
* This function is called by NPDU handler routine to check if
* the requested DADR is valid or not.
*                                                                             
* @param pstReqProcInfo [in] Px queue node containing DADR
* @param bDADRPresent   [in] Flag specifies if DADR is present or not
*
* @return 
* true - if destination DADR is presnet in virtual device table.
* false - if destination DADR is absent.
*
*/
bool Device_Look_Up(processInfo_t *pstReqProcInfo, bool bDADRPresent);

/**
*
* DESCRIPTION
* This function is called to intialize the Devicelook up table with 
* deafult value (-1). 
*
* @param [in] void
* @return [out] void
*
*/
void Device_Look_UpIntialization(void);

/**
*
* DESCRIPTION
* Function to fill request data and trigger respective device process thread.
* If device process queue has empty location, address of received process Q node
* is saved in device process queue.
* If VD are supported in stack & requestis broadcast, address of newly allocated
* memory for process Q node is saved in device process queue.
*
* @param bIsBroadcastReq [in] Specifies if unicast or broadcast request
* @param pvVirtualDev	 [in] Virtual device data
* @param pstReqProcInfo  [in] Pointer to Px Q node
*
* @return [out] BACNET_RETURN_TYPE enumeration
* - BACDEL_SUCCESS means send the response immediately
* - BACDEL_ERROR means discard the packet
* - BACDEL_CONTINUE means data is sent to device process thread
* - BACDEL_OTHER means no empty space in queue
*
*/
BACNET_RETURN_TYPE Fill_Device_Queue(
	bool bIsBroadcastReq,
	void *pvVirtualDev,
	processInfo_t *pstReqProcInfo);

#endif /* DEV_MGMT_H */
