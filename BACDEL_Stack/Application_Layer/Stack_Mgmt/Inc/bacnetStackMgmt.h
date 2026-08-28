/********************************************************************************
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
*   SoftDEL Systems Ltd.                     	india@softdel.com         
*   3rd Floor, Pentagon P4,						http://www.softdel.com  
*	Magarpatta City, Hadapsar
*	Pune - 411 028   							 
*                                                                         
*   FILE
*	bacnetStackMgmt.h
*                                                                      
*   AUTHORS                                                                             
*	Abhinay Masurekar, Sagar Limaye, Harshal Mangale, 
*	Prashant Badgujar, M.Venu, Ashish Verma, Pratham Murkute
*                                                                         
*   DESCRIPTION                                                            
*	Defines basics threads, stack init and de-init functions required 
*	for functioning of stack. 
*	- Process Thread
*	- Transmit Thread
*	- Receive Thread
*	- Deletion Thread
*
********************************************************************************/

#ifndef STACK_MGMT_H
#define STACK_MGMT_H


/** header files */
#include "osalFreeRTOS.h"
#include "bacDELDef.h"
#include "pduServiceStructure.h"
#include"bacDELStackConfig.h"
#include "osalFreeRTOS.h"
/** Enums, Structures, etc. for APDU Segmentation */
#ifdef SEGMENTATION_SUPPORTED

/** Enum for Status Of Segments of APDU data */
typedef enum 
{
    SEGMENT_STATE_IDLE,
    SEGMENT_STATE_FILL_REQUEST,
    SEGMENT_STATE_SEND_REQUEST,
    SEGMENT_STATE_REQUEST_SEND_FAILED,
    SEGMENT_STATE_AWAIT_RESPONSE,
    SEGMENT_STATE_RESPONSE_RECEIVED,
    SEGMENT_STATE_TIMEOUT,
    SEGMENT_STATE_FIXED,
    SEGMENT_STATE_ERROR,
	SEGMENT_STATE_AWAIT_SEGMENT
}SEGMENT_STATE;

/**	structure that holds data for each segment of apdu */
typedef struct APDUSegment
{
	/** invoke id for segment - it should be same for all segments */
	uint8_t					m_u8InvokeId;
	/** sequence no. of the segment */
    uint8_t					m_u8SequenceNumber;
	/** stores the length of the encoded packet (fixed + variable) - Tx side */
	/** stores the length of variable part of data from the segment received - Rx side */
	uint16_t				m_u16PacketLength;
	/** state of segment for processing */
	SEGMENT_STATE			m_eServiceState;
	/** no. of retries for segment */
	int32_t					m_i32DevRetryCnt;
	/**	Segment timeout time */
    int32_t					m_i32DevSegTimeout;
	/** pointer to point to variable part of apdu data that the segment will send - Tx side */ 
	/** pointer that will hold variable part to received segment - Rx side */
	uint8_t	*				m_ptrToData;
	/** pointer to next segment */
	struct APDUSegment *	m_pstNext;
	/** states if segment is last or no */
    bool					m_bMoreFollows;
}APDUSegment_t;

/**	structure to store data related to transmitted segments */
typedef struct TxSegmentInfo
{
	/** copy u16BytesToCopy no of bytes to end of fixed part of every encoded packet */
	/** note : 6 bytes are needed for encoding fixed part of segmented confirm request apdu 
			: 5 bytes are needed for encoding fixed part of segmented complex ack apdu  */
	uint16_t	m_u16BytesToCopy;
	/** state if the process is waiting fo seg ack or not */
	bool_t		m_bWaitForSegAck;
}TxSegmentInfo_t; 

/**	structure to store data related to received segments */
typedef struct RxSegmentInfo
{
	/** next expected segment */
	uint8_t		m_u8NextSegment;
	/** no of packets received in sequence */
	uint8_t		m_u8PacketCount;
	/** states if segACK is to be sent or not */
	bool_t		m_bSendSegAck;
}RxSegmentInfo_t; 

/** union for RxSegmentInfo - TxSegmentInfo to optimize memory space */
typedef union RxTxSegInfo
{
	/** structure to hold data related to all Transmitted segments */
	TxSegmentInfo_t		m_stTxSegmentInfo;
	/** structure to hold data related to all Received segments */
	RxSegmentInfo_t		m_stRxSegmentInfo;
}RxTxSegInfo_t;

