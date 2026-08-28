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
*   FILE
*	SegmentationGenericHandler.c
*
*   AUTHORS
*   Pratham N. Murkute
*
*   DESCRIPTION
*	This file contains all code related to segmented APDU message 
*	transmission and reception.
*
*
******************************************************************************/

#ifdef SEGMENTATION_SUPPORTED

/* includes OS specfic header file */
#include "osalFreeRTOS.h"
#include "datalinkMSTP.h"
/* includes */
#include "SegmentationGenericHandler.h"
#include "pduEncodeDecode.h"
#include "bacnetAPDUHandler.h"
#include "bacnetNPDUHandler.h"
#include "bacnetInitiateServiceMgmt.h"

#include "miscMiscellaneous.h"

/* global structure to create doubly linklist for segmentation. */
ComplexAckDataQue_t g_stComplexAckDataQue = {0};
RxSegmentsDataQue_t g_stRxSegmentsDataQue = {0}; 

extern DB_t SMCfg;
/* globals from other files */
extern processDataQue_t g_stProcDataQ;
extern osMutexId_t m_hRxSegMtxLockHandle;
extern osMutexId_t m_hCmplxAckMtxLockHandle;
extern osSemaphoreId_t g_hSegmentationSemaphoreHandle;
extern osMutexId_t m_hInitMtxLockHandle;
/*
*   Global variable for Segmentation: 
*	Actual Window Size
*   Proposed Window Size 
*/
uint8_t g_u8ActualWindowSize = ACTUAL_WINDOW_SIZE;
uint8_t g_u8ProposedWindowSize = PROPOSED_WINDOW_SIZE;

/** Thread handle for segmententation thread. */
Thread_H g_hSegmentationThread;
/** Semaphore handle for segmententation thread */
Sem_H g_hSegmentationSemaphore;
/** Thread exit flag for segmententation thread */
bool g_bSegmentThreadExit = FALSE;

/**	api to create linklist for segmentation */
static BACNET_RETURN_TYPE Create_LinkList_For_Segmentation(processInfo_t *pstProcQInfo,
												  LINK_LIST_TYPE eListType);

/** checks if segment is next expected, duplicate, out of order or unknown */
static BACNET_RETURN_TYPE Validate_Segment(processInfo_t *pstProcQInfo, processInfo_t **pstOutProcQInfo);

/** updates instance of processQ to send SegAck	*/
static void Update_ProcessQ_To_Send_SegACK(processInfo_t *pstRxProcQInfo, processInfo_t *pstProcQInfo);

/** on reception of final segment, combines all segments & create new instance in processQ
	for further processing */
static void Combine_Data_and_Fill_Process_Queue(processInfo_t *pstRxProcQInfo, processInfo_t **pstOutProcQInfo);

/*************************************************************************************	
*
*	DESCRIPTION
*	Function to encode the APDU segment / packet.
*	
*	@param	pstProcQInfo	[in]	Pointer to process queue instance.
*	@param	pstAPDUSegData	[in]	Pointer to apdu segmentation data instance.
*	@param	pstApduSegment	[in]	Pointer to the segment to be encoded.
*	
*	@return		[out]	returns the length of encoded apdu_segment on success
*	
**************************************************************************************/
static int32_t Encode_Segment(processInfo_t *pstProcQInfo, Apdu_Segment_Data_t *pstAPDUSegData, 
					   APDUSegment_t *pstApduSegment, uint8_t *au8EndodedData)
{
	uint16_t	u16Index = 0;
	uint16_t	u16Result = 0;
	uint16_t	u16APDULen = 0;
	BACNET_PDU_TYPE ePduType;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Segment: enter \r\n");
	#endif

    if(NULL == pstProcQInfo || NULL == pstAPDUSegData || NULL == pstApduSegment || NULL == au8EndodedData)
    {
        return u16APDULen;
    }

	/* get pdu type */
	ePduType = pstProcQInfo->m_stProcessData.m_ePDUType;

		/* encode pdu type */
		au8EndodedData[u16Index++] = pstProcQInfo->m_stProcessData.m_ePDUType;
	    
		/* encode segmentation support */  
		au8EndodedData[0] |= BIT3;
		
		/* encode more follows */
		if(pstApduSegment->m_bMoreFollows)
			au8EndodedData[0] |= BIT2;
		else
		{
			; /* Do nothing */
		}
			/* keep it as it is i.e. FALSE */

		if(PDU_TYPE_CONFIRMED_SERVICE_REQUEST == ePduType)
		{
			/* encode segmented response accepted */
			if(pstProcQInfo->m_stProcessData.m_stAPDU.m_bSegmentedResponseAccepted)
				au8EndodedData[0] |= BIT1;

			/* encode max segs & max resp */
			au8EndodedData[u16Index++] = Encode_Max_Segs_Max_Apdu(
										pstProcQInfo->m_stProcessData.m_stAPDU.m_i32MaxSegs,
										pstProcQInfo->m_stProcessData.m_stAPDU.m_i32MaxResp);
		}
	    
		/* Encode invoke id */
		au8EndodedData[u16Index++] = pstApduSegment->m_u8InvokeId;
		/* Encode sequence number */
		au8EndodedData[u16Index++] = pstApduSegment->m_u8SequenceNumber;
		/* Encode proposed / actual window size */
		au8EndodedData[u16Index++] = pstAPDUSegData->m_u8ActualWindowSize;
		/* Encode service choice */
		au8EndodedData[u16Index++] = pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice;

		if(PDU_TYPE_COMPLEX_ACK == ePduType)
			u16APDULen += 5;	/* 5 bytes encoded */
		else /* PDU_TYPE_CONFIRMED_SERVICE_REQUEST */
			u16APDULen += 6;	/* 6 bytes encoded */

		if(pstApduSegment->m_bMoreFollows)
		{
			/* copy data from original pdu */
			memcpy(&au8EndodedData[u16Index], pstApduSegment->m_ptrToData, 
					pstAPDUSegData->m_uRxTxSegmentInfo.m_stTxSegmentInfo.m_u16BytesToCopy);
			/* total apdu length = fixed part + variable part */
			u16APDULen += pstAPDUSegData->m_uRxTxSegmentInfo.m_stTxSegmentInfo.m_u16BytesToCopy;
		}
		else
		{
			/* calculate no of bytes to copy for last segment */
			u16Result = pstAPDUSegData->m_u32ActualDataSize -
				(	(pstAPDUSegData->m_u8NoOfPackets-1) * 
					(pstAPDUSegData->m_uRxTxSegmentInfo.m_stTxSegmentInfo.m_u16BytesToCopy)	);
			/* copy data from original pdu */
			memcpy(&au8EndodedData[u16Index], pstApduSegment->m_ptrToData, 
					u16Result);
			/* total apdu length = fixed part + variable part */
			u16APDULen += u16Result;
		}
		/* save the length of encoded data for every segment */
		pstApduSegment->m_u16PacketLength = u16APDULen;

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Encode_Segment: exit \r\n");
	#endif
	return u16APDULen;
}

/*************************************************************************************	
*
*	DESCRIPTION
*	Function to encode & send (E&S) segments.
*		-	this api E&S's a maximum of m_u8ActualWindowSize segments or upto last 
*			segment, whichever is earlier.
*		-	encoding starts from seg.no. (i8SegAckReceivedFor+1)
*	
*	@param	pstProcQInfo		[in]	Pointer to process queue instance.
*	@param	pstAPDUSegData		[in]	Pointer to apdu segmentation data instance.
*	@param	i8SegAckReceivedFor	[in]	Segment no of last segment that received ack.
*	
*	@return		BACDEL_SUCCESS on successessful transmission of all segments
*				else BACDEL_ERROR if any 1 of the segment returned unsuccessful 
*				transmission.				
*	
**************************************************************************************/
BACNET_RETURN_TYPE APDU_Packet_Encode_Handler(	processInfo_t *pstProcQInfo,
												int8_t i8SegAckReceivedFor,
												Apdu_Segment_Data_t *pstAPDUSegData)
{
	BACNET_RETURN_TYPE eReturnVal = BACDEL_SUCCESS;
	uint8_t	u8Count = 0;
	uint8_t	u8PacketCount = 0;
	uint8_t	u8FromSegment = 0;
	int32_t	i32APDULen = 0;	
	APDUSegment_t *pstApduSegment = NULL;
	APDUSegment_t *pstApduSegmentTemp = NULL;
	uint8_t au8EndodedData[MAX_APDU_LENGTH_ACCEPTED] = {0};

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: APDU_Packet_Encode_Handler: enter \r\n");
	#endif

    if(NULL == pstProcQInfo || NULL == pstAPDUSegData)
    {
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
						APDU_Packet_Encode_Handler: null pointers \r\n");
		#endif
		return BACDEL_ERROR;
	}

	/*	i8SegAckReceivedFor will start from -1(default initial value)
		& receives the segment no of last segment that received ack, so add 1 to it */
	u8FromSegment = (uint8_t)(i8SegAckReceivedFor+1);

	/* change the wait for segACK flag */
	pstAPDUSegData->m_uRxTxSegmentInfo.m_stTxSegmentInfo.m_bWaitForSegAck = FALSE;

	/* get address of 1st segment */
	pstApduSegment = pstAPDUSegData->m_pstSegment;
    if(NULL == pstApduSegment)
    {
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
		APDU_Packet_Encode_Handler: null pstApduSegment pointer \r\n");
		#endif
		/* change the wait for segACK flag */
		pstAPDUSegData->m_uRxTxSegmentInfo.m_stTxSegmentInfo.m_bWaitForSegAck = TRUE;
		return BACDEL_ERROR;
	}
	for(u8Count=0; u8Count<u8FromSegment; u8Count++)
	{
		/* move to next segment */
		pstApduSegment = pstApduSegment->m_pstNext;
		if(NULL == pstApduSegment)
		{
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
							APDU_Packet_Encode_Handler: null pointer \r\n");
			#endif
			/* change the wait for segACK flag */
			pstAPDUSegData->m_uRxTxSegmentInfo.m_stTxSegmentInfo.m_bWaitForSegAck = TRUE;
			return BACDEL_ERROR;
		}
	}

	/* TODO - in this loop...
				-say 3 segments are to be sent.
				-if encoding of any 1 segment fails.
				-it still sends the remaining segments.	*/
	while(NULL != pstApduSegment)
	{
		if(SEGMENT_STATE_SEND_REQUEST == pstApduSegment->m_eServiceState)
		{
			/* call encode apdu */
			i32APDULen = Encode_Segment(pstProcQInfo, pstAPDUSegData, pstApduSegment, au8EndodedData);
			eReturnVal = NPDU_Encode_Handler(pstProcQInfo, au8EndodedData, (uint16_t)i32APDULen);
			if((i32APDULen < 0) || (BACDEL_ERROR == eReturnVal))
			{
				/* encoding apdu failed or npdu_encode_handler returned error */
				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
								APDU_Packet_Encode_Handler: encoding segment failed \r\n");
				#endif
				pstApduSegment->m_eServiceState = SEGMENT_STATE_REQUEST_SEND_FAILED;
			}
			/*	maintain local packet / segment count, i.e. no of segments encoded in 
				single function call to APDU_Packet_Encode_Handler */
			u8PacketCount++;
			if(u8PacketCount > pstAPDUSegData->m_u8ActualWindowSize)
			{
				/* no of packets encoded is morethan actual_window_size */
				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: APDU_Packet_Encode_Handler: \
									no. of packets encoded is more than actual window size \r\n");
				#endif
				pstApduSegment->m_eServiceState = SEGMENT_STATE_ERROR;
				/* decrement packet count to handle this error */
				u8PacketCount--;
				break;
			}
			/* save the address of 1st segment that was encoded */
			if(1 == u8PacketCount)
				pstApduSegmentTemp = pstApduSegment;
			/* maintain last transmitted segment */
			pstAPDUSegData->m_i8LastSegment = pstApduSegment->m_u8SequenceNumber;
			/* re-initialize the buffer */
			memset(au8EndodedData, 0, MAX_APDU_LENGTH_ACCEPTED);
		}//end of if
		/* move to next segment */
		if(NULL == pstApduSegment->m_pstNext)
				break;	/* this was the last segment */
		pstApduSegment = pstApduSegment->m_pstNext;
	}//end of while

	/* change the state of segments encoded & sent on network to, await_response */
	pstApduSegment = pstApduSegmentTemp;
	 
	Osal_Wait_Mutex( m_hCmplxAckMtxLockHandle , INFINITE);
	
	for(;u8PacketCount>0; u8PacketCount--)
	{
		/* reset the segment timeout */
		pstApduSegment->m_i32DevSegTimeout = pstAPDUSegData->m_u32DevSegmentTimeout;
		/* case to check if state is not ERROE or SEND_FAILED or timeout */
		if(SEGMENT_STATE_SEND_REQUEST != pstApduSegment->m_eServiceState)
		 {
			;
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: APDU_Packet_Encode_Handler: \
										invalid segment_state  \r\n");
			#endif
		 }
		else
			/* successfully encoded segment & sent, now wait for response */
			pstApduSegment->m_eServiceState = SEGMENT_STATE_AWAIT_RESPONSE;
		/* move to next segment */
		if(NULL == pstApduSegment->m_pstNext)

				break;	/* this was the last segment */
		pstApduSegment = pstApduSegment->m_pstNext;
	}
	/* release the mutex lock - Tx side */
	Osal_Release_Mutex( m_hCmplxAckMtxLockHandle );
	/* change the wait for segACK flag */
	pstAPDUSegData->m_uRxTxSegmentInfo.m_stTxSegmentInfo.m_bWaitForSegAck = TRUE;

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: APDU_Packet_Encode_Handler: exit \r\n");
	#endif
	return eReturnVal;
}

