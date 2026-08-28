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
*	datalinkMSTPHandler.c
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

/** header file includes */
#include "datalinkMSTPHandler.h"
#include "datalinkMSTP.h"
#include "datalinkMSTPStateMachines.h"
#include "bacnetNPDUHandler.h"

extern osMutexId_t g_hsMstpTxMutexLockHandle;

extern UART_HandleTypeDef huart2;
extern uint8_t SendPacketBacnet(UART_HandleTypeDef *huart, uint8_t* data, uint16_t size);

/** application callback function pointer for transmitting data 
on mstp serial bus */
App_MstpTx_Interface_t afMstpTxFunctionCb;

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
uint8_t CalHeaderCRC(bool bIsTxData, uint8_t *pu8Data)
{
	/* local variables */
	uint8_t u8CrcVal = 0;
	uint16_t u16cnt = 0;
	uint16_t u16crc = 0;
	uint16_t u16size = 0;
	uint16_t u16numshifts = 0;
	uint16_t u16index = 0;
	uint8_t *pu8Testbufp = NULL;
	#ifdef DEBUG_LOGFILES
	static int framecount = 0;
	uint8_t uLastByte = 0; 
	#endif

	/* calculate CRC to check received value. 
	remember receive is a circlar que decrements from the tail */
	if(!bIsTxData)
	{
		/* */
		u16size = 6;
		pu8Testbufp = &pu8Data[0];      
		u16index = BnRxQue.i16Tail;
		u16numshifts = 2;

		/* increment past the preamble */
		if(u16index >= (BN_QUESIZE - u16numshifts))
			u16index -= (BN_QUESIZE - u16numshifts);
		else
			u16index = u16index + u16numshifts; 
	}
	/* calculate CRC for transmit message.
	remember xmit is a linear queue and	decrements from the tail */
	else 
	{
		/* */
		u16size = 5;
		pu8Testbufp = &pu8Data[0];
		u16index = 2; //skip preamble
	}

	/* initialize CRC value */
	u8CrcVal = 0xFF;

	/* remember CRC calculation does not include preamble */
	for(u16cnt = 0; u16cnt < u16size; u16cnt++)
	{
		#ifdef DEBUG_LOGFILES
		uLastByte = pu8Testbufp[u16index];
		#endif

		#ifdef DEBUG_LOGFILES
		fprintf(tempRxFP, "\nHeader_CRC2_%u_%u: ", g_iTheTime,u16size);
		#endif
		#ifdef DEBUG_LOGFILES
		//fprintf(tempRxFP, " %02X(%u   ) ", pu8testbufp[u16index], u16cnt);
		fprintf(tempRxFP, "A_%02X(%u   ) ", pu8Data[u16index], u16index);
		#endif

		/* calculate CRC */
		u16crc = u8CrcVal ^ pu8Testbufp[u16index];
		u16crc = u16crc ^ (u16crc << 1) ^ (u16crc << 2) ^ (u16crc << 3) ^ (u16crc << 4) ^ 
			(u16crc << 5) ^	(u16crc << 6) ^ (u16crc << 7);
		u8CrcVal = (u16crc & 0xfe) ^ ((u16crc >> 8) & 0x01);

		#ifdef DEBUG_LOGFILES
		fprintf(tempRxFP, "\nHeader_CRC_Val_ %u_ %u: ", u16CrcVal, bIsTxData);
		#endif


		/* receive CRC */
		if(!bIsTxData)
		{
			if(u16index < (MAX_MSTP_MPDU - 1))
				u16index++;
			else
				u16index = 0;
		}
		/* transmit CRC */
		else
		{
			u16index++;
		}
	}

	/* return the calculated CRC value */
	if(!bIsTxData)
	{
		#ifdef DEBUG_LOGFILES
		/* log CRC results */
		uint8_t testCRC = u16CrcVal - 0x55;
		if(0 != testCRC)
		{
			if(0x2 == g_stMstpVariables.ReceivedSource)
			{
				extern FILE *tempTSMFp;
				if (NULL != tempTSMFp)
				{
					fprintf(tempTSMFp, "\nGT = %u: HEADER CRC: Invalid: \
					Index = %d, Frame Cnt = %d, rcvd CRC=%x, dest = %x, frame=%x, length = %d", 
					g_iTheTime, u16index, framecount, uLastByte, g_stMstpVariables.ReceivedDestination, 
					g_stMstpVariables.ReceivedFrameType, BnRxQue.DataLength);
				}
				// BN_QUESIZE = 514
				printf("\nGT = %u: HEADER CRC: Invalid: \
				Index = %d, Frame Cnt = %d, rcvd CRC=%x, dest = %x, frame=%x, length = %d, rcvdSrc = %d", 
				g_iTheTime, u16index, framecount, uLastByte, g_stMstpVariables.ReceivedDestination, 
				g_stMstpVariables.ReceivedFrameType, BnRxQue.DataLength, g_stMstpVariables.ReceivedSource);
				framecount = 0;
			}
		}
		#endif

		/* receive CRC value */
		//u8RetVal = u8CrcVal - 0x55;
		#ifdef DEBUG_LOGFILES
		fprintf(tempRxFP, "\nHeader_CRC_Val2_ %u_ %u: ", u8RetVal, u16CrcVal);
		#endif
		return(u8CrcVal - 0x55);
	}
	else
	{
		/* transmit CRC value */
		return(0xff - u8CrcVal);
	}
}

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
uint16_t CalDataCRC(bool bIsTxData, uint8_t *pu8Data, uint16_t u16DataLength)
{
	/* local variables */
	uint16_t u16crcvalue = 0;
	uint16_t u16cnt = 0;
	uint16_t u16crclow = 0;
	uint16_t u16size = 0;
	uint16_t u16index = 0;
	uint8_t u8numshifts = 0;
	uint8_t *pu8testbufp = NULL;

	/* calculate CRC to check received value. 
	remember receive is a circlar que decrements from the tail */
	if(!bIsTxData)
	{
		/* get the index from tail */
		u16index = BnRxQue.i16Tail;
		pu8testbufp = &pu8Data[0];

		/* increment past the preamble and header frame
			to the message location where data length is  store */
		/* get size */
		u8numshifts = 6;
		if(u16index >= (MAX_MSTP_MPDU - u8numshifts))
			u16index -= (MAX_MSTP_MPDU - u8numshifts);
		else
			u16index = u16index + u8numshifts;

		/* */
		u16size = pu8testbufp[u16index] + 2;

		/* get start of data */
		u8numshifts = 2;
		/* increment past the preamble */
		if(u16index >= (MAX_MSTP_MPDU - u8numshifts))
			u16index -= (MAX_MSTP_MPDU - u8numshifts);
		else
			u16index = u16index + u8numshifts;

		#ifdef DEBUG_LOGFILES
		fprintf(tempRxFP, "\nData_CRC2_%u_%u: ", g_iTheTime,u16size);
		#endif
	}
	/* calculate CRC for transmit message.
	remember xmit is a linear queue and	decrements from the tail */
	else 
	{
		/* */
		pu8testbufp = &pu8Data[0];
		u16index = 8;
		u16size = u16DataLength - 8;
	}

	/* initialize CRC value */
	u16crcvalue = 0xFFFF;

	for(u16cnt = 0; u16cnt < u16size; u16cnt++)
	{
		#ifdef DEBUG_LOGFILES
		//fprintf(tempRxFP, " %02X(%u   ) ", pu8testbufp[u16index], u16cnt);
		fprintf(tempRxFP, "A_%02X(%u   ) ", pu8Data[u16index], u16index);
		#endif
		/* calculate CRC */
		u16crclow = (u16crcvalue & 0xFF) ^ pu8testbufp[u16index];
		u16crcvalue = (u16crcvalue >>8) ^ (u16crclow << 8) ^ (u16crclow << 3)
			^ (u16crclow << 12) ^ (u16crclow >> 4)
			^ (u16crclow & 0x0F) ^ ((u16crclow & 0x0F) << 7);

		/* receive CRC */
		if(!bIsTxData)
		{
			/* */
			if(u16index < (MAX_MSTP_MPDU - 1))
				u16index++;
			else
				u16index = 0;
		}
		/* transmit CRC */
		else 
		{
			u16index++;
		}
	}

	/* return the calculated CRC value */
	if(bIsTxData == FALSE)
	{
		/* receive CRC value */
		return(u16crcvalue - 0xF0B8);
	}
	else
	{
		/* transmit CRC value */
		return(0xffff - u16crcvalue);
	}
}

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
	uint16_t u16DataLen)
{
	/* local variables */
	volatile BACNET_RETURN_TYPE eRetVal = BACDEL_SUCCESS;

	/* check if application callback is registered to transmit data */
	/* check input pointers */
	if( NULL == pu8TxData || NULL == pu16SilentTimer)
	{
		#ifdef DEBUG_LOGFILES
		extern FILE *tempTSMFp;
		fprintf(tempTSMFp, "\nMSTP_SendDataOnSerialBus: Null pointers exiting\n");
		#endif
		/* return error */
		return BACDEL_ERROR;
	}
	
	/* wait on mutex */
//	osMutexWait(g_hsMstpTxMutexLockHandle,INFINITE);
	osMutexAcquire(g_hsMstpTxMutexLockHandle,INFINITE);
	/* enable transmitter */
	g_bTransmitEnble = TRUE;

	/* clear the silence timer */
	*pu16SilentTimer = 0;


	/* send callback to application to send data on transmit lines */
	eRetVal = SendPacketBacnet(&huart2,pu8TxData, u16DataLen);

	if(eRetVal != 1)
	{
		*pu16SilentTimer = 0;
	}

	/* clear the silence timer */

	/* disable transmitter */
	g_bTransmitEnble = FALSE;

	 osMutexRelease(g_hsMstpTxMutexLockHandle);
	/* return from function */
	return eRetVal;
}

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
	processInfo_t *pstMstpProcInfo)
{
	/* local variables */
    BACNET_RETURN_TYPE eReturnValue = BACDEL_SUCCESS;
	uint16_t u16DataBufLen = 0;
	WordSplit_u uLenUnion = {0}; 

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackMSTPLayer: \
	MSTP_Decode_Handler: Entry \r\n");
	#endif

	/* null check for input pointers */
	if(NULL == pu8DataBuffer || NULL == pstMstpProcInfo)
	{
		/* null input pointers */
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1, "BACnetStackMSTPLayer: \
		MSTP_Decode_Handler: null input pointers  \r\n");
		#endif        
        return BACDEL_ERROR;
	}

	/* check if the Recived array are holding values */
    if(u16RcvBytes <= MSTP_TRUNCATED_PACKET)
    {
        /* No bytes received in packet */
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1, "BACnetStackMSTPLayer: \
		MSTP_Decode_Handler: No bytes received \r\n");
		#endif        
        return BACDEL_ERROR;
    }

    /* check the received frame is started with preamble */
    if(pu8DataBuffer[MSTP_PREAMBLE_LOCATION] != MSTP_PREAMBLE_FIRST || 
		pu8DataBuffer[MSTP_PREAMBLE_LOCATION + 1] != MSTP_PREAMBLE_SECOND)
    {
        /* preamble is incorrect */
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1, "BACnetStackMSTPLayer: \
		MSTP_Decode_Handler: Frame premable is incorrect \r\n");
        #endif
        return BACDEL_ERROR;
    }

	/* decode frame type */
	pstMstpProcInfo->m_stProcessData.m_eFrameType = 
		(MSTP_FRAME_TYPE)pu8DataBuffer[MSTP_FRAME_TYPE_LOCATION];

	/* check for invalid frame */
	if(MAX_MSTP_FRAME_TYPE <= pu8DataBuffer[MSTP_FRAME_TYPE_LOCATION])
	{
        /* invalid mstp frame type */
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1, "BACnetStackMSTPLayer: \
		MSTP_Decode_Handler: Invalid mstp frame type \r\n");
        #endif
        return BACDEL_ERROR;
    }

	/* decode destination address */
	pstMstpProcInfo->m_stProcessData.m_stIUTAddr.u8IpAddrs[0] = 
		pu8DataBuffer[MSTP_DA_LOCATION];
	pstMstpProcInfo->m_stProcessData.m_stIUTAddr.u8mac_len = MSTP_ADDRESS_LEN;

	/* decode source address */
	pstMstpProcInfo->m_stProcessData.m_stRmDvAddr.u8IpAddrs[0] = 
		pu8DataBuffer[MSTP_SA_LOCATION];
	pstMstpProcInfo->m_stProcessData.m_stRmDvAddr.u8mac_len = MSTP_ADDRESS_LEN;

	/* decode data length */
	/* get the msb of length */
	uLenUnion.stBytes.u8HighByte =  pu8DataBuffer[MSTP_LENGTH_LOCATION + 1];
	/* get the lsb of length */
	uLenUnion.stBytes.u8LowByte =  pu8DataBuffer[MSTP_LENGTH_LOCATION];

	/* calculate length */ 
	u16DataBufLen = uLenUnion.u16Word;

    /* validate received length and decoded length are same */
    if(u16DataBufLen != (u16RcvBytes - MSTP_HEADER_SIZE))
    {
        /* inconsistent data packet received */
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1, "BACnetStackMSTPLayer: \
		MSTP_Decode_Handler: Inconsistent data packet received \r\n");
		#endif
        return BACDEL_ERROR;
    }	

	/* get actual data length */
	u16DataBufLen =  (u16RcvBytes - MSTP_HEADER_SIZE);

    /* if data is present & is either UNICAST/BROADCAST packet, decode NPDU segment */
    if((u16DataBufLen > 0) && (MSTP_INVALID_PACKET != u16DataBufLen))
    {
        /* data sent to npdu layer for further decoding */
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL2, "BACnetStackMSTPLayer: \
		MSTP_Decode_Handler: Data sent to NPDU layer for further decoding \r\n");
		#endif
        
        /* call npdu handler function to decode npdu */ 
		eReturnValue = NPDU_Decode_Handler(pstMstpProcInfo, &pu8DataBuffer[MSTP_HEADER_SIZE], u16DataBufLen);
    }
	else
	{
		/* return error, incorrect data received */
		eReturnValue = BACDEL_ERROR;
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackMSTPLayer: \
	MSTP_Decode_Handler: Exit \r\n");
	#endif
	return eReturnValue;
}

