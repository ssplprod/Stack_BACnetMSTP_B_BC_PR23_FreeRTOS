/**********************************************************************************************
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
*	serviceNwLayerMsg_B.h
*
*	AUTHORS
*	Pratham N. Murkute
*
*   DESCRIPTION
*	This file contains function handlers for all B-side network layer 
*	protocol services.
*
*   RELEASE HISTORY
*   DATE            NAME				DESCRIPTION
*	18-Feb-2014		Pratham N. M.		File creation.
*	18-Feb-2014		Pratham N. M.		Added WHO-IS-Router-to-Network request decoder.
*
*
***********************************************************************************************/

#ifndef NW_LYR_MSG_B
#define NW_LYR_MSG_B
#ifdef NETWORK_LAYER_MESSAGE_B

/** include header files */
#include "bacnetStackMgmt.h"


/** 
*
* DESCRIPTION
* Function decodes the WHO-IS-Router-to-Network request.
*
* @param  pstProcQNode	[in] Pointer to save decoded data.
* @param  pu8Npdu		[in] Request data buffer.
* @param  i16NpduLen	[in] Length of request.
*
* @return [out] BACNET_RETURN_TYPE.
*                                                                      
*/
BACNET_RETURN_TYPE WHO_IS_Router_To_Nw_Decode_Handler(
	processInfo_t *pstProcQNode, 
	uint8_t *pu8Npdu, 
	int32_t i32NpduLen);

/**
*
* DESCRIPTION
* Handles messages at the decode NPDU level of the BACnet stack.
* This API decodes the messages that are coming from BVLC layer
* handler and construct APDU packets for processing by APDU layes handler.
*
* @param pu8PduData			[in] Received PDU containing NPDU and APDU data
* @param u16PduLen			[in] No. of bytes recived
* @param pstNpduProcInfo	[in/out] Pointer to process queue node to save data
* @param pstNpduData		[in] Network layer data.
* @param u16ApduOffset		[in] offset
*
* @return BACNET_RETURN_TYPE [out]  Success or error
*
*/
BACNET_RETURN_TYPE NPDU_NwLayer_Msg_Decode_Handler(
	processInfo_t *pstNpduProcInfo,
    uint8_t *pu8PduData,
    uint16_t u16PduLen,
	Bacnet_Npdu_Data_t *pstNpduData,
	int16_t u16ApduOffset);

/**
*
* DECRIPTION
* Function sends Network-No-Is.
*
* @param pstDestAddr	 [in] Destination device address
* @param pvVirtualDev	 [in] Virtual device data
* @param bIsNwLayerMsg   [in] flag to send i-am-router-to-network
* @param eDestType		 [in] Destination type
*
* @return void			[out] No return value
*
*/
void Send_NW_NO(BACnetAddress_t *pstDestAddr,
	void *pvVirtualDev,
	bool bIsNwLayerMsg,
	DESTINATION_TYPE eDestType);

#endif /* NETWORK_LAYER_MESSAGE_B */
#endif /* NW_LYR_MSG_B */
