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
*	datalinkMSTPStateMachines.h
*                                                                      
*   AUTHORS                                                                     
*	Anagha Chitale
*                                                                         
*   DESCRIPTION                                                            
*	Functions for MSTP layer of stack.
*	Functions related to receive, master and slave state machines.
*
**********************************************************************************/

#ifndef MSTP_TSM_H
#define MSTP_TSM_H

/** header file includes */
#include "../../../Api/Inc/bacDELStackConfig.h"
#include "../../../OSAL/Inc/osalFreeRTOS.h"

#ifdef DEBUG_LOGFILES
extern uint16_t g_iTheTime;
extern FILE *tempTSMFp;
extern FILE *tempRxTSMFp;
extern FILE *tempRxFP;
extern FILE *ReqRespLog;
#endif

/** compile as C code */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/****************************************************************************
 ****             SoftTimer counts  used for BACnet support
 ****************************************************************************/
#define SM_NUM_POLLED						50			/** Number of tokens received or used before Poll for Master (fixed)*/
#define SM_T_NO_TOKEN						500			/** 500ms, Silence time for loss of token (fixed) */
#define SM_T_SLOT							10			/** 10ms Time slot */
#define SM_USAGE_TIMEOUT					35/*50*/			/** Token usage Timer of 15*2 = 30 mS */
#define SM_MIN_OCTETS					 	4			/** 4 number of "events (octets) for active line (fixed) */
#define SM_RETRY_TOKEN						1			/** 1 Retry token */
#define SM_FRAME_ABORT						50//4			/** (60 bit times) 100 ms max (fixed) */
#define SM_REPLY_DELAY						230			/** Send Post-Poned-Reply frame in within 250 ms */
#define	SM_POST_DRIVE						2			/** (15 bit times) (fixed)*/
extern uint16_t SM_REPLY_TIMEOUT;

/****************************************************************************
 ****             BACnet standard MSTP Frame Fields
 ****************************************************************************/
#define BN_PREAMBLE1						0
#define BN_PREAMBLE2						1
#define BN_FRAME_TYPE						2
#define BN_DESTINATION_ADD					3
#define BN_SOURCE_ADD						4
#define BN_LENGTHM							5
#define BN_LENGTHL							6
#define BN_HEADER_CRC						7
#define BN_DATA_START						8


/****************************************************************************
 ****             Receive finite state machine states (RFSM)
 ****************************************************************************/
/** Value for state machine IDLE state*/
#define BN_EMSM_IDLE						0
/** Value for state machine PREAMBLE state*/
#define BN_EMSM_PREAMBLE					1
/** Value for state machine HEADER state*/
#define BN_EMSM_HEADER						2
/** Value for state machine HEADERCRC state*/
#define BN_EMSM_HEADERCRC					3
/** Value for state machine DATA state*/
#define BN_EMSM_DATA						4
/** Value for state machine DATACRC  state*/
#define BN_EMSM_DATACRC						5
/** Value for state machine SKIPDATA state*/
#define BN_EMSM_SKIPDATA					6

/****************************************************************************
 ****             Master Finite State Machine states (MFSM)
 ****************************************************************************/
#define BACNET_STATE_IDLE                   0
#define BACNET_STATE_USE_TOKEN              1
#define BACNET_STATE_DONE_WITH_TOKEN        2
#define BACNET_STATE_PASS_TOKEN             3
#define BACNET_STATE_NO_TOKEN               4
#define BACNET_STATE_WAIT_FOR_REPLY         5
#define BACNET_STATE_POLL_FOR_MASTER        6
#define BACNET_STATE_ANSWER_DATA_REQUEST    7
#define BACNET_STATE_INITIAL                21

/****************************************************************************
 ****             MSTP frame mapping
 ****************************************************************************/
#define BACNET_FRM_TOKEN					0	/** Passing Token Frame */
#define BACNET_FRM_PFM						1	/** Poll for Master */
#define BACNET_FRM_REPLY_TO_PFM				2	/** Reply to Poll For Master */
#define BACNET_FRM_TEST_REQUEST				3	/** Reply to Test request */
#define BACNET_FRM_TEST_REPONSE				4	/** Reply to Test Response */
#define BACNET_FRM_DATA_EXP_REPLY			5	/** Data Frame expecting a reply */
#define BACNET_FRM_DATA_NOTEXP_REPLY		6	/** Data Frame not expecting a reply */
#define BACNET_FRM_REPLY_POSTPONED			7	/** Postponed Reply */


/****************************************************************************
 ****             BACnet Standard Header Characters
 ****************************************************************************/
#define BNC_PREAMBLE1						0x55
#define BNC_PREAMBLE2						0xFF


/****************************************************************************
 ****             defines for mstp variables
 ****************************************************************************/