/****************************************************************************************
*                                                                    
*	DESCRIPTION:
*	This function creates APDU segments & fills all data related to that segment in
*	a structure. It also initializes the apdu_segmentation_data structure i.e 
*	Apdu_Segment_Data_t * pstAPDUSegmentData
*
*		-	this api does all calculations related to no. of segments to be formed,
*			no of bytes to be that the segment should encode, etc.
*		-	it also creates a linklist of the segments.
*
*	@param	pAPDUData		[in]    The actual encoded apdu data without segmentation.
*	@param	u16APDULen      [in]    Total length of encoded APDU.
*	@param	pstProcQInfo	[in]	Pointer to process queue instance.
*	
*	@return    BACDEL_SUCCESS / BACDEL_ERROR
*
*****************************************************************************************/
BACNET_RETURN_TYPE Create_APDU_Packets(processInfo_t *pstProcQInfo,
										uint8_t *pAPDUData,
										uint32_t u32APDULen)
{
	/* local variables */
	BACNET_RETURN_TYPE eReturnVal = BACDEL_SUCCESS;
	Apdu_Segment_Data_t * pstAPDUSegmentData = NULL;
	APDUSegment_t *pApduSegment[MAX_SEGMENTS_TX] = {NULL};	
	uint8_t	u8Count = 0;
	uint32_t u32DevSegmentTimeout = 0;
	BACNET_PDU_TYPE ePduType;
    virtualDevData_t* pstDeviceStruct = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Create_APDU_Packets: entry \r\n");
	#endif

    if(NULL == pstProcQInfo || NULL == pAPDUData)
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Create_APDU_Packets: \
                            null input pointers. \r\n");
		#endif
        return BACDEL_ERROR;
    }

    pstDeviceStruct = pstProcQInfo->m_pvReqDevStruct;
    if(NULL == pstDeviceStruct)
    {
        return BACDEL_ERROR;
    }

	/* read apdu segment timeout property */
	u32DevSegmentTimeout = pstDeviceStruct->m_stDevObject.
						m_stAPDUSegTimeout.m_u32Val / CONVERT_TO_SECONDS;

	/* update pstProcQInfo */
	/* make m_eState i.e. state of process queue to PROC_CONTINUE i.e. in use */
	pstProcQInfo->m_eState = PROC_CONTINUE;
	/* set the m_bSegmentedMessage flag to TRUE so that initiate thread will know that
		the request initiated needs message segmentation.
		so that it will change its m_eServiceState STATE_SEGMENTED_CONFIRMATION
		which will stop main apdu_timeout timer monitoring this request for retries */
	/* this state will be changed to STATE_AWAIT_RESPONSE when all the segments are 
		successfully transmitted. */
	pstProcQInfo->m_stProcessData.m_stAPDU.m_bSegmentedMessage = TRUE;
	/* get pdu type */
	ePduType = pstProcQInfo->m_stProcessData.m_ePDUType;

	/* allocate memory to save segmentation data */
	pstAPDUSegmentData = (Apdu_Segment_Data_t *)OSAL_Malloc(sizeof(Apdu_Segment_Data_t),
														__FILE__, __FUNCTION__, __LINE__);
	if(NULL == pstAPDUSegmentData)
	{
		/* failed memory allocation so return error */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: Create_APDU_Packets: malloc error \r\n");
		#endif
		return BACDEL_MALLOC_ERROR;
	}
	/* link allocated memory with the pointer in global structure for initiated request */
	pstProcQInfo->m_pstAPDUSegmentData = pstAPDUSegmentData;

	/* fill the data */
	pstAPDUSegmentData->m_i32PDUType = pstProcQInfo->m_stProcessData.m_ePDUType;
	pstAPDUSegmentData->m_u8ProposedWindowSize = g_u8ProposedWindowSize;
	pstAPDUSegmentData->m_u8ActualWindowSize = pstAPDUSegmentData->m_u8ProposedWindowSize;
	pstAPDUSegmentData->m_pstSegment = NULL;
	pstAPDUSegmentData->m_u32DevSegmentTimeout = u32DevSegmentTimeout;	/* device segment timeout */
	pstAPDUSegmentData->m_u32ActualDataSize = 0;
	pstAPDUSegmentData->m_u8NoOfPackets = 0;
	if(PDU_TYPE_COMPLEX_ACK == ePduType)
		pstAPDUSegmentData->m_bServer = TRUE;	/* this is server device */

	
	/* calculation related to bytes */
	if(PDU_TYPE_COMPLEX_ACK == ePduType)
		/* fixed part for complex ack pdu is 3 bytes */
		pstAPDUSegmentData->m_u32ActualDataSize = u32APDULen - 3;
	else /* PDU_TYPE_CONFIRMED_SERVICE_REQUEST */
		/* fixed part for confirmed request pdu is of size 4 bytes */
		pstAPDUSegmentData->m_u32ActualDataSize = u32APDULen - 4;
	
	/* copy u16BytesToCopy no of bytes to end of fixed part of every encoded packet */
	if(PDU_TYPE_COMPLEX_ACK == ePduType)
		/* note : 5 bytes are needed for encoding fixed part of segmented apdu */
		pstAPDUSegmentData->m_uRxTxSegmentInfo.m_stTxSegmentInfo.m_u16BytesToCopy = 
			(uint16_t)pstProcQInfo->m_stProcessData.m_stAPDU.m_i32MaxResp - 5;
	else /* PDU_TYPE_CONFIRMED_SERVICE_REQUEST */
		/* note : 6 bytes are needed for encoding fixed part of segmented apdu */
		pstAPDUSegmentData->m_uRxTxSegmentInfo.m_stTxSegmentInfo.m_u16BytesToCopy = 
			pstProcQInfo->m_stProcessData.m_stDstInfo.m_u16MaxAPDULenAccepted - 6;

	/* no of packets to be created */
	pstAPDUSegmentData->m_u8NoOfPackets = 
		(uint8_t)(pstAPDUSegmentData->m_u32ActualDataSize /
		pstAPDUSegmentData->m_uRxTxSegmentInfo.m_stTxSegmentInfo.m_u16BytesToCopy);
	if((pstAPDUSegmentData->m_u32ActualDataSize %
		pstAPDUSegmentData->m_uRxTxSegmentInfo.m_stTxSegmentInfo.m_u16BytesToCopy) != 0)
	{
		/* result of division was fraction so add 1 more packet */
		pstAPDUSegmentData->m_u8NoOfPackets += 1;
	}

	/* check for max segments that can be */
	if(pstAPDUSegmentData->m_u8NoOfPackets >= (DEFAULT_SEGMENTS_TX))
	{
		if(PDU_TYPE_COMPLEX_ACK == ePduType)
		{
			/* max segments exceeded, send abort */
			OSAL_Free(pstAPDUSegmentData, __FILE__, __FUNCTION__, __LINE__);
            pstAPDUSegmentData = NULL;
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer: Create_APDU_Packets: "
						"Max Segments Limit reached. \r\n");
			#endif
			return BACDEL_LOCAL_BUFFER_EXCEEDED;
		}
		else
		{
			/* free allocated memory */
			OSAL_Free(pstAPDUSegmentData, __FILE__, __FUNCTION__, __LINE__);
            pstAPDUSegmentData = NULL;
			/* no of segments to be sent is more than max segments */
			/* FIXME: Notify Application That Can't send Request */
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer: Create_APDU_Packets: "
						"Max Segments Limit reached. \r\n");
			#endif
			return BACDEL_ERROR;
		}
	}

	/* check for max segments of client device */
	/* reference bug id - 4415 */
	if(pstProcQInfo->m_stProcessData.m_stAPDU.m_i32MaxSegs < pstAPDUSegmentData->m_u8NoOfPackets)
	{
		if(PDU_TYPE_COMPLEX_ACK == ePduType)
		{
			/* max segments exceeded, send abort */
			OSAL_Free(pstAPDUSegmentData, __FILE__, __FUNCTION__, __LINE__);
            pstAPDUSegmentData = NULL;
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer: Create_APDU_Packets: "
			"Max Segments Limit reached. \r\n");
			#endif
			return BACDEL_LOCAL_BUFFER_EXCEEDED;
		}
		else
		{
			; /* do nothing */
			/* check if condition needs to be handled for confirmed PDU type */
		}
	}

	/* maintain last transmitted segment */
	pstAPDUSegmentData->m_i8LastSegment = -1;
	/* maintain, uptill which segment successful segACK is rceived */
	pstAPDUSegmentData->m_i8SegAckFor = -1;
	/* wait for SegACK */
	pstAPDUSegmentData->m_uRxTxSegmentInfo.m_stTxSegmentInfo.m_bWaitForSegAck = FALSE;
	/* all segs sent & ack's received is false - default value */
	pstAPDUSegmentData->m_bAllSegments = FALSE;
	
	/* pointer to actual apdu data */
	pstAPDUSegmentData->m_pu8ActualApduData = 
		(uint8_t *)OSAL_Malloc((u32APDULen), __FILE__, __FUNCTION__, __LINE__);
	if(NULL == pstAPDUSegmentData->m_pu8ActualApduData)
	{
		/* free allocated memory */
		OSAL_Free(pstAPDUSegmentData, __FILE__, __FUNCTION__, __LINE__);
        pstAPDUSegmentData = NULL;
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: Create_APDU_Packets: \
									malloc error \r\n");
		#endif
		return BACDEL_MALLOC_ERROR;
	}
	/* copy data from static memory to dynamic memory */
	memcpy(pstAPDUSegmentData->m_pu8ActualApduData, pAPDUData, u32APDULen);

	for(u8Count=0; u8Count < pstAPDUSegmentData->m_u8NoOfPackets; u8Count++)
	{
		/* allocate memory for all segment information */
		pApduSegment[u8Count] = (APDUSegment_t *)OSAL_Malloc(sizeof(APDUSegment_t),
														__FILE__, __FUNCTION__, __LINE__);
		if(NULL == pApduSegment[u8Count])
		{
			/* TODO - Free the segments */
			/* memory allocation failed */
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: Create_APDU_Packets: \
										malloc error \r\n");
			#endif
			return BACDEL_MALLOC_ERROR;
		}
		else	/* fill data in segment */
		{
			/* state of packet */
			pApduSegment[u8Count]->m_eServiceState = SEGMENT_STATE_FILL_REQUEST;
			/* segment timeout */
			pApduSegment[u8Count]->m_i32DevSegTimeout = u32DevSegmentTimeout;
			/* retry count */
			pApduSegment[u8Count]->m_i32DevRetryCnt = 0;
			/* sequence number */
			pApduSegment[u8Count]->m_u8SequenceNumber = u8Count;

			/* invoke id */
			pApduSegment[u8Count]->m_u8InvokeId = pstProcQInfo->m_stProcessData.m_stAPDU.m_u8InvokeId;
		
			/* more follows */
			if(u8Count == (pstAPDUSegmentData->m_u8NoOfPackets -1))
				pApduSegment[u8Count]->m_bMoreFollows = FALSE;	/* last packet */
			else 
				pApduSegment[u8Count]->m_bMoreFollows = TRUE;

			/* pointer to variable part of APDU data that the packet will send */
			if(u8Count == 0)
				if(PDU_TYPE_COMPLEX_ACK == ePduType)
					pApduSegment[u8Count]->m_ptrToData = 
					&pstAPDUSegmentData->m_pu8ActualApduData[3];	/* 1st packet */
				else /* PDU_TYPE_CONFIRMED_SERVICE_REQUEST */
					pApduSegment[u8Count]->m_ptrToData = 
					&pstAPDUSegmentData->m_pu8ActualApduData[4];	/* 1st packet */
			else 
				pApduSegment[u8Count]->m_ptrToData = 
				pApduSegment[u8Count-1]->m_ptrToData + 
				pstAPDUSegmentData->m_uRxTxSegmentInfo.m_stTxSegmentInfo.m_u16BytesToCopy;
		
			/* maintain link between segments / packets */
			if(u8Count == 0)
				pstAPDUSegmentData->m_pstSegment = pApduSegment[u8Count];	/* 1st packet */
			else
				pApduSegment[u8Count-1]->m_pstNext = pApduSegment[u8Count];
		}
	}

	/* Check PDU TYPE & take necessary action */
	if(PDU_TYPE_COMPLEX_ACK == ePduType)
	{
		/* save all data from process que for complex ack */
		eReturnVal = Create_LinkList_For_Segmentation(pstProcQInfo, LINK_LIST_SEGMENTATION_TRANSMIT);
		if(BACDEL_SUCCESS != eReturnVal)
		{
			/* TODO - Free the segments */
			/* free malloced memory */
            if(NULL != pstAPDUSegmentData->m_pu8ActualApduData)
                OSAL_Free(pstAPDUSegmentData->m_pu8ActualApduData,
				    __FILE__, __FUNCTION__, __LINE__);
            pstAPDUSegmentData->m_pu8ActualApduData = NULL;
            if(NULL != pstAPDUSegmentData)
			    OSAL_Free(pstAPDUSegmentData, __FILE__, __FUNCTION__, __LINE__);
            pstAPDUSegmentData = NULL;
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: Create_APDU_Packets\
            Create_LinkList_For_Segmentation fails \r\n");
			#endif
			return eReturnVal;
		}
	}

	/* 1st segment is ready to be sent now, since data related to all segments is ready */
	pApduSegment[0]->m_eServiceState = SEGMENT_STATE_SEND_REQUEST;

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Create_APDU_Packets: exit \r\n");
	#endif
	return eReturnVal;
}

