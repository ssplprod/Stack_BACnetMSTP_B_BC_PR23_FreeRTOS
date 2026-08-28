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
*   SoftDEL Systems Ltd.                     		    india@softdel.com         
*   3rd Floor, Pentagon P4,								http://www.softdel.com  
*	Magarpatta City, Hadapsar
*	Pune - 411 028   							 
*                                                                         
******************************************************************************/
/****************************************************************************** 
*                                                                         
*   FILE
*                                                                      
*   AUTHORS                                                                             
*	 Harshal Mangale
*                                                                         
*   DESCRIPTION                                                            
*                                                                                
*   RELEASE HISTORY                                                                                                                            
*	DATE            NAME		   DESCRIPTION
*   05/08/2011      Harshal M.     File Created
*   16/09/2011      M.Venu         Added m_i8BVLCFunctionType element 
*                                  for Argument_List_t structure
*	16/09/2011		Harshal M.	   Added State in INITIATE_SERVICE_STATE
*	                               Changed Datatype of Indexes for Initiate queue
*   20/09/2011      Ashish Verma   Modified Structure Argument_List_t.
*   05/10/2011      Ashish verma   New structure added :: error_response_t,
*                                  error_wpm_response_t, wpm_request_t,
*                                  rpm_response_t, rpm_request_t, wp_request_t,
*                                  rp_response_t, rp_request_t, property_value_t,
*                                  cov_notification_t, cov_subscribe_t, 
*                                  dob_i_have_t, dob_who_has_t, ddb_i_am_t,
*                                  ddb_who_is_t, service_choice_u,apdu_packet_t
*                                  npdu_packet_t, bacnetip_arguments_t.
*                                  Following strcutures were removed :: 
*                                  Request_Param_t, Argument_List_t.
*   12/10/2011      Ashish Verma   Added fucntions Generate_Token_ID(),
*                                  BACDEL_Set_Default_Parameters().
*   13/10/2011      Ashish Verma   Modified BACDEL_Set_Default_Parameters() & added 
*                                  Validate_Received_Request(). Modifed enum
*                                  INITIATOR_REPSPONSE_TYPE.
******************************************************************************/

#ifndef INITIATE_SERVICE_H
#define INITIATE_SERVICE_H

#include "osalFreeRTOS.h"
#include "bacDELApi.h"
#include "pduServiceStructure.h"
#include "objDevice.h"
#include "pduDataEncodeDecode.h"

#define MAX_INITIATE_SERVICES MAX_PACKET        // need to be same as MAX_PACKET

#define DATA_NOT_AVALIABLE -1
#define RESPONSE_RECEIVE    1
#define RESPONSE_AWAIT      2
#define RESPONSE_TIMEOUT    255
#define CONVERT_TO_SECONDS  1000
#define DEFAULT_PORT_NUMBER 65535


/** Semaphore handles for initiate thread */
extern osSemaphoreId_t g_hInitiateSemaphoreHandle;
/** Thread exit for initiate thread */
extern bool g_bInitiateThreadExit;