/** Set if BACnet SoleMaster */
#define BN_SOLEMASTER						((uint8_t)0x01)
/**Set if Invalid Frame was received */
#define BN_REC_INVALID_FRM					((uint8_t)0x02)
/**Set if Valid Frame was received */
#define BN_REC_VALID_FRM					((uint8_t)0x04)
/**Indicates any time  of receive error, limited to errors found in Receive state machine */
#define BN_REC_ERROR						((uint8_t)0x10)
/**Indicates data is ready to xmit */
#define BN_XMIT_READY						((uint8_t)0x20)
/**BroadCast Address */
#define BACNET_BROADCAST					255
  
/****************************************************************************
 ****                Que Error codes
 ****************************************************************************/
#define BN_RxTx_ERRCODE_CRC					0
#define BN_RxTx_ERRCODE_2MUCH_DATA			1
#define BN_RxTx_ERRCODE_NO_PENDINGAVAIL		2

/****************************************************************************
 ****                         Send Message Switch labels
 ****************************************************************************/
#define BN_SMSM_INIT						0
#define BN_SMSM_IDLE						1
#define BN_SMSM_EXECUTE						2

/** Command buffer */
#define BN_QUESIZE							MAX_MSTP_MPDU

/****************************************************************************
 ****                         BACnet implementation defines
 ****************************************************************************/
#define BACNET_CRC_FUNC_REC					0
#define BACNET_CRC_FUNC_XMIT				1
#define GOOD_BACNET_HEADERCRC				0x55
#define GOOD_BACNET_DATACRC					0xF0B8
#define BACNET_SHORT_FRAME					8

/****************************************************************************
 ****
//Pending Queue holds data data retrieved from slower resources
// Example:
//   - Drive
//   - Real Time Clock
//   - On board non volatile memory
//Not sure at this time if this included the received message or
//just the response.
//Each que is linear and always starts from zero and increments 
//to MAX_MPDU_MSTP. But the whole entity is a circular structure
//of linear queues. There are BN_NUM_OF_PENDING_MSG linear queues
//in this entity.
 ****************************************************************************/
/** Value for Empty Status  */
#define BN_PENDING_STATUS_EMPTY				0 
/** Value for Busy Status */          
#define BN_PENDING_STATUS_BUSY				1     
/** Value for Ready Status */      
#define BN_PENDING_STATUS_READY				2   
/** Only use for command que */        
#define BN_PENDING_STATUS_CMDFILLED			1          
/** Value for pending command message */ 
#define BN_NUM_OF_PENDING_CMD_MSG			1
/** Value for pending response message */             
#define BN_NUM_OF_PENDING_RSP_MSG			2

/** Boolean value for No Reply */        
#define BN_PENDING_REPLY_NO					0  
/** Boolean value for Yes Reply */         
#define BN_PENDING_REPLY_YES				1           
/** Multiple properties support */
#define BN_NUM_MULTI_ACCESS					21 

/** TRUE/FLASE macros used in MSTP */
#define MSTP_TRUE							(1)
/** TRUE/FLASE macros used in MSTP */
#define MSTP_FALSE							(0)

/****************************************************************************
 ****            Structures
 ****************************************************************************/

typedef struct DEF_BN_PDU
{
   uint8_t	PropertyIdentifier;		/**  Property identifier		*/
   uint8_t	PropertyIndex;			/**  Property index				*/
   uint8_t	Priority;				/**  Priority					*/
   uint8_t	Data_Type;				/**  Data type					*/
   uint8_t	Data_Len;				/**  Data length				*/
   uint8_t	StatusFlags;			/**	 BACnet IN_Alarm, Fault, Over-ridden, out of service ...	*/
   uint8_t	StatusBits;				/**	 For Status of accessing individual Property				*/
   uint16_t Error;                  /**  Error Class + Error code	*/
   uint16_t ObjectType;             /**  Object type				*/
   uint32_t InstanceNumber;			/**  Instance number			*/
}def_BN_PDU;

/**
 @struct DefPendingMsg
 @brief
    This structure defines Pending message elements  .
*/ 
typedef struct DEFPendingMsg
{
   uint8_t	m_ucServiceChoice;					/** Service choice					*/	
   uint8_t	Max_Frame_Limit;					/** Maximum frame limit				*/
   uint16_t NumofChar;							/** Number of characters in the response message */
   uint8_t	Status;								/** Status of message				*/	
   uint8_t	DestinationAdd;						/** BACnet message destination		*/
   uint8_t	PDU_CNT;							/** PDU Count						*/
   uint16_t Error;								/** Error PDU TYPE +  Error Reason	*/
   uint8_t Data[MAX_MSTP_MPDU];				    /** Actual Data						*/
   def_BN_PDU BN_PDU[BN_NUM_MULTI_ACCESS];
   bool Reply;									/** Indicates if a reply is expected */
}DefPendingMsg;