/***********************************************************************************
*
*	DESCRIPTION
*   This function copies the data to the structure to be passed to A-Side
*   Interface in relation to SEGMENTATION. 
*	Based on the data passing mechanism & after this function returns :
*		-	either further segments are sent 
*		-	or if all segments have been successfully sent then, segmentation 
*			data is cleared & state of initiated request in pstInitiateInfoQ
*			is changed to AWAIT_RESPONSE
*
*	@param	pstRspData			[in]	Pointer to structure form servicelayer which has
*										stored the received data.
*	@param	pstAPDUSegmentData	[out]	Pointer to structure which holds the
*										segmentation related data.
*
*	@return VOID
*
***********************************************************************************/
bool Data_Exchange_A_Side_For_Segmentation(processInfo_t* pstRspData,
										   Apdu_Segment_Data_t* pstAPDUSegmentData)
{
	/* local varables */
	uint8_t	u8Count = 0;
	APDUSegment_t* pstApduSegment = NULL;
	int8_t i8PrevSegAckFor = 0;
	uint8_t u8ExpectedSeqNo = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
								Data_Exchange_A_Side_For_Segmentation: entry \r\n");
	#endif

	/* check for null pointers */
	if(!pstRspData || !pstAPDUSegmentData )
		return FALSE;

	/* change the wait for segACK flag */
	pstAPDUSegmentData->m_uRxTxSegmentInfo.m_stTxSegmentInfo.m_bWaitForSegAck = FALSE;

	/* check for duplicate SegACK */
	if(pstRspData->m_stProcessData.m_stAPDU.m_u8SequenceNumber <= 
		pstAPDUSegmentData->m_i8SegAckFor)
	{
        if(PDU_TYPE_ABORT == pstRspData->m_stProcessData.m_ePDUType)
        {
            /* make all segments true to discard this segmentation node as
            segment exchanges is aborted */
            pstAPDUSegmentData->m_bAllSegments = TRUE;
        }
		/* this is a duplicate SegACK that is received, so ignore */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer: \
						Data_Exchange_A_Side_For_Segmentation: Duplicate SegACK \r\n");
		#endif
		/* set flag to continue retries */
		pstAPDUSegmentData->m_uRxTxSegmentInfo.m_stTxSegmentInfo.m_bWaitForSegAck = TRUE;
		return FALSE;
	}

	/* if seg-ack received for first packet of segmented data */
	if(-1 == pstAPDUSegmentData->m_i8SegAckFor)
	{
		/* make previous Seg Ack count as default = 0 */
		i8PrevSegAckFor = 0;
		/* set expected seq no of segment */
		u8ExpectedSeqNo = 0;
	}
	else
	{
		/* backup the last seg-ack number */
		i8PrevSegAckFor = pstAPDUSegmentData->m_i8SegAckFor;
		/* set expected seq no of segment, check as per actual window size */
		u8ExpectedSeqNo = i8PrevSegAckFor + pstAPDUSegmentData->m_u8ActualWindowSize;
	}	

	/* return if received sequence number is invalid */
	/* reference bug id - 3902 */
	if(u8ExpectedSeqNo >= pstRspData->m_stProcessData.m_stAPDU.m_u8SequenceNumber)
	{
		;// do nothing
	}
	else
	{
		/* this is a invalid SegACK that is received, so ignore */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer: \
		Data_Exchange_A_Side_For_Segmentation: Invalid SegACK \r\n");
		#endif
		/* set flag to continue retries */
		pstAPDUSegmentData->m_uRxTxSegmentInfo.m_stTxSegmentInfo.m_bWaitForSegAck = TRUE;
		return FALSE;
	}

	/* update the pstAPDUSegmentData */
	pstAPDUSegmentData->m_i8SegAckFor = 
		pstRspData->m_stProcessData.m_stAPDU.m_u8SequenceNumber;

	if(!pstRspData->m_stProcessData.m_stAPDU.m_u8SequenceNumber)
	{
		/* 1st segment received ack, so update actual window as per client requirement */
		if(g_u8ProposedWindowSize < pstRspData->m_stProcessData.m_stAPDU.m_u8ProposedWindowNumber)
		{
			/* do segmentation as per stack's actual w */
			pstAPDUSegmentData->m_u8ActualWindowSize = g_u8ProposedWindowSize;
		}
		else
		{
			pstAPDUSegmentData->m_u8ActualWindowSize = 
				pstRspData->m_stProcessData.m_stAPDU.m_u8ProposedWindowNumber;
		}
	}
	else if(pstRspData->m_stProcessData.m_stAPDU.m_u8SequenceNumber ==
		(pstAPDUSegmentData->m_u8NoOfPackets-1))
	{
		/* all segments have been transmitted & last SegAck is received */
		pstAPDUSegmentData->m_bAllSegments = TRUE;
	}
	
	/* acquire the mutex lock - Tx side */
	Osal_Wait_Mutex( m_hCmplxAckMtxLockHandle , INFINITE);
	/* get address of 1st segment */
	pstApduSegment = pstAPDUSegmentData->m_pstSegment;
	/* change state of segments 0-m_i8SegAckReceivedFor to STATE_RESPONSE_RECEIVED */
	if(pstAPDUSegmentData->m_i8SegAckFor <= (pstAPDUSegmentData->m_u8NoOfPackets-1))
	{
		for(u8Count=0; u8Count <= pstAPDUSegmentData->m_i8SegAckFor; u8Count++)
		{
			pstApduSegment->m_eServiceState = SEGMENT_STATE_RESPONSE_RECEIVED;
			/* move to next segment */
			pstApduSegment = pstApduSegment->m_pstNext;
		}
	}
	else
	{
		/* release the mutex lock - Tx side */
		Osal_Release_Mutex( m_hCmplxAckMtxLockHandle );
		/* restore previous seg ack no for retries */
		pstAPDUSegmentData->m_i8SegAckFor = i8PrevSegAckFor;
		/* set flag to continue retries */
		pstAPDUSegmentData->m_uRxTxSegmentInfo.m_stTxSegmentInfo.m_bWaitForSegAck = TRUE;
		return FALSE;
	}

	/* TODO - Handling for Negative SegACK is taken care in this function.
		In case a -ve seg ack is received its retry count for corresponding 
		segments is increased by checking the value of last transmitted segment.
		NOTE - the retry count is commented for now */

	if(!pstAPDUSegmentData->m_bAllSegments)
	{
		if(pstRspData->m_stProcessData.m_stAPDU.m_bNAK)
		{
			/* negative segmentACK received */
			pstAPDUSegmentData->m_bNAK = TRUE;
		}
		/* segmentACK received */
		/* to send further data, change state of next segments to be sent */
		for(u8Count=0; u8Count<pstAPDUSegmentData->m_u8ActualWindowSize ; u8Count++)
		{
			/* change the state to send_request */
			pstApduSegment->m_eServiceState = SEGMENT_STATE_SEND_REQUEST;
			/* check for -ve SegAck */
			if(pstAPDUSegmentData->m_bNAK  && 
				(pstApduSegment->m_u8SequenceNumber <= 
				pstAPDUSegmentData->m_i8LastSegment))
			{
				/* negative segmentACK received */
				//pstApduSegment->m_i32DevRetryCnt++;
				/* if retry count is more than max retries issue a timeout */
			}
			if(NULL == pstApduSegment->m_pstNext)
				/* this was the last segment, get out of for loop */
				break;
			/* move to next segment */
			pstApduSegment = pstApduSegment->m_pstNext;
		}
		/* reset the m_bNAK */
		pstAPDUSegmentData->m_bNAK = FALSE;
	}// end of if(!pstAPDUSegmentData->m_bAllSegmentsSent)
	else;	/* do nothing here */

	/* release the mutex lock - Tx side */
	//Osal_Release_Mutex(g_stComplexAckDataQue.m_hCmplxAckMtxLock);
	 
	Osal_Release_Mutex( m_hCmplxAckMtxLockHandle );
	/* check if all segments have been transmitted & last SegAck is received */
	if(!pstAPDUSegmentData->m_bAllSegments)
	{
		/* all segments not transmitted */
		/* change the wait for segACK flag */

		pstAPDUSegmentData->m_uRxTxSegmentInfo.m_stTxSegmentInfo.m_bWaitForSegAck = TRUE;
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
								Data_Exchange_A_Side_For_Segmentation: exit \r\n");
	#endif
	return TRUE;
}

/**
*
*	DESCRIPTION
*	This function changes the state of segmentation data from linklist to PROC_DONE
*   so that the timer free's this node from linklist.
*
*	@param		pstProcQInfo	[in]	Instance to the processQ / Process Linklist
*										from which memories are to be freed.
*	@param		eListType		[in]	Type of link list either Transmit side or Receive side.
*
*/
void Clear_Segmentation_Data_State(processInfo_t *pstProcQInfo, LINK_LIST_TYPE eListType)
{
	/* local variables */

    /* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_Segmentation_Data_State: entry \r\n");
	#endif

	if(NULL == pstProcQInfo)
	{
		/* error case */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
        Clear_Segmentation_Data_State: null input pointer \r\n");
		#endif
		return;	
	}

    /* switch to list type */
    switch(eListType)
    {
    case LINK_LIST_SEGMENTATION_TRANSMIT:
        /* acquire mutex lock */
        //Osal_Wait_Mutex(g_stComplexAckDataQue.m_hCmplxAckMtxLock, INFINITE);
		 
		Osal_Wait_Mutex( m_hCmplxAckMtxLockHandle , INFINITE);
        /* update state */
        pstProcQInfo->m_eState = PROC_DONE;
        /* release mutex lock */
       // Osal_Release_Mutex(g_stComplexAckDataQue.m_hCmplxAckMtxLock);
		 
		Osal_Release_Mutex( m_hCmplxAckMtxLockHandle );
        break;

    case LINK_LIST_SEGMENTATION_RECEIVE:
        /* acquire mutex lock */
        //Osal_Wait_Mutex(g_stRxSegmentsDataQue.m_hRxSegMtxLock, INFINITE);
		 
		Osal_Wait_Mutex(m_hRxSegMtxLockHandle, INFINITE);
        /* update state */
        pstProcQInfo->m_eState = PROC_DONE;
        /* release mutex lock */
        //Osal_Release_Mutex(g_stRxSegmentsDataQue.m_hRxSegMtxLock);
		 
		Osal_Release_Mutex( m_hRxSegMtxLockHandle );
        break;

    default:
        break;
    }// switch ends.

    /* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Clear_Segmentation_Data_State: exit \r\n");
	#endif
}

/*******************************************************************************************
*
*	DESCRIPTION
*	This function clears all the memories that were allocated to save the data
*	related to segmentation.
*
*	@param		pstProcQInfo	[in]	Instance to the processQ / Process Linklist
*										from which memories are to be freed.
*	@param		eListType		[in]	Type of link list either Transmit side or Receive side.
*
*******************************************************************************************/
void Clear_Segmentation_Data(processInfo_t *pstProcQInfo, LINK_LIST_TYPE eListType)
{
	/* local variables */
	Apdu_Segment_Data_t * pstAPDUSegmentData = NULL;
	APDUSegment_t* pstApduSegment = NULL;
	APDUSegment_t* pstApduSegmentTemp = NULL;
	BACNET_PDU_TYPE ePduType;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Clear_Segmentation_Data: entry \r\n");
	#endif

	if(NULL == pstProcQInfo)
	{
		/* error case */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: Clear_Segmentation_Data: \
									null input pointer \r\n");
		#endif
		return;	
	}

    /* get addresses */
	pstAPDUSegmentData = pstProcQInfo->m_pstAPDUSegmentData;
	if(NULL == pstAPDUSegmentData)
		return;	/* error case */

	/* save the pdu type */
	ePduType = pstProcQInfo->m_stProcessData.m_ePDUType;
	if(pstProcQInfo->m_stProcessData.m_ePDUType != pstProcQInfo->m_pstAPDUSegmentData->m_i32PDUType)
		return;	/* error case */

	/* Get the mutex lock */
    if(LINK_LIST_SEGMENTATION_TRANSMIT == eListType && 
        PDU_TYPE_CONFIRMED_SERVICE_REQUEST == ePduType)
    {
        ; /* NOTE : this is fix for situation when segmented confirmed request is sent
          & device stops initiating any request after all segments are transmitted or abort or 
          reject is sent by server device. */
          /* no need to acquire mutex for confirmed segmented request as it is 
          not saved in g_stComplexAckDataQue */
    }
	else if(LINK_LIST_SEGMENTATION_TRANSMIT == eListType && 
        PDU_TYPE_COMPLEX_ACK == ePduType)
	{
		/* acquire the mutex lock - Tx side */
		//Osal_Wait_Mutex(g_stComplexAckDataQue.m_hCmplxAckMtxLock, INFINITE);
		 
		Osal_Wait_Mutex( m_hCmplxAckMtxLockHandle , INFINITE);
	}
	else if(LINK_LIST_SEGMENTATION_RECEIVE == eListType)
	{
		/* acquire the mutex lock - Rx side */
		//Osal_Wait_Mutex(g_stRxSegmentsDataQue.m_hRxSegMtxLock, INFINITE);
		 
		Osal_Wait_Mutex(m_hRxSegMtxLockHandle, INFINITE);
	}
    else
    {
    	; /* do nothing */
    }

    /* get 1st segment */
    pstApduSegment = pstAPDUSegmentData->m_pstSegment;

	while(NULL != pstApduSegment)
	{
		/* save next segment address */
		pstApduSegmentTemp = pstApduSegment->m_pstNext;
		if(LINK_LIST_SEGMENTATION_RECEIVE == eListType)
		{
			/* free memory allocated to save received encoded data */
			if(NULL != pstApduSegment->m_ptrToData)
				OSAL_Free(pstApduSegment->m_ptrToData, __FILE__, __FUNCTION__, __LINE__);
			else
			{
				;	/* this is error case */
			}
            pstApduSegment->m_ptrToData = NULL;
		}
		/* free allocated memory for segment */
		OSAL_Free(pstApduSegment, __FILE__, __FUNCTION__, __LINE__);
		/* move to next segment */
		pstApduSegment = pstApduSegmentTemp;
	}
	pstAPDUSegmentData->m_pstSegment = NULL;

	/* free the memory allocated to store the actual(fixed + variable part) 
	APDU data for transmission side */
	/* free the memory allocated to store the actual(variable part only) 
	APDU data for reception side */
	if(NULL != pstAPDUSegmentData->m_pu8ActualApduData)
		OSAL_Free(pstAPDUSegmentData->m_pu8ActualApduData , 
		__FILE__, __FUNCTION__, __LINE__);
    pstAPDUSegmentData->m_pu8ActualApduData = NULL;

	/* free the memory allocated to pstAPDUSegmentData - META DATA */
	if(NULL != pstAPDUSegmentData)
		OSAL_Free(pstAPDUSegmentData , __FILE__, __FUNCTION__, __LINE__);
	pstProcQInfo->m_pstAPDUSegmentData = NULL;
	
	/* reset the flag as segmentation related data is removed from the node */
	pstProcQInfo->m_stProcessData.m_stAPDU.m_bSegmentedMessage = FALSE;

	/* check pdu type, if its complex_ack then maintain the pointers in g_stComplexAckDataQue */
	if(LINK_LIST_SEGMENTATION_TRANSMIT == eListType && PDU_TYPE_COMPLEX_ACK == ePduType)
	{
		if(g_stComplexAckDataQue.m_pstProcessInfoQ == 
			g_stComplexAckDataQue.m_pstProcessInfoQ_End_Address)
		{
			/* only 1 node in link list, clear it */
			OSAL_Free(pstProcQInfo , __FILE__, __FUNCTION__, __LINE__);
			/* clear the pointers in global structure */
			g_stComplexAckDataQue.m_pstProcessInfoQ = NULL;
			g_stComplexAckDataQue.m_pstProcessInfoQ_End_Address = NULL;
		}
		else if(NULL == g_stComplexAckDataQue.m_pstProcessInfoQ->m_pstPreviousProcessInfo &&
				g_stComplexAckDataQue.m_pstProcessInfoQ == pstProcQInfo)
		{
			/* this is the 1st node */
			(pstProcQInfo->m_pstNextProcessInfo)->m_pstPreviousProcessInfo = NULL;
			g_stComplexAckDataQue.m_pstProcessInfoQ = pstProcQInfo->m_pstNextProcessInfo;
			OSAL_Free(pstProcQInfo , __FILE__, __FUNCTION__, __LINE__);
		}
		else if(NULL == g_stComplexAckDataQue.m_pstProcessInfoQ_End_Address->m_pstNextProcessInfo &&
				g_stComplexAckDataQue.m_pstProcessInfoQ_End_Address == pstProcQInfo)
		{
			/* last node of list */
			(pstProcQInfo->m_pstPreviousProcessInfo)->m_pstNextProcessInfo = NULL;
			g_stComplexAckDataQue.m_pstProcessInfoQ_End_Address = 
				pstProcQInfo->m_pstPreviousProcessInfo;
			OSAL_Free(pstProcQInfo , __FILE__, __FUNCTION__, __LINE__);
		}
		else
		{
			/* middle node */
			if((NULL == pstProcQInfo->m_pstNextProcessInfo) ||
				(NULL == pstProcQInfo->m_pstPreviousProcessInfo))
			{
				/* error case */
				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: Clear_Segmentation_Data: \
											null pointers \r\n");
				#endif
				/* release the mutex lock - Tx side */
				//Osal_Release_Mutex(g_stComplexAckDataQue.m_hCmplxAckMtxLock);
				 
				Osal_Release_Mutex( m_hCmplxAckMtxLockHandle );
				return; 
			}
			(pstProcQInfo->m_pstPreviousProcessInfo)->m_pstNextProcessInfo = 
				pstProcQInfo->m_pstNextProcessInfo;
			(pstProcQInfo->m_pstNextProcessInfo)->m_pstPreviousProcessInfo = 
				pstProcQInfo->m_pstPreviousProcessInfo;
			OSAL_Free(pstProcQInfo , __FILE__, __FUNCTION__, __LINE__);
		}
		/* release the mutex lock - Tx side */
		Osal_Release_Mutex( m_hCmplxAckMtxLockHandle );
	}

	/* check if processInfo pointer received is from reception side */
	/* in either case free the memory allocated to that pointer from linklist */
	if(LINK_LIST_SEGMENTATION_RECEIVE == eListType)
	{
		if(g_stRxSegmentsDataQue.m_pstRxProcessInfoQ == 
			g_stRxSegmentsDataQue.m_pstRxProcessInfoQ_End_Address)
		{
			/* only 1 node in link list, clear it */
			OSAL_Free(pstProcQInfo , __FILE__, __FUNCTION__, __LINE__);
			/* clear the pointers in global structure */
			g_stRxSegmentsDataQue.m_pstRxProcessInfoQ = NULL;
			g_stRxSegmentsDataQue.m_pstRxProcessInfoQ_End_Address = NULL;
		}
		else if(NULL == g_stRxSegmentsDataQue.m_pstRxProcessInfoQ->m_pstPreviousProcessInfo &&
				g_stRxSegmentsDataQue.m_pstRxProcessInfoQ == pstProcQInfo)
		{
			/* this is the 1st node */
			(pstProcQInfo->m_pstNextProcessInfo)->m_pstPreviousProcessInfo = NULL;
			g_stRxSegmentsDataQue.m_pstRxProcessInfoQ = pstProcQInfo->m_pstNextProcessInfo;
			OSAL_Free(pstProcQInfo , __FILE__, __FUNCTION__, __LINE__);
		}
		else if(NULL == g_stRxSegmentsDataQue.m_pstRxProcessInfoQ_End_Address->m_pstNextProcessInfo &&
				g_stRxSegmentsDataQue.m_pstRxProcessInfoQ_End_Address == pstProcQInfo)
		{
			/* last node of list */
			(pstProcQInfo->m_pstPreviousProcessInfo)->m_pstNextProcessInfo = NULL;
			g_stRxSegmentsDataQue.m_pstRxProcessInfoQ_End_Address = 
				pstProcQInfo->m_pstPreviousProcessInfo;
			OSAL_Free(pstProcQInfo , __FILE__, __FUNCTION__, __LINE__);
		}
		else
		{
			/* middle node */
			if((NULL == pstProcQInfo->m_pstNextProcessInfo) ||
				(NULL == pstProcQInfo->m_pstPreviousProcessInfo))
			{
				/* error case */
				#ifdef DEBUG_PRINTF
				Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: Clear_Segmentation_Data: \
											null pointers \r\n");
				#endif
				/* release the mutex lock - Rx side */
				//Osal_Release_Mutex(g_stRxSegmentsDataQue.m_hRxSegMtxLock);
				 
				Osal_Release_Mutex(m_hRxSegMtxLockHandle);
				return; 
			}
			(pstProcQInfo->m_pstPreviousProcessInfo)->m_pstNextProcessInfo = 
				pstProcQInfo->m_pstNextProcessInfo;
			(pstProcQInfo->m_pstNextProcessInfo)->m_pstPreviousProcessInfo = 
				pstProcQInfo->m_pstPreviousProcessInfo;
			OSAL_Free(pstProcQInfo , __FILE__, __FUNCTION__, __LINE__);
		}
		/* release the mutex lock - Rx side */
		//Osal_Release_Mutex(g_stRxSegmentsDataQue.m_hRxSegMtxLock);
		 
		Osal_Release_Mutex(m_hRxSegMtxLockHandle);
	}

	Osal_Release_Mutex( m_hCmplxAckMtxLockHandle );//mstp
	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Clear_Segmentation_Data: exit \r\n");
	#endif
	return;
}

/********************************************************************************************
*
*	DESCRIPTION
*		This is a Doubly Link List.
*	
*	LINK_LIST_SEGMENTATION_TRANSMIT	=	link list for transmission of segmented complex ack.
*	LINK_LIST_SEGMENTATION_RECEIVE	=	link list for reception of segmented complex ack & 
*										segmented confirm request.	
*	
*********************************************************************************************/
static BACNET_RETURN_TYPE Create_LinkList_For_Segmentation(processInfo_t *pstProcQInfo,
												  LINK_LIST_TYPE eListType)
{
	processInfo_t *pstProcQInfoNew = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Create_LinkList_For_Segmentation: entry \r\n");
	#endif

	/* check for null input pointer */
	if(NULL == pstProcQInfo)
		return BACDEL_ERROR;

	/* allocate memory */
	pstProcQInfoNew = (processInfo_t *)OSAL_Malloc(sizeof(processInfo_t),
												__FILE__, __FUNCTION__, __LINE__);
	if(NULL == pstProcQInfoNew)
	{
		/* failed memory allocation so return error */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: Create_LinkList_For_Segmentation: \
									malloc error \r\n");
		#endif
		return BACDEL_MALLOC_ERROR;
	}
	/* make m_eState i.e. state of process queue to PROC_CONTINUE i.e. in use */
	pstProcQInfoNew->m_eState = PROC_CONTINUE;

	if(LINK_LIST_SEGMENTATION_TRANSMIT == eListType)
	{
		/* copy all data from the original complex ack to new location in complexACK linklist */
		memcpy(pstProcQInfoNew, pstProcQInfo, sizeof(processInfo_t));

		/* acquire the mutex lock  - Tx side */
		//Osal_Wait_Mutex(g_stComplexAckDataQue.m_hCmplxAckMtxLock, INFINITE);
		 
		Osal_Wait_Mutex( m_hCmplxAckMtxLockHandle , INFINITE);

		/* maintain the link list pointers */
		if(NULL == g_stComplexAckDataQue.m_pstProcessInfoQ)
		{
			pstProcQInfoNew->m_pstNextProcessInfo = NULL;
			pstProcQInfoNew->m_pstPreviousProcessInfo = NULL;
			/* update the root node address only when list is empty & 1st node for the list is created */
			g_stComplexAckDataQue.m_pstProcessInfoQ = pstProcQInfoNew;
			/* as this is 1st node, end node is also the same */
			g_stComplexAckDataQue.m_pstProcessInfoQ_End_Address = pstProcQInfoNew;
		}
		else
		{
			pstProcQInfoNew->m_pstNextProcessInfo = NULL;
			pstProcQInfoNew->m_pstPreviousProcessInfo = g_stComplexAckDataQue.m_pstProcessInfoQ_End_Address;
			/* create the list here */
			(g_stComplexAckDataQue.m_pstProcessInfoQ_End_Address)->m_pstNextProcessInfo = pstProcQInfoNew;
			g_stComplexAckDataQue.m_pstProcessInfoQ_End_Address = pstProcQInfoNew;
		}

		/* release the mutex - Tx side */
		Osal_Release_Mutex( m_hCmplxAckMtxLockHandle );
	}

	else if(LINK_LIST_SEGMENTATION_RECEIVE == eListType)
	{

		/* copy all data from the original complex ack to new location in complexACK linklist */
		memcpy(pstProcQInfoNew, pstProcQInfo, sizeof(processInfo_t));

		/* acquire the mutex lock - Rx side */
		Osal_Wait_Mutex(m_hRxSegMtxLockHandle, INFINITE);

		/* maintain the link list pointers */
		if(NULL == g_stRxSegmentsDataQue.m_pstRxProcessInfoQ)
		{
			pstProcQInfoNew->m_pstNextProcessInfo = NULL;
			pstProcQInfoNew->m_pstPreviousProcessInfo = NULL;
			/* update the root node address only when list is empty & 1st node for the list is created */
			g_stRxSegmentsDataQue.m_pstRxProcessInfoQ = pstProcQInfoNew;
			/* as this is 1st node, end node is also the same */
			g_stRxSegmentsDataQue.m_pstRxProcessInfoQ_End_Address = pstProcQInfoNew;
		}
		else
		{
			pstProcQInfoNew->m_pstNextProcessInfo = NULL;
			pstProcQInfoNew->m_pstPreviousProcessInfo = g_stRxSegmentsDataQue.m_pstRxProcessInfoQ_End_Address;
			/* create the list here */
			(g_stRxSegmentsDataQue.m_pstRxProcessInfoQ_End_Address)->m_pstNextProcessInfo = pstProcQInfoNew;
			g_stRxSegmentsDataQue.m_pstRxProcessInfoQ_End_Address = pstProcQInfoNew;
		}	
		
		/* release the mutex - Rx side */
		Osal_Release_Mutex( m_hRxSegMtxLockHandle );
	}	
	else 
	{
		/* dead case - ideally won't ocur */
		return BACDEL_ERROR;
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Create_LinkList_For_Segmentation: exit \r\n");
	#endif
	return BACDEL_SUCCESS;
}

/*************************************************************************************
*
*	DESCRIPTION:
*   Function to handle message segment no 0 & accordingly create linklist (receive side)
*	to save segmentation related data.
*	-	this function validates every segment with seq.no. 0
*
*	@param		pProcQIndex			[in]	pointer to instance of process queue to fill
										decoded fixed parameters 
*
*	@return    BACNET_RETURN_TYPE - SUCCESS / ERROR.
*
**************************************************************************************/
BACNET_RETURN_TYPE Create_Segmentation_Node(processInfo_t *pstProcQInfo)
{
	BACNET_RETURN_TYPE eReturnVal = BACDEL_SUCCESS;
	Apdu_Segment_Data_t * pstAPDUSegmentData = NULL;
	BACNET_PDU_TYPE ePduType;
	processInfo_t *pstRxProcQInfo = NULL;
	uint32_t u32DevSegmentTimeout = 0;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Create_Segmentation_Node: entry \r\n");
	#endif

    if(NULL == pstProcQInfo)
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: Create_Segmentation_Node: \
									 null input pointer \r\n");
		#endif
		return BACDEL_ERROR;
    }

	/* validate the received segment 1st */
	eReturnVal = Validate_Segment(pstProcQInfo, &pstRxProcQInfo);
	if(NULL != pstRxProcQInfo && BACDEL_ERROR != eReturnVal)
	{
		/* this is duplicate segment with seq.no. 0 which is already present */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL2,"BACnetStackAppLayer: Create_Segmentation_Node: \
									 duplicate 1st segment received \r\n");
		#endif
		return BACDEL_ERROR;
	}

	/* allocate memory to save segmentation data */
	pstAPDUSegmentData = (Apdu_Segment_Data_t *)OSAL_Malloc(sizeof(Apdu_Segment_Data_t),
														__FILE__, __FUNCTION__, __LINE__);
	if(NULL == pstAPDUSegmentData)
	{
		/* failed memory allocation so return error */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
									Create_Segmentation_Node: malloc error \r\n");
		#endif
		return BACDEL_MALLOC_ERROR;
	}
	/* link allocated memory with the pointer in global structure for initiated request */
	pstProcQInfo->m_pstAPDUSegmentData = pstAPDUSegmentData;

	/* read apdu segment timeout property */
    u32DevSegmentTimeout = ((virtualDevData_t*)pstProcQInfo->m_pvReqDevStruct)->m_stDevObject.
						m_stAPDUSegTimeout.m_u32Val / CONVERT_TO_SECONDS;

	/* get pdu type */
	ePduType = pstProcQInfo->m_stProcessData.m_ePDUType;

	/* fill the segmentation data */
	pstAPDUSegmentData->m_i32PDUType = pstProcQInfo->m_stProcessData.m_ePDUType;
	pstAPDUSegmentData->m_u8ProposedWindowSize = 
        pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ProposedWindowNumber;
	pstAPDUSegmentData->m_pstSegment = NULL;
	pstAPDUSegmentData->m_bNAK = FALSE;
	pstAPDUSegmentData->m_u8NoOfPackets = 0;
	pstAPDUSegmentData->m_u32ActualDataSize = 0;
	pstAPDUSegmentData->m_u32DevSegmentTimeout = u32DevSegmentTimeout;
	pstAPDUSegmentData->m_i8LastSegment = -1;
	pstAPDUSegmentData->m_i8SegAckFor = -1;
	pstAPDUSegmentData->m_pu8ActualApduData = NULL;
	pstAPDUSegmentData->m_bAllSegments = FALSE;
	if(PDU_TYPE_CONFIRMED_SERVICE_REQUEST == ePduType)
		pstAPDUSegmentData->m_bServer = TRUE;	/* this is server device */

	/* set default values for m_stRxSegmentInfo */
	pstAPDUSegmentData->m_uRxTxSegmentInfo.m_stRxSegmentInfo.m_bSendSegAck = FALSE;
	pstAPDUSegmentData->m_uRxTxSegmentInfo.m_stRxSegmentInfo.m_u8NextSegment = 0;
	pstAPDUSegmentData->m_uRxTxSegmentInfo.m_stRxSegmentInfo.m_u8PacketCount = 0;

	/* Deside Actual Window Size */
	if(pstAPDUSegmentData->m_u8ProposedWindowSize < g_u8ActualWindowSize)
	{
		/* use the received proposed window size */
		pstAPDUSegmentData->m_u8ActualWindowSize = pstAPDUSegmentData->m_u8ProposedWindowSize;
	}
	else 
	{
		/* use the stack's actual window size */
		pstAPDUSegmentData->m_u8ActualWindowSize = g_u8ActualWindowSize;
	}

	/* save all data from process que & create a link list */
	eReturnVal = Create_LinkList_For_Segmentation(pstProcQInfo, LINK_LIST_SEGMENTATION_RECEIVE);
	if(BACDEL_SUCCESS != eReturnVal)
	{
		/* free malloced memory */
		OSAL_Free(pstAPDUSegmentData, __FILE__, __FUNCTION__, __LINE__);
        pstAPDUSegmentData = NULL;
		return eReturnVal;
	}

    /* search initiated request for this invoke id & update its state to 
        STATE_SEGMENTED_CONFIRMATION - only if received segment belongs to
        complex ack pdu */
    if(PDU_TYPE_COMPLEX_ACK == ePduType)
    {
        Update_InitiateQ_State(pstProcQInfo->m_stProcessData.m_stAPDU.m_u8InvokeId,
            STATE_SEGMENTED_CONFIRMATION,
			&pstProcQInfo->m_stProcessData.m_stRmDvAddr);
    }

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: : exit \r\n");
	#endif
	return eReturnVal;
}