/** structure that will have all information related to segmentation of any APDU */
typedef struct ApduSegmentData
{
	/** indicates if all segments were transmitted & all of them received segACK - Tx side */
	/** indicates if all segments are received - Rx side */
	bool_t		m_bAllSegments;
	/** indicates if this device is server or client */
    bool_t		m_bServer;
	/** indicates if the segACK to be sent is negative ack */
	bool_t		m_bNAK;
	/** proposed window size */
	uint8_t		m_u8ProposedWindowSize;
	/** actual window size */
	/*	the no of max segments that can be sent in 1 go */
	uint8_t		m_u8ActualWindowSize;
	/** total no of segments to be sent - Tx side */
	/** total no of segments received - Rx side */
	uint8_t		m_u8NoOfPackets;
	/** indicates the segment that was last transmitted - Tx side */
	/** indicates the segment that was last received - Rx side */
	int8_t		m_i8LastSegment;
	/** indicates segACK was received for which segment - Tx side */
	/** indicates segACK was sent for which segment - Rx side */
	int8_t		m_i8SegAckFor;
	/** data size excluding fixed part of unsegmented apdu that is to be transmitted - Tx side */
	/** data size excluding fixed part of unsegmented apdu that is received - Rx side */
	/*	if LEN is total lenth then,
		LEN - 4 is actual data size, as fixed part in unsegmented confirmed apdu msg is 4 bytes 
		LEN - 3 is actual data size, as fixed part in unsegmented complex Ack apdu msg is 3 bytes*/
	uint32_t	m_u32ActualDataSize;
	/** pointer to actual APDU data without segmentation - Tx side */
	/** pointer to variable part of actual APDU data without segmentation - Rx side */
	uint8_t	*	m_pu8ActualApduData;
	/** type of pdu */
	BACNET_PDU_TYPE		m_i32PDUType;
	/**	Segment timeout time */
    uint32_t	m_u32DevSegmentTimeout;
	/** pointer to 1st segment */
	APDUSegment_t	*	m_pstSegment;
	/** union to store the Rx & Tx side data */
	RxTxSegInfo_t		m_uRxTxSegmentInfo;
}Apdu_Segment_Data_t;

#endif /* SEGMENTATION_SUPPORTED */

/** SoftTimer counts used for special state */
#if defined(__JC__)
#define	SM_STARTUP_DELAY			90000		/* 90 second (To detect it on JC System that the device has been offline) */
#else
#define	SM_STARTUP_DELAY			0			/** Normally no-delay */
#endif

/** MACROS, TYPEDEFS AND ENUMERATIONS. */
/* Constant MACROS. */
#define RX_SOCKET_TIMEOUT 1000      /** Timeout value for Rx */
#define MAX_RX_PKT	1506	        /** Max paxket value */

extern osSemaphoreId_t g_hPxSemaphoreHandle;


/** The TX Thread Counter Semaphore. */
extern osSemaphoreId_t g_hTxSemaphoreHandle;
extern osSemaphoreId_t g_hLockSemaphoreHandle;

/** Thread ID for Deletion thread.*/
extern Thread_ID g_dwDeletionThreadId;

/* enum's for the deletion thread */
typedef enum {
    POINTER_TYPE_DEFAULT = 0,
    POINTER_TYPE_DELETE_DEVICE = 1,
    POINTER_TYPE_DELETE_OBJECT = 2,
    POINTER_TYPE_INITIATE_Q = 3,
    POINTER_TYPE_PROCESS_Q = 4,
    POINTER_TYPE_SEGMENTATION_DATA = 5,
	POINTER_TYPE_VD_REQUEST_Q = 6,
    /* Add new values here */
	POINTER_TYPE_MAX
} POINTER_TYPE;

/**
*                                                                         
* Name - msgBuffer_t                                         
*                                                                      
* DESCRIPTION                                                            
* This structure defines the Receive queue data buffer.
*
*/                                                                         
typedef struct msgBuffer
{
	/** FALSE if queue location empty */
    bool m_eState;

    /** Received packet length */
	uint32_t m_u32PktLen;

    /** Data for received packet */
	/* 10 bytes added as buffer */
	uint8_t m_u8Buffer[MAX_MPDU+1];
}msgBuffer_t;

/**
*
* Name - processInfo_t
*
* DESCRIPTION
* This sturcture holds the decoded data for received raw hex data.
* It stores the request/response packet in logical structures for easy access.
*
*/