/**
 @struct DefBnRxTxQue
 @brief
    This structure defines Rx/Tx buffer and relevant elements
*/
typedef struct BN_RX_TX_Q
{
	uint16_t		i16Head;				/**	Head											*/
	uint16_t		i16Tail;				/**	Tail											*/
	uint16_t        MessageExtractHead;		/**	Header used while extracting incoming packet	*/	
	uint16_t        DataLength;				/**	Length of query packet							*/
	uint8_t         State;					/** State											*/	
	uint16_t        MessageCnt;				/** Message count									*/
	uint8_t			ErrCode;				/** Error code										*/
	uint8_t			RxTsilence;				/** monitors silence time on Rx line				*/
	uint16_t        RxsCRA_OLD;				/** Holds change of Head count in 2ms time out		*/
	uint8_t			ErrCnt;					/** Number of error bytes received/transmitted		*/
	uint8_t			Status;					/** Done, Active, Error								*/
	uint8_t			Data[BN_QUESIZE];		/** RX/TX Data										*/
}DefBnRxTxQue;

/**
 @struct Def_BN_CMDPending
 @brief
    This structure defines Pending command structure .
*/
typedef struct DEF_BN_CMDPending
{    
   uint8_t i8Tail;           /** Head */
   uint8_t i8Head;           /** Tail */
   DefPendingMsg Msg[BN_NUM_OF_PENDING_CMD_MSG];         
}Def_BN_CMDPending;


/**
 @union WordSplit_u
 @brief
    This union defines storing of data in high and low bytes.
*/
/** Unions used to handle EndianNess */
typedef union {
   struct
   {
         uint8_t u8HighByte;
         uint8_t u8LowByte;
   }stBytes;
    uint16_t u16Word;
} WordSplit_u;

extern Def_BN_CMDPending BN_PendingCmd;			/** Global Pending Command Buffer of 200 bytes size	*/


extern bool_t g_bTransmitEnble;
extern bool_t g_bReplyDelayTimeoutFlag;
extern bool_t g_bTokenAvailable;
extern bool_t g_bDataAvlblToSend;

extern DefBnRxTxQue BnRxQue;				/** Global Receive Buffer of 200 bytes size */
extern DefBnRxTxQue BnTxQue;				/** Global Transmit Buffer of 200 bytes size */
extern uint8_t g_u8ReplyPostPoned;			/** Check for replay postpone */

extern osMessageQueueId_t qUartReceiveHandle;
extern osMessageQueueId_t g_hPxSemaphoreHandle;

/****************************************************************************
 ****             Function Prototypes
 ****************************************************************************/

/* Function for updating soft timers required to run BACnet MSTP state machine */
void BACnet_StateMachine_Timers(void);

/* Function for BACDEL_MSTP_Master_Node_Tsm of MSTP layer in stack */
void BACDEL_MSTP_Master_Node_Tsm(void);

/*  Function for BACDEL_MSTP_Receive_Frame_TSM of MSTP layer in stack */
void BACDEL_MSTP_Receive_Frame_TSM(void);

/* Copies Unsigned 16 byte type value into a given array */
void MoveUnsigned16toString(uint8_t* destination,  uint16_t value);

/* Function for initializing the state machine elements */
//static void InitializeSMElements(void);

/* Function for initializing variables related to the MSTP state machine */
BACNET_RETURN_TYPE BACDEL_MSTP_InitializeStack(uint8_t *pu8FileName);

/* Function to copy data in receive queue */
void Fill_Data_In_ReceiveQ(uint8_t *pu8RxBuf, uint16_t u16DataLength);

/* Function to fill received valid data in received state machine */
void MSTP_FillValidData(void);

/* Function to extract received valid data in received state machine */
void MSTP_ExtractValidData(void);




/**
*
* Name : MstpFillRcvdata_t
* 
* DESCRIPTION 
* This structure is used to stored valid data in received in receive 
* state machine.
*
*/
typedef struct
{
	/*  received destination */
	uint8_t				m_u8ReceivedDestination;
	/*  received source */
	uint8_t				m_u8ReceivedSource;
	/*  mstp frame type */
	uint8_t				m_u8ReceivedFrameType;
	/*  validity of frame */
	uint8_t				m_u8FrameValidity;

}MstpFillRcvdata_t;

/**
*
* Name : MstpFillRcvQueNode_t
* 
* DESCRIPTION 
* This queue is used to stored valid data in received in receive 
* state machine.
*
*/
typedef struct MstpFillRcvQueNode
{
	MstpFillRcvdata_t   m_stRcvData;

}MstpFillRcvQueNode_t;

/**
*
* Name : MstpFillRcvQue_t
* 
* DESCRIPTION 
* This queue is used to stored valid data in received in receive 
* state machine.
*
*/
typedef struct MstpFillRcvQue
{
	/* queue index to fill data */
	uint32_t		 m_u32FillIndex;
	/* queue index to process data */
	uint32_t		 m_u32ProcIndex;
	/* mstp tx data queue */
	MstpFillRcvQueNode_t m_stDataNode[3];

}MstpFillRcvQue_t;


/** mstp rx data queue. */
extern MstpFillRcvQue_t g_astMstpFillRcvQueue;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MSTP_TSM_H */