/*************************************************************************************************
*	DESCRIPTION:
*		Function decides if the received segment is either expected or duplicate or out of order.
*		BACDEL_CONTINUE		=	expected new segment received, save data. 
*		BACDEL_SUCCESS		=	out of order segment, send seg ack.
*		BACDEL_ERROR		=	default return value. 
*		BACDEL_OTHER		=	duplicate segment received, do nothing.
*
*	@param		pstProcQInfo	[in]	instance of process queue containing segment received.
*	@param		pstOutProcQInfo	[out]	pointer to instance of process dsata to which the segment 
*										belongs.
*
*	@return		BACNET_RETURN_TYPE
*
**************************************************************************************************/
static BACNET_RETURN_TYPE Validate_Segment(processInfo_t *pstProcQInfo, processInfo_t **pstOutProcQInfo)
{
	BACNET_RETURN_TYPE eReturnVal = BACDEL_ERROR;
	/*	 */
	processInfo_t *pstRxProcQInfo = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Validate_Segment: entry \r\n");
	#endif

    if(NULL == pstOutProcQInfo || NULL == pstProcQInfo)
    {
        return eReturnVal;
    }

	/* acquire the mutex lock */
	Osal_Wait_Mutex(m_hRxSegMtxLockHandle, INFINITE);
	/* get the address of 1st pstProcQInfo in global linklist */
	pstRxProcQInfo = g_stRxSegmentsDataQue.m_pstRxProcessInfoQ;

	while(NULL != pstRxProcQInfo)
	{
		/* match invoke id & address of segment source */
		if(	(pstRxProcQInfo->m_stProcessData.m_stAPDU.m_u8InvokeId ==
			pstProcQInfo->m_stProcessData.m_stAPDU.m_u8InvokeId) 
				&&
			(!memcmp(&pstRxProcQInfo->m_stProcessData.m_stRmDvAddr, 
				&pstProcQInfo->m_stProcessData.m_stRmDvAddr, 
				sizeof(BACnetAddress_t))) )
		{
			/* match the pdu type & service choice */
			if( (pstRxProcQInfo->m_stProcessData.m_ePDUType == pstProcQInfo->m_stProcessData.m_ePDUType)
					&&
				(pstRxProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice == 
				 pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ServiceChoice) )
			{	
				/* check if this is expected segment */
				if(pstProcQInfo->m_stProcessData.m_stAPDU.m_u8SequenceNumber == 
					pstRxProcQInfo->m_pstAPDUSegmentData->m_uRxTxSegmentInfo.
					m_stRxSegmentInfo.m_u8NextSegment)
				{
					pstRxProcQInfo->m_pstAPDUSegmentData->m_bNAK = FALSE;
					/* validation successful */
					eReturnVal = BACDEL_CONTINUE;
				}
				else if(pstProcQInfo->m_stProcessData.m_stAPDU.m_u8SequenceNumber <=
					pstRxProcQInfo->m_pstAPDUSegmentData->m_i8LastSegment)
				{
					/* duplicate segment received */
					/* ignore this segment */
					eReturnVal = BACDEL_OTHER;
				}
				else if(pstProcQInfo->m_stProcessData.m_stAPDU.m_u8SequenceNumber >
					pstRxProcQInfo->m_pstAPDUSegmentData->m_uRxTxSegmentInfo.
					m_stRxSegmentInfo.m_u8NextSegment)
				{
					eReturnVal = BACDEL_SUCCESS;
					/* send negative SegACK */
					pstRxProcQInfo->m_pstAPDUSegmentData->m_bNAK = TRUE;
				}
			}
		}//27-07-12:Change thhe place of break
		if(BACDEL_CONTINUE == eReturnVal) /* expected segment received */
			break;
		else if(BACDEL_SUCCESS == eReturnVal) /* segment out of order, send segACK */
			break;
		else if(BACDEL_OTHER == eReturnVal) /* duplicate segment received, ignore this */
			break;
		/* move to next node in linklist */
		if(NULL != pstRxProcQInfo)
			pstRxProcQInfo = pstRxProcQInfo->m_pstNextProcessInfo;
	}

	/* release the mutex */
	Osal_Release_Mutex( m_hRxSegMtxLockHandle );

	/* update pstOutProcQInfo to return the value */
	/*	returns null - if segment is not valid.
				null - if seg.no. is 0 & is not duplicate segment.
				pointer - if expected seg match found */
	*pstOutProcQInfo = pstRxProcQInfo;

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Validate_Segment: exit \r\n");
	#endif
	return eReturnVal;
}

