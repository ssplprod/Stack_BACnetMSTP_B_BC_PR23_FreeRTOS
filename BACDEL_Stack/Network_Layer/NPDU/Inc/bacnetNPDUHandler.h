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
*	bacnetNPDUHandler.h
*                                                                      
*   AUTHORS                                                                     
*	Prashant Badgujar, Ashish Verma
*                                                                         
*   DESCRIPTION                                                            
*	Functions for Encoding and Decoding of NPDU layer header.
*
**********************************************************************************/

#ifndef NPDU_H
#define NPDU_H

/** header files */
#include "bacDELDef.h"
#include "pduServiceStructure.h"
#include "bacnetStackMgmt.h"

/** hop count default value */
#define DFLT_HOP_COUNT 255

/* default priority with which a BACnet packet is sent on TCP/IP */
#ifndef DEFAULT_NPDU_PRIORITY
#define DEFAULT_NPDU_PRIORITY MESSAGE_PRIORITY_NORMAL
#endif

/** compile as C code */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
*
* DESCRIPTION 
* Handles messages at the decode NPDU level of the BACnet stack. 
* This API decodes the messages that are coming from BVLC layer
* handler and construct APDU packets for processing by APDU layes handler.
*                                                  
* @param pu8PduData [in] Received PDU containing NPDU and APDU data
* @param u16PduLen  [in] No. of bytes recived
* @param pstNpduProcInfo [in/out] Pointer to process queue node to save data
*
* @return  [out]  BACNET_RETURN_TYPE enumerations
* This function is called from process (Px) thread:
* - BACDEL_SUCCESS will trigger Tx thread to send response
* - BACDEL_CONTINUE will do nothing
* - BACDEL_ERROR or any other value will free Px-Q node
*
*/
BACNET_RETURN_TYPE NPDU_Decode_Handler(
    processInfo_t *pstNpduProcInfo,
    uint8_t *pu8PduData,
    uint16_t u16PduLen);

/**
*
* DESCRIPTION                                                                          
* Handles messages at the encode NPDU level of the BACnet stack. 
* This function encodes the messages that are coming from APDU 
* layer handler and pass NPDU packets to BVLC layer handler.
*
* @param pstNpduProcInfo [in] Service APDU-NPDU data
* @param pu8ApduData	 [in] Encoded APDU data
* @param u16ApduLen		 [in] Encoded APDU data length
*
* @return  [out]  BACNET_RETURN_TYPE enumerations
* BACDEL_SUCCESS on success else any other error code
*
*/
BACNET_RETURN_TYPE NPDU_Encode_Handler(
	processInfo_t *pstNpduProcInfo,
	uint8_t *pu8ApduData,
	uint16_t u16ApduLen);

/**
*
* DESCRIPTION 
* Initialize NPDU data to default values.
* Fill input data into NPDU structure.
*
* @param ePriority	 [in] Network layer priority value
* @param u8MsgType   [in] Message-Type in case this is nw layer message
* @param pstNpduData [in] Pointer to NPDU data
* @param bDataExpectReply	[in] True if message should have a reply
* @param bIsNetworkLayerMsg [in] Specifies if this is network layer msg or not
*
* @return [out] void
*
*/
void NPDU_Set_Data(
   Bacnet_Npdu_Data_t *pstNpduData,
   bool bDataExpectReply,
   BACNET_MESSAGE_PRIORITY ePriority,
   bool bIsNetworkLayerMsg,
   uint8_t u8MsgType);
   
/**
*
* DESCRIPTION
* Decode the NPDU portion of a received message, particularly the NCPI byte.
* The Network Layer Protocol Control Information byte is described
* in section 6.2.2 of the BACnet standard.
*
* @param pstDestAddress [in] To save destination information
* @param pstSrcAddress  [in] To save source information
* @param pstNpduData    [in] To save decoded NPDU data
* @param pu8PduData     [in] Received data

* @return i32Len [out] Number of bytes decoded, 0 or -ve if error.
*
*/
int32_t NPDU_Decode_Pdu(
	uint8_t *pu8PduData,
	BACnetAddress_t *pstDestAddress,
	BACnetAddress_t *pstSrcAddress,
	Bacnet_Npdu_Data_t *pstNpduData);

/**
*
* DESCRIPTION
* Encode the NPDU portion of a message to be sent, based on the NPDU data
* and associated data.
* If this is to be a Network Layer Control Message, there are probably
* more bytes which will need to be encoded following the ones encoded here.
* The Network Layer Protocol Control Information byte is described
* in section 6.2.2 of the BACnet standard.
*
* @param pstDestAddress [in] Destination information
* @param pstSrcAddress  [in] Source information
* @param pstNpduData    [in] NPDU data to be encoded
* @param pu8NpduBuffer [out] Buffer to save encoded data

* @return i32Len [out] Number of bytes encoded, 0 or -ve if error.
*
*/
int32_t NPDU_Encode_Pdu(
	uint8_t *pu8NpduBuffer,
	BACnetAddress_t *pstDestAddress,
	BACnetAddress_t *pstSrcAddress,
	Bacnet_Npdu_Data_t *pstNpduData);

/**
*
* DESCRIPTION
* Copy the NPDU data information from source to destination.
*
* @param pstSrcAddress   [in] Source data structure
* @param pDestAddress [out] Destination data structure
*
* @return [out] void
*
*/
void NPDU_Copy_Data(
	Bacnet_Npdu_Data_t *pDestNpduData,
	Bacnet_Npdu_Data_t *pSrcNpduData);

   

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* NPDU_H */
