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
*	datalinkMSTPHandler.h
*                                                                      
*   AUTHORS                                                                     
*	Anagha Chitale, Sarvesh Junnarkar
*                                                                         
*   DESCRIPTION                                                            
*	Functions for MSTP layer of stack.
*	Functions related to encoding and decoding of MSTP layer.
*	Functions related to header and data CRC calculations.
*
**********************************************************************************/

#ifndef MSTP_HANDLER_H
#define MSTP_HANDLER_H

/** header file includes */
#include "../../../Api/Inc/bacDELStackConfig.h"
#include "../../../Application_Layer/Stack_Mgmt/Inc/bacnetStackMgmt.h"
#include "../../../Api/Inc/bacDELApi.h"

/** compile as C code */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** mstp buffer size */
#define MSTP_TRUNCATED_PACKET	0
#define MSTP_INVALID_PACKET		65535
	
/* application callback function pointer for transmitting data 
on mstp serial bus */
extern App_MstpTx_Interface_t afMstpTxFunctionCb;

/**
*
* DESCRIPTION                                                                          
* Calculates CRC of MSTP header field.
*
* @param bIsTxData	[in] Flag to identify the type of Data
*						 FALSE if Rx data, TRUE if Tx data
* @param pu8Data	[in] Encoded APDU+NPDU data
*
* @return [out] Calculated CRC value (1 byte)
*
*/
uint8_t CalHeaderCRC(bool bIsTxData, uint8_t *pu8Data);

/**
*
* DESCRIPTION                                                                          
* Calculates CRC of MSTP Data field.
*
* @param bIsTxData		[in] Flag to identify the type of data
*							 FALSE if Rx data, TRUE if Tx data
* @param pu8Data		[in] Encoded APDU + NPDU data
* @param u16DataLength  [in] Length of data encoded
*
* @return  [out]  Calculated CRC (2 byte)
*
*/
uint16_t CalDataCRC(bool bIsTxData, uint8_t *pu8Data, uint16_t u16DataLength);

/**
*
* DESCRIPTION                                                                          
* Function to send data on serial bus.
* This function will call the registerd mstp tx callback function.
*
* @param pu8TxData		 [in] Data buffer
* @param pu16SilentTimer [in] Pointer to silence-timer counter
* @param u16DataLen      [in] Length of data to be transmitted
*
* @return [out] BACDEL_SUCCESS on success else error code 
*
*/
BACNET_RETURN_TYPE MSTP_SendDataOnSerialBus(
	uint8_t *pu8TxData,	
	uint16_t *pu16SilentTimer,
	uint16_t u16DataLen);

/**
*
* DESCRIPTION 
* Handles messages at the decode MSTP level of the BACnet stack. 
* This function decodes the messages that are coming from serial port.
* After decoding MSTP layer data, the data is further sent to NPDU-APDU layers.
*                                                  
* @param pu8DataBuffer	  [in] Received PDU containing MSTP, NPDU and APDU data
* @param u16RcvBytes      [in] No. of bytes received
* @param pstMstpProcInfo  [in/out] Pointer to process queue node to save data
*
* @return  [out]  BACNET_RETURN_TYPE enumerations
* This function is called from process (Px) thread:
* - BACDEL_SUCCESS will trigger Tx thread to send response
* - BACDEL_CONTINUE will do nothing
* - BACDEL_ERROR or any other value will free Px-Q node
*
*/
BACNET_RETURN_TYPE MSTP_Decode_Handler(
	uint8_t *pu8DataBuffer,
	uint16_t u16RcvBytes,
	processInfo_t *pstMstpProcInfo);

/**
*
* DESCRIPTION                                                                          
* Handles messages at the encode MSTP level of the BACnet stack. 
* This function encodes the messages that are coming from NPDU layer 
* and transmit data on serial port.
*
* @param pstMstpProcInfo [in] Service APDU:NPDU:MSTP data
* @param pu8DataBuffer	 [in] Encoded APDU+NPDU data
* @param u16DataBufLen	 [in] Encoded APDU+NPDU data length
*
* @return  [out]  BACNET_RETURN_TYPE enumerations
* BACDEL_SUCCESS on success else any other error code
*
*/
BACNET_RETURN_TYPE MSTP_Encode_Handler(
	processInfo_t *pstMstpProcInfo,
	uint8_t *pu8DataBuffer, 
	uint16_t u16DataBufLen);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MSTP_HANDLER_H */