/*********************************************************************************************
*
*	DESCRIPTION:
*		This function updates the process queue with the parameters to send proper SegACK for 
*	the received segment.
*		-	resets the m_u8PacketCount.
*		-	resets the m_bSendSegAck & m_bNAK flags.
*		-	maintains the value of m_i8SegAckSentFor i.e. last SegACK sent for seq.no.
*
**********************************************************************************************/
static void Update_ProcessQ_To_Send_SegACK(processInfo_t *pstRxProcQInfo, processInfo_t *pstProcQInfo)
{
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Update_ProcessQ_To_Send_SegACK: entry \r\n");
	#endif

	if(!pstRxProcQInfo || !pstProcQInfo)
		return;	

	if(NULL != pstRxProcQInfo->m_pstAPDUSegmentData)
	{	
		/* change pdu type */
		pstProcQInfo->m_stProcessData.m_ePDUType = PDU_TYPE_SEGMENT_ACK;
		/* set if this is server or client */
		pstProcQInfo->m_stProcessData.m_stAPDU.m_bServer = 
			pstRxProcQInfo->m_pstAPDUSegmentData->m_bServer;
		/* check if this is for negative SegACK */ 
		pstProcQInfo->m_stProcessData.m_stAPDU.m_bNAK = 
			pstRxProcQInfo->m_pstAPDUSegmentData->m_bNAK;
		/* update the seq.no with seq.no. of last received segment */
		pstProcQInfo->m_stProcessData.m_stAPDU.m_u8SequenceNumber = 
			pstRxProcQInfo->m_pstAPDUSegmentData->m_i8LastSegment;
		/* set the actual window size */
		pstProcQInfo->m_stProcessData.m_stAPDU.m_u8ProposedWindowNumber = 
			pstRxProcQInfo->m_pstAPDUSegmentData->m_u8ActualWindowSize;

		/* maintain upto which segment segACK is send */
		pstRxProcQInfo->m_pstAPDUSegmentData->m_i8SegAckFor = 
			pstRxProcQInfo->m_pstAPDUSegmentData->m_i8LastSegment;
		/* reset packet count */		
		pstRxProcQInfo->m_pstAPDUSegmentData->m_uRxTxSegmentInfo.
			m_stRxSegmentInfo.m_u8PacketCount = 0;
		/* reset the SegACK flag */
		pstRxProcQInfo->m_pstAPDUSegmentData->m_uRxTxSegmentInfo.
			m_stRxSegmentInfo.m_bSendSegAck = FALSE;
		/* reset the -ve SegACK flag */
		pstRxProcQInfo->m_pstAPDUSegmentData->m_bNAK = FALSE;
	}
	

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Update_ProcessQ_To_Send_SegACK: exit \r\n");
	#endif
}

