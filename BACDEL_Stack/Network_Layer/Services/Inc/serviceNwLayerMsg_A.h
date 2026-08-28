/*************************************************************************************
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
*
*   FILE NAME
*	serviceNwLayerMsg_A.h
*
*	AUTHORS
*	Pratham N. Murkute
*
*   DESCRIPTION
*	This file contains function handlers for all A-side network layer 
*	protocol services.
*
*   RELEASE HISTORY
*   DATE            NAME				DESCRIPTION
*	18-Feb-2014		Pratham N. M.		File creation.
*	18-Feb-2014		Pratham N. M.		I-AM-Router-To-Nw decoder.
*
*
*************************************************************************************/

#ifndef NW_LYR_MSG_A
#define NW_LYR_MSG_A
#ifdef NETWORK_LAYER_MESSAGE_A

/** include header files */
#include "bacnetStackMgmt.h"
#include "serviceGenericHandler.h"


/** Global linklist to save I-Am-Router-To-Network data */
extern IamRouterToNwBindData_t *g_pstIamRouterToNw;


/**
*
* DESCRIPTION
* Function decodes the I-AM-Router-To-Nw responses.
*
* @param  pstProcQNode	[in] Pointer to save decoded data.
* @param  pu8Npdu		[in] Request data buffer.
* @param  i16NpduLen	[in] Length of request.
*
* @return [out] BACNET_RETURN_TYPE.
*                                                                      
*/
BACNET_RETURN_TYPE I_AM_Router_To_Nw_Decode_Handler(
	processInfo_t *pstProcQNode, 
	uint8_t *pu8Npdu, 
	int32_t i32NpduLen);


#endif /* NETWORK_LAYER_MESSAGE_A */
#endif /* NW_LYR_MSG_A */