/** <@>
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
	uint16_t u16DataBufLen)
{
	/* local variables */
    BACNET_RETURN_TYPE eReturnValue = BACDEL_SUCCESS;
	uint8_t au8MstpData[MAX_MSTP_MPDU] = {0};
    BACnetAddress_t *pstDestAddress = NULL;
    MSTP_FRAME_TYPE eFrameType = MAX_MSTP_FRAME_TYPE;
	WordSplit_u uWordByteData = {0};
	uint16_t u16EncodeLen = 0;
	uint16_t u16CRCValue = 0;
	uint32_t u32FillIndex = 0;
	uint8_t u8TokenArrIndex = 0;
	MstpTxDataQue_t  *pstTxQueTofill = NULL;
	MstpTxDataQueNode_t *pstTxQNode = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackMSTPLayer: \
	MSTP_Encode_Handler: Entry \r\n");
	#endif

	/* null check for input pointers */
	if(NULL == pu8DataBuffer || NULL == pstMstpProcInfo)
	{
		/* null input pointers */
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL1, "BACnetStackMSTPLayer: \
		MSTP_Encode_Handler: null input pointers  \r\n");
		#endif
        return BACDEL_ERROR;
	}

    /* get the destiantion and source address */
    pstDestAddress = &pstMstpProcInfo->m_stProcessData.m_stRmDvAddr;

	/* set the frame types as per pdu types */
	/* default frame type */
	pstMstpProcInfo->m_stProcessData.m_eFrameType = MSTP_FRAME_TYPE_BACNET_DNER;
	/* change only for confirmed request pdu */
	if(PDU_TYPE_CONFIRMED_SERVICE_REQUEST == pstMstpProcInfo->m_stProcessData.m_ePDUType||
			   g_stMstpVariables.TokenRequiredSegmentation)
	{
		pstMstpProcInfo->m_stProcessData.m_eFrameType = MSTP_FRAME_TYPE_BACNET_DER;
	}

	/* get the frame type */
	eFrameType = pstMstpProcInfo->m_stProcessData.m_eFrameType;

	/* encode preamble */
	au8MstpData[MSTP_PREAMBLE_LOCATION] = MSTP_PREAMBLE_FIRST;
	au8MstpData[MSTP_PREAMBLE_LOCATION + 1] = MSTP_PREAMBLE_SECOND;

	/* encode frame type */
	au8MstpData[MSTP_FRAME_TYPE_LOCATION] = (uint8_t)eFrameType;

	/* encode destination address */
	au8MstpData[MSTP_DA_LOCATION] = pstDestAddress->u8IpAddrs[0];

	/* encode source address */
	au8MstpData[MSTP_SA_LOCATION] = g_stMstpVariables.ThisStation;

	/* encode data length */
	uWordByteData.u16Word = u16DataBufLen;
	/* encode msb of length */
	au8MstpData[MSTP_LENGTH_LOCATION + 1] =  uWordByteData.stBytes.u8HighByte;
	/* encode lsb of length */
	au8MstpData[MSTP_LENGTH_LOCATION] =  uWordByteData.stBytes.u8LowByte;

	/* calculate and encode header CRC */
	au8MstpData[MSTP_HEADER_CRC_LOCATION] = CalHeaderCRC(TRUE, au8MstpData);

	/* increment packet length  after header and header CRC field is encoded */
	u16EncodeLen += MSTP_HEADER_SIZE;

	/* copy the complete data in MSTP Protocol Data Unit packet */
	memcpy(&au8MstpData[u16EncodeLen], pu8DataBuffer, u16DataBufLen);

    /* add length of NPDU packet */
    u16EncodeLen += u16DataBufLen;

	/* calculate and encode data crc */
	u16CRCValue = CalDataCRC(TRUE, au8MstpData, u16EncodeLen);

	uWordByteData.u16Word = u16CRCValue;
	/* encode lsb of data crc */
	au8MstpData[MSTP_DATA_LOCATION + u16DataBufLen + 1] = uWordByteData.stBytes.u8LowByte;
	/* encode msb of data crc */
	au8MstpData[MSTP_DATA_LOCATION + u16DataBufLen] = uWordByteData.stBytes.u8HighByte;

	/* increment data and data CRC field is encoded */
	u16EncodeLen += MSTP_DATA_CRC_SIZE;

	/* check if request pdu type */
	if(PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST == pstMstpProcInfo->m_stProcessData.m_ePDUType ||
	   PDU_TYPE_CONFIRMED_SERVICE_REQUEST == pstMstpProcInfo->m_stProcessData.m_ePDUType ||
	   g_stMstpVariables.TokenRequiredSegmentation)
	{
		/* set the flag as token is required */
		pstMstpProcInfo->m_stProcessData.m_bIsTokenRequired = TRUE;
	}
	/* set the flag as token is required */

	/* check token is required or not */
	if(TRUE == pstMstpProcInfo->m_stProcessData.m_bIsTokenRequired)
	{
		/* if service is of pdu confirmed/unconfirmed request */
		if(PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST == pstMstpProcInfo->m_stProcessData.m_ePDUType ||
			PDU_TYPE_CONFIRMED_SERVICE_REQUEST == pstMstpProcInfo->m_stProcessData.m_ePDUType  ||
			g_stMstpVariables.TokenRequiredSegmentation)
		{
			/* save in request queue */
			u8TokenArrIndex = MSTP_TX_QUE_REQUEST;
			g_stMstpVariables.TokenRequiredSegmentation=FALSE;
		}
		/* else if response pdu type */
		else
		{
			/* save in response queue */
			u8TokenArrIndex = MSTP_TX_QUE_RESPONSE;
		}

		/* save pointer to fill queue */
		pstTxQueTofill = &g_astMstpTxQueue[u8TokenArrIndex];
		u32FillIndex = pstTxQueTofill->m_u32FillIndex;
		pstTxQNode = &pstTxQueTofill->m_stDataNode[u32FillIndex];

		/* check for empty queue and total count is less than max count */
		if(FALSE == pstTxQNode->m_bState && pstTxQueTofill->m_u32PacketCount < MAX_MSTP_TX_QUE_LENGTH)
		{
			/* reset previous data if any */
			memset(pstTxQNode, 0x00, sizeof(MstpTxDataQueNode_t));

			/* copy encoded data length */
			pstTxQNode->m_u16DataLength = u16EncodeLen;
			/* copy encoded data */
			memcpy(&pstTxQNode->m_au8buffer[0], &au8MstpData[0], u16EncodeLen);
			/* save frame type */
			pstTxQNode->m_eFrameType = eFrameType;
			/* save initiate queue index reference */
			pstTxQNode->m_pvInitQueueRef = pstMstpProcInfo->m_pvInitQRef;

			/* change queue index state */
			pstTxQNode->m_bState = TRUE;

			/* increment the queue packet count */
			++pstTxQueTofill->m_u32PacketCount;

			/* increment fill index, to point to next index */
			++pstTxQueTofill->m_u32FillIndex;
    		if(MAX_MSTP_TX_QUE_LENGTH <= pstTxQueTofill->m_u32FillIndex)
			{
				/* set to zero */
				pstTxQueTofill->m_u32FillIndex = 0;
			}
		}
		else
		{
			// FIXME
			/* fill error response and send back to APDU layer */
			/* return error from encoder */
			eReturnValue = BACDEL_ERROR;
		}
	}
	else
	{
		/* if this is normal response, token not required */
		/* send the response */
		eReturnValue = MSTP_SendDataOnSerialBus(&au8MstpData[0],
			&g_stMstpVariables.Tsilence, u16EncodeLen);
		if(BACDEL_SUCCESS != eReturnValue)
		{
			/* log debug message */
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL1, "BACnetStackMSTPLayer: \
			MSTP_Encode_Handler: Could not transmit data on serial bus \r\n");
			#endif
		}
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackMSTPLayer: \
	MSTP_Encode_Handler: Exit \r\n");
	#endif
	return eReturnValue;
}


/************************** end of datalinkMSTPHandler.c file ***************************/