/*********************************************************************************************
*
*	DESCRIPTION:
*		Function to decode & handle received segmented meaasges.
*		-	validates new received message segment & combines it previous ones.
*		-	checks if segACK is to be sent.
*		-	fills process queue with complete decoded message for further processing
*			after all segments have been received.
*
*	@param		pstProcQInfo		[in]    pointer to instance of process queue that 
*											received segmented data.
*	@param		pu8ServiceRequest	[in]	pointer to start of variable part of Apdu Data
*	@param		i16ServiceLen		[in]	length of Apdu request to process
*	@param		pstOutProcQInfo		[out]   pointer to instance of process queue to that
*											DvProcess / Initiate threads will use.
*
*	@return    BACNET_RETURN_TYPE	Depends on return type of validate segment & last segment 
*									received.
*
*********************************************************************************************/
BACNET_RETURN_TYPE APDU_Packet_Decode_Handler(processInfo_t *pstProcQInfo,
											  uint8_t *pu8ServiceRequest,
											  int32_t i32ServiceLen,
											  processInfo_t **pstOutProcQInfo)
{
	BACNET_RETURN_TYPE eReturnVal = BACDEL_SUCCESS;
	/* thi pointer holds address of member of receive linklist to which segment belongs */
	processInfo_t *pstRxProcQInfo = NULL; 
	processInfo_t *pstTemp = NULL; 
	/* local pointers */
	bacnetRequestData_t *pstRcvProcessData = NULL;
	APDUSegment_t *pApduSegment = NULL;
	APDUSegment_t *pApduSegmentTemp = NULL;
	Apdu_Segment_Data_t *pstAPDUSegmentData = NULL;
	uint16_t u16MaxSegments = 0;
    /* type of pdu */
    BACNET_PDU_TYPE ePduType = 0;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: APDU_Packet_Decode_Handler: entry \r\n");
	#endif

	if((NULL == pstProcQInfo) || (NULL == pu8ServiceRequest) ||
        (NULL == pstOutProcQInfo))
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: APDU_Packet_Decode_Handler: \
									Null input pointers \r\n");
		#endif
		return BACDEL_ERROR;
	}

	/* read max segments accepted property */
	u16MaxSegments = ((virtualDevData_t*)pstProcQInfo->m_pvReqDevStruct)->
        m_stDevObject.m_stMaxSegAccepted.m_u16Val;

    /* save pdu type */
    ePduType = pstProcQInfo->m_stProcessData.m_ePDUType;

	/* now validate the segment received */
	eReturnVal = Validate_Segment(pstProcQInfo, &pstRxProcQInfo);
	if(NULL == pstRxProcQInfo)
	{
		/* unknown segment */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: APDU_Packet_Decode_Handler: \
									Segment Validation Failed \r\n");
		#endif
		return BACDEL_ERROR;	
	}

	/* set the in use flag for process thread */
	BIT_SET(pstRxProcQInfo->m_u8NodeInUse, PROCESS_Q_PX_THREAD_BIT_NO);

	/* assign local pointers */
	pstRcvProcessData = &pstProcQInfo->m_stProcessData;
	pstAPDUSegmentData = pstRxProcQInfo->m_pstAPDUSegmentData;
    if(NULL == pstAPDUSegmentData || NULL == pstRcvProcessData)
    {
		#ifdef DEBUG_PRINTF
        Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: APDU_Packet_Decode_Handler: \
										null pointers \r\n");
		#endif
		/* clear the in use flag for process thread */
		BIT_CLEAR(pstRxProcQInfo->m_u8NodeInUse, PROCESS_Q_PX_THREAD_BIT_NO);
        Clear_Segmentation_Data_State(pstRxProcQInfo, LINK_LIST_SEGMENTATION_RECEIVE);
		return BACDEL_ERROR;
    }

	/* expected new segment received, process it */
	if(	(BACDEL_CONTINUE == eReturnVal) && 
		(NULL != pstAPDUSegmentData) && 
		(NULL != pstRcvProcessData))
	{
		/* allocate memory for all segment information */
		pApduSegment = (APDUSegment_t *)OSAL_Malloc(sizeof(APDUSegment_t),
														__FILE__, __FUNCTION__, __LINE__);
		if(NULL == pApduSegment)
		{
			/* memory allocation failed */
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: APDU_Packet_Decode_Handler: \
										malloc error \r\n");
			#endif
			/* clear the in use flag for process thread */
			BIT_CLEAR(pstRxProcQInfo->m_u8NodeInUse, PROCESS_Q_PX_THREAD_BIT_NO);
			Clear_Segmentation_Data_State(pstRxProcQInfo, LINK_LIST_SEGMENTATION_RECEIVE);
			return BACDEL_ERROR;
		}
		/* fill segment data */
		pApduSegment->m_eServiceState = SEGMENT_STATE_IDLE;
		pApduSegment->m_bMoreFollows = pstRcvProcessData->m_stAPDU.m_bMoreFollows;
		pApduSegment->m_pstNext = NULL;
		pApduSegment->m_ptrToData = NULL;
		pApduSegment->m_u16PacketLength = (uint16_t)i32ServiceLen;
		pApduSegment->m_u8InvokeId = pstRcvProcessData->m_stAPDU.m_u8InvokeId;
		pApduSegment->m_u8SequenceNumber = pstRcvProcessData->m_stAPDU.m_u8SequenceNumber;
		/* set segment timeout value - refer claue 5.4.1 - Twait_for_seg */
		pApduSegment->m_i32DevSegTimeout = (pstAPDUSegmentData->m_u32DevSegmentTimeout * 4);

		/* allocate memory to save variable part of apdu_data */
		pApduSegment->m_ptrToData = (uint8_t *)OSAL_Malloc((i32ServiceLen) ,
													__FILE__, __FUNCTION__, __LINE__);
        if(NULL == pApduSegment->m_ptrToData)
		{
			/* memory allocation failed */
			#ifdef DEBUG_PRINTF
			Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: APDU_Packet_Decode_Handler: \
										malloc error \r\n");
			#endif
			/* clear the in use flag for process thread */
			BIT_CLEAR(pstRxProcQInfo->m_u8NodeInUse, PROCESS_Q_PX_THREAD_BIT_NO);
			Clear_Segmentation_Data_State(pstRxProcQInfo, LINK_LIST_SEGMENTATION_RECEIVE);
			/* free malloced memory */
	        OSAL_Free(pApduSegment, __FILE__, __FUNCTION__, __LINE__);
			return BACDEL_ERROR;
		}
		/* copy the data */
		memcpy(pApduSegment->m_ptrToData, pu8ServiceRequest, i32ServiceLen);

		/* update packet info */
		pstAPDUSegmentData->m_i8LastSegment = pApduSegment->m_u8SequenceNumber;
		pstAPDUSegmentData->m_u8NoOfPackets++;
		pstAPDUSegmentData->m_bAllSegments = !pApduSegment->m_bMoreFollows;
		pstAPDUSegmentData->m_uRxTxSegmentInfo.m_stRxSegmentInfo.m_u8NextSegment = 
			pApduSegment->m_u8SequenceNumber + 1;
		pstAPDUSegmentData->m_uRxTxSegmentInfo.m_stRxSegmentInfo.m_u8PacketCount++;
		pstAPDUSegmentData->m_u32ActualDataSize += pApduSegment->m_u16PacketLength;

        /* acquire the mutex lock */
		Osal_Wait_Mutex(m_hRxSegMtxLockHandle, INFINITE);

		if(!pApduSegment->m_u8SequenceNumber) /* 1st packet */
		{
			/* 1st segment received, send segACK */
			pstAPDUSegmentData->m_uRxTxSegmentInfo.m_stRxSegmentInfo.m_bSendSegAck = TRUE;
			/* link 1st segment pointer with pointer for 1st packet in apdu_segment_data structure 
				i.e. pstRxProcQInfo */
			pstAPDUSegmentData->m_pstSegment = pApduSegment;	
		}
		else if(!pApduSegment->m_bMoreFollows) /* last packet */
		{
			/* last segment received, send segACK */
			pstAPDUSegmentData->m_uRxTxSegmentInfo.m_stRxSegmentInfo.m_bSendSegAck = TRUE;
			/* link with previous last segment in the list */
			pApduSegmentTemp = pstAPDUSegmentData->m_pstSegment;
			while(NULL != pApduSegmentTemp->m_pstNext)
				pApduSegmentTemp = pApduSegmentTemp->m_pstNext;
			pApduSegmentTemp->m_pstNext = pApduSegment;	
		}
		else /* middle packet */
		{
			/* link with previous last segment in the list */
			pApduSegmentTemp = pstAPDUSegmentData->m_pstSegment;
			while(NULL != pApduSegmentTemp->m_pstNext)
				pApduSegmentTemp = pApduSegmentTemp->m_pstNext;
			pApduSegmentTemp->m_pstNext = pApduSegment;	
			/* check if seg ack is required to be sent */
			if(pstAPDUSegmentData->m_uRxTxSegmentInfo.m_stRxSegmentInfo.m_u8PacketCount == 
				pstAPDUSegmentData->m_u8ActualWindowSize)
			{
				/* no of segments received is equal to the actual window size, send segACK */
				pstAPDUSegmentData->m_uRxTxSegmentInfo.m_stRxSegmentInfo.m_bSendSegAck = TRUE;
				/* reset packet count - reseted in Update_ProcessQ...() */	
				//pstAPDUSegmentData->m_stRxSegmentInfo.m_u8PacketCount = 0;
			}
		}

		/* get pointer of 1st segment */
		pApduSegmentTemp = pstAPDUSegmentData->m_pstSegment;
		while(NULL != pApduSegmentTemp)
		{
			/* when new segment is received then, change the state of its previous segment */
			if(SEGMENT_STATE_AWAIT_SEGMENT == pApduSegmentTemp->m_eServiceState)
				pApduSegmentTemp->m_eServiceState = SEGMENT_STATE_FIXED;
			pApduSegmentTemp = pApduSegmentTemp->m_pstNext;
		}
		if(FALSE == pstAPDUSegmentData->m_bAllSegments)
			/* wait for next segment, change state of current segment */
			pApduSegment->m_eServiceState = SEGMENT_STATE_AWAIT_SEGMENT;
		else pApduSegment->m_eServiceState = SEGMENT_STATE_FIXED;	/* last segment received */

		/* release the mutex */
		Osal_Release_Mutex(m_hRxSegMtxLockHandle);
	}
	else if(BACDEL_SUCCESS == eReturnVal) /* segment out of order */
	{
		/* last segment received was out of order, send SegAck */
		pstAPDUSegmentData->m_uRxTxSegmentInfo.m_stRxSegmentInfo.m_bSendSegAck = TRUE;
		/* increment m_u8PacketCount, packet count for current window */
		pstAPDUSegmentData->m_uRxTxSegmentInfo.m_stRxSegmentInfo.m_u8PacketCount++;
	}
	else if(BACDEL_OTHER == eReturnVal) /* duplicate segment */
	{
		/* do nothing just return error to free process queue memory */
		eReturnVal = BACDEL_ERROR;
		/* do not increment m_u8PacketCount, packet count for current window as this is 
			duplicate segnment */
		//pstAPDUSegmentData->m_uRxTxSegmentInfo.m_stRxSegmentInfo.m_u8PacketCount++;
		if(pstAPDUSegmentData->m_uRxTxSegmentInfo.m_stRxSegmentInfo.m_u8PacketCount == 
			pstAPDUSegmentData->m_u8ActualWindowSize)
		{
			/* no of segments received is equal to the actual window size, send segACK */
			pstAPDUSegmentData->m_uRxTxSegmentInfo.m_stRxSegmentInfo.m_bSendSegAck = TRUE;
			/* reset packet count - reseted in Update_ProcessQ...() */		
			//pstAPDUSegmentData->m_stRxSegmentInfo.m_u8PacketCount = 0;
		}
	}
	else
	{
		/* dead case - ideally should not occur */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: APDU_Packet_Decode_Handler: \
									Unexpected Segment \r\n");
		#endif
		/* clear the in use flag for process thread */
		BIT_CLEAR(pstRxProcQInfo->m_u8NodeInUse, PROCESS_Q_PX_THREAD_BIT_NO);
		return BACDEL_ERROR;	
	}

	/* check if max segments accepted limit is exceeded */
    if(BACDEL_CONTINUE == eReturnVal && NULL != pApduSegment)
    {
	    if((pApduSegment->m_u8SequenceNumber == u16MaxSegments-1) &&
		    (TRUE == pApduSegment->m_bMoreFollows))
	    {
		    /* max segments accepted limit reached, bt the last segment received states 
			    that more segments are going to come so send abort */
		    /* change pdu type */
		    pstProcQInfo->m_stProcessData.m_ePDUType = PDU_TYPE_ABORT;
		    /* check if this is server or client */
		    pstProcQInfo->m_stProcessData.m_stAPDU.m_bServer = 
			    pstRxProcQInfo->m_pstAPDUSegmentData->m_bServer;
		    pstProcQInfo->m_stProcessData.m_stAPDU.m_u32ErrorCode = ABORT_REASON_APDU_TOO_LONG;
			#ifdef DEBUG_PRINTF
		    Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: APDU_Packet_Decode_Handler: \
									    Max Segments Accepted limit exceeded \r\n");
			#endif
			eReturnVal = BACDEL_SUCCESS;

            /* Inform initiated request, that max segments limit reached */
            if(PDU_TYPE_COMPLEX_ACK == ePduType)
            {
                /* NOTE : code added to inform application that max segments exceeded */
				pstTemp = OSAL_Malloc(sizeof(processInfo_t), __FILE__, __FUNCTION__, __LINE__);
				if(NULL != pstTemp)
				{
					/* copy the received recoded data */
					memcpy(pstTemp, pstProcQInfo, sizeof(processInfo_t));
					pstTemp->m_bIsDynamic = TRUE;
					pstTemp->m_stProcessData.m_stAPDU.m_pvServiceRequestData = NULL;
					
					/* assign pointer and update return value */
					*pstOutProcQInfo = pstTemp;
					eReturnVal = BACDEL_LOCAL_BUFFER_EXCEEDED;
				}
            }
			/* clear the in use flag for process thread */
			BIT_CLEAR(pstRxProcQInfo->m_u8NodeInUse, PROCESS_Q_PX_THREAD_BIT_NO);
            /* clear segmentation data */
		    Clear_Segmentation_Data_State(pstRxProcQInfo, LINK_LIST_SEGMENTATION_RECEIVE);
		    return eReturnVal;
	    }
    }

	/* check if SegACK is to be sent */
	if(pstAPDUSegmentData->m_uRxTxSegmentInfo.m_stRxSegmentInfo.m_bSendSegAck)
	{
		/* update process queue to send seg ack */
		Update_ProcessQ_To_Send_SegACK(pstRxProcQInfo, pstProcQInfo);
		/* change return value to BACDEL_SUCCESS */
		eReturnVal = BACDEL_SUCCESS;
	}
	
	/* Check for m_bAllSegmentsReceived, all segments are received.
		combine all segments data to from unsegmented actual data & send it
		to APDU_Decode_Handler for actual service decoding */
	if(pstAPDUSegmentData->m_bAllSegments)
	{		
		/* call the function to combine the segments */
		Combine_Data_and_Fill_Process_Queue(pstRxProcQInfo, pstOutProcQInfo);

		/* change return value to BACDEL_OTHER, to indicate that all 
			segments are received & combined, now send for further decoding */
		eReturnVal = BACDEL_OTHER;

		/* search initiated request for this invoke id & update its state to 
            AWAIT-RESPONSE, only if received segment belongs to complex ack pdu */
        if(PDU_TYPE_COMPLEX_ACK == pstRxProcQInfo->m_stProcessData.m_ePDUType)
        {
            Update_InitiateQ_State(pstRxProcQInfo->m_stProcessData.
                m_stAPDU.m_u8InvokeId, STATE_AWAIT_RESPONSE,
				&pstRxProcQInfo->m_stProcessData.m_stRmDvAddr);
        }
	}

	/* clear the in use flag for process thread */
	BIT_CLEAR(pstRxProcQInfo->m_u8NodeInUse, PROCESS_Q_PX_THREAD_BIT_NO);

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: APDU_Packet_Decode_Handler: exit \r\n");
	#endif
	return eReturnVal;
}

