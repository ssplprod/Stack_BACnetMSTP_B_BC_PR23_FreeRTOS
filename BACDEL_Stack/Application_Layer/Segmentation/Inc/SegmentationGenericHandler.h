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
*	SegmentationGenericHandler.h
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

#ifndef SEGMENTATION_H
#define SEGMENTATION_H
#ifdef SEGMENTATION_SUPPORTED

/**	includes	*/
#include "bacDELDef.h"
#include "bacDELApi.h"
#include "bacnetStackMgmt.h"
#include "osalFreeRTOS.h"

/*
*   Global variable for Segmentation: 
*	Actual Window Size
*   Proposed Window Size 
*/
extern uint8_t g_u8ActualWindowSize;
extern uint8_t g_u8ProposedWindowSize;

/** thread handle for segmententation thread */
extern Thread_H g_hSegmentationThread;
/** semaphore handle for segmententation thread */
extern Sem_H g_hSegmentationSemaphore;
/** thread exit flag for segmententation thread */
extern bool g_bSegmentThreadExit;
///** semaphore handle for Ack thread */

/* enum for type of link list */
typedef enum DOUBLY_LINK_LIST
{
	LINK_LIST_SEGMENTATION_TRANSMIT = 0,
	LINK_LIST_SEGMENTATION_RECEIVE = 1
}LINK_LIST_TYPE;

/* structure to form linked-list of received segmented messages */
typedef struct RxSegmentsDataQue
{
	/* address of the first node in the list */
    processInfo_t *m_pstRxProcessInfoQ; 

	/* address of the last node in the list */
    processInfo_t *m_pstRxProcessInfoQ_End_Address;

	 /* mutex lock */
	Mutex_H  m_hRxSegMtxLock;		
}RxSegmentsDataQue_t;

/* global structure for receiving segmented messages - declaration only */
extern RxSegmentsDataQue_t g_stRxSegmentsDataQue;

/* structure to form queue of segmented complex ack's. */
typedef struct ComplexAckDataQue
{
	/* address of the first node in the list */
    processInfo_t *m_pstProcessInfoQ; 

	/* address of the last node in the list */
    processInfo_t *m_pstProcessInfoQ_End_Address;

	/* mutex lock */
	Mutex_H  m_hCmplxAckMtxLock;		
}ComplexAckDataQue_t;

/* global structure for segmented_complex_ack queue  - declaration only */
extern ComplexAckDataQue_t g_stComplexAckDataQue;

/** function to encode & send apdu segments / packets */
BACNET_RETURN_TYPE APDU_Packet_Encode_Handler(
	processInfo_t *pstProcQInfo,
	int8_t i8SegAckReceivedFor,
	Apdu_Segment_Data_t *pstAPDUSegData);

/**	api to create apdu segments & store segmentation related data */
BACNET_RETURN_TYPE Create_APDU_Packets(
	processInfo_t *pstProcQInfo,
	uint8_t *pAPDUData,
	uint32_t u32APDULen);

/**	function to exchange segment ack data to a-side or b-side */
bool Data_Exchange_A_Side_For_Segmentation(
	processInfo_t *pstRspData,
	Apdu_Segment_Data_t* pstAPDUSegmentData);

/**	function to clear all data related to segmentation */
void Clear_Segmentation_Data(processInfo_t *pstProcQInfo, LINK_LIST_TYPE eListType);

/** function to change state of segmentation node for deletion */
void Clear_Segmentation_Data_State(processInfo_t *pstProcQInfo, LINK_LIST_TYPE eListType);

/**	function to create new node in Rx linklist to store received segmented message */
/** creates node only when valid 0th segment is received */
BACNET_RETURN_TYPE Create_Segmentation_Node(processInfo_t *pstProcQInfo);

/** function to handle all received segmented messages */
BACNET_RETURN_TYPE APDU_Packet_Decode_Handler(
	processInfo_t *pstProcQInfo,
	uint8_t *pu8ServiceRequest,
	int32_t i32ServiceLen,
	processInfo_t **pstOutProcQInfo);

/** function to update state of initiated request when segmented complex ack is 
    received or segmented complex ack is timed out. */
void Update_InitiateQ_State(uint8_t u8InvokeId, 
	INITIATE_SERVICE_STATE eServiceState,
	BACnetAddress_t *pstRmtDevAddr);

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
void Segmentation_Thread_Task(void);

#endif	/*	SEGMENTATION_SUPPORTED	*/
#endif	/*	SEGMENTATION_H	*/