/** Semaphore handle for acknowledgement thread */
extern Sem_H g_hAcknowledgementSemaphore;
/** Thread exit for initiate thread */
extern bool g_bAcknowledgementThreadExit;
/**
*                                                                         
*Name - InitiateInfo_t                                         
*                                                                      
*DESCRIPTION                                                            
*   This sturcture defines the data buffer that is member of queue data.
*   It stores the request/response packet in logical structures for easy 
*   access.
*
*/                                                                         
typedef struct INITIATE_INFO_Q
{
    /** State of queue for processing */
	INITIATE_SERVICE_STATE m_eServiceState;

    /* For Handling Service Parameters */
    processInfo_t m_ReqProcInfo;

	/* flags for fallback mechanism */
	bool m_bRPMError;
	bool m_bRPObjListError;

	/** Used for fallback mechanism */
    void * m_pRpmRequestParam;  //For storing current property pointer
    void * m_pBackupRpmReqParam;    //For storing head of RPM request

    rpm_response_t *m_pstRpmIPArgs;

    BACNET_CONF_DATA    *m_pstReadData;
    int16_t     m_i16ObjListCnt;

    /** For storing the Token ID */
    uint32_t u32TokenID;

    /** save the device id that initiated this request */
    uint32_t u32DeviceId;

	/** flag indicates if node is in use by other threads,
	so that the node is not deleted from initiate queue */
	/* bit usage is as follows:
	   0 - 1 - 2 - 3 - 4 - 5 - 6 - 7 
	   initiate thread - ack thread - seg thread - aside api - px thread */
	uint8_t m_u8NodeInUse;
	#define INITIATE_Q_INIT_THREAD_BIT_NO	0
	#define INITIATE_Q_ACK_THREAD_BIT_NO	1
	#define INITIATE_Q_SEG_THREAD_BIT_NO	2
	#define INITIATE_Q_ASIDE_API_BIT_NO		3
	#define INITIATE_Q_PX_THREAD_BIT_NO		4

    /* save the response for requests */
    bacnetip_arguments_t m_stIpArgs;

    /** Callback function pointer for Timeout/Response registered by application */
    App_Callback_Interface_t pvCallBackFunc;

    /* Provides the address of the previous link list location */
    struct INITIATE_INFO_Q *pstPreviousAddress;

    /* Provides the address of the previous link list location */
    struct INITIATE_INFO_Q *pstNextAddress;
}InitiateInfo_t;

/**
*                                                                         
*Name - InitiateDataQue_t                                         
*                                                                      
*DESCRIPTION                                                            
*   This sturcture defines the process queue data along with start & 
*   fill index. It also provide the mutex which will be needed
*   in case synchronizing the access to Queue.
*
*/                                                                         
typedef struct
{
    /** Index to process data from queue */
    //int32_t	m_i32InitQProcIdx;

    /** Index to fill data in queue */
	int32_t	m_i32InitiateQFillIndex;

    /** Total buffer size */
	ulong32_t   m_ul32BlkSize;

    /** Number of COV registered */
    uint16_t m_u16COVCount;

    /*  Link List to store request to send on network
        Entries in this array persist till its Timeout or 
        response is received and status is notified to apllication  
    */ 
	InitiateInfo_t   *m_pstInitiateInfoQ;

    /* Has address of the last update queue index */
    InitiateInfo_t   *m_pstInitiateInfoQ_End_Address;

    /* Process Queue containing response for initiated request */
    processInfo_t *m_pstRspQueue[MAX_INITIATE_SERVICES];

	/* Process Queue containing response for initiated request */
    processInfo_t *m_pstSegRspQueue[MAX_INITIATE_SERVICES];

    /** Index of processing data from m_pstProcessQueue*/
    int32_t m_i32RspQIndex;

    /** Index of fill data in m_pstProcessQueue*/
    int32_t m_i32RspQFillIndex;

	/** Index of processing seg ack data from Process Queue */
    int32_t m_i32SegRspQIndex;

    /** Index of fill seg ack data in Process Queue */
    int32_t m_i32SegRspQFillIndex;

    /** Mutex lock */
	Mutex_H  m_hInitMtxLock;			

}InitiateDataQue_t;

/* Dynamic Queue to hold Initiated request parameters */
extern InitiateDataQue_t g_stReqInitiateDataQue;

/* enum to traverse the Initiate Q to find data */
typedef enum {
    FIND_NONE = 0,
    FIND_STATE = 1,
    FIND_STATE_NEXT_TO = 2,
    FIND_INVOKE_ID = 3,
	FIND_TOKEN_ID = 4,
    /** add new values here */
    FIND_DEFAULT
} FIND_IN_INITIATE_Q;


//BACNET_RETURN_TYPE Start_Initiate_Thread(void);

BACNET_RETURN_TYPE Delete_Initiate_Thread(void);

BACNET_RETURN_TYPE Initiate_Service_Interface(processInfo_t *pstRspProcessData);

///** function to generate Invoke ID */ 
bool Generate_Invoke_ID(uint16_t *pu16InvokeID,
	BACNET_SERVICES_SUPPORTED eServiceSupport);