/*******************************************************************************************
*	DESCRIPTION:
*		Function combines all segments of message & copy all data of that process from 
*		SEGMENTATION_RECEIVE linklist to process queue available index. & returns the 
*		address of the index in process queue.
*
********************************************************************************************/
static void Combine_Data_and_Fill_Process_Queue(processInfo_t *pstRxProcQInfo, processInfo_t **pstOutProcQInfo)
{
	/* local variables */

	uint32_t u32ApduLength = 0;
	uint32_t u32Length = 0;
	APDUSegment_t *pApduSegment = NULL;
	Apdu_Segment_Data_t *pstAPDUSegmentData = NULL;
	/* Proess data Queue counter */
	uint32_t u32IdDQCnt = 0;
	/* to save empty queue location of process queue */
	processInfo_t *pstNewProcQInfo = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Combine_Data_and_Fill_Process_Queue: entry \r\n");
	#endif

    if(NULL == pstRxProcQInfo || NULL == pstOutProcQInfo)
    {
        return;
    }
	/* get segmentation data */
	pstAPDUSegmentData = pstRxProcQInfo->m_pstAPDUSegmentData;
    if(NULL == pstAPDUSegmentData)
    {
        return;
    }
	pApduSegment = pstAPDUSegmentData->m_pstSegment;

	/* get total apdu data size - variable part */
	u32ApduLength = pstAPDUSegmentData->m_u32ActualDataSize;

	/* allocate memory for all segment information */
	pstRxProcQInfo->m_pstAPDUSegmentData->m_pu8ActualApduData = 
		(uint8_t *)OSAL_Malloc((u32ApduLength) ,__FILE__, __FUNCTION__, __LINE__);
	if(NULL == pstRxProcQInfo->m_pstAPDUSegmentData->m_pu8ActualApduData)
	{
		/* memory allocation failed */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: Combine_Data_and_Fill_Process_Queue: \
									malloc error \r\n");
		#endif
		return;
	}

	while(NULL != pApduSegment)
	{
		/* copy data from each segment */
		memcpy(&pstRxProcQInfo->m_pstAPDUSegmentData->m_pu8ActualApduData[u32Length],
			pApduSegment->m_ptrToData, pApduSegment->m_u16PacketLength);
		u32Length += pApduSegment->m_u16PacketLength;
		/* move to next segment */
		pApduSegment = pApduSegment->m_pstNext;
	}
	/* error added for testing */
	//pstRxProcQInfo->m_pstAPDUSegmentData->m_stRxSegmentInfo.m_pu8ActualApduData[0] = 255;

	/* calculated total apdu length & cross check */
	if(u32Length != u32ApduLength)
	{
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: Combine_Data_and_Fill_Process_Queue: \
									decoded data length mismatch \r\n");
		#endif
		/* free memory allocated to save segmentation data */
		Clear_Segmentation_Data_State(pstRxProcQInfo, LINK_LIST_SEGMENTATION_RECEIVE);
		return;
	}

	/* make the message unsegmented after combining all the segments */
	pstRxProcQInfo->m_stProcessData.m_stAPDU.m_bMoreFollows = FALSE;
	pstRxProcQInfo->m_stProcessData.m_stAPDU.m_bSegmentedMessage = FALSE;

	/* find empty queue index in process queue */
	u32IdDQCnt = Find_Empty_Queue_Index();
	if(UINT32_MAX != u32IdDQCnt)
	{
		/* get the pointer to new queue index */
		pstNewProcQInfo = &g_stProcDataQ.m_stProcessQueue[u32IdDQCnt];
		/* Modify the status of the queue index so that it does not get overwritten */
		pstNewProcQInfo->m_eState = PROC_CONTINUE;
		/* Copy combined data to process queue */
		memcpy(pstNewProcQInfo, pstRxProcQInfo, sizeof(processInfo_t));
		/* Return the address of index of processQ acquired */
		*pstOutProcQInfo = pstNewProcQInfo;
		/* Also return the address of processInfo from linklist so that it can be 
			freed after complete decoding */
		(*pstOutProcQInfo)->m_pstNextProcessInfo = pstRxProcQInfo;
	}
	else
	{
		;
		/* TODO - send abort reply */
		/* process queue is not empty */
		#ifdef DEBUG_PRINTF
		Print_DebugMsg(DEBUG_LEVEL0,"BACnetStackAppLayer: \
		Combine_Data_and_Fill_Process_Queue: Process queue is not empty. \r\n");
		#endif
	}

	/* function exit */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: Combine_Data_and_Fill_Process_Queue: exit \r\n");
	#endif
}