typedef struct PROCESS_INFO_Q
{
    /** State of queue processing */
	DDQ_STATE m_eState;

    /** Data value */
	bacnetRequestData_t	m_stProcessData;


    /* Pointer of Device structure for respective Data */
    void *m_pvReqDevStruct;

	/* pointer to save initiate queue node refernce */
	void *m_pvInitQRef;


	#ifdef SEGMENTATION_SUPPORTED
	/* For stroring Segmentation related data */
	Apdu_Segment_Data_t * m_pstAPDUSegmentData;

	/* For creating linklist of complex ack's that need segmentation */
	struct PROCESS_INFO_Q * m_pstNextProcessInfo;
	struct PROCESS_INFO_Q * m_pstPreviousProcessInfo;
	#endif

    /** variable that indicates if this instance of PROCESS_INFO_Q is dynamic or static */
    bool    m_bIsDynamic;
	
	/** flag indicates if node is in use by other threads,
	so that the memory is not released */
	/* bit usage is as follows:
	   0 - 1 - 2 - 3 - 4 - 5 - 6 - 7 
	   initiate thread - ack thread - seg thread - px thread - seg timer */
	uint8_t m_u8NodeInUse;
	#define PROCESS_Q_INIT_THREAD_BIT_NO	0
	#define PROCESS_Q_ACK_THREAD_BIT_NO		1
	#define PROCESS_Q_SEG_THREAD_BIT_NO		2
	#define PROCESS_Q_PX_THREAD_BIT_NO		3
	#define PROCESS_Q_SEG_TIMER_BIT_NO		4

	/** duplicate buffer to copy data from receive Q node i.e. raw hex data
	received from the network by the receive thread */
	msgBuffer_t  m_stPacketData;

}processInfo_t;

/**
*                                                                         
* Name - processDataQue_t                                         
*                                                                      
* DESCRIPTION                                                            
* This sturcture defines the process queue data along with process & 
* fill index. It also provide the mutex which will be needed
* in case synchronizing the access to Queue.
*
*/                                                                         
typedef struct
{
    /** Index to process data from queue */
    uint32_t	m_u32QProcIdx;

    /** Index to fill data in queue */
	uint32_t	m_u32QFillIndex;

    /** Total buffer size */
	ulong32_t   m_ul32BlkSize;

    /** Buffer of bacnet data pkt processed */
	processInfo_t   m_stProcessQueue[MAX_PACKET];

}processDataQue_t;

/**
*                                                                       
* Name - msgQue_t                                         
*                                                                      
* DESCRIPTION                                                            
* This sturcture defines the Receive queue along with block 
* size, fill & process index. It also defins the mutex 
* lock required to synchronize the queue.
*
*/                                                                         
typedef struct
{
    /** Index to process data from queue */
    uint32_t	    m_u32QProcIdx;

    /** Index to fill data in queue */
	uint32_t	    m_u32QFillIndex;

    /** Total buffer size */
	ulong32_t	    m_ul32BlkSize;

    /** Message buffer */
	msgBuffer_t		m_astRxQueue[MAX_PACKET];

}msgQue_t;

/* Rx Thread API which will get read message from NETWORK
 * over UDP socket, queue up data in RX Queue & increment
 * the process thread semaphore count. */
void Receive_Thread(void);

/* Proces thread API which reads data from RX queue and performs
 * decoding of request */
void Process_Thread_Task(void);

/* Tx Thread API which gets data from process queue and performs 
 * encoding of response to send it over NETWORK using UDP socket. */
void Transmit_Thread_Task(void);

/* This function is used to acquire empty location in Process queue. */
int32_t Find_Empty_Queue_Index(void);

/* This module is responsible to initialize the Service Layer, DLL threads and system objects. */

BACNET_RETURN_TYPE Init_ServiceLayer(uint8_t *pu8IPAddrs, uint16_t u16UdpPortNum);


/* This module is responsible to cleanup the Service Layer, DLL threads and system objects. */
BACNET_RETURN_TYPE DeInit_ServiceLayer(void);

/* This function is used to stop writing to Receive Q */
BOOL Stop_Receive_Req(void);

/* This function finds device with id u32DeviceId in Host device linklist */
void * Find_In_Host_Device_List(uint32_t u32DeviceId);

/* This function finds device with id u32DeviceId in Remote device linklist */
void * Find_In_Remote_Device_List(uint32_t u32DeviceId);

/**
*
* DESCRIPTION                                                                          
* This function registers the handlers of all the services supported
* by the BACnet stack. 
* Call this function at stack initialization.
*
*/  
void Init_Service_Handlers(void);

#endif /* STACK_MGMT_H */