void Free_Invoke_ID(uint8_t u8CurrInvokeID);

////TIMER_CALLBACK Initiate_Service_Retry_Timer(Pvoid lpParam, BOOLEAN TimerOrWaitFired); <@>

/** function to generate Token ID */
uint32_t Generate_Token_ID(void);

/** validate the received request before sending */
BACNET_RETURN_TYPE Validate_Received_Request(bacnetip_arguments_t
                                                   *pstBACnetArgs);

///** clear request if response received, timedout or error */
////void Clear_InitiateQ(InitiateInfo_t **dpstInitiateQ);
void Clear_InitiateQ(void);

///** clear all initiated requests present in initiate Q */
void Clear_Initiated_Request(void);

///** make the state of initiate Q instance to error to delete it */
void Clear_InitiateQ_State(InitiateInfo_t **dpstInitiateQ);

/** function to free a-side response data for rpm, wpm, etc. */
void Free_Argument_Memory(void *pstMPSData, BACNET_SERVICES_SUPPORTED  eServiceSupported);

void Data_Exchange_A_Side(processInfo_t* pstRspData,
                          InitiateInfo_t *pstInitiateInfoQ);

void Copy_DDB_A_Data(InitiateInfo_t* pstInitiateQ);

void ClearInitiateQTimer(void);

void Fill_BacnetIP_Arguments(
	InitiateInfo_t *pstInitQData,
	processInfo_t *pstRspQueue ,
	bacnetip_arguments_t *pstIpArguments);

/** function to convert DT error code into BACnet error codes. */
void Convert_DT_Error_To_Error_Code(
	BACNET_ERROR_CODE *peErrCode, 
	BACNET_ERROR_CLASS *peErrClass,
	int32_t i32DtErrorCode);

/* function to finds desired node from initiate queue */
InitiateInfo_t *Traverse_InitiateInfo_Queue(
    FIND_IN_INITIATE_Q eFindBy, 
    INITIATE_SERVICE_STATE eStatus, uint8_t u8InvokeId, 
    InitiateInfo_t *pstInitiateQ,
	uint32_t u32TokenID,
	BACnetAddress_t *pstRespRmDevAddr);

/**	Function fills the initiator response structure as per the error value. */
void Fill_Initiator_Response(BACNET_RETURN_TYPE eReturnVal,
							initiator_response_t *pstInitRet,
							InitiateInfo_t **pstInitiateInfoQ);

#ifdef SEGMENTATION_SUPPORTED
/* Timer routine for segment retry mechanism, checking for segment_awaited & response_awaited
	segments & freeing the timed-out segments */
void SegmentationQTimer_Timer(void);
#endif


TIMER_CALLBACK Initiate_Service_Retry_Timer(void* lpParam, bool TimerOrWaitFired);

/* function to fill broadcast message parameters */
BACNET_RETURN_TYPE Fill_Destination_Parameter(
	bacnetRequestData_t *pstReqParam, 
	bacnetip_arguments_t *pstServiceArgs);

/** function to copy destination address parameter as per destination type */
BACNET_RETURN_TYPE Copy_Destination_Parameter(
	DESTINATION_TYPE eDestType,
	bool bDestTypeFlag,
	uint32_t u32DestDevId,
	BACnetAddress_t *pstDestinationAdd,
	BACnetAddress_t *pstReturnDestAddr);

/** Initiate thread. This thread encodes and sends all a-side requests on n/w */
void Initiate_Thread_Task(void);

/**Clear Initiate Request parameters and IpArguments param,after application reads
 response parameters.*/
void Clear_InitiateQ_Data(InitiateInfo_t *pstInitiateQ);

/** Acknowledgment thread task*/
void Acknowledgement_Thread_Task(void);

/** Fill the Aside Error Response*/
BACNET_RETURN_TYPE Fill_Aside_Error_Response(
				InitiateInfo_t *pstInitiateQIndex,
				BACNET_ERROR_CLASS eErrorClass,
				BACNET_ERROR_CODE eErrorCode,
				bool bFillErrorFlag);

#endif /* INITIATE_SERVICE_H */