/*************************************************************************************
*
*	DESCRIPTION:
*   Function to update state of initiated request when segmented complex ack is 
*   received or segmented complex ack is timed out.
*
*	@param u8InvokeId [in] invoke Id of initiated request.
*   @param eServiceState [in] new state for initiated request.
*
*	@returns nothing.
*
**************************************************************************************/
void Update_InitiateQ_State(uint8_t u8InvokeId, 
                    INITIATE_SERVICE_STATE eServiceState,
					BACnetAddress_t *pstRmtDevAddr)
{
    /* pointer to traverse initiate que list */
    InitiateInfo_t *pstInitiateQ = NULL;

	/* function entry */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Update_InitiateQ_State: entry \r\n");
	#endif

    /* wait for mutex */
    Osal_Wait_Mutex( m_hInitMtxLockHandle, INFINITE);
    /* get pointer of 1st request in initiate Q */
    pstInitiateQ = g_stReqInitiateDataQue.m_pstInitiateInfoQ;

    while(NULL != pstInitiateQ)
    {
        if(u8InvokeId == pstInitiateQ->m_ReqProcInfo.m_stProcessData.
            m_stAPDU.m_u8InvokeId &&
			!memcmp(pstRmtDevAddr, &pstInitiateQ->m_ReqProcInfo.
			m_stProcessData.m_stRmDvAddr, sizeof(BACnetAddress_t)))
        {
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL1,"BACnetStackAppLayer: \
            Update_InitiateQ_State: state of invoke id %d updated to %d. \r\n",
            u8InvokeId, eServiceState);
			#endif
            /* update the state */
            pstInitiateQ->m_eServiceState = eServiceState;
            break;
        }
        /* move to next initiated request */
        pstInitiateQ = pstInitiateQ->pstNextAddress;
    }

    /* release mutex */
	Osal_Release_Mutex(m_hInitMtxLockHandle);

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3,"BACnetStackAppLayer: \
    Update_InitiateQ_State: exit \r\n");
	#endif
    return;
}

/**
*
* DESCRIPTION
* Segmentation thread.
* This thread process all segment ack's.
*
* @param  [in]  void.
* @return [out] nothing.
*
*/
//Thread_API Segmentation_Thread(void* lpThreadParameter)

void Segmentation_Thread_Task(void)
{
    /* local variables */
    virtualDevData_t* pstVirtualDev = NULL;
	InitiateInfo_t *pstInitiateQIndex = NULL;
	processInfo_t *pstRspData = NULL;

	/* local variables */
	bacnetRequestData_t *pstReqParam = NULL;
	Apdu_Segment_Data_t *pstAPDUSegmentData = NULL;
	bool bReturnValue = FALSE;
	processInfo_t *pstProcessInfoQ = NULL;
	processInfo_t *pstTempProcInfoQ = NULL;
	
	/* function entry */
	#ifdef DEBUG_PRINTF
	Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Segmentation_Thread: Entry \r\n");
	#endif

	/* thread routine */
    while(1)
    {
		/* reset pointers */
		pstRspData = NULL;


		/* wait for infinite, until the semaphore is released */
		OSAL_Wait_Sem(g_hSegmentationSemaphoreHandle, INFINITE);//v43

		/* get pointer to response parameters */
        pstRspData = g_stReqInitiateDataQue.m_pstSegRspQueue[g_stReqInitiateDataQue.
            m_i32SegRspQIndex];

		/* check pointers */
		if(NULL == pstRspData)
		{
			/* handle the error */
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL1, "BACnetStackAppLayer: \
			Segmentation_Thread: null pointers \r\n");
			#endif
			continue;
		}

        /* get virtual device data */
        pstVirtualDev = pstRspData->m_pvReqDevStruct;

		if(NULL == pstVirtualDev)
		{
			/* handle the error */
			#ifdef DEBUG_PRINTF
            Print_DebugMsg(DEBUG_LEVEL1, "BACnetStackAppLayer: \
			Segmentation_Thread: null VD pointers \r\n");
			#endif
			continue;
		}

		// Ack from SERVER
		/* Segment ACK was received from server i.e. segACK is response of initiated request */
		if(pstRspData->m_stProcessData.m_stAPDU.m_bServer)
		{

			pstInitiateQIndex = Traverse_InitiateInfo_Queue(      //v43
							FIND_INVOKE_ID, 0,
							pstRspData->m_stProcessData.m_stAPDU.m_u8InvokeId,
							NULL, 0, (BACnetAddress_t*)pstRspData);

			if(NULL != pstInitiateQIndex)
			{
				/* set the in use flag for segmentation thread */
				BIT_SET(pstInitiateQIndex->m_u8NodeInUse, INITIATE_Q_SEG_THREAD_BIT_NO);

				/* get pointer to original request parameters */
				pstReqParam = &pstInitiateQIndex->m_ReqProcInfo.m_stProcessData;

				/* get pointer of apdu segmentation data */
				pstAPDUSegmentData = pstInitiateQIndex->m_ReqProcInfo.m_pstAPDUSegmentData;

				if((pstReqParam->m_stAPDU.m_u8InvokeId ==
					pstRspData->m_stProcessData.m_stAPDU.m_u8InvokeId)
					&&
					(NULL != pstAPDUSegmentData))
				{

					/* exchange data */
					bReturnValue = Data_Exchange_A_Side_For_Segmentation(pstRspData,
									pstAPDUSegmentData);

					if(!pstAPDUSegmentData->m_bAllSegments &&
						TRUE == bReturnValue)
					{

						/* all segments are not transmitted, so transmitt remaining */
						APDU_Packet_Encode_Handler(&pstInitiateQIndex->m_ReqProcInfo,
							pstAPDUSegmentData->m_i8SegAckFor,
							pstAPDUSegmentData);
						bReturnValue = FALSE;
					}
                    else if(!pstAPDUSegmentData->m_bAllSegments &&
                        FALSE == bReturnValue)
					{
                        ;/* do nothing */
						/* all segments are not transmitted but received either
						- duplicate seg ack or
						- seg ack with invalid data */
					}
					else
					{
						/* all segments are transmitted, so clear allocated memories */
						Clear_Segmentation_Data(&pstInitiateQIndex->m_ReqProcInfo,
							LINK_LIST_SEGMENTATION_TRANSMIT);
                        pstInitiateQIndex->m_ReqProcInfo.m_pstAPDUSegmentData = NULL;

						/* Request sent succesfully wait for response */
						/* update state in Initiate Queue for original request */
						if(STATE_SEGMENTED_CONFIRMATION == pstInitiateQIndex->m_eServiceState)
						{
							/* change to await only if in segmented state */
							/* check added to avoid retries due to thread scheduling */
							pstInitiateQIndex->m_eServiceState = STATE_AWAIT_RESPONSE;
							/* reset apdu timeout count down */
							pstInitiateQIndex->m_ReqProcInfo.m_stProcessData.m_i32DevTimeout =
								(pstVirtualDev->m_stDevObject.
								m_stAPDUTimeout.m_u32Val)/CONVERT_TO_SECONDS;
						}
					}
				}
				else
				{
					; /* error case */
				}

				/* clear the in use flag for segmentation thread */
				BIT_CLEAR(pstInitiateQIndex->m_u8NodeInUse, INITIATE_Q_SEG_THREAD_BIT_NO);
			}
		}// end of SegACK from SERVER
		
		// Ack from CLIENT
		/* Segment ACK was received from client i.e. segACK is response of complex ACK */
		else if(!pstRspData->m_stProcessData.m_stAPDU.m_bServer)	 
		{
			/* scan Complex ack ProcessInfo Queue for same invoke ID */
			pstProcessInfoQ = g_stComplexAckDataQue.m_pstProcessInfoQ;

			while(pstProcessInfoQ != NULL )
			{
                /* get next node */
                pstTempProcInfoQ = pstProcessInfoQ->m_pstNextProcessInfo;
				/* get pointer to original request parameters */
				pstReqParam = &(pstProcessInfoQ->m_stProcessData);
				/* get pointer of apdu segmentation data */
				pstAPDUSegmentData = pstProcessInfoQ->m_pstAPDUSegmentData;

				if((pstReqParam->m_stAPDU.m_u8InvokeId == 
					pstRspData->m_stProcessData.m_stAPDU.m_u8InvokeId) 
					&&
					(NULL != pstAPDUSegmentData))
				{
					/* exchange data */
					bReturnValue = Data_Exchange_A_Side_For_Segmentation(pstRspData,
									pstAPDUSegmentData);
					if(!pstAPDUSegmentData->m_bAllSegments &&
						TRUE == bReturnValue)
					{
						/* all segments are not transmitted, so transmitt remaining */
						 g_stMstpVariables.TokenRequiredSegmentation=TRUE;
						APDU_Packet_Encode_Handler(pstProcessInfoQ, 
							pstAPDUSegmentData->m_i8SegAckFor, 
							pstAPDUSegmentData);
						bReturnValue = FALSE;
					}
                    else if(!pstAPDUSegmentData->m_bAllSegments && 
                        FALSE == bReturnValue)
					{
                        ;/* do nothing */
						/* all segments are not transmitted but received either 
						- duplicate seg ack or 
						- seg ack with invalid data */
					}
					else
					{
						/* complex ack sent succesfully */
						/* update state in process Queue for original complex ack */
						/* all segments are transmitted, so clear allocated memories */
						Clear_Segmentation_Data_State(pstProcessInfoQ,
							LINK_LIST_SEGMENTATION_TRANSMIT);
					}
					break;
				}
				else
				{
					; /* error case */
				}
				/* move to next element in pstProcessInfoQ */
				pstProcessInfoQ = pstTempProcInfoQ;
			}
		}// end of SegACK from CLIENT 

		/* Clear response data after processing parameters */
		/* Check Response queue is filled */
		if(NULL != pstRspData)
		{
			/* Clear Response Process Info from Process queue */
			Clear_Process_Info(pstRspData);
            
			/* Reintialize the parametres of the structure */
			memset(pstRspData, 0, sizeof(processInfo_t));

			/* Set State of Process Info data as PROC_INIT. Used in ProcessQueue 
			   element to Reuse for received NW data, Not much useful for Initiate
			   Queue as Initiate Queue uses other State param */
			pstRspData->m_eState = PROC_INIT;

			/* free memory if dynamically allocated */
			if(TRUE == pstRspData->m_bIsDynamic)
            {
				; // TODO - uncomment if required
                //OSAL_Free(pstRspData, __FILE__, __FUNCTION__, __LINE__);
            }
		}

		/* Delete entry from response queue */
		g_stReqInitiateDataQue.m_pstSegRspQueue
			[g_stReqInitiateDataQue.m_i32SegRspQIndex] = NULL;

		/* Increment m_i32SegRspQIndex to point to next location */
		g_stReqInitiateDataQue.m_i32SegRspQIndex++;
		if(MAX_INITIATE_SERVICES ==  g_stReqInitiateDataQue.m_i32SegRspQIndex)
		{
			g_stReqInitiateDataQue.m_i32SegRspQIndex = 0;
		}

    }// end of while

	/* function exit */
	#ifdef DEBUG_PRINTF
    Print_DebugMsg(DEBUG_LEVEL3, "BACnetStackAppLayer: \
	Segmentation_Thread : Exit \r\n");
	#endif
    return;
}

#endif /* SEGMENTATION_SUPPORTED */
